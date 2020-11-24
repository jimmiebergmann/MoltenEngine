/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/


#include "Editor/Application.hpp"
#include "Molten/Window/Window.hpp"
#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Gui/Layers/RootLayer.hpp"
#include "Molten/Gui/Widget.hpp"
#include "Molten/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Gui/Widgets/PaddingWidget.hpp"
#include "Molten/Gui/Widgets/VerticalGridWidget.hpp"
#include "Molten/Gui/Widgets/SpacerWidget.hpp"
#include <chrono>
#include <thread>


namespace Molten
{

    namespace Editor
    {

        Application::Application() :

            m_viewMatrixBuffer(nullptr),

            m_gridPipeline(nullptr),
            m_gridVertexBuffer(nullptr),
            m_gridIndexBuffer(nullptr),
            m_gridMatrixDescriptorSet(nullptr),
            m_gridColor1PushLocation(0),
            m_gridColor2PushLocation(0),

            m_objectPipeline(nullptr),
            m_objectVertexBuffer(nullptr),
            m_objectIndexBuffer(nullptr),
            m_objectMatrixDescriptorSet(nullptr),
            m_objectTextureDescriptorSet(nullptr),
            m_objectPosPushLocation(0),
            m_objectColorPushLocation(0),
            m_objectTexture(nullptr),

            m_programTime(0.0f),
            m_deltaTime(0.0f)//,
            //m_guiCanvas()
        { }

        Application::~Application()
        {
            Unload();
        }

        int Application::Start(int /*argv*/, char** /*argc*/)
        {
            try
            {
                Load();
            }
            catch (Exception & e)
            {
                PlatformWindow::Message(PlatformWindow::Type::Error, "Molten Engine loading error.", e.what());
                return -1;
            }          

            m_window->Show();
            m_deltaTimer.Reset();
            m_secondTimer.Reset();
            while (m_window->IsOpen())
            {
                Tick();
            }

            return 0;
        }

        void Application::Load()
        {
            m_window = std::unique_ptr<Window>(Window::Create());
            Vector2ui32 windowSize = { 800, 600 };
            std::string windowTitle = "Molten Editor";
            if (!m_window)
            {
                throw Exception("Failed to create editor window.");
            }
            if (!m_window->Open(windowTitle, windowSize, &m_logger))
            {
                throw Exception("Failed to open editor window.");
            }

            m_window->OnResize.Connect([&](Vector2ui32)
            {
                Tick();
            });
            m_window->OnDpiChange.Connect([&](Vector2ui32 dpi)
            {
                m_logger.Write(Logger::Severity::Info, "Changed DPI: " + std::to_string(dpi.x) + ", " + std::to_string(dpi.y));
            });
            m_window->OnScaleChange.Connect([&](Vector2f32 scale)
            {
                m_logger.Write(Logger::Severity::Info, "Changed scale: " + std::to_string(scale.x) + ", " + std::to_string(scale.y));
            });

            m_renderer = std::unique_ptr<Renderer>(Renderer::Create(Renderer::BackendApi::Vulkan));
            if (!m_renderer)
            {
                throw Exception("Failed to create Vulkan renderer.");
            }

            if (!m_renderer->Open(*m_window, Version(1, 1), &m_logger))
            {
                throw Exception("Failed to open Vulkan renderer.");
            }

            FramedUniformBufferDescriptor uniformBufferDesc;
            uniformBufferDesc.size = 512;
            m_viewMatrixBuffer = m_renderer->CreateFramedUniformBuffer(uniformBufferDesc);
            if (!m_viewMatrixBuffer)
            {
                throw Exception("Failed to create uniform buffer.");
            }

            LoadGridPipeline();
            LoadObjectPipeline();

            m_camera.SetPosition({ 0.0f, -3.0f, 0.0f });
            m_camera.SetDirection({ 0.3f, 1.0f, 0.0f });
            m_camera.SetProjectionType(Scene::Camera::ProjectionType::Perspective);
            m_camera.SetFieldOfView(Degrees(60));
            m_camera.SetWindowSize(m_window->GetSize());

            m_window->OnResize.Connect([&](Vector2ui32 size)
            {
                m_camera.SetWindowSize(size);
                //m_canvas->SetSize(size);
            });

            //LoadGui();
        }

        void Application::LoadGui()
        {
            m_canvasRenderer = Gui::CanvasRenderer::Create(*m_renderer, &m_logger);
            m_canvas = Gui::Canvas::Create(m_canvasRenderer);

            auto layer = Gui::Canvas::CreateChild<Gui::RootLayer>(m_canvas, Gui::LayerPosition::Top);
            auto grid = Gui::Layer::CreateChild<Gui::VerticalGrid>(layer, 
                10.0f, Gui::PaddingData{ 10.0f, 20.0f, 30.0f, 40.0f }, Gui::PaddingData{ 10.0f, 20.0f, 30.0f, 40.0f });
            
            Gui::Widget::CreateChild<Gui::Spacer>(grid);
            Gui::Widget::CreateChild<Gui::Spacer>(grid);
            Gui::Widget::CreateChild<Gui::Spacer>(grid);
            auto button1 = Gui::Widget::CreateChild<Gui::Button>(grid);
            //auto padding2 = Gui::Widget::CreateChild<Gui::Padding>(grid, 10.0f, 20.0f, 30.0f, 40.0f);
            //auto button2 = Gui::Widget::CreateChild<Gui::Button>(grid);
            //button2->margin = Gui::MarginData{ 10.0f, 20.0f, 30.0f, 40.0f };
            //auto button3 = Gui::Widget::CreateChild<Gui::Button>(grid);
            
            button1->onPress.Connect(
                [&](int)
            {
                m_logger.Write(Logger::Severity::Info, "Pressed button 1");
            });

            /*button2->onPress.Connect(
                [&](int)
            {
                m_logger.Write(Logger::Severity::Info, "Pressed button 2");
            });*/
            
        }

        void Application::LoadGridPipeline()
        {
            LoadGridShaders();

            PipelineDescriptor pipelineDesc;
            pipelineDesc.topology = Pipeline::Topology::LineList;
            pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
            pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;
            pipelineDesc.cullMode = Pipeline::CullMode::None;
            pipelineDesc.vertexScript = &m_gridVertexScript;
            pipelineDesc.fragmentScript = &m_gridFragmentScript;

            m_gridPipeline = m_renderer->CreatePipeline(pipelineDesc);
            if (!m_gridPipeline)
            {
                throw Exception("Failed to create render pipeline.");
            }


            struct Vertex
            {
                Vector3f32 position;
                Vector4f32 color;
            };

            std::vector<Vertex> vertexData =
            {
                {{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }}, {{ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
                {{ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }}, {{ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }},
                {{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }}, {{ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }}
            };
            for (int32_t i = -5; i <= 5; i++)
            {
                vertexData.push_back({ { static_cast<float>(i), -5.0f, -0.1f }, { 0.7f, 0.7f, 0.7f, 1.0f } });
                vertexData.push_back({ { static_cast<float>(i), 5.0f, -0.1f }, { 0.7f, 0.7f, 0.7f, 1.0f } });
                vertexData.push_back({ { -5.0f, static_cast<float>(i), -0.1f }, { 0.7f, 0.7f, 0.7f, 1.0f } });
                vertexData.push_back({ { 5.0f, static_cast<float>(i), -0.1f }, { 0.7f, 0.7f, 0.7f, 1.0f } });
            }

            std::vector<uint16_t> indices;
            for (size_t i = 0; i < vertexData.size(); i++)
            {
                indices.push_back(static_cast<uint16_t>(i));
            }

            VertexBufferDescriptor vertexPositionBufferDesc;
            vertexPositionBufferDesc.vertexCount = static_cast<uint32_t>(vertexData.size());
            vertexPositionBufferDesc.vertexSize = sizeof(Vertex);
            vertexPositionBufferDesc.data = static_cast<const void*>(&vertexData[0]);
            m_gridVertexBuffer = m_renderer->CreateVertexBuffer(vertexPositionBufferDesc);
            if (!m_gridVertexBuffer)
            {
                throw Exception("Failed to create position vertex buffer.");
            }

            IndexBufferDescriptor indexBufferDesc;
            indexBufferDesc.indexCount = static_cast<uint32_t>(indices.size());
            indexBufferDesc.data = static_cast<const void*>(&indices[0]);
            indexBufferDesc.dataType = IndexBuffer::DataType::Uint16;
            m_gridIndexBuffer = m_renderer->CreateIndexBuffer(indexBufferDesc);
            if (!m_gridIndexBuffer)
            {
                throw Exception("Failed to create index buffer.");
            }

            auto descriptorSetDescriptor1 = FramedDescriptorSetDescriptor{
                m_gridPipeline, 0,
                {{ 0,  m_viewMatrixBuffer }}
            };
            m_gridMatrixDescriptorSet = m_renderer->CreateFramedDescriptorSet(descriptorSetDescriptor1);
            if (!m_gridMatrixDescriptorSet)
            {
                throw Exception("Failed to create grid matrix descriptor set.");
            }

            m_gridColor1PushLocation = m_renderer->GetPushConstantLocation(m_gridPipeline, 0);
            m_gridColor2PushLocation = m_renderer->GetPushConstantLocation(m_gridPipeline, 1);
        }

        void Application::LoadGridShaders()
        {
            // Vertex script.
            {
                auto& script = m_gridVertexScript;

                auto& inputs = script.GetInputInterface();
                auto& inPos = inputs.AddMember<Vector3f32>();
                auto& inColor = inputs.AddMember<Vector4f32>();

                auto& pushConstants = script.GetPushConstants();
                //pushConstants.AddMember<Vector4f32>(12345);
                auto& pushPos = pushConstants.AddMember<Vector4f32>(0);

                auto& outputs = script.GetOutputInterface();
                auto& outColor = outputs.AddMember<Vector4f32>();
                auto* outPos = script.GetVertexOutput();

                auto& descSets = script.GetDescriptorSets();
                auto* descSet = descSets.AddSet(0);
                auto* ubo = descSet->AddBinding<Shader::Visual::VertexUniformBuffer>(0);
                
                auto& uProjView = ubo->AddPin<Matrix4x4f32>();
                auto& uModelView = ubo->AddPin<Matrix4x4f32>();

                auto& inPosVec4 = script.CreateFunction<Shader::Visual::Functions::Vec3ToVec4f32>();
                inPosVec4.GetInputPin(0)->Connect(inPos);
                static_cast<Shader::Visual::InputPin<float>*>(inPosVec4.GetInputPin(1))->SetDefaultValue(1.0f);

                auto& posMult = script.CreateOperator<Shader::Visual::Operators::MultVec4f32>();
                posMult.GetInputPin(0)->Connect(pushPos);
                posMult.GetInputPin(1)->Connect(*inPosVec4.GetOutputPin());

                auto& projModelmat = script.CreateOperator<Shader::Visual::Operators::MultMat4f32>();
                projModelmat.GetInputPin(0)->Connect(uProjView);
                projModelmat.GetInputPin(1)->Connect(uModelView);

                auto& finalPos = script.CreateOperator<Shader::Visual::Operators::MultMat4Vec4f32>();
                finalPos.GetInputPin(0)->Connect(*projModelmat.GetOutputPin());
                finalPos.GetInputPin(1)->Connect(*posMult.GetOutputPin());

                outPos->GetInputPin()->Connect(*finalPos.GetOutputPin());
                outColor.Connect(inColor);
            }
            // Fragment script
            {
                auto& script = m_gridFragmentScript;

                auto& inputs = script.GetInputInterface();
                auto& inColor = inputs.AddMember<Vector4f32>();

                auto& pushConstants = script.GetPushConstants();
                auto& pcColor = pushConstants.AddMember<Vector4f32>(0);
                auto& pcColor2 = pushConstants.AddMember<Vector4f32>(1);

                auto& outputs = script.GetOutputInterface();
                auto& outColor = outputs.AddMember<Vector4f32>();

                auto& mult = script.CreateOperator<Shader::Visual::Operators::MultVec4f32>();
                mult.GetInputPin(0)->Connect(inColor);
                mult.GetInputPin(1)->Connect(pcColor);

                auto& mult2 = script.CreateOperator<Shader::Visual::Operators::MultVec4f32>();
                mult2.GetInputPin(0)->Connect(*mult.GetOutputPin());
                mult2.GetInputPin(1)->Connect(pcColor2);


                //auto add = script.CreateOperatorNode<Shader::Operator::AddVec4f32>();
                //auto const1 = script.CreateConstantNode<Vector4f32>({ 0.0f, 0.0f, 0.3f, 0.0f });
                //auto const2 = script.CreateConstantNode<Vector4f32>({ 1.0f, 0.5f, 0.0f, 1.0f });
                //auto cos = script.CreateFunctionNode<Shader::Function::CosVec4f32>();

                outColor.Connect(*mult2.GetOutputPin());
                //add->GetInputPin(0)->Connect(*mult->GetOutputPin());
                //add->GetInputPin(1)->Connect(*const1->GetOutputPin());
                //mult->GetInputPin(0)->Connect(*inColor->GetOutputPin());
                //mult->GetInputPin(1)->Connect(*cos->GetOutputPin());
                //cos->GetInputPin()->Connect(*const2->GetOutputPin());
            }

            Shader::GlslGenerator::GlslTemplate glslTemplate;
            std::vector<Shader::Visual::Script*> visualScripts = { &m_gridVertexScript, &m_gridFragmentScript };
            if (!Shader::GlslGenerator::GenerateGlslTemplate(glslTemplate, visualScripts, &m_logger))
            {         
            }

            Shader::GlslGenerator glslGenerator;

            //stageTemplate.pushConstantTemplate.offsets = &glslTemplates.pushConstantTemplate.stageOffsets[0];
            auto vertGlsl = glslGenerator.Generate(m_gridVertexScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplate);

            //stageTemplate.pushConstantTemplate.offsets = &glslTemplates.pushConstantTemplate.stageOffsets[1];
            auto fragGlsl = glslGenerator.Generate(m_gridFragmentScript, Shader::GlslGenerator::Compability::SpirV);

            std::string vertStr(vertGlsl.begin(), vertGlsl.end());
            std::string fragStr(fragGlsl.begin(), fragGlsl.end());

            m_logger.Write(Logger::Severity::Info, "vert -------------------------------------");
            m_logger.Write(Logger::Severity::Info, vertStr);
            m_logger.Write(Logger::Severity::Info, "-------------------------------------");
            m_logger.Write(Logger::Severity::Info, "frag -------------------------------------");
            m_logger.Write(Logger::Severity::Info, fragStr);
            m_logger.Write(Logger::Severity::Info, "-------------------------------------");
        }

        void Application::LoadObjectPipeline()
        {
            LoadObjectShaders();

            PipelineDescriptor pipelineDesc;
            pipelineDesc.topology = Pipeline::Topology::TriangleList;
            pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
            pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;
            pipelineDesc.cullMode = Pipeline::CullMode::Back;
            pipelineDesc.vertexScript = &m_objectVertexScript;
            pipelineDesc.fragmentScript = &m_objectFragmentScript;

            m_objectPipeline = m_renderer->CreatePipeline(pipelineDesc);
            if (!m_objectPipeline)
            {
                throw Exception("Failed to create render pipeline.");
            }


            struct Vertex
            {
                Vector3f32 position;
                Vector2f32 uv;
            };

            std::vector<Vertex> vertexData =
            {
                {{ 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f}}, 
                {{ 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f}},
                {{ 1.0f, 0.0f, 1.0f }, {1.0f, 1.0f}},
                {{ 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}}
            };

            std::vector<uint16_t> indices = { 0, 1, 2, 0, 2, 3 };

            VertexBufferDescriptor vertexPositionBufferDesc;
            vertexPositionBufferDesc.vertexCount = static_cast<uint32_t>(vertexData.size());
            vertexPositionBufferDesc.vertexSize = sizeof(Vertex);
            vertexPositionBufferDesc.data = static_cast<const void*>(&vertexData[0]);
            m_objectVertexBuffer = m_renderer->CreateVertexBuffer(vertexPositionBufferDesc);
            if (!m_objectVertexBuffer)
            {
                throw Exception("Failed to create position vertex buffer.");
            }

            IndexBufferDescriptor indexBufferDesc;
            indexBufferDesc.indexCount = static_cast<uint32_t>(indices.size());
            indexBufferDesc.data = static_cast<const void*>(&indices[0]);
            indexBufferDesc.dataType = IndexBuffer::DataType::Uint16;
            m_objectIndexBuffer = m_renderer->CreateIndexBuffer(indexBufferDesc);
            if (!m_objectIndexBuffer)
            {
                throw Exception("Failed to create index buffer.");
            }

            auto descriptorSetDescriptor1 = FramedDescriptorSetDescriptor{
                m_objectPipeline, 10,
                {{ 1000000,  m_viewMatrixBuffer }}
            };
            m_objectMatrixDescriptorSet = m_renderer->CreateFramedDescriptorSet(descriptorSetDescriptor1);
            if (!m_objectMatrixDescriptorSet)
            {
                throw Exception("Failed to create object matrix descriptor set.");
            }

            m_objectPosPushLocation = m_renderer->GetPushConstantLocation(m_objectPipeline, 0);
            m_objectColorPushLocation = m_renderer->GetPushConstantLocation(m_objectPipeline, 1);

            const Vector2ui32 textureDimensions = { 2, 2 };
            uint8_t textureData[2 * 2 * 4] = {
                255, 0, 0, 255,     0, 255, 0, 255,
                0, 0, 255, 255,     255, 0, 255, 255,
            };

            TextureDescriptor textureDesc = {};
            textureDesc.dimensions = textureDimensions;
            textureDesc.data = textureData;
            m_objectTexture = m_renderer->CreateTexture(textureDesc);
            if (!m_objectTexture)
            {
                throw Exception("Failed to create texture.");
            }

            auto descriptorSetDescriptor2 = DescriptorSetDescriptor{
                m_objectPipeline, 40,
                {{ 100,  m_objectTexture }}
            };
            m_objectTextureDescriptorSet = m_renderer->CreateDescriptorSet(descriptorSetDescriptor2);
            if (!m_objectTextureDescriptorSet)
            {
                throw Exception("Failed to create object texture descriptor set.");
            }
        }

        void Application::LoadObjectShaders()
        {
            // Vertex script.
            {
                auto& script = m_objectVertexScript;

                auto& inputs = script.GetInputInterface();
                auto& inPos = inputs.AddMember<Vector3f32>();
                auto& inUv = inputs.AddMember<Vector2f32>();

                auto& pushConstants = script.GetPushConstants();
                auto& pushPos = pushConstants.AddMember<Vector4f32>(0);
                auto& pushColor = pushConstants.AddMember<Vector4f32>(1);

                auto& outputs = script.GetOutputInterface();
                auto& outColor = outputs.AddMember<Vector4f32>();
                auto& outUv = outputs.AddMember<Vector2f32>();
                auto* outPos = script.GetVertexOutput();

                auto& descSets = script.GetDescriptorSets();
                auto* descSet = descSets.AddSet(10);
                auto* ubo = descSet->AddBinding<Shader::Visual::VertexUniformBuffer>(1000000);

                auto& uProjView = ubo->AddPin<Matrix4x4f32>();
                auto& uModelView = ubo->AddPin<Matrix4x4f32>();

                auto& inPosVec4 = script.CreateFunction<Shader::Visual::Functions::Vec3ToVec4f32>();
                inPosVec4.GetInputPin(0)->Connect(inPos);
                static_cast<Shader::Visual::InputPin<float>*>(inPosVec4.GetInputPin(1))->SetDefaultValue(1.0f);

                auto& posAdd = script.CreateOperator<Shader::Visual::Operators::AddVec4f32>();
                posAdd.GetInputPin(0)->Connect(pushPos);
                posAdd.GetInputPin(1)->Connect(*inPosVec4.GetOutputPin());

                auto& projModelmat = script.CreateOperator<Shader::Visual::Operators::MultMat4f32>();
                projModelmat.GetInputPin(0)->Connect(uProjView);
                projModelmat.GetInputPin(1)->Connect(uModelView);

                auto& finalPos = script.CreateOperator<Shader::Visual::Operators::MultMat4Vec4f32>();
                finalPos.GetInputPin(0)->Connect(*projModelmat.GetOutputPin());
                finalPos.GetInputPin(1)->Connect(*posAdd.GetOutputPin());

                outPos->GetInputPin()->Connect(*finalPos.GetOutputPin());
                outColor.Connect(pushColor);

                outUv.Connect((inUv));
            }
            // Fragment script
            {
                auto& script = m_objectFragmentScript;

                auto& inputs = script.GetInputInterface();
                /*auto& inColor = */inputs.AddMember<Vector4f32>();
                auto& inUv = inputs.AddMember<Vector2f32>();

                auto& descSets = script.GetDescriptorSets();
                auto* descSet = descSets.AddSet(40);
                auto* sampler = descSet->AddBinding<Shader::Sampler2D>(100);


                auto& textureColor = script.CreateFunction<Shader::Visual::Functions::Texture2D>();
                textureColor.GetInputPin(0)->Connect(*sampler->GetOutputPin());
                textureColor.GetInputPin(1)->Connect(inUv);

                auto& outputs = script.GetOutputInterface();
                auto& outColor = outputs.AddMember<Vector4f32>();

                outColor.Connect(*textureColor.GetOutputPin());
            }

            Shader::GlslGenerator::GlslTemplate glslTemplate;
            std::vector<Shader::Visual::Script*> visualScripts = { &m_objectVertexScript, &m_objectFragmentScript };
            if (!Shader::GlslGenerator::GenerateGlslTemplate(glslTemplate, visualScripts, &m_logger))
            {
            }

            Shader::GlslGenerator glslGenerator;

            auto vertGlsl = glslGenerator.Generate(m_objectVertexScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplate);

            //stageTemplate.pushConstantTemplate.offsets = &glslTemplates.pushConstantTemplate.stageOffsets[1];
            auto fragGlsl = glslGenerator.Generate(m_objectFragmentScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplate);

            std::string vertStr(vertGlsl.begin(), vertGlsl.end());
            std::string fragStr(fragGlsl.begin(), fragGlsl.end());

            m_logger.Write(Logger::Severity::Info, "vert -------------------------------------");
            m_logger.Write(Logger::Severity::Info, vertStr);
            m_logger.Write(Logger::Severity::Info, "-------------------------------------");
            m_logger.Write(Logger::Severity::Info, "frag -------------------------------------");
            m_logger.Write(Logger::Severity::Info, fragStr);
            m_logger.Write(Logger::Severity::Info, "-------------------------------------");
        }

        void Application::Unload()
        {
            /*if (m_canvasRenderer)
            {
                m_canvasRenderer->Close();
            }*/

            if (m_renderer)
            {
                if (m_renderer->IsOpen())
                {
                    m_renderer->WaitForDevice();
                }

                if (m_viewMatrixBuffer)
                {
                    m_renderer->DestroyFramedUniformBuffer(m_viewMatrixBuffer);
                    m_viewMatrixBuffer = nullptr;
                }

                UnloadObjectPipeline();
                UnloadGridPipeline();

                m_renderer.reset();              
            }

            if (m_window)
            {
                m_window.reset();
            }
        }

        void Application::UnloadGridPipeline()
        {
            if(m_gridMatrixDescriptorSet)
            {
                m_renderer->DestroyFramedDescriptorSet(m_gridMatrixDescriptorSet);
                m_gridMatrixDescriptorSet = nullptr;
            }
            if (m_gridIndexBuffer)
            {
                m_renderer->DestroyIndexBuffer(m_gridIndexBuffer);
                m_gridIndexBuffer = nullptr;
            }
            if (m_gridVertexBuffer)
            {
                m_renderer->DestroyVertexBuffer(m_gridVertexBuffer);
                m_gridVertexBuffer = nullptr;
            }
            if (m_gridPipeline)
            {
                m_renderer->DestroyPipeline(m_gridPipeline);
                m_gridPipeline = nullptr;
            }
        }

        void Application::UnloadObjectPipeline()
        {
            if (m_objectTextureDescriptorSet)
            {
                m_renderer->DestroyDescriptorSet(m_objectTextureDescriptorSet);
                m_objectTextureDescriptorSet = nullptr;
            }
            if (m_objectTexture)
            {
                m_renderer->DestroyTexture(m_objectTexture);
                m_objectTexture = nullptr;
            }          
            if (m_objectMatrixDescriptorSet)
            {
                m_renderer->DestroyFramedDescriptorSet(m_objectMatrixDescriptorSet);
                m_objectMatrixDescriptorSet = nullptr;
            }
            if (m_objectIndexBuffer)
            {
                m_renderer->DestroyIndexBuffer(m_objectIndexBuffer);
                m_objectIndexBuffer = nullptr;
            }
            if (m_objectVertexBuffer)
            {
                m_renderer->DestroyVertexBuffer(m_objectVertexBuffer);
                m_objectVertexBuffer = nullptr;
            }
            if (m_objectPipeline)
            {
                m_renderer->DestroyPipeline(m_objectPipeline);
                m_objectPipeline = nullptr;
            }
        }

        void Application::Tick()
        {
            m_programTime = m_programTimer.GetTime().AsSeconds<float>();
            m_deltaTime = m_deltaTimer.GetTime().AsSeconds<float>();
            m_deltaTimer.Reset();

            if (!Update())
            {
                return;
            }
            Draw();
        }

        bool Application::Update()
        {
            constexpr float lookSpeed = 0.5f;
            constexpr float movementSpeed = 4.0f;
            static Vector2i32 lastPosition{ 0, 0 };

            m_window->Update();
            if (!m_window->IsOpen())
            {
                return false;
            }

            if(m_secondTimer.GetTime() > Seconds(1.0f))
            {

                m_window->SetTitle( "Molten Editor. FPS: " + std::to_string(static_cast<size_t>(1.0f / m_deltaTime)));
                m_secondTimer.Reset();
            }

            

            UserInput userInput = m_window->GetUserInput();
            UserInput::Event event;
            while (userInput.PollEvent(event))
            {
                switch (event.type)
                {
                case UserInput::Event::Type::MouseMove:
                {
                    if (Mouse::IsDown(Mouse::Button::Right))
                    {
                        auto posDiff = Vector2f32(event.mouseMoveEvent.position - lastPosition) * lookSpeed;
                        if (posDiff.x != 0)
                        {
                            m_camera.AddYaw(Degrees(-posDiff.x));
                        }
                        if (posDiff.y != 0)
                        {
                            m_camera.AddPitch(Degrees(-posDiff.y));
                        }
                    }
                    lastPosition = event.mouseMoveEvent.position;
                }
                break;
                case UserInput::Event::Type::KeyDown:
                {
                    switch (event.keyboardEvent.key)
                    {
                    case Keyboard::Key::A: m_camera.SetPosition(m_camera.GetPosition() - (m_camera.GetRightDirection() * movementSpeed * m_deltaTime)); break;
                    case Keyboard::Key::D: m_camera.SetPosition(m_camera.GetPosition() + (m_camera.GetRightDirection() * movementSpeed * m_deltaTime)); break;
                    case Keyboard::Key::W: m_camera.SetPosition(m_camera.GetPosition() + (m_camera.GetForwardDirection() * movementSpeed * m_deltaTime)); break;
                    case Keyboard::Key::S: m_camera.SetPosition(m_camera.GetPosition() - (m_camera.GetForwardDirection() * movementSpeed * m_deltaTime)); break;
                    case Keyboard::Key::Q: m_camera.SetPosition(m_camera.GetPosition() + (m_camera.GetUpDirection() * movementSpeed * m_deltaTime)); break;
                    case Keyboard::Key::E: m_camera.SetPosition(m_camera.GetPosition() - (m_camera.GetUpDirection() * movementSpeed * m_deltaTime)); break;
                    case Keyboard::Key::Up: m_camera.SetFieldOfView(m_camera.GetFieldOfView() - Degrees(40.0f * m_deltaTime)); break;
                    case Keyboard::Key::Down: m_camera.SetFieldOfView(m_camera.GetFieldOfView() + Degrees(10.0f * m_deltaTime)); break;
                    case Keyboard::Key::Left: m_camera.AddRoll(Degrees(-50.0f * m_deltaTime)); break;
                    case Keyboard::Key::Right: m_camera.AddRoll(Degrees(+50.0f * m_deltaTime)); break;
                    case Keyboard::Key::Escape: m_window->Close(); return false;
                    default: break;
                    }
                }
                break;
                case UserInput::Event::Type::KeyReleased:
                {
                    switch (event.keyboardEvent.key)
                    {
                    case Keyboard::Key::P:
                    {
                        m_camera.SetProjectionType(m_camera.GetProjectionType() == Scene::Camera::ProjectionType::Perspective ? 
                            Scene::Camera::ProjectionType::Orthographic : 
                            Scene::Camera::ProjectionType::Perspective);
                    }
                        break;
                    default: break;
                    }
                }
                break;
                default: break;
                }
            }

            m_camera.PostProcess();

            /*m_canvas->SetSize(m_window->GetSize());
            m_canvas->SetScale(m_window->GetScale());
            m_canvas->Update(Seconds(m_deltaTime));*/

            return true;
        }

        void Application::Draw()
        {
            auto windowSize = m_window->GetSize();
            if (!windowSize.x || !windowSize.y)
            {
                std::this_thread::sleep_for(std::chrono::duration<double>(0.01f));
                return;
            }

            //std::this_thread::sleep_for(std::chrono::duration<double>(0.2f));

            m_renderer->Resize(windowSize);

            m_renderer->BeginDraw();

            m_renderer->BindPipeline(m_gridPipeline);

            const auto projViewMatrix = m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix();

            struct UniformBuffer
            {
                Shader::PaddedType<Matrix4x4f32> projViewMatrix;
                Shader::PaddedType<Matrix4x4f32> modelMatrix;
            };

            UniformBuffer bufferData1;
            bufferData1.projViewMatrix = projViewMatrix;
            bufferData1.modelMatrix = Matrix4x4f32::Identity();
            //bufferData1.modelMatrix.Translate({ std::sin(m_programTime * 3.0f) * 0.25f, 0.0f, 0.2f });
            m_renderer->UpdateFramedUniformBuffer(m_viewMatrixBuffer, 0, sizeof(UniformBuffer), &bufferData1);

            // Grid
            m_renderer->BindFramedDescriptorSet(m_gridMatrixDescriptorSet);
            m_renderer->PushConstant(m_gridColor1PushLocation, { 0.4f, 0.8f, 0.7f, 1.0f });
            m_renderer->PushConstant(m_gridColor2PushLocation, { 0.5f, 0.5f, 0.5f, 1.0f });
            m_renderer->DrawVertexBuffer(m_gridIndexBuffer, m_gridVertexBuffer);

            // Object
            m_renderer->BindPipeline(m_objectPipeline);
            m_renderer->BindFramedDescriptorSet(m_objectMatrixDescriptorSet);
            m_renderer->BindDescriptorSet(m_objectTextureDescriptorSet);
            m_renderer->PushConstant(m_objectPosPushLocation, { 0.0f, 0.0f, 0.0f, 1.0f });
            m_renderer->PushConstant(m_objectColorPushLocation, { 1.0f, 1.0f, 1.0f, 1.0f });
            m_renderer->DrawVertexBuffer(m_objectIndexBuffer, m_objectVertexBuffer);

            //m_canvas->Draw();

            m_renderer->EndDraw();
        }


    }

}
