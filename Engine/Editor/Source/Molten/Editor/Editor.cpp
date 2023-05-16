/*
* MIT License
*
* Copyright (c) 2023 Jimmie Bergmann
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


#include "Molten/Editor/Editor.hpp"
#include "Molten/Logger.hpp"

#include "Molten/Graphics/Gui/CanvasRenderer.hpp"
#include "Molten/Graphics/Gui/Layers/SingleRootLayer.hpp"
#include "Molten/Graphics/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/GridWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/PaneWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/DockerWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/LabelWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/ViewportWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/MenuBarWidget.hpp"

#include "Molten/Graphics/Gui2/Style.hpp"
#include "Molten/Graphics/Gui2/Widgets/ButtonWidget.hpp"

#include "Molten/EditorFramework/Project.hpp"
#include "Molten/EditorFramework/FileFormat/Mesh/ObjMeshFile.hpp"

#include <random>

namespace Molten::Editor
{

    // Editor implementations.
    Editor::Editor(Semaphore& cancellationSemaphore) :
        m_isRunning(false),
        m_cancellationSemaphore(cancellationSemaphore),
        m_windowTitle("Molten Editor"),
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

            

            /*Clock tickTimer;
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
            }*/
        });

        startedSemaphore.Wait();
        return true;
    }

    bool Editor::Load(const EditorDescriptor& descriptor)
    {
        const auto projectName = "my_project";
        const auto projectPath = "" / std::filesystem::path{ projectName };
        if (!LoadProject(projectPath)) 
        {
            Logger::WriteInfo(m_logger.get(), "Could not load project, creating new project.");

            if (!CreateProject("", projectName))
            {
                Logger::WriteError(m_logger.get(), "Failed to create project.");
                return false;
            }

            if (!LoadProject(projectPath))
            {
                Logger::WriteError(m_logger.get(), "Failed to load project file after creation.");
                return false;
            }
        }

        //if (!LoadWindow(descriptor) ||
        //    !LoadRenderer(descriptor) ||
        //    !LoadRenderPasses() /*||
        //    !LoadGui()*/)
        //{
        //    return false;
        //}

        /*const auto windowUnfocusedSleepTime = Seconds(
            1.0 / (descriptor.windowUnfocusedFpsLimit.has_value() ?
            static_cast<double>(descriptor.windowUnfocusedFpsLimit.value()) : 15.0)
        );
        m_unfocusedWindowFpsLimiter.SetSleepTime(windowUnfocusedSleepTime);

        if(descriptor.fpsLimit.has_value()) 
        {
            const auto fpsLimit = static_cast<double>(descriptor.fpsLimit.value()) + 0.25;
            m_fpsLimiter.SetSleepTime(Seconds(1.0 / fpsLimit));
        }

        m_window->Show();*/
        return true;
    }

    bool Editor::LoadProject(const std::filesystem::path& /*path*/)
    {
        return false;
    }

    bool Editor::CreateProject(const std::filesystem::path& path, const std::string& filename)
    {     
        auto fullPath = path.empty() ? filename : path / filename;
        std::filesystem::remove_all(fullPath);

        auto createProjectResult = EditorFramework::Project::Create(path, filename);
        if (createProjectResult != EditorFramework::CreateProjectResult::Success)
        {
            Logger::WriteError(m_logger.get(), "Failed to create project.");
            return false;
        }

        auto openProjectResult = EditorFramework::Project::Open(fullPath / (filename + ".mproj"));
        if (!openProjectResult.HasValue())
        {
            Logger::WriteError(m_logger.get(), "Failed to open project.");
            return false;
        }


        return true;
    }

    //bool Editor::LoadWindow(const EditorDescriptor&)
    //{
    //    WindowDescriptor windowDescriptor;
    //    windowDescriptor.size = { 1600, 1200 };
    //    windowDescriptor.title = m_windowTitle;
    //    windowDescriptor.enableDragAndDrop = true;
    //    windowDescriptor.logger = m_logger.get();

    //    m_window = Window::Create(windowDescriptor);
    //    if (!m_window || !m_window->IsOpen())
    //    {
    //        Logger::WriteError(m_logger.get(), "Failed to create editor window.");
    //        return false;
    //    }

    //    m_window->OnResize.Connect([&](Vector2ui32)
    //    {
    //        Tick();
    //    });

    //    m_window->OnDpiChange.Connect([&](Vector2ui32 dpi)
    //    {
    //        m_logger->Write(Logger::Severity::Info, "Changed DPI: " + std::to_string(dpi.x) + ", " + std::to_string(dpi.y));
    //    });

    //    m_window->OnScaleChange.Connect([&](Vector2f32 scale)
    //    {
    //        m_logger->Write(Logger::Severity::Info, "Changed scale: " + std::to_string(scale.x) + ", " + std::to_string(scale.y));
    //    });

    //    m_window->OnFilesDropEnter = [&](const std::vector<std::filesystem::path>& files) -> bool
    //    {
    //        return ValidateFileDrops(files);
    //    };
    //    m_window->OnFilesDrop = [&](const std::vector<std::filesystem::path>& files)
    //    {          
    //        return ProcessFileDrops(files);
    //    };

    //    return true;
    //}

    //bool Editor::LoadRenderer(const EditorDescriptor& descriptor)
    //{
    //    auto* logger = descriptor.enableGpuLogging ? m_logger.get() : nullptr;

    //    const auto api = descriptor.backendRendererApi.has_value() ? 
    //        descriptor.backendRendererApi.value() : Renderer::BackendApi::Vulkan;

    //    const auto apiVersion = descriptor.backendRendererApiVersion.has_value() ?
    //        descriptor.backendRendererApiVersion.value() : Version(1, 0);

    //    const auto rendererDesc = RendererDescriptor{ *m_window, apiVersion, logger };

    //    m_renderer = Renderer::Create(api, rendererDesc);
    //    if (!m_renderer)
    //    {
    //        Logger::WriteError(m_logger.get(), "Failed to create renderer.");
    //        return false;
    //    }

    //    return true;
    //}

    //bool Editor::LoadRenderPasses()
    //{
    //    /*auto renderPass = m_renderer->GetSwapChainRenderPass();
    //    renderPass->SetRecordFunction([&](auto& commandBuffer) {
    //       m_canvasRenderer->SetCommandBuffer(commandBuffer);
    //        //m_rootView->canvasRenderer->SetCommandBuffer(commandBuffer);
    //        //m_rootView->canvas->Draw();

    //       m_canvasRenderer->SetCommandBuffer(commandBuffer);
    //       m_canvas->Draw();
    //    });*/

    //    return true;
    //}

    /*bool Editor::LoadGui()
    {
        const auto canvasRendererDesc = Gui2::CanvasRendererDescriptor{
            .backendRenderer = *m_renderer.get(),
            .logger = m_logger.get()
        };

        m_canvasRenderer = Gui2::CanvasRenderer::Create(canvasRendererDesc);
        if (!m_canvasRenderer)
        {
            return false;
        }

        auto style = std::make_shared<Gui2::Style>();

        m_canvas = std::make_unique<Gui2::Canvas>(m_canvasRenderer, style);

        [[maybe_unused]] auto button1 = m_canvas->CreateChild<Gui2::Button>();

        auto button2Style = std::make_shared<Gui2::ButtonStyle>();
        button2Style->colors.normal = { 0.0f, 1.0f, 0.0f, 1.0f };

        [[maybe_unused]] auto button2 = button1->CreateChild<Gui2::Button>(button2Style);

        auto button3Style = std::make_shared<Gui2::ButtonStyle>();
        button3Style->colors.normal = { 0.0f, 0.0f, 1.0f, 1.0f };

        [[maybe_unused]] auto button3 = button2->CreateChild<Gui2::Button>(button3Style);

        return true;
    }*/

   /* bool Editor::LoadGuiViews()
    {
        m_fontNameRepository.AddSystemDirectories();

        const auto rootViewDesc = RootViewDescriptor{
           .renderer = *m_renderer,
           .fontNameRepository = m_fontNameRepository,
           .logger = m_logger.get()
        };

        if (m_rootView = RootView::Create(rootViewDesc); !m_rootView)
        {
            return false;
        }

        const auto sceneViewDesc = SceneViewDescriptor{
            .renderer = *m_renderer,
            .rootWidget = *m_rootView->pageView,
            .deltaTime = m_deltaTime,
            .logger = m_logger.get()
        };

        if (m_sceneView = SceneView::Create(sceneViewDesc); !m_sceneView)
        {
            return false;
        }

        return true;
    }

    bool Editor::LoadGuiSignals()
    {
        m_sceneView->dockerWidget->onCursorChange.Connect([&](Mouse::Cursor cursor) {
            m_window->SetCursor(cursor);
        });

        m_sceneView->viewportWidget->onResize.Connect([&](const auto size) {
            m_sceneView->sceneViewport->Resize(size);
        });
        m_sceneView->viewportWidget->onIsVisible.Connect([&]() {
            if (m_sceneView->sceneViewport->renderPass)
            {
                m_renderPasses.push_back(m_sceneView->sceneViewport->renderPass);
            }
        });

        return true;
    }*/

    void Editor::Exit()
    {
        if (m_renderer)
        {
            m_renderer->WaitForDevice();
        }

        m_isRunning = false;
        m_cancellationSemaphore.NotifyAll();
    }

    bool Editor::Tick()
    {
        //m_preUpdateCallbacks.Dispatch();

        if(!UpdateWindow())
        {
            return false;
        }
        if(!HandleWindowFocus())
        {
            return true;
        }

        m_renderPasses.clear();

        //UpdateCanvas();

        //m_postUpdateCallbacks.Dispatch();

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

            /*m_avgFpsLabel->text.Set("Avg FPS : " + std::to_string(averageFps));
            m_minFpsLabel->text.Set("Min FPS : " + std::to_string(minFps));
            m_maxFpsLabel->text.Set("Max FPS : " + std::to_string(maxFps));*/

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

    /*void Editor::UpdateCanvas()
    {
        auto& userInput = m_window->GetUserInput();
        //auto& canvas = *m_rootView->canvas;

        UserInput::Event inputEvent;
        while (userInput.PollEvent(inputEvent))
        {
            m_canvas->PushUserInputEvent(inputEvent);
        }

        m_canvas->SetSize(m_window->GetSize());
        m_canvas->SetScale(m_window->GetScale());
        m_canvas->Update();
    }*/

    /*void Editor::UpdateCanvas()
    {
       auto& userInput = m_window->GetUserInput();
        auto& canvas = *m_rootView->canvas;

        UserInput::Event inputEvent;
        while (userInput.PollEvent(inputEvent))
        {
            canvas.PushUserInputEvent(inputEvent);
        }

        canvas.SetSize(m_window->GetSize());
        canvas.SetScale(m_window->GetScale());
        canvas.Update(m_deltaTime);
    }*/

    bool Editor::ValidateFileDrops(const std::vector<std::filesystem::path>& files)
    {
        if (files.size() != 1)
        {
            return false;
        }

        const auto& file = files.front();
        if (file.extension() != ".obj")
        {
            return false;
        }

        Logger::WriteInfo(m_logger.get(), "Dragging obj file: " + file.string());

        return true;
    }

    bool Editor::ProcessFileDrops(const std::vector<std::filesystem::path>& files)
    {
        if (files.size() != 1)
        {
            return false;
        }

        /*const auto& file = files.front();
        Logger::WriteInfo(m_logger.get(), "Dropping obj file: " + file.string());

        [[maybe_unused]] auto dummy = m_threadPool.Execute([&, filename = file]() {
            ObjMeshFile objFile;
            ObjMeshFileReader objFileReader;

            m_preUpdateCallbacks.Add([&]() {
                m_loadingProgressBar->value = 0.0;
            });

            auto onProgressConnection = objFileReader.onProgress.Connect([&](const double progress) {
                m_postUpdateCallbacks.Add([this, progress]() {
                    m_loadingProgressBar->value = progress;
                });
            });

            const Clock clock;
            const auto result = objFileReader.ReadFromFile(objFile, filename, m_threadPool);
            const auto readTime = clock.GetTime();

            onProgressConnection.Disconnect();

            if (!result.IsSuccessful())
            {
                Logger::WriteError(m_logger.get(), "Model loading failed:" + result.GetError().message);
                return;
            }

            Logger::WriteInfo(m_logger.get(), "Read file in: " + std::to_string(readTime.AsSeconds<float>()) + "s.");
            Logger::WriteInfo(m_logger.get(), "Objects: " + std::to_string(objFile.objects.size()));

            m_postUpdateCallbacks.Add([&]() {
                Logger::WriteInfo(m_logger.get(), "Model successfully loaded!");
            });
        });*/

        return true;
    }

}
