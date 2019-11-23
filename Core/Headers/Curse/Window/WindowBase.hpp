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

#ifndef CURSE_CORE_WINDOW_WINDOWBASE_HPP
#define CURSE_CORE_WINDOW_WINDOWBASE_HPP

#include "Curse/Math/Vector.hpp"
#include "Curse/Platform/Win32Headers.hpp"
#include "Curse/Platform/X11Headers.hpp"
#include <string>

namespace Curse
{

    /**
    * @brief Base class of application windows.
    */
    class CURSE_API WindowBase
    {

    public:

        /**
        * @brief Virtual destructor.
        */
        virtual ~WindowBase();

        /**
        * @brief Open window.
        */
        virtual void Open(const std::string& title, const Vector2ui32 size) = 0;

        /**
        * @brief Close window.
        */
        virtual void Close() = 0;

        /**
        * @brief Update window.
        */
        virtual void Update() = 0;

        /**
        * @brief Show window.
        *
        * @param show Shows window if true, else hides window.
        */
        virtual void Show(const bool show = true) = 0;

        /**
        * @brief Hide window.
        */
        virtual void Hide() = 0;

        /**
        * @brief Checks if window has been created and is open.
        *        An open window is not the same as "currently showing".
        */
        virtual bool IsOpen() const = 0;

        /**
        * @brief Get initial size of window.
        */
        virtual Vector2ui32 GetInitialSize() const = 0;

        /**
        * @brief Get current size of window.
        */
        virtual Vector2ui32 GetCurrentSize() const = 0;

    #if defined(CURSE_PLATFORM_WINDOWS)
       
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
    
    #elif defined(CURSE_PLATFORM_WINDOWS)
       
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

}

#endif
