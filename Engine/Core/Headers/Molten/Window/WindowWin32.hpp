/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_WINDOW_WINDOWWIN32_HPP
#define MOLTEN_CORE_WINDOW_WINDOWWIN32_HPP

#include "Molten/Window/Window.hpp"

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

#include "Molten/Memory/Reference.hpp"

namespace Molten
{

    /** Window class of win32 application windows. */
    class MOLTEN_API WindowWin32 : public Window
    {

    public:

        /** Constructor. */
        WindowWin32();

        /** Constructs and opens window. */
        WindowWin32(const std::string& title, const Vector2ui32 size, Logger * logger = nullptr);

        /** Destructor. */
        ~WindowWin32();

        /** Open window. */
        virtual bool Open(const std::string& title, const Vector2ui32 size, Logger* logger = nullptr) override;

        /** Close window. */
        virtual void Close() override;

        /** Update window. */
        virtual void Update() override;

        /** Show window.
         *
         * @param show Shows window if true, else hides window.
         * @param signal Signals OnShow if true. OnShow is not being signaled if current status equals to parameter show.
         */
        virtual void Show(const bool show = true, const bool signal = false) override;

        /** Hide window.
         *
         * @param signal Signals OnMaximize if true. OnShow is not being signaled if current status equals to parameter show.
         */
        virtual void Hide(const bool signal = false) override;

        /** Checks if window has been created and is open.
         *  An open window is not the same as "currently showing".
         */
        virtual bool IsOpen() const override;

        /** Checks if window is present and showing on screen.
         *  A showing window is not the same as "open".
         */
        virtual bool IsShowing() const override;

        /** Checks if window is maximized. */
        virtual bool IsMaximized() const override;

        /** Checks if window is minimized.
         *  Window is now minimized when closed.
         */
        virtual bool IsMinimized() const override;

        /** Checks if window is focused or not.
         *  A window is considered focused when it is selected and will receive user inputs.
         *  Minimizing or clicking on another desktop window will kill focus of this window and result in a return value of false.
         */
        virtual bool IsFocused() const override;

        /** Maximize window.
         *
         * @param signal Signals OnMaximize if true. OnMaximize is not being signaled if window already is maximized.
         */
        virtual void Maximize(const bool signal = false) override;

        /** Minimize window.
         *
         * @param signal Signals OnMinimize if true. OnMinimize is not being signaled if window already is minimized.
         */
        virtual void Minimize(const bool signal = false) override;

        /** Change current position of window.
         *
         * @param position New position of window.
         * @param signal Signals OnMove if true. OnMove is not being signaled if new position equals to current position.
         */
        virtual void Move(const Vector2i32& position, const bool signal = false) override;

        /** Change current size of window.
         *
         * @param size New size of window.
         * @param signal Signals OnResize if true. OnResize is not being signaled if new size equals to current size.
         */
        virtual void Resize(const Vector2ui32& size, const bool signal = false) override;

        /** Set current title of window. */
        virtual void SetTitle(const std::string& title) override;

        /** Get current DPI of window. */
        virtual Vector2ui32 GetDpi() const override;

        /** Get current scale of window.
         *  Same as GetDpi, but returning a floating point vector equal to DPI / 96.
         */
        virtual Vector2f32 GetScale() const override;

        /** Get current size of window. */
        virtual Vector2ui32 GetSize() const override;

        /** Get current position of window. */
        virtual Vector2i32 GetPosition() const override;

        /** Get title of window, being rendered in client area. */
        virtual std::string GetTitle() const override;

        /** Set current cursor.*/
        virtual void SetCursor(const Mouse::Cursor cursor) override;

        /** Get user input of window.
         *  The method Update is being called and managed by the window.
         */
        virtual UserInput& GetUserInput() override;

        /** Get user input of window.
         *  The method Update is being called and managed by the window.
         */
        virtual const UserInput& GetUserInput() const override;

        /** Get win32 window handle. Only available on Windows. */
        /**@{*/
        virtual HWND GetWin32Window() override;
        virtual const HWND GetWin32Window() const override;
        /**@}*/

        /** Get win32 device context handle. Only available on Windows. */
        /**@{*/
        virtual HDC GetWin32DeviceContext() override;
        virtual const HDC GetWin32DeviceContext() const override;
        /**@}*/

        /** Get win32 instance handle. Only available on Windows.*/
        /**@{*/
        virtual HINSTANCE GetWin32Instance() override;
        virtual const HINSTANCE GetWin32Instance() const override;
        /**@}*/

    private:

        static LRESULT WindowProcStatic(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

        class DynamicFunctions
        {

        public:

            DynamicFunctions();
            ~DynamicFunctions();

            bool SetProcessDPIAware();

        private:

            DynamicFunctions(const DynamicFunctions&) = delete;
            DynamicFunctions(DynamicFunctions&&) = delete;

            HMODULE m_module;

        #if (MOLTEN_PLATFORM_WINDOWS_SUPPORT_MULTI_MONITOR_DPI)
            using SetProcessDpiMultiDisplayAwarenessFunc = HRESULT(*)(::PROCESS_DPI_AWARENESS);
            SetProcessDpiMultiDisplayAwarenessFunc m_setProcessDpiMultiDisplayAwareness;
        #endif

            using SetProcessDpiSingleDisplayAwarenessFunc = BOOL(*)();
            SetProcessDpiSingleDisplayAwarenessFunc m_setProcessDpiSingleDisplayAwareness;

        };

        static DynamicFunctions s_DynamicFunctions;

        Logger* m_logger;
        HWND m_window;
        HINSTANCE m_instance;
        HDC m_deviceContext;
        DWORD m_style; //< Win32 style of window.
        DWORD m_extendedStyle; //< Win32 extended style of window.
        std::string m_windowClassName;

        bool m_showing;
        bool m_maximized;
        bool m_minimized;
        bool m_focused;
        Vector2ui32 m_size;
        Vector2i32 m_position;
        std::string m_title;
        Vector2ui32 m_dpi;
        Mouse::Cursor m_cursor;

        UserInput m_userInput;

    };

}

#endif

#endif
