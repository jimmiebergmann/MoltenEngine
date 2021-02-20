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

//#include "Molten/Gui/Layers/DockingLayer.hpp"
//#include "Molten/Gui/Layers/RootLayer.hpp"

#include "Molten/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Gui/Widgets/PaddingWidget.hpp"
#include "Molten/Gui/Widgets/VerticalGridWidget.hpp"
#include "Molten/Gui/Widgets/SpacerWidget.hpp"
#include "Molten/Gui/Widgets/PaneWidget.hpp"
#include "Molten/Gui/Widgets/DockerWidget.hpp"


/*#include "Molten/Window/Window.hpp"
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
#include <thread>*/


namespace Molten::Editor
{

    // Editor implementations.
    Editor::Editor(Semaphore& cancellationSemaphore) :
        m_isRunning(false),
        m_cancellationSemaphore(cancellationSemaphore),
        m_windowTitle("Molten Editor")
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
            !LoadGui())
        {
            return false;
        }

        const auto windowUnfocusedSleepTime = Seconds(
            1.0 / (descriptor.windowUnfocusedFpsLimit.has_value() ?
            static_cast<double>(descriptor.windowUnfocusedFpsLimit.value()) : 20.0)
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

    bool Editor::LoadWindow(const EditorDescriptor& descriptor)
    {
        m_window = std::unique_ptr<Window>(Window::Create());
        const Vector2ui32 windowSize = { 800, 600 };
        if (!m_window)
        {
            Logger::WriteError(m_logger.get(), "Failed to create editor window.");
            return false;
        }
        if (!m_window->Open(m_windowTitle, windowSize, m_logger.get()))
        {
            Logger::WriteError(m_logger.get(), "Failed to open editor window.");
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

    bool Editor::LoadGui()
    {
        m_canvasRenderer = Gui::CanvasRenderer::Create(*m_renderer, m_logger.get());
        m_canvas = std::make_shared<Gui::Canvas<Gui::DefaultSkin>>(*m_renderer, m_canvasRenderer);


        // PANE BUTTON TEST
        /*auto grid = m_canvas->CreateChild<Gui::VerticalGrid>();
        grid->margin = Gui::PaddingType{ 10.0f, 20.0f, 30.0f, 40.0f };
        grid->padding = Gui::PaddingType{ 10.0f, 20.0f, 30.0f, 40.0f };

        auto button1 = grid->CreateChild<Gui::Button>();
        button1->onPress.Connect([&](int)
        {
            Logger::WriteInfo(m_logger.get(), "Pressed button 1.");
        });

        grid->CreateChild<Gui::Spacer>();

        auto grid2 = grid->CreateChild<Gui::VerticalGrid>();
        grid2->margin = Gui::PaddingType{ 5.0f, 5.0f, 5.0f, 5.0f };
        grid2->padding = Gui::PaddingType{ 15.0f, 25.0f, 5.0f, 5.0f };
        grid2->cellSpacing = 10.0f;

        grid2->CreateChild<Gui::Button>();
        grid2->CreateChild<Gui::Button>();

        grid->CreateChild<Gui::Spacer>();
        grid->CreateChild<Gui::Button>();
        grid->CreateChild<Gui::Spacer>();
        grid->CreateChild<Gui::Pane>(Vector2f32{ 200.0f, 200.0f });*/


        // DOCKER TEST
        auto docker = m_canvas->CreateChild<Gui::Docker>();
        docker->margin = { 2.0f, 2.0f, 2.0f, 2.0f };

        docker->onCursorChange.Connect([&](Mouse::Cursor cursor)
        {
            m_window->SetCursor(cursor);
        });

        auto pane1 = docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Left, true, Vector2f32{ 100.0f, 100.0f });
        docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Right, false, Vector2f32{ 100.0f, 100.0f });
        docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Bottom, false, Vector2f32{ 100.0f, 100.0f });
        auto pane4 = docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Right, false, Vector2f32{ 100.0f, 100.0f });
        docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Top, false, Vector2f32{ 100.0f, 100.0f });
        docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Left, false, Vector2f32{ 100.0f, 100.0f });


        pane1->CreateChild<Gui::Button>();
        pane4->CreateChild<Gui::Button>();


        
        

        /*
        Gui::Widget<Gui::DefaultSkin>::CreateChild<Gui::Spacer>(grid);
        Gui::Widget<Gui::DefaultSkin>::CreateChild<Gui::Spacer>(grid);
        Gui::Widget<Gui::DefaultSkin>::CreateChild<Gui::Spacer>(grid);
        auto button1 = Gui::Widget<Gui::DefaultSkin>::CreateChild<Gui::Button>(grid);*/
        //auto padding2 = Gui::Widget::CreateChild<Gui::Padding>(grid, 10.0f, 20.0f, 30.0f, 40.0f);
        //auto button2 = Gui::Widget::CreateChild<Gui::Button>(grid);
        //button2->margin = Gui::MarginData{ 10.0f, 20.0f, 30.0f, 40.0f };
        //auto button3 = Gui::Widget::CreateChild<Gui::Button>(grid);
        return true;
    }

    void Editor::Exit()
    {
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

        UpdateCanvas();

        m_renderer->Resize(m_window->GetSize());
        m_renderer->BeginDraw();

        m_canvas->Draw();

        m_renderer->EndDraw();

        return true;
    }

    bool Editor::UpdateWindow()
    {
        if(m_windowTitleUpdateClock.GetTime() >= Seconds(1.0f))
        {
            const auto fps = 1.0f / m_deltaTime.AsSeconds<double>();
            m_window->SetTitle(m_windowTitle + " - FPS: " + std::to_string(fps));
            
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
        static Time prevSleepTime = {};

        const auto windowSize = m_window->GetSize();
        bool isMinimized = !windowSize.x || !windowSize.y;
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

}
