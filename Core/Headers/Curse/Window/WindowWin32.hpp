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

#ifndef CURSE_CORE_WINDOW_WINDOW_WIN32_HPP
#define CURSE_CORE_WINDOW_WINDOW_WIN32_HPP

#include "Curse/Window/WindowBase.hpp"

#if defined CURSE_PLATFORM_WINDOWS

#include "Curse/Platform/Win32Headers.hpp"
#include <stdexcept>

namespace Curse
{

    /**
    * @brief  Window class of win32 application windows.
    */
    class CURSE_API WindowWin32 : public WindowBase
    {

    public:

        /**
        * @brief Constructor.
        */
        WindowWin32();

        /**
        * @brief Constructs and opens window.
        */
        WindowWin32(const std::string& title, const Vector2ui32 size);

        /**
        * @brief Destructor.
        */
        ~WindowWin32();

        /**
        * @brief Open window.
        */
        virtual void Open(const std::string& title, const Vector2ui32 size);

        /**
        * @brief Close window.
        */
        virtual void Close();

        /**
        * @brief Update window.
        */
        virtual void Update();

        /**
        * @brief Show window.
        *
        * @param show Shows window if true, else hides window.
        */
        virtual void Show(const bool show = true);

        /**
        * @brief Hide window.
        */
        virtual void Hide();

        /**
        * @brief Checks if window has been created and is open.
        *        An open window is not the same as "currently showing".
        */
        virtual bool IsOpen() const;

        /**
        * @brief Get win32 window handle. Only available on Windows.
        */
        virtual HWND GetWin32Window() const;

        /**
        * @brief Get win32 instance handle. Only available on Windows.
        */
        virtual HINSTANCE GetWin32Instance() const;

        /**
        * @brief Get win32 device context handle. Only available on Windows.
        */
        virtual HDC GetWin32DeviceContext() const;

    private:

        static LRESULT WindowProcStatic(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

        HWND m_window;
        HINSTANCE m_instance;
        HDC m_deviceContext;
        DWORD m_style; //< Win32 style of window.
        DWORD m_extendedStyle; //< Win32 extended style of window.
        std::string m_windowClassName;
        Vector2ui32 m_initialSize;
        Vector2ui32 m_currentSize;
        std::string m_title;

    };

}

#endif

#endif
