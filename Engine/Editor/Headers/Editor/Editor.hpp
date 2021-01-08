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

#ifndef MOLTEN_EDITOR_EDITOR_HPP
#define MOLTEN_EDITOR_EDITOR_HPP

/*
#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Scene/Camera.hpp"
#include "Molten/System/Clock.hpp"
#include "Molten/Window/Window.hpp"
#include "Molten/Gui/Canvas.hpp"
#include <memory>*/

#include "Molten/Window/Window.hpp"
#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Gui/Canvas.hpp"
#include "Molten/Gui/Skins/DefaultSkin.hpp"
#include "Molten/System/Semaphore.hpp"
#include "Molten/System/Clock.hpp"
#include <optional>
#include <thread>
#include <atomic>

namespace Molten
{
    class Logger;
}

namespace Molten::Editor
{

    class EditorDescriptor
    {

    public:

        std::shared_ptr<Logger> logger;
        std::optional<Renderer::BackendApi> backendRendererApi;
        std::optional<Version> backendRendererApiVersion;
        std::optional<uint32_t> fpsLimit;
        std::optional<uint32_t> windowUnfocusedFpsLimit;

    };


    /** Editor class. */
    class Editor
    {

    public:

        /** Constructor. */
        Editor(Semaphore& cancellationSemaphore);

        /** Destructor. */
        ~Editor();

        bool Open(const EditorDescriptor& descriptor);

        /* Deleted operations. */
        /**@{*/
        Editor(const Editor&) = delete;
        Editor(Editor&&) = delete;
        Editor& operator =(const Editor&) = delete;
        Editor& operator =(Editor&&) = delete;
        /**@}*/

    private:

        bool Load(const EditorDescriptor& descriptor);
        bool LoadWindow(const EditorDescriptor& descriptor);
        bool LoadRenderer(const EditorDescriptor& descriptor);
        bool LoadGui();

        void Exit();

        bool Tick();
        bool UpdateWindow();
        bool HandleWindowFocus();
        void UpdateCanvas();

        std::atomic_bool m_isRunning;
        Semaphore& m_cancellationSemaphore;

        std::shared_ptr<Logger> m_logger;
        std::unique_ptr<Window> m_window;
        std::string m_windowTitle;
        std::unique_ptr<Renderer> m_renderer;
        std::thread m_thread;
        Gui::CanvasRendererPointer m_canvasRenderer;
        Gui::CanvasPointer<Gui::DefaultSkin> m_canvas;
        SleepClock m_fpsLimiter;
        SleepClock m_unfocusedWindowFpsLimiter;

        Time m_deltaTime;
        Clock m_windowTitleUpdateClock;

    };

}

#endif