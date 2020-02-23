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
#include "Curse/Window/Window.hpp"
#include "Curse/Renderer/Renderer.hpp"
#include <chrono>
#include <thread>

namespace Curse
{

    namespace Editor
    {

        Application::Application() :
            m_pipeline(nullptr),
            m_vertexStage(nullptr),
            m_fragmentStage(nullptr),
            m_vertexBuffer(nullptr),
            m_indexBuffer(nullptr),
            m_uniformBuffer(nullptr),
            m_uniformBlock(nullptr),
            m_programTime(0.0f),
            m_deltaTime(0.0f) ,
            m_cameraPosition{ 0.0f, 0.0f, 3.0f }
        { }

        Application::~Application()
        {
            Unload();
        }

        int Application::Start(int argv, char** argc)
        {
            try
            {
                Load();
            }
            catch (Exception & e)
            {
                PlatformWindow::Message(PlatformWindow::Type::Error, "Curse Engine loading error.", e.what());
                return -1;
            }          

            m_window->Show();
            m_deltaTimer.Reset();
            while (m_window->IsOpen())
            {
                Tick();
            }

            return 0;
        }

        void Application::Load()
        {
            m_window = std::unique_ptr<Window>(Curse::Window::Create());
            Curse::Vector2ui32 windowSize = { 800, 600 };
            std::string windowTitle = "Curse Editor";
            if (!m_window)
            {
                throw Exception("Failed to create editor window.");
            }
            if (!m_window->Open(windowTitle, windowSize, &m_logger))
            {
                throw Exception("Failed to open editor window.");
            }

            auto resizeCallback = [&](Vector2ui32)
            {
                Tick();
            };

            m_window->OnMaximize.Connect(resizeCallback);
            m_window->OnMinimize.Connect(resizeCallback);
            m_window->OnResize.Connect(resizeCallback);

            m_renderer = std::unique_ptr<Renderer>(Curse::Renderer::Create(Curse::Renderer::BackendApi::Vulkan));
            if (!m_renderer)
            {
                throw Exception("Failed to create Vulkan renderer.");
            }

            if (!m_renderer->Open(*m_window, Curse::Version(1, 1), &m_logger))
            {
                throw Exception("Failed to open Vulkan renderer.");
            }

            LoadPipeline();
        }

        void Application::LoadPipeline()
        {
            LoadShaders();

            Curse::PipelineDescriptor pipelineDesc;
            pipelineDesc.topology = Curse::Pipeline::Topology::TriangleList;
            pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Fill;
            pipelineDesc.frontFace = Curse::Pipeline::FrontFace::Clockwise;
            pipelineDesc.cullMode = Curse::Pipeline::CullMode::Back;
            pipelineDesc.vertexStage = m_vertexStage;
            pipelineDesc.fragmentStage = m_fragmentStage;

            m_pipeline = m_renderer->CreatePipeline(pipelineDesc);
            if (!m_pipeline)
            {
                throw Exception("Failed to create render pipeline.");
            }


            struct Vertex
            {
                Curse::Vector3f32 position;
                Curse::Vector4f32 color;
            };

            static const uint32_t vertexCount = 4;
            static const Vertex vertices[vertexCount] =
            {
                { { -0.5f, -0.5, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
                { { 0.5f, -0.5, 0.0f },  { 0.0f, 1.0f, 0.0f, 1.0f } },
                { { 0.5f, 0.5, 0.0f },   { 0.0f, 0.0f, 1.0f, 1.0f } },
                { { -0.5f, 0.5, 0.0f },  { 1.0f, 0.0f, 1.0f, 1.0f } }
            };

            static const uint32_t indexCount = 6;
            static const uint16_t indices[indexCount] =
            {
                0, 1, 2, 0, 2, 3
            };

            Curse::VertexBufferDescriptor vertexBufferDesc;
            vertexBufferDesc.vertexCount = vertexCount;
            vertexBufferDesc.vertexSize = sizeof(Vertex);
            vertexBufferDesc.data = static_cast<const void*>(&vertices[0]);
            m_vertexBuffer = m_renderer->CreateVertexBuffer(vertexBufferDesc);
            if (!m_vertexBuffer)
            {
                throw Exception("Failed to create vertex buffer.");
            }

            Curse::IndexBufferDescriptor indexBufferDesc;
            indexBufferDesc.indexCount = indexCount;
            indexBufferDesc.data = static_cast<const void*>(indices);
            indexBufferDesc.dataType = Curse::IndexBuffer::DataType::Uint16;
            m_indexBuffer = m_renderer->CreateIndexBuffer(indexBufferDesc);
            if (!m_indexBuffer)
            {
                throw Exception("Failed to create index buffer.");
            }

            Curse::UniformBufferDescriptor uniformBufferDesc;
            uniformBufferDesc.size = 512;
            m_uniformBuffer = m_renderer->CreateUniformBuffer(uniformBufferDesc);
            if (!m_uniformBuffer)
            {
                throw Exception("Failed to uniform buffer.");
            }

            Curse::UniformBlockDescriptor uniformBlockDesc;
            uniformBlockDesc.id = 0;
            uniformBlockDesc.buffer = m_uniformBuffer;
            uniformBlockDesc.pipeline = m_pipeline;
            m_uniformBlock = m_renderer->CreateUniformBlock(uniformBlockDesc);
            if (!m_uniformBlock)
            {
                throw Exception("Failed to uniform block.");
            }
        }

        void Application::LoadShaders()
        {
            // Vertex script.
            {
                auto& script = m_vertexScript;

                auto& inBlock = script.GetInputBlock();
                auto inPos = inBlock.AppendNode<Vector3f32>();
                auto inColor = inBlock.AppendNode<Vector4f32>();

                auto& outBlock = script.GetOutputBlock();
                auto outColor = outBlock.AppendNode<Vector4f32>();
                auto outPos = script.GetVertexOutputNode();

                auto uBlock0 = script.CreateUniformBlock(0);
                auto uProjView = uBlock0->AppendNode<Matrix4x4f32>();
                auto uModel = uBlock0->AppendNode<Matrix4x4f32>();

                auto inPosVec4 = script.CreateFunctionNode<Shader::Function::Vec3ToVec4f32>();
                inPosVec4->GetInputPin(0)->Connect(*inPos->GetOutputPin());
                static_cast<Shader::InputPin<float>*>(inPosVec4->GetInputPin(1))->SetDefaultValue(1.0f);

                auto projModelmat = script.CreateOperatorNode<Shader::Operator::MultMat4f32>();
                projModelmat->GetInputPin(0)->Connect(*uProjView->GetOutputPin());
                projModelmat->GetInputPin(1)->Connect(*uModel->GetOutputPin());

                auto finalPos = script.CreateOperatorNode<Shader::Operator::MultMat4Vec4f32>();
                finalPos->GetInputPin(0)->Connect(*projModelmat->GetOutputPin());
                finalPos->GetInputPin(1)->Connect(*inPosVec4->GetOutputPin());

                outPos->GetInputPin()->Connect(*finalPos->GetOutputPin());
                outColor->GetInputPin()->Connect(*inColor->GetOutputPin());
            }
            // Fragment script
            {
                auto& script = m_fragmentScript;

                auto& inBlock = script.GetInputBlock();
                auto inColor = inBlock.AppendNode<Vector4f32>();

                auto& outBlock = script.GetOutputBlock();
                auto outColor = outBlock.AppendNode<Vector4f32>();

                auto mult = script.CreateOperatorNode<Shader::Operator::MultVec4f32>();
                auto add = script.CreateOperatorNode<Shader::Operator::AddVec4f32>();
                auto const1 = script.CreateConstantNode<Vector4f32>({ 0.0f, 0.0f, 0.3f, 0.0f });
                auto const2 = script.CreateConstantNode<Vector4f32>({ 1.0f, 0.5f, 0.0f, 1.0f });
                auto cos = script.CreateFunctionNode<Shader::Function::CosVec4f32>();

                outColor->GetInputPin()->Connect(*add->GetOutputPin());
                add->GetInputPin(0)->Connect(*mult->GetOutputPin());
                add->GetInputPin(1)->Connect(*const1->GetOutputPin());
                mult->GetInputPin(0)->Connect(*inColor->GetOutputPin());
                mult->GetInputPin(1)->Connect(*cos->GetOutputPin());
                cos->GetInputPin()->Connect(*const2->GetOutputPin());
            }

            m_vertexStage = m_renderer->CreateVertexShaderStage(m_vertexScript);
            if (!m_vertexStage)
            {
                throw Exception("Failed to create vertex shader stage.");
            }
            m_fragmentStage = m_renderer->CreateFragmentShaderStage(m_fragmentScript);
            if (!m_fragmentStage)
            {
                throw Exception("Failed to create fragment shader stage.");
            }
        }

        void Application::Unload()
        {
            if (m_renderer)
            {
                m_renderer->WaitForDevice();

                if (m_uniformBlock)
                {
                    m_renderer->DestroyUniformBlock(m_uniformBlock);
                    m_uniformBlock = nullptr;
                }
                if (m_uniformBuffer)
                {
                    m_renderer->DestroyUniformBuffer(m_uniformBuffer);
                    m_uniformBuffer = nullptr;
                }
                if (m_indexBuffer)
                {
                    m_renderer->DestroyIndexBuffer(m_indexBuffer);
                    m_indexBuffer = nullptr;
                }
                if (m_vertexBuffer)
                {
                    m_renderer->DestroyVertexBuffer(m_vertexBuffer);
                    m_vertexBuffer = nullptr;
                }
                if (m_pipeline)
                {
                    m_renderer->DestroyPipeline(m_pipeline);
                    m_pipeline = nullptr;
                }
                if (m_fragmentStage)
                {
                    m_renderer->DestroyFragmentShaderStage(m_fragmentStage);
                    m_fragmentStage = nullptr;
                }
                if (m_vertexStage)
                {
                    m_renderer->DestroyVertexShaderStage(m_vertexStage);
                    m_vertexStage = nullptr;
                }

                m_renderer.reset();              
            }

            if (m_window)
            {
                m_window.reset();
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
            constexpr float cameraSpeed = 1.0f;

            m_window->Update();
            if (!m_window->IsOpen())
            {
                return false;
            }

            Curse::UserInput userInput = m_window->GetUserInput();
            Curse::UserInput::Event event;
            while (userInput.PollEvent(event))
            {
                switch (event.type)
                {
                case Curse::UserInput::Event::Type::KeyDown:
                {
                    switch (event.keyboardEvent.key)
                    {
                    case Curse::Keyboard::Key::A: m_cameraPosition.x -= cameraSpeed * m_deltaTime; break;
                    case Curse::Keyboard::Key::D: m_cameraPosition.x += cameraSpeed * m_deltaTime; break;
                    case Curse::Keyboard::Key::W: m_cameraPosition.y -= cameraSpeed * m_deltaTime; break;
                    case Curse::Keyboard::Key::S: m_cameraPosition.y += cameraSpeed * m_deltaTime; break;
                    case Curse::Keyboard::Key::Q: m_cameraPosition.z -= cameraSpeed * m_deltaTime; break;
                    case Curse::Keyboard::Key::E: m_cameraPosition.z += cameraSpeed * m_deltaTime; break;
                    default: break;
                    }
                }
                break;
                default: break;
                }
            }

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

            m_renderer->Resize(windowSize);

            m_renderer->BeginDraw();

            m_renderer->BindPipeline(m_pipeline);

            float fov = static_cast<float>(m_window->GetSize().x) / static_cast<float>(m_window->GetSize().y);
            auto projViewMatrix = Curse::Matrix4x4f32::Perspective(60.0f, fov, 0.1f, 100.0f) *
                Curse::Matrix4x4f32::LookAtPoint(m_cameraPosition, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });

            struct UniformBuffer
            {
                Shader::PaddedType<Curse::Matrix4x4f32> projViewMatrix;
                Shader::PaddedType<Curse::Matrix4x4f32> modelMatrix;
            };

            UniformBuffer bufferData1;
            bufferData1.projViewMatrix = projViewMatrix;
            bufferData1.modelMatrix = Curse::Matrix4x4f32::Identity();
            bufferData1.modelMatrix.Translate({ std::sin(m_programTime * 3.0f) * 0.25f, 0.0f, 0.2f });

            UniformBuffer bufferData2;
            bufferData2.projViewMatrix = projViewMatrix;
            bufferData2.modelMatrix = Curse::Matrix4x4f32::Identity();
            bufferData2.modelMatrix.Translate({ 0.0f, std::cos(m_programTime * 3.0f) * 0.25f, 0.0f });

            m_renderer->UpdateUniformBuffer(m_uniformBuffer, 0, sizeof(UniformBuffer), &bufferData1);
            m_renderer->UpdateUniformBuffer(m_uniformBuffer, 256, sizeof(UniformBuffer), &bufferData2);

            m_renderer->BindUniformBlock(m_uniformBlock, 0);
            m_renderer->DrawVertexBuffer(m_indexBuffer, m_vertexBuffer);

            m_renderer->BindUniformBlock(m_uniformBlock, 256);
            m_renderer->DrawVertexBuffer(m_indexBuffer, m_vertexBuffer);

            m_renderer->EndDraw();
        }


    }

}
