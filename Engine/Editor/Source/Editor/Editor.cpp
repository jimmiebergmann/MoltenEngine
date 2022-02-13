/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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
#include "Molten/Gui/Widgets/GridWidget.hpp"
#include "Molten/Gui/Widgets/PaneWidget.hpp"
#include "Molten/Gui/Widgets/DockerWidget.hpp"
#include "Molten/Gui/Widgets/LabelWidget.hpp"
#include "Molten/Gui/Widgets/ViewportWidget.hpp"
#include "Molten/Gui/Widgets/MenuBarWidget.hpp"

#include "Molten/FileFormat/Mesh/ObjMeshFile.hpp"

namespace Molten::Editor
{

    // Editor implementations.
    Editor::Editor(Semaphore& cancellationSemaphore) :
        m_isRunning(false),
        m_cancellationSemaphore(cancellationSemaphore),
        m_windowTitle("Molten Editor"),
        m_viewportCapacityPolicy{ BufferCapacityScalarPolicy{ 100 }, BufferCapacityScalarPolicy{ 100 } },
        m_fpsTracker(8),
        m_threadPool(0, 2, 0)
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
        windowDescriptor.size = { 1600, 1200 };
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


        m_window->OnFilesDropEnter = [&](const std::vector<std::filesystem::path>& files) -> bool
        {
            if(files.size() != 1)
            {
                return false;
            }

            const auto& file = files.front();
            if(file.extension() != ".obj")
            {
                return false;
            }

            Logger::WriteInfo(m_logger.get(), "Dragging obj file: " + file.string());

            return true;
        };
        m_window->OnFilesDrop = [&](const std::vector<std::filesystem::path>& files)
        {
            if (files.size() != 1)
            {
                return false;
            }

            const auto& file = files.front();
            Logger::WriteInfo(m_logger.get(), "Dropping obj file: " + file.string());

            [[maybe_unused]] auto dummy = m_threadPool.Execute([&, filename = file]()
                {
                    ObjMeshFile objFile;
                    ObjMeshFileReader objFileReader;

                    m_preUpdateCallbacks.Add([&]()
                        {
                            m_loadingProgressBar->value = 0.0;
                        });

                    auto onProgressConnection = objFileReader.onProgress.Connect([&](const double progress)
                        {
                            m_postUpdateCallbacks.Add([this, progress]()
                                {
                                    m_loadingProgressBar->value = progress;
                                });
                        });

                    const Clock clock;
                    const auto result = objFileReader.ReadFromFile(objFile, filename, m_threadPool);
                    const auto readTime = clock.GetTime();

                    onProgressConnection.Disconnect();
                    

                    if(!result.IsSuccessful())
                    {
                        Logger::WriteError(m_logger.get(), "Model loading failed:" + result.GetError().message);
                        return;
                    }

                    Logger::WriteInfo(m_logger.get(), "Read file in: " + std::to_string(readTime.AsSeconds<float>()) + "s.");
                    Logger::WriteInfo(m_logger.get(), "Objects: " + std::to_string(objFile.objects.size()));

                    m_postUpdateCallbacks.Add([&]()
                        {
                            Logger::WriteInfo(m_logger.get(), "Model successfully loaded!");
                        });
                });

            return true;
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

        auto* logger = descriptor.enableGpuLogging ? m_logger.get() : nullptr;

        if (!m_renderer->Open(*m_window, version, logger))
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
        m_fontNameRepository.AddDirectory("C:/Users/sours/AppData/Local/Microsoft/Windows/Fonts");

        m_canvasRenderer = Gui::CanvasRenderer::Create(*m_renderer, m_logger.get());
        m_canvas = std::make_shared<Gui::Canvas<Gui::EditorTheme>>(*m_canvasRenderer, m_fontNameRepository);

        {
	        auto* layer = m_canvas->CreateLayer<Gui::SingleRootLayer>(Gui::LayerPosition::Top);

            auto* rootGrid = layer->CreateChild<Gui::Grid>(Gui::GridDirection::Vertical);
            rootGrid->cellSpacing = 0.0f;

            {
				auto* menuBar = rootGrid->CreateChild<Gui::MenuBar>();
                {
                    auto fileMenu = menuBar->AddMenu("File");
                    fileMenu->AddItem("New Project")->onClick.Connect(
                        [&]() { Logger::WriteInfo(m_logger.get(), "New Project"); });
                    fileMenu->AddItem("Open Project")->onClick.Connect(
                        [&]() { Logger::WriteInfo(m_logger.get(), "Open Project"); });
                    fileMenu->AddItem("Import Asset")->onClick.Connect(
                        [&]() { Logger::WriteInfo(m_logger.get(), "Import Asset"); });
                    fileMenu->AddItem("Exit")->onClick.Connect(
                        [&]() { this->Exit(); });
                }
                {
                    auto aboutMenu = menuBar->AddMenu("Help");
                    aboutMenu->AddItem("About")->onClick.Connect(
                        [&]() { Logger::WriteInfo(m_logger.get(), "About"); });
                    aboutMenu->AddItem("Licenses")->onClick.Connect(
                        [&]() { Logger::WriteInfo(m_logger.get(), "Licenses"); });
                }
	        }

	        auto* docker = rootGrid->CreateChild<Gui::Docker>();

	        docker->margin = { 4.0f, 4.0f, 4.0f, 4.0f };

	        docker->onCursorChange.Connect([&](Mouse::Cursor cursor) {
	            m_window->SetCursor(cursor);
	        });

	        auto sceneViewport = docker->CreateChild<Gui::Viewport>(Gui::DockingPosition::Right);
	        sceneViewport->onResize.Connect([&, viewport = sceneViewport](const auto size) {
	            OnSceneViewportResize(viewport, size);
	        });
	        sceneViewport->onIsVisible.Connect([&]() {
	            m_renderPasses.push_back(m_viewportRenderPass);
	        });

	        docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Bottom, "Assets")->size = { Gui::Size::Pixels{ 250.0f }, Gui::Size::Pixels{ 300.0f } };

	        auto inspector = docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Right, "Inspector");
    		inspector->size = { Gui::Size::Pixels{ 350.0f }, Gui::Size::Pixels{ 200.0f } };
	        inspector->padding = { 3.0f, 3.0f, 3.0f, 3.0f };

    		auto vertGrid = inspector->CreateChild<Gui::Grid>(Gui::GridDirection::Vertical);
	        vertGrid->CreateChild<Gui::Label>("Location:", 18)->position = { Gui::Position::Fixed::Center, Gui::Position::Fixed::Center };

    		auto button = vertGrid->CreateChild<Gui::Button>();
            button->size.x = Gui::Size::Fit::Content;
            button->size.y = Gui::Size::Fit::Content;
	        button->onPress.Connect([&](auto) {
	            Logger::WriteInfo(m_logger.get(), "You pressed me!");
	        });
	        button->CreateChild<Gui::Label>("Click me!", 18);

            m_avgFpsLabel = vertGrid->CreateChild<Gui::Label>("", 18);
            m_minFpsLabel = vertGrid->CreateChild<Gui::Label>("", 18);
            m_maxFpsLabel = vertGrid->CreateChild<Gui::Label>("", 18);

            m_loadingProgressBar = vertGrid->CreateChild<Gui::ProgressBar>();
            m_loadingProgressBar->position = { Gui::Position::Pixels{ 0.0f }, Gui::Position::Pixels{ 100.0f } };

        }

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
        m_preUpdateCallbacks.Dispatch();

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

        m_postUpdateCallbacks.Dispatch();

        /*if (m_modelLoaded)
        {
            Logger::WriteInfo(m_logger.get(), "Model successfully loaded!");
        }*/

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

            /*m_window->SetTitle(
                m_windowTitle + " - avg FPS: " + std::to_string(averageFps) + 
                " | min FPS: " + std::to_string(minFps) +
                " | max FPS: " + std::to_string(maxFps));*/

            m_avgFpsLabel->text.Set("Avg FPS : " + std::to_string(averageFps));
            m_minFpsLabel->text.Set("Min FPS : " + std::to_string(minFps));
            m_maxFpsLabel->text.Set("Max FPS : " + std::to_string(maxFps));

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
        auto createTextures = [&]()->std::array<SharedRenderResource<FramedTexture2D>, 2>
        {
            const auto dimensions = Vector2ui32{
                m_viewportCapacityPolicy[0].GetCapacity(),
                m_viewportCapacityPolicy[1].GetCapacity()
            };

            TextureDescriptor2D colorTextureDesc = {};
            colorTextureDesc.dimensions = dimensions;
            colorTextureDesc.type = TextureType::Color;
            colorTextureDesc.initialUsage = TextureUsage::Attachment;
            colorTextureDesc.format = ImageFormat::URed8Green8Blue8Alpha8;
            colorTextureDesc.internalFormat = ImageFormat::URed8Green8Blue8Alpha8;

        	auto colorTexture = m_renderer->CreateFramedTexture(colorTextureDesc);
            if (!colorTexture)
            {
                return {};
            }

            TextureDescriptor2D depthTextureDesc = {};
            depthTextureDesc.dimensions = dimensions;
            depthTextureDesc.type = TextureType::DepthStencil;
            depthTextureDesc.initialUsage = TextureUsage::Attachment;
            depthTextureDesc.format = ImageFormat::SDepthFloat24StencilUint8;
            depthTextureDesc.internalFormat = ImageFormat::SDepthFloat24StencilUint8;

        	auto depthTexture = m_renderer->CreateFramedTexture(depthTextureDesc);
            if (!depthTexture)
            {
                return {};
            }

            return { std::move(colorTexture), std::move(depthTexture) };
        };

        const Vector2b capacitySetSizeResult = {
		    m_viewportCapacityPolicy[0].SetSize(size.x),
		    m_viewportCapacityPolicy[1].SetSize(size.y)
        };

        if (!m_viewportRenderPass)
        {
            auto [colorTexture, depthTexture] = createTextures();

            viewport->SetTexture(colorTexture);

            const RenderPassDescriptor renderPassDesc = {
                size,
                {
                    {
                        colorTexture,
                        RenderPassAttachmentColorClearValue{ { 0.0f, 0.0f, 0.0f, 0.0f} },
                        TextureType::Color,
                        TextureUsage::Attachment,
                        TextureType::Color,
                        TextureUsage::ReadOnly
                    },
                    {
                        depthTexture,
                        RenderPassAttachmentDepthStencilClearValue{ 1.0f, uint8_t{0} },
                        TextureType::DepthStencil,
                        TextureUsage::Attachment
                    }
                }
            };

            m_viewportRenderPassUpdateDesc = {
                size,
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

            m_viewportRenderPass = m_renderer->CreateRenderPass(renderPassDesc);
            if (!m_viewportRenderPass)
            {
                Exit();
                return;
            }

            m_viewportRenderPass->SetRecordFunction([&](CommandBuffer& commandBuffer) {
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
            m_viewportRenderPassUpdateDesc.dimensions = size;

            if(const bool createNewTexture = capacitySetSizeResult.x || capacitySetSizeResult.y; createNewTexture)
            {
                auto [colorTexture, depthTexture] = createTextures();

                viewport->SetTexture(colorTexture);

                m_viewportRenderPassUpdateDesc.attachments[0].texture = colorTexture;
                m_viewportRenderPassUpdateDesc.attachments[1].texture = depthTexture;
            }

            if(!m_renderer->UpdateRenderPass(*m_viewportRenderPass, m_viewportRenderPassUpdateDesc))
            {
                Exit();
                return;
            }
        }        
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
