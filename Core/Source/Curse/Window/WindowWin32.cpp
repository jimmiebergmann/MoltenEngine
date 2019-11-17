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

#include "Curse/Window/WindowWin32.hpp"

#if defined CURSE_PLATFORM_WINDOWS

#include "Curse/System/Exception.hpp"
#include <sstream>

namespace Curse
{

    WindowWin32::WindowWin32() :
        m_window(NULL),
        m_instance(NULL),
        m_deviceContext(NULL),
        m_style(0),
        m_extendedStyle(0),
        m_windowClassName(""),
        m_initialSize(0, 0),
        m_currentSize(0, 0),
        m_title("")
    {
    }

    WindowWin32::WindowWin32(const std::string& title, const Vector2ui32 size) :
        WindowWin32()
    {
        Open(title, size);
    }

    WindowWin32::~WindowWin32()
    {
        Close();
    }

    static std::string CreateGuid()
    {
        union GuidStruct
        {
            GUID guid;
            uint16_t part;
        } guid;

        if (::CoCreateGuid(&guid.guid) != S_OK)
        {
            throw Exception("Win32: Failed to generate GUID.");
        }

        uint16_t* ptr = &guid.part;
        std::stringstream ss;
        ss << std::hex;    
        ss << ptr[0] << ptr[1] << "-" << ptr[3]  << "-" << ptr[4] << "-" << ptr[5] << ptr[6] << ptr[7];

        return std::move(ss.str());
    }

    void WindowWin32::Open(const std::string& title, const Vector2ui32 size)
    {
        Close();

        m_extendedStyle = WS_EX_APPWINDOW;
        m_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_BORDER;

        auto guid = CreateGuid();
        std::string className = "CurseEngine_" + guid;

        WNDCLASS winClass;
        HINSTANCE winInstance = ::GetModuleHandle(NULL);
        winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        winClass.lpfnWndProc = (WNDPROC)WindowWin32::WindowProcStatic;
        winClass.cbClsExtra = 0;
        winClass.cbWndExtra = 0;
        winClass.hInstance = winInstance;
        winClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        winClass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
        winClass.lpszClassName = className.c_str();
        winClass.lpszMenuName = NULL;

        if (!::RegisterClass(&winClass))
        {
            throw Exception("Win32: Failed to register Window class.");
        }
        m_windowClassName = className;
        m_instance = winInstance;

        RECT windowRect;
        windowRect.left = static_cast<LONG>(0);
        windowRect.right = static_cast<LONG>(size.x);
        windowRect.top = static_cast<LONG>(0);
        windowRect.bottom = static_cast<LONG>(size.y);
        if (!::AdjustWindowRectEx(&windowRect, m_style, FALSE, m_extendedStyle))
        {
            throw Exception("Win32: Failed to adjust window rect.");
        }

        m_window = ::CreateWindowEx(
            m_extendedStyle,
            className.c_str(),
            m_title.c_str(), //stringToWideString(m_title).c_str(),
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | m_style,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            NULL,
            NULL,
            winInstance,
            this);

        if (m_window == NULL)
        {
            throw Exception("Win32: Failed to create window.");
        }
        m_deviceContext = GetDC(m_window);
  
        m_title = title;
        m_initialSize = m_currentSize = size;
    }

    void WindowWin32::Close()
    {
        if (m_deviceContext && !::ReleaseDC(m_window, m_deviceContext))
        {
            throw Exception("Win32: Failed to release device context.");
        }
        m_deviceContext = NULL;

        if (m_window && !::DestroyWindow(m_window))
        {
            throw Exception("Win32: Failed to destroy class.");
        }
        m_window = NULL;
        
        if (m_windowClassName.size() && !::UnregisterClass(m_windowClassName.c_str(), m_instance))
        {
            throw Exception("Win32: Failed to unregister class.");
        }
        m_instance = NULL;

        m_windowClassName.clear();
        
    }

    void WindowWin32::Update()
    {
        MSG message;
        while (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_SYSCOMMAND &&
                message.wParam == SC_KEYMENU)
            {
                break;
            }

            ::TranslateMessage(&message);
            ::DispatchMessage(&message);
        }
    }

    void WindowWin32::Show(const bool show)
    {
        auto flag = show ? true : SW_HIDE;
        ::ShowWindow(m_window, flag);
    }

    void WindowWin32::Hide()
    {
        ::ShowWindow(m_window, SW_HIDE);
    }

    bool WindowWin32::IsOpen() const
    {
        return m_window != NULL;
    }

    HWND WindowWin32::GetWin32Window() const
    {
        return m_window;
    }

    HINSTANCE WindowWin32::GetWin32Instance() const
    {
        return m_instance;
    }

    HDC WindowWin32::GetWin32DeviceContext() const
    {
        return m_deviceContext;
    }

    LRESULT WindowWin32::WindowProcStatic(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_NCCREATE)
        {
            ::SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams);
        }
        else
        {
            WindowWin32* windowPtr = (WindowWin32*)::GetWindowLongPtr(window, GWLP_USERDATA);

            if (windowPtr != NULL)
            {
                return windowPtr->WindowProc(window, message, wParam, lParam);
            }
        }

        return DefWindowProc(window, message, wParam, lParam);
    }

    LRESULT WindowWin32::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CLOSE:
        {
            Close();
            return 0;
        }
        break;
        default: break;
        }

        return ::DefWindowProc(window, message, wParam, lParam);
    }

}

#endif