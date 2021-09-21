/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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


#include "Editor/Editor.hpp"
#include "Molten/Logger.hpp"

#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Gui/Layers/SingleRootLayer.hpp"
#include "Molten/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Gui/Widgets/PaddingWidget.hpp"
#include "Molten/Gui/Widgets/VerticalGridWidget.hpp"
//#include "Molten/Gui/Widgets/SpacerWidget.hpp"
#include "Molten/Gui/Widgets/PaneWidget.hpp"
#include "Molten/Gui/Widgets/DockerWidget.hpp"
#include "Molten/Gui/Widgets/LabelWidget.hpp"
#include "Molten/Gui/Widgets/ViewportWidget.hpp"


namespace Molten::Editor
{

    // Editor implementations.
    Editor::Editor(Semaphore& cancellationSemaphore) :
        m_isRunning(false),
        m_cancellationSemaphore(cancellationSemaphore),
        m_windowTitle("Molten Editor"),
        m_fpsTracker(8)
    {}

    Editor::~Editor()
    {
        if(m_thread.joinable())
        {
            m_thread.join();
        }
    }

    bool Editor::Open(const EditorDescriptor& descriptor)
    {
        m_logger = descriptor.logger;

        Semaphore startedSemaphore;

        m_thread = std::thread([&]()
        {
            m_isRunning = true;
            const EditorDescriptor descriptorCopy = descriptor;

            startedSemaphore.NotifyOne();

            if (!Load(descriptorCopy))
            {
                Exit();
                return;
            }

            Clock tickTimer;
            while(m_isRunning)
            {
                m_fpsLimiter.Reset();               

                m_deltaTime = tickTimer.GetTime();
                m_fpsTracker.RegisterSampleFrame(m_deltaTime);
                tickTimer.Reset();

                if (!Tick() || !m_isRunning)
                {
                    Exit();
                    return;
                }

                m_fpsLimiter.PrecisionSleep();                
            }
        });

        startedSemaphore.Wait();
        return true;
    }

    bool Editor::Load(const EditorDescriptor& descriptor)
    {
        if (!LoadWindow(descriptor) ||
            !LoadRenderer(descriptor) ||
            !LoadRenderPasses() ||
            !LoadGui())
        {
            return false;
        }

        const auto windowUnfocusedSleepTime = Seconds(
            1.0 / (descriptor.windowUnfocusedFpsLimit.has_value() ?
            static_cast<double>(descriptor.windowUnfocusedFpsLimit.value()) : 15.0)
        );
        m_unfocusedWindowFpsLimiter.SetSleepTime(windowUnfocusedSleepTime);

        if(descriptor.fpsLimit.has_value()) 
        {
            const auto fpsLimit = static_cast<double>(descriptor.fpsLimit.value()) + 0.25;
            m_fpsLimiter.SetSleepTime(Seconds(1.0 / fpsLimit));
        }

        m_window->Show();
        return true;
    }

    bool Editor::LoadWindow(const EditorDescriptor&)
    {
        WindowDescriptor windowDescriptor;
        windowDescriptor.size = { 800, 600 };
        windowDescriptor.title = m_windowTitle;
        windowDescriptor.enableDragAndDrop = true;
        windowDescriptor.logger = m_logger.get();

        m_window = Window::Create(windowDescriptor);
        if (!m_window || !m_window->IsOpen())
        {
            Logger::WriteError(m_logger.get(), "Failed to create editor window.");
            return false;
        }

        m_window->OnResize.Connect([&](Vector2ui32)
        {
            Tick();
        });

        m_window->OnDpiChange.Connect([&](Vector2ui32 dpi)
        {
            m_logger->Write(Logger::Severity::Info, "Changed DPI: " + std::to_string(dpi.x) + ", " + std::to_string(dpi.y));
        });

        m_window->OnScaleChange.Connect([&](Vector2f32 scale)
        {
            m_logger->Write(Logger::Severity::Info, "Changed scale: " + std::to_string(scale.x) + ", " + std::to_string(scale.y));
        });


        m_window->OnFilesDropEnter = [&](std::vector<std::filesystem::path>& files) -> bool
        {
            Logger::WriteInfo(m_logger.get(), "Drag and drop: Entering files:");
            for (const auto& file : files)
            {
                Logger::WriteInfo(m_logger.get(), "   " + file.string());
            }

            return true;
        };
        m_window->OnFilesDropMove = [&](const Vector2i32& position)
        {
            Logger::WriteInfo(m_logger.get(), "Drag and drop: Move: " + std::to_string(position.x) + ", " + std::to_string(position.y));
        };
        m_window->OnFilesDropLeave = [&]()
        {
            Logger::WriteInfo(m_logger.get(), "Drag and drop: Leave.");
        };
        m_window->OnFilesDrop = [&](std::vector<std::filesystem::path>& files)
        {
            Logger::WriteInfo(m_logger.get(), "Drag and drop: Dropping files:");
            for(const auto& file : files)
            {
                Logger::WriteInfo(m_logger.get(), "   " + file.string());
            }
        };


        return true;
    }

    bool Editor::LoadRenderer(const EditorDescriptor& descriptor)
    {
        const auto api = descriptor.backendRendererApi.has_value() ? 
            descriptor.backendRendererApi.value() : Renderer::BackendApi::Vulkan;

        m_renderer = std::unique_ptr<Renderer>(Renderer::Create(api));
        if (!m_renderer)
        {
            Logger::WriteError(m_logger.get(), "Failed to create renderer.");
            return false;
        }

        const auto version = descriptor.backendRendererApiVersion.has_value() ?
            descriptor.backendRendererApiVersion.value() : Version(1, 0);

        if (!m_renderer->Open(*m_window, version, m_logger.get()))
        {
            Logger::WriteError(m_logger.get(), "Failed to open renderer.");
            return false;
        }

        return true;
    }

    bool Editor::LoadRenderPasses()
    {
        // GUI
        auto renderPass = m_renderer->GetSwapChainRenderPass();
        renderPass->SetRecordFunction([&](auto& commandBuffer)
        {
            m_canvasRenderer->SetCommandBuffer(commandBuffer);
            m_canvas->Draw();
        });

        return true;
    }

    bool Editor::LoadGui()
    {
        m_fontNameRepository.AddSystemDirectories();

        m_canvasRenderer = Gui::CanvasRenderer::Create(*m_renderer, m_logger.get());
        m_canvas = std::make_shared<Gui::Canvas<Gui::EditorTheme>>(*m_canvasRenderer, m_fontNameRepository);

        auto* layer = m_canvas->CreateLayer<Gui::SingleRootLayer>(Gui::LayerPosition::Top);

        auto* docker = layer->CreateChild<Gui::Docker>();

        docker->margin = { 4.0f, 4.0f, 4.0f, 4.0f };

        docker->onCursorChange.Connect([&](Mouse::Cursor cursor)
        {
            m_window->SetCursor(cursor);
        });

        docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Left, false, "Tools", Gui::WidgetSize{ 200.0f, 200.0f });

        auto sceneViewport = docker->CreateChild<Gui::Viewport>(Gui::DockingPosition::Right, true);
        sceneViewport->onResize.Connect([&, viewport = sceneViewport](const auto size)
        {
            OnSceneViewportResize(viewport, size);
        });

        sceneViewport->onIsVisible.Connect([&]()
        {
            m_renderPasses.push_back(m_viewportRenderPass);
        });

        docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Bottom, false, "Assets", Gui::WidgetSize{ 250.0f, 250.0f });

        auto inspector = docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Right, false, "Inspector", Gui::WidgetSize{ 300.0f, 200.0f });
        auto vertGrid = inspector->CreateChild<Gui::VerticalGrid>();
        vertGrid->CreateChild<Gui::Label>("Location:", 18);
        auto button = vertGrid->CreateChild<Gui::Button>();
        button->onPress.Connect([&](int)
        {
            Logger::WriteInfo(m_logger.get(), "You pressed me!");
        });
        button->CreateChild<Gui::Label>("Click me!", 18);
    

        return true;
    }

    void Editor::Exit()
    {
        m_renderer->WaitForDevice();

        m_isRunning = false;
        m_cancellationSemaphore.NotifyAll();
    }

    bool Editor::Tick()
    {
        if(!UpdateWindow())
        {
            return false;
        }
        if(!HandleWindowFocus())
        {
            return true;
        }

        m_renderPasses.clear();

        UpdateCanvas();

        m_renderPasses.push_back(m_renderer->GetSwapChainRenderPass());

        if(!m_renderer->DrawFrame(m_renderPasses))
        {
            return false;
        }

        return true;
    }

    bool Editor::UpdateWindow()
    {
        if(m_windowTitleUpdateClock.GetTime() >= Seconds(1.0f))
        {
            const auto averageFps = static_cast<int64_t>(1.0 / m_fpsTracker.GetAverageFrameTime().AsSeconds<double>());
            const auto minFps = static_cast<int64_t>(1.0 / m_fpsTracker.GetMaxFrameTime().AsSeconds<double>());
            const auto maxFps = static_cast<int64_t>(1.0 / m_fpsTracker.GetMinFrameTime().AsSeconds<double>());

            m_window->SetTitle(
                m_windowTitle + " - avg FPS: " + std::to_string(averageFps) + 
                " | min FPS: " + std::to_string(minFps) +
                " | max FPS: " + std::to_string(maxFps));

            m_fpsTracker.ResetFrameSamples();

            m_windowTitleUpdateClock.Reset();
        }
       
        m_window->Update();
        if (!m_window->IsOpen())
        {
            return false;
        }
        return true;
    }

    bool Editor::HandleWindowFocus()
    {
        const auto windowSize = m_window->GetSize();
        const bool isMinimized = !windowSize.x || !windowSize.y;
        if(isMinimized || !m_window->IsFocused())
        {
            m_unfocusedWindowFpsLimiter.Sleep();
            m_unfocusedWindowFpsLimiter.Reset();
            return !isMinimized;
        }

        return true;
    }

    void Editor::UpdateCanvas()
    {
        auto& userInput = m_window->GetUserInput();

        UserInput::Event inputEvent;
        while (userInput.PollEvent(inputEvent))
        {
            m_canvas->PushUserInputEvent(inputEvent);
        }

        m_canvas->SetSize(m_window->GetSize());
        m_canvas->SetScale(m_window->GetScale());
        m_canvas->Update(m_deltaTime);
    }

    void Editor::OnSceneViewportResize(Gui::Viewport<Gui::EditorTheme>* viewport, const Vector2ui32 size)
    {
        TextureDescriptor2D colorTextureDesc = {};
        colorTextureDesc.dimensions = size;
        colorTextureDesc.type = TextureType::Color;
        colorTextureDesc.initialUsage = TextureUsage::Attachment;
        colorTextureDesc.format = ImageFormat::URed8Green8Blue8Alpha8;
        colorTextureDesc.internalFormat = ImageFormat::URed8Green8Blue8Alpha8;
        auto colorTexture = m_renderer->CreateFramedTexture(colorTextureDesc);

        if (!colorTexture)
        {
            return;
        }

        TextureDescriptor2D depthTextureDesc = {};
        depthTextureDesc.dimensions = size;
        depthTextureDesc.type = TextureType::DepthStencil;
        depthTextureDesc.initialUsage = TextureUsage::Attachment;
        depthTextureDesc.format = ImageFormat::SDepthFloat24StencilUint8;
        depthTextureDesc.internalFormat = ImageFormat::SDepthFloat24StencilUint8;
        auto depthTexture = m_renderer->CreateFramedTexture(depthTextureDesc);

        if (!depthTexture)
        {
            return;
        }

        if (!m_viewportRenderPass)
        {
            const RenderPassDescriptor renderPassDesc = {
                colorTexture->GetDimensions(),
                {
                    {
                        colorTexture,
                        Vector4f32{ 0.0f, 0.0f, 0.0f, 0.0f },
                        TextureType::Color,
                        TextureUsage::Attachment,
                        TextureType::Color,
                        TextureUsage::ReadOnly
                    },
                    {
                        depthTexture,
                        Vector4f32{ 1.0f },
                        TextureType::DepthStencil,
                        TextureUsage::Attachment
                    }
                }
            };

            m_viewportRenderPass = m_renderer->CreateRenderPass(renderPassDesc);
            if (!m_viewportRenderPass)
            {
                Exit();
                return;
            }

            m_viewportRenderPass->SetRecordFunction([&](CommandBuffer& commandBuffer)
            {
                DrawSceneViewport(commandBuffer);
            });

            if(!LoadSceneViewport())
            {
                Exit();
                return;
            }
        }
        else
        {
            const RenderPassUpdateDescriptor renderPassUpdateDesc = {
                colorTexture->GetDimensions(),
                {
                    {
                        colorTexture,
                        TextureType::Color,
                        TextureUsage::Attachment,
                        TextureType::Color,
                        TextureUsage::ReadOnly
                    },
                    {
                        depthTexture,
                        TextureType::DepthStencil,
                        TextureUsage::Attachment
                    }
                }
            };

            if(!m_renderer->UpdateRenderPass(*m_viewportRenderPass, renderPassUpdateDesc))
            {
                Exit();
                return;
            }
        }

        viewport->SetTexture(std::move(colorTexture));
    }

    bool Editor::LoadSceneViewport()
    {
        struct Vertex
        {
            Vector3f32 position;
            Vector4f32 color;
        };

        const std::array<Vertex, 8> vertexData =
        {
            Vertex{ { -10.0f, 10.0f, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f} },
            Vertex{ { 10.0f, 10.0f, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f} },
            Vertex{ { 10.0f, -10.0f, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f} },
            Vertex{ { -10.0f, -10.0f, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f} },

            Vertex{ { -15.0f, 10.0f, -20.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
            Vertex{ { 5.0f, 10.0f, -20.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
            Vertex{ { 5.0f, -10.0f, -20.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
            Vertex{ { -15.0f, -10.0f, -20.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
        };

        const std::array<uint16_t, 12> indices =
        {
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7
        };

        VertexBufferDescriptor vertexPositionBufferDesc;
        vertexPositionBufferDesc.vertexCount = static_cast<uint32_t>(vertexData.size());
        vertexPositionBufferDesc.vertexSize = sizeof(Vertex);
        vertexPositionBufferDesc.data = static_cast<const void*>(vertexData.data());
        m_viewportSceneData.vertexBuffer = m_renderer->CreateVertexBuffer(vertexPositionBufferDesc);
        if (!m_viewportSceneData.vertexBuffer)
        {
            return false;
        }

        IndexBufferDescriptor indexBufferDesc;
        indexBufferDesc.indexCount = static_cast<uint32_t>(indices.size());
        indexBufferDesc.data = static_cast<const void*>(indices.data());
        indexBufferDesc.dataType = IndexBuffer::DataType::Uint16;
        m_viewportSceneData.indexBuffer = m_renderer->CreateIndexBuffer(indexBufferDesc);
        if (!m_viewportSceneData.indexBuffer)
        {
            return false;
        }

        Shader::Visual::VertexScript vertexScript;
        Shader::Visual::FragmentScript fragmentScript;

        { // Vertex
            auto& script = vertexScript;

            auto& inputs = script.GetInputInterface();
            auto& vertexPosition = inputs.AddMember<Vector3f32>();
            auto& vertexColor = inputs.AddMember<Vector4f32>();

            auto& pushConstants = script.GetPushConstants();
            auto& projection = pushConstants.AddMember<Matrix4x4f32>(0);

            auto& positionComposit = script.CreateComposite<Shader::Visual::Composites::Vec4f32FromVec3f32Float32>();
            positionComposit.GetInput<0>().Connect(vertexPosition);
            positionComposit.GetInput<1>().SetDefaultValue(1.0f);

            auto& projectedVertexPosition = script.CreateOperator<Shader::Visual::Operators::MultMat4f32Vec4f32>();
            projectedVertexPosition.GetLeftInput().Connect(projection);
            projectedVertexPosition.GetRightInput().Connect(positionComposit.GetOutput());

            auto* outPosition = script.GetVertexOutput();
            outPosition->GetInputPin()->ConnectBase(*projectedVertexPosition.GetOutputPin());

            auto& outputs = script.GetOutputInterface();
            auto& outColor = outputs.AddMember<Vector4f32>();
            outColor.Connect(vertexColor);
        }
        { // Fragment
            auto& script = fragmentScript;

            auto& inputs = script.GetInputInterface();
            auto& vertexColor = inputs.AddMember<Vector4f32>();

            auto& outputs = script.GetOutputInterface();
            auto& outColor = outputs.AddMember<Vector4f32>();

            outColor.Connect(vertexColor);
        }

        VisualShaderProgramDescriptor shaderProgramDesc;
        shaderProgramDesc.vertexScript = &vertexScript;
        shaderProgramDesc.fragmentScript = &fragmentScript;
        auto shaderProgram = m_renderer->CreateShaderProgram(shaderProgramDesc);
        if (!shaderProgram)
        {
            return false;
        }

        PipelineDescriptor pipelineDesc;
        pipelineDesc.cullMode = Pipeline::CullMode::None;
        pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
        pipelineDesc.topology = Pipeline::Topology::TriangleList;
        pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;
        pipelineDesc.renderPass = m_viewportRenderPass;
        pipelineDesc.shaderProgram = shaderProgram;
        m_viewportSceneData.pipeline = m_renderer->CreatePipeline(pipelineDesc);
        if (!m_viewportSceneData.pipeline)
        {
            return false;
        }

        m_viewportSceneData.projectionLocation = m_renderer->GetPushConstantLocation(*m_viewportSceneData.pipeline, 0);

        return true;
    }

    void Editor::DrawSceneViewport(CommandBuffer& commandBuffer)
    {
        // Very temporary scene viewport rendering.
        static float totalDelta = 0.0f;
        totalDelta += m_deltaTime.AsSeconds<float>();
        const auto lookAtX = std::sin(totalDelta * 4.0f) * 20.0f;

        const auto projectionMatrix = Matrix4x4f32::Perspective(Degrees(60), 1.0f, 0.1f, 100.0f);
        const auto viewMatrix = Matrix4x4f32::LookAtPoint({ 0.0f, 0.0f, 60.0f }, { lookAtX, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        const auto finalProjecton = projectionMatrix * viewMatrix;

        commandBuffer.BindPipeline(*m_viewportSceneData.pipeline);
        commandBuffer.PushConstant(m_viewportSceneData.projectionLocation, finalProjecton);
        commandBuffer.DrawVertexBuffer(*m_viewportSceneData.indexBuffer, *m_viewportSceneData.vertexBuffer);
    }

}
