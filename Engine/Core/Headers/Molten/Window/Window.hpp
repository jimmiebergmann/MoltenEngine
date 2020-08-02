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

#ifndef MOLTEN_CORE_WINDOW_WINDOW_HPP
#define MOLTEN_CORE_WINDOW_WINDOW_HPP

#include "Molten/Platform/Win32Headers.hpp"
#include "Molten/Platform/X11Headers.hpp"
#include "Molten/System/Signal.hpp"
#include "Molten/System/UserInput.hpp"
#include "Molten/Math/Vector.hpp"
#include <string>

namespace Molten
{

    class Logger;
    class UserInput;

    /**
    * @brief Base class of application windows.
    */
    class MOLTEN_API Window
    {

    public:

        /**
         * @brief Static function for creating a window object for the current platform.
         *        Window is being opened if parameters are passed.
         *
         * @return Pointer to window, nullptr if no window is available on the current platform.
         */
        static Window* Create();      

        /**
        * @brief On DPI change signal.
        */
        Signal<Vector2ui32> OnDpiChange;

        /**
        * @brief On maximize signal.
        */
        Signal<Vector2i32> OnMaximize;

        /**
        * @brief On minimize signal.
        */
        Signal<Vector2i32> OnMinimize;

        /**
        * @brief On resize signal.
        */
        Signal<Vector2i32> OnMove;

        /**
        * @brief On resize signal.
        */
        Signal<Vector2ui32> OnResize;

        /**
        * @brief On scale change signal.
        *        Same as OnDpiChange, but returning a floating point vector equal to DPI / 96.
        */
        Signal<Vector2f32> OnScaleChange;

        /**
        * @brief On show/hide signal.
        */
        Signal<bool> OnShow;

        /**
        * @brief Virtual destructor.
        */
        virtual ~Window();

        /**
        * @brief Open window.
        */
        virtual bool Open(const std::string& title, const Vector2ui32 size, Logger* logger = nullptr) = 0;

        /**
        * @brief Close window.
        */
        virtual void Close() = 0;

        /**
        * @brief Update window.
        */
        virtual void Update() = 0;

        /**
        * @brief Checks if window has been created and is open.
        *        An open window is not the same as "currently showing".
        */
        virtual bool IsOpen() const = 0;

        /**
        * @brief Checks if window is present and showing on screen.
        *        A showing window is not the same as "open".
        */
        virtual bool IsShowing() const = 0;

        /**
        * @brief Checks if window is maximized.
        */
        virtual bool IsMaximized() const = 0;

        /**
        * @brief Checks if window is minimized.
        *        Window is now minimized when closed.
        */
        virtual bool IsMinimized() const = 0;

        /**
        * @brief Show window.
        *
        * @param show Shows window if true, else hides window.
        * @param signal Signals OnShow if true. OnShow is not being signaled if current status equals to parameter show.
        */
        virtual void Show(const bool show = true, const bool signal = false) = 0;

        /**
        * @brief Hide window.
        *
        * @param signal Signals OnMaximize if true. OnShow is not being signaled if current status equals to parameter show.
        */
        virtual void Hide(const bool signal = false) = 0;

        /**
        * @brief Maximize window.
        *
        * @param signal Signals OnMaximize if true. OnMaximize is not being signaled if window already is maximized.
        */
        virtual void Maximize(const bool signal = false) = 0;

        /**
        * @brief Minimize window.
        *
        * @param signal Signals OnMinimize if true. OnMinimize is not being signaled if window already is minimized.
        */
        virtual void Minimize(const bool signal = false) = 0;

        /**
        * @brief Change current position of window.
        *
        * @param position New position of window.
        * @param signal Signals OnMove if true. OnMove is not being signaled if new position equals to current position.
        */
        virtual void Move(const Vector2i32& position, const bool signal = false) = 0;

        /**
        * @brief Change current size of window.
        *
        * @param size New size of window.
        * @param signal Signals OnResize if true. OnResize is not being signaled if new size equals to current size.
        */
        virtual void Resize(const Vector2ui32& size, const bool signal = false) = 0;

        /**
        * @brief Set current title of window.
        */
        virtual void SetTitle(const std::string& title) = 0;

        /**
        * @brief Get current DPI of window.
        */
        virtual Vector2ui32 GetDpi() const = 0;

        /**
        * @brief Get current scale of window.
        *        Same as GetDpi, but returning a floating point vector equal to DPI / 96.
        */
        virtual Vector2f32 GetScale() const = 0;

        /**
        * @brief Get current size of window.
        */
        virtual Vector2ui32 GetSize() const = 0; 

        /**
        * @brief Get current position of window.
        */
        virtual Vector2i32 GetPosition() const = 0;

        /**
        * @brief Get title of window, being rendered in client area.
        */
        virtual std::string GetTitle() const = 0;

        /**
        * @brief Get user input of window.
        *        The method Update is being called and managed by the window.
        */
        virtual UserInput& GetUserInput() = 0;

        /**
        * @brief Get user input of window.
        *        The method Update is being called and managed by the window.
        */
        virtual const UserInput& GetUserInput() const = 0;

    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
       
        /**
        * @brief Get win32 window handle. Only available on Windows.
        */
        virtual HWND GetWin32Window() const = 0;

        /**
        * @brief Get win32 instance handle. Only available on Windows.
        */
        virtual HINSTANCE GetWin32Instance() const = 0;

        /**
        * @brief Get win32 device context handle. Only available on Windows.
        */
        virtual HDC GetWin32DeviceContext() const = 0;
    
    #elif MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX
       
        /**
        * @brief Get X11 display Device. Only available on Linux.
        */
        virtual ::Display * GetX11DisplayDevice( ) const = 0;

        /**
        * @brief Get X11 window Device. Only available on Linux.
        */
        virtual ::Window GetX11WindowDevice( ) const = 0;

        /**
        * @brief Get X11 screen Device. Only available on Linux.
        */
        virtual int GetX11ScreenDevice( ) const = 0;

    #endif

    };

    /**
    * @brief Platform independed window class.
    */
    class MOLTEN_API PlatformWindow
    {

    public:

        /**
        * @brief Enumerator describing window type.
        */
        enum class Type
        {
            Info,
            Error
        };

        /**
        * @brief Show a modal window with a OK button.
        */
        static void Message(const Type type, const std::string& title, const std::string& content);

        /**
        * @brief Show a modal window with a Yes and No button.
        *
        * @return true if user pressed the Yes button, false if No button was pressed.
        */
        static bool MessageConfirm(const Type type, const std::string& title, const std::string& content);

    };

}

#endif
