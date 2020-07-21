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
#include "Curse/Gui/Behaviors/KeyboardListenerWidget.hpp"
#include "Curse/Gui/Behaviors/MouseListenerWidget.hpp"
#include "Curse/Gui/WidgetTemplates.hpp"
#include <chrono>
#include <thread>

#include "Curse/Renderer/Shader/Generator/VulkanShaderGenerator.hpp"

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
            m_deltaTime(0.0f),
            m_guiCanvas()
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

            m_camera.SetPosition({ 0.0f, -3.0f, 0.0f });
            m_camera.SetDirection({ 0.3f, 1.0f, 0.0f });
            m_camera.SetProjectionType(Scene::Camera::ProjectionType::Perspective);
            m_camera.SetFieldOfView(Degrees(60));
            m_camera.SetWindowSize(m_window->GetSize());
            m_window->OnResize.Connect([&](Vector2ui32 size)
            {
                m_camera.SetWindowSize(size);
            });

            LoadGui();
        }


        void Application::LoadGui()
        {
            m_guiCanvas.Load(m_renderer.get(), &m_logger);

            auto grid = m_guiCanvas.Add<Gui::VerticalGrid>(m_guiCanvas.GetRoot());
            auto button1 = m_guiCanvas.Add<Gui::Button, Gui::MouseListener>(grid);
            auto padding = m_guiCanvas.Add<Gui::Padding, Gui::MouseListener>(grid, 10.0f, 20.0f, 30.0f, 40.0f);
            auto button2 = m_guiCanvas.Add<Gui::Button, Gui::MouseListener>(padding);

            button1->GetComponent<Gui::MouseListener>()->OnClick = 
                [&](Mouse::Button button, const Curse::Vector2f32& pos)
            {
                m_logger.Write(Logger::Severity::Info, "Pressed button 1 at: " + std::to_string(pos.x) + ", " + std::to_string(pos.y));
            };

            button2->GetComponent<Gui::MouseListener>()->OnClick =
                [&](Mouse::Button button, const Curse::Vector2f32& pos)
            {
                m_logger.Write(Logger::Severity::Info, "Pressed button 2 at: " + std::to_string(pos.x) + ", " + std::to_string(pos.y));
            };

        }

        void Application::LoadPipeline()
        {
            LoadShaders();

            Curse::PipelineDescriptor pipelineDesc;
            pipelineDesc.topology = Curse::Pipeline::Topology::LineList;
            pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Fill;
            pipelineDesc.frontFace = Curse::Pipeline::FrontFace::Clockwise;
            pipelineDesc.cullMode = Curse::Pipeline::CullMode::None;
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

            Curse::VertexBufferDescriptor vertexPositionBufferDesc;
            vertexPositionBufferDesc.vertexCount = static_cast<uint32_t>(vertexData.size());
            vertexPositionBufferDesc.vertexSize = sizeof(Vertex);
            vertexPositionBufferDesc.data = static_cast<const void*>(&vertexData[0]);
            m_vertexBuffer = m_renderer->CreateVertexBuffer(vertexPositionBufferDesc);
            if (!m_vertexBuffer)
            {
                throw Exception("Failed to create position vertex buffer.");
            }

            Curse::IndexBufferDescriptor indexBufferDesc;
            indexBufferDesc.indexCount = static_cast<uint32_t>(indices.size());
            indexBufferDesc.data = static_cast<const void*>(&indices[0]);
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
                throw Exception("Failed to create uniform buffer.");
            }

            Curse::UniformBlockDescriptor uniformBlockDesc;
            uniformBlockDesc.id = 0;
            uniformBlockDesc.buffer = m_uniformBuffer;
            uniformBlockDesc.pipeline = m_pipeline;
            m_uniformBlock = m_renderer->CreateUniformBlock(uniformBlockDesc);
            if (!m_uniformBlock)
            {
                throw Exception("Failed to create uniform block.");
            }
        }

        void Application::LoadShaders()
        {
            // Vertex script.
            {
                auto& script = m_vertexScript;

                auto& inputs = script.GetInputInterface();
                auto inPos = inputs.AddMember<Vector3f32>();
                auto inColor = inputs.AddMember<Vector4f32>();

                auto& outputs = script.GetOutputInterface();
                auto outColor = outputs.AddMember<Vector4f32>();
                auto outPos = script.GetVertexOutputVariable();

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

                auto& inputs = script.GetInputInterface();
                auto inColor = inputs.AddMember<Vector4f32>();

                auto& outputs = script.GetOutputInterface();
                auto outColor = outputs.AddMember<Vector4f32>();

                /*auto mult = script.CreateOperatorNode<Shader::Operator::MultVec4f32>();
                auto add = script.CreateOperatorNode<Shader::Operator::AddVec4f32>();
                auto const1 = script.CreateConstantNode<Vector4f32>({ 0.0f, 0.0f, 0.3f, 0.0f });
                auto const2 = script.CreateConstantNode<Vector4f32>({ 1.0f, 0.5f, 0.0f, 1.0f });
                auto cos = script.CreateFunctionNode<Shader::Function::CosVec4f32>();*/

                outColor->GetInputPin()->Connect(*inColor->GetOutputPin());
                /*add->GetInputPin(0)->Connect(*mult->GetOutputPin());
                add->GetInputPin(1)->Connect(*const1->GetOutputPin());
                mult->GetInputPin(0)->Connect(*inColor->GetOutputPin());
                mult->GetInputPin(1)->Connect(*cos->GetOutputPin());
                cos->GetInputPin()->Connect(*const2->GetOutputPin());*/
            }

            auto vertGlsl = Shader::VulkanGenerator::GenerateGlsl(m_vertexScript);
            auto fragGlsl = Shader::VulkanGenerator::GenerateGlsl(m_fragmentScript);

            std::string vertStr(vertGlsl.begin(), vertGlsl.end());
            std::string fragStr(fragGlsl.begin(), fragGlsl.end());

            m_logger.Write(Logger::Severity::Info, "vert -------------------------------------");
            m_logger.Write(Logger::Severity::Info, vertStr);
            m_logger.Write(Logger::Severity::Info, "-------------------------------------");
            m_logger.Write(Logger::Severity::Info, "frag -------------------------------------");
            m_logger.Write(Logger::Severity::Info, fragStr);
            m_logger.Write(Logger::Severity::Info, "-------------------------------------");


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
            m_guiCanvas.Unload();

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
            constexpr float cameraSpeed = 4.0f;
            static Vector2i32 lastPosition{ 0, 0 };

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
                case Curse::UserInput::Event::Type::MouseMove:
                {
                    if (Mouse::IsDown(Mouse::Button::Right))
                    {
                        auto posDiff = event.mouseMoveEvent.position - lastPosition;
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
                case Curse::UserInput::Event::Type::KeyDown:
                {
                    switch (event.keyboardEvent.key)
                    {
                    case Curse::Keyboard::Key::A: m_camera.SetPosition(m_camera.GetPosition() - (m_camera.GetRightDirection() * cameraSpeed * m_deltaTime)); break;
                    case Curse::Keyboard::Key::D: m_camera.SetPosition(m_camera.GetPosition() + (m_camera.GetRightDirection() * cameraSpeed * m_deltaTime)); break;
                    case Curse::Keyboard::Key::W: m_camera.SetPosition(m_camera.GetPosition() + (m_camera.GetForwardDirection() * cameraSpeed * m_deltaTime)); break;
                    case Curse::Keyboard::Key::S: m_camera.SetPosition(m_camera.GetPosition() - (m_camera.GetForwardDirection() * cameraSpeed * m_deltaTime)); break;
                    case Curse::Keyboard::Key::Q: m_camera.SetPosition(m_camera.GetPosition() + (m_camera.GetUpDirection() * cameraSpeed * m_deltaTime)); break;
                    case Curse::Keyboard::Key::E: m_camera.SetPosition(m_camera.GetPosition() - (m_camera.GetUpDirection() * cameraSpeed * m_deltaTime)); break;
                    case Curse::Keyboard::Key::Up: m_camera.SetFieldOfView(m_camera.GetFieldOfView() - Degrees(40.0f * m_deltaTime)); break;
                    case Curse::Keyboard::Key::Down: m_camera.SetFieldOfView(m_camera.GetFieldOfView() + Degrees(10.0f * m_deltaTime)); break;
                    case Curse::Keyboard::Key::Left: m_camera.AddRoll(Degrees(-50.0f * m_deltaTime)); break;
                    case Curse::Keyboard::Key::Right: m_camera.AddRoll(Degrees(+50.0f * m_deltaTime)); break;
                    case Curse::Keyboard::Key::Escape: m_window->Close(); return false;
                    default: break;
                    }
                }
                break;
                case Curse::UserInput::Event::Type::KeyReleased:
                {
                    switch (event.keyboardEvent.key)
                    {
                    case Curse::Keyboard::Key::P:
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

            m_guiCanvas.Update();

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

            const auto projViewMatrix = m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix();

            struct UniformBuffer
            {
                Shader::PaddedType<Curse::Matrix4x4f32> projViewMatrix;
                Shader::PaddedType<Curse::Matrix4x4f32> modelMatrix;
            };

            UniformBuffer bufferData1;
            bufferData1.projViewMatrix = projViewMatrix;
            bufferData1.modelMatrix = Curse::Matrix4x4f32::Identity();
            //bufferData1.modelMatrix.Translate({ std::sin(m_programTime * 3.0f) * 0.25f, 0.0f, 0.2f });

            /*UniformBuffer bufferData2;
            bufferData2.projViewMatrix = projViewMatrix;
            bufferData2.modelMatrix = Curse::Matrix4x4f32::Identity();
            bufferData2.modelMatrix.Translate({ 0.0f, std::cos(m_programTime * 3.0f) * 0.25f, 0.0f });*/

            m_renderer->UpdateUniformBuffer(m_uniformBuffer, 0, sizeof(UniformBuffer), &bufferData1);
            //m_renderer->UpdateUniformBuffer(m_uniformBuffer, 256, sizeof(UniformBuffer), &bufferData2);

            m_renderer->BindUniformBlock(m_uniformBlock, 0);
            m_renderer->DrawVertexBuffer(m_indexBuffer, m_vertexBuffer);

            /*m_renderer->BindUniformBlock(m_uniformBlock, 256);
            m_renderer->DrawVertexBuffer(m_indexBuffer, m_vertexBuffer);*/

            m_guiCanvas.Draw();

            m_renderer->EndDraw();
        }


    }

}
