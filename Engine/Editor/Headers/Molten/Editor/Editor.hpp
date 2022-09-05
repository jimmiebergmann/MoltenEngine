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

#ifndef MOLTEN_EDITOR_EDITOR_HPP
#define MOLTEN_EDITOR_EDITOR_HPP

#include "Molten/Editor/View/RootView.hpp"
#include "Molten/Editor/View/SceneView.hpp"
#include "Molten/Graphics/Window/Window.hpp"
#include "Molten/Graphics/Renderer.hpp"
#include "Molten/Graphics/Gui/Canvas.hpp"
#include "Molten/System/Semaphore.hpp"
#include "Molten/System/Clock.hpp"
#include "Molten/Utility/FpsTracker.hpp"
#include "Molten/System/ThreadPool.hpp"
#include "Molten/Utility/FunctionDispatcher.hpp"
#include <optional>
#include <thread>
#include <atomic>

namespace Molten
{
    class Logger;
}

namespace Molten::Editor
{

    /** Editor creation descriptor struct. */
    struct EditorDescriptor
    {
        std::shared_ptr<Logger> logger;
        bool enableGpuLogging = false;
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
        explicit Editor(Semaphore& cancellationSemaphore);

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

        [[nodiscard]] bool Load(const EditorDescriptor& descriptor);
        [[nodiscard]] bool LoadWindow(const EditorDescriptor& descriptor);
        [[nodiscard]] bool LoadRenderer(const EditorDescriptor& descriptor);
        [[nodiscard]] bool LoadRenderPasses();
        [[nodiscard]] bool LoadGui();
        [[nodiscard]] bool LoadGuiViews();
        [[nodiscard]] bool LoadGuiSignals();

        void Exit();

        bool Tick();
        bool UpdateWindow();
        bool HandleWindowFocus();
        void UpdateCanvas();

        bool ValidateFileDrops(const std::vector<std::filesystem::path>& files);
        bool ProcessFileDrops(const std::vector<std::filesystem::path>& files);

        std::atomic_bool m_isRunning;
        Semaphore& m_cancellationSemaphore;

        std::shared_ptr<Logger> m_logger;
        std::unique_ptr<Window> m_window;
        std::string m_windowTitle;
        std::unique_ptr<Renderer> m_renderer;
        RenderPasses m_renderPasses;
        std::thread m_thread;
        Gui::FontNameRepository m_fontNameRepository;

        SleepClock m_fpsLimiter;
        SleepClock m_unfocusedWindowFpsLimiter;
        FpsTracker m_fpsTracker;

        Time m_deltaTime;
        Clock m_windowTitleUpdateClock;

        ThreadPool m_threadPool;
        FunctionDispatcher m_preUpdateCallbacks;
        FunctionDispatcher m_postUpdateCallbacks;

        // NEW
        std::unique_ptr<RootView> m_rootView;
        std::unique_ptr<SceneView> m_sceneView;

    };

}

#endif