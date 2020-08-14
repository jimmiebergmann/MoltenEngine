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

#include "Molten/Window/WindowWin32.hpp"

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

#include "Molten/System/Exception.hpp"
#include "Molten/Logger.hpp"
#include <sstream>

#define MOLTEN_WINDOW_LOG(severity, message) if(m_logger){ m_logger->Write(severity, message); }

namespace Molten
{

    // Static helper functions.
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
        ss << ptr[0] << ptr[1] << "-" << ptr[3] << "-" << ptr[4] << "-" << ptr[5] << ptr[6] << ptr[7];

        return std::move(ss.str());
    }

    static bool IsExtendedKey(const LPARAM lParam)
    {
        return lParam & 0x1000000ULL;
    }

    static void ConvertExtendedWin32Key(Keyboard::Key& key, const LPARAM lParam)
    {
        switch (key)
        {
        case Keyboard::Key::ControlLeft:
        {
            if (IsExtendedKey(lParam))
            {
                key = Keyboard::Key::ControlRight;
            }
        } break;
        default: break;
        }
    }

    // Dynamic win32 implementations.
    WindowWin32::DynamicFunctions::DynamicFunctions() :
        m_module(NULL),
    #if (MOLTEN_PLATFORM_WINDOWS_SUPPORT_MULTI_MONITOR_DPI)
        m_setProcessDpiMultiDisplayAwareness(NULL),
    #endif 
        m_setProcessDpiSingleDisplayAwareness(NULL)
    {
        // Multi monitor DPI.
    #if (MOLTEN_PLATFORM_WINDOWS_SUPPORT_MULTI_MONITOR_DPI)

        m_module = ::LoadLibrary("Shcore.dll");
        if (m_module != NULL)
        {
            m_setProcessDpiMultiDisplayAwareness = reinterpret_cast<SetProcessDpiMultiDisplayAwarenessFunc>(::GetProcAddress(m_module, "SetProcessDpiAwareness"));
            if (m_setProcessDpiMultiDisplayAwareness != NULL)
            {
                return;
            }
        }

    #endif

        // Single monitor DPI.
        m_module = ::LoadLibrary("user32.dll");
        if (m_module != NULL)
        {
            m_setProcessDpiSingleDisplayAwareness = reinterpret_cast<SetProcessDpiSingleDisplayAwarenessFunc>(::GetProcAddress(m_module, "SetProcessDPIAware"));
        }
    }

    WindowWin32::DynamicFunctions::~DynamicFunctions()
    {
        if (m_module != NULL)
        {
            ::FreeLibrary(m_module);
        }
    }

    bool WindowWin32::DynamicFunctions::SetProcessDPIAware()
    {
    #if (MOLTEN_PLATFORM_WINDOWS_SUPPORT_MULTI_MONITOR_DPI)
        if (m_setProcessDpiMultiDisplayAwareness)
        {
            auto ret = m_setProcessDpiMultiDisplayAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
            return ret == S_OK || ret == E_ACCESSDENIED;
        }
        
    #else
        if (m_setProcessDpiSingleDisplayAwareness)
        {
            return m_setProcessDpiSingleDisplayAwareness() != 0;
        }
    #endif
        
        return false;
    }

    WindowWin32::DynamicFunctions WindowWin32::s_DynamicFunctions;


    // Win32 window implementations.
    WindowWin32::WindowWin32() :
        m_logger(nullptr),
        m_window(NULL),
        m_instance(NULL),
        m_deviceContext(NULL),
        m_style(0),
        m_extendedStyle(0),
        m_windowClassName(""),
        m_showing(false),
        m_maximized(false),
        m_minimized(false),
        m_size(0, 0),
        m_position(0, 0),
        m_title(""),
        m_dpi(96, 96)
    {
    }

    WindowWin32::WindowWin32(const std::string& title, const Vector2ui32 size, Logger* logger) :
        WindowWin32()
    {
        Open(title, size, logger);
    }

    WindowWin32::~WindowWin32()
    {
        Close();
    }

    bool WindowWin32::Open(const std::string& title, const Vector2ui32 size, Logger* logger)
    {
        Close();

        m_logger = logger;

        m_extendedStyle = WS_EX_APPWINDOW;
        m_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_BORDER;

        auto guid = CreateGuid();
        std::string className = "MoltenEngine_" + guid;

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
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to register Window class.");
            return false;
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
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to adjust window rect.");
            return false;
        }

        m_window = ::CreateWindowEx(
            m_extendedStyle,
            className.c_str(),
            title.c_str(), //stringToWideString(m_title).c_str(),
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
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to create window.");
            return false;
        }
        m_deviceContext = GetDC(m_window);
  
        if (!(::GetWindowRect(m_window, &windowRect)))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to retreive window rect.");
            return false;
        }

        m_dpi = { 1, 1 };
        auto dpiX = GetDeviceCaps(m_deviceContext, LOGPIXELSX);
        auto dpiY = GetDeviceCaps(m_deviceContext, LOGPIXELSY);
        if (dpiX > 0)
        {
            m_dpi.x = static_cast<uint32_t>(dpiX);
        }
        if (dpiY > 0)
        {
            m_dpi.y = static_cast<uint32_t>(dpiY);
        }

        m_size.x = static_cast<int32_t>(windowRect.right - windowRect.left);
        m_size.y = static_cast<int32_t>(windowRect.bottom - windowRect.top);
        m_position.x = static_cast<int32_t>(windowRect.left);
        m_position.y = static_cast<int32_t>(windowRect.top);      
        m_title = title;

        return true;
    }

    void WindowWin32::Close()
    {
        if (m_deviceContext && !::ReleaseDC(m_window, m_deviceContext))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to release window's device context.");
        }
        m_deviceContext = NULL;

        if (m_window && !::DestroyWindow(m_window))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to destroy window class.");
        }
        m_window = NULL;
        
        if (m_windowClassName.size() && !::UnregisterClass(m_windowClassName.c_str(), m_instance))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to unregister window class.");
        }
        m_instance = NULL;
        
        m_logger = nullptr;
        m_windowClassName.clear();
        
        m_showing = false;
        m_maximized = false;
        m_minimized = false;
        m_size = { 0,0 };
        m_position = { 0,0 };       
    }

    void WindowWin32::Update()
    {       
        m_userInput.Begin();

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

        m_userInput.End();
    }

    void WindowWin32::Show(const bool show, const bool signal)
    {
        if (m_showing == show)
        {
            return;
        }

        m_showing = show;

        auto flag = show ? SW_SHOW : SW_HIDE;
        ::ShowWindow(m_window, flag);

        if (signal)
        {
            OnShow(m_showing);
        }
    }

    void WindowWin32::Hide(const bool signal)
    {
        if (!m_showing)
        {
            return;
        }

        m_showing = false;
        ::ShowWindow(m_window, SW_HIDE);

        if (signal)
        {
            OnShow(m_showing);
        }
    }

    bool WindowWin32::IsOpen() const
    {
        return m_window != NULL;
    }

    bool WindowWin32::IsShowing() const
    {
        return m_showing;
    }

    bool WindowWin32::IsMaximized() const
    {
        return m_maximized;
    }

    bool WindowWin32::IsMinimized() const
    {
        return m_minimized;
    }

    void WindowWin32::Maximize(const bool /*signal*/)
    {
        if (!m_window)
        {
            return;
        }
    }

    void WindowWin32::Minimize(const bool /*signal*/)
    {
        if (!m_window)
        {
            return;
        }
    }

    void WindowWin32::Move(const Vector2i32& position, const bool signal)
    {
        if (!m_window || position == m_position)
        {
            return;
        }

        m_position = position;

        const UINT flags = SWP_NOOWNERZORDER | SWP_NOSIZE;
        if (!(::SetWindowPos(m_window, HWND_NOTOPMOST, m_position.x, m_position.y, 0, 0, flags)))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to move window.");
            return;
        }

        if (signal)
        {
            OnMove(m_position);
        }
    }

    void WindowWin32::Resize(const Vector2ui32& size, const bool signal)
    {
        if (!m_window || size == m_size)
        {
            return;
        }

        m_size = size;

        const UINT flags = SWP_NOOWNERZORDER | SWP_NOMOVE;
        if (!(::SetWindowPos(m_window, HWND_NOTOPMOST, 0, 0, m_size.x, m_size.y, flags)))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to resize window.");
            return;
        }

        if (signal)
        {
            OnResize(m_size);
        }
    }

    void WindowWin32::SetTitle(const std::string& title)
    {
        ::SetWindowText(m_window, title.c_str());
        m_title = title;
    }

    Vector2ui32 WindowWin32::GetDpi() const
    {
        return m_dpi;
    }

    Vector2f32 WindowWin32::GetScale() const
    {
        return { static_cast<float>(m_dpi.x) / 96.0f, static_cast<float>(m_dpi.y) / 96.0f };
    }

    Vector2ui32 WindowWin32::GetSize() const
    {
        return m_size;
    }

    Vector2i32 WindowWin32::GetPosition() const
    {
        return m_position;
    }

    std::string WindowWin32::GetTitle() const
    {
        return m_title;
    }

    UserInput& WindowWin32::GetUserInput()
    {
        return m_userInput;
    }
    const UserInput& WindowWin32::GetUserInput() const
    {
        return m_userInput;
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

        // Window events.
        case WM_CREATE:
        {
            if (!s_DynamicFunctions.SetProcessDPIAware())
            {
                MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to make window DPI aware. Make sure user32.dll is available.");
            }
        } break;
        case WM_CLOSE:
        {
            Close();
            return 0;
        } break;
        case WM_SHOWWINDOW:
        {
            const bool status = wParam ? true : false;
            if (status == m_showing)
            {
                break;
            }

            m_showing = status;
            OnShow(m_showing);
        } break;
#if defined(WM_DPICHANGED)
        case WM_DPICHANGED:
        {
            Vector2ui32 dpi = { 1, 1 };
            auto dpiX = LOWORD(wParam);
            auto dpiY = HIWORD(wParam);
            if (dpiX > 0)
            {
                dpi.x = static_cast<uint32_t>(dpiX);
            }
            if (dpiY > 0)
            {
                dpi.y = static_cast<uint32_t>(dpiY);
            }

            if (dpi != m_dpi)
            {
                m_dpi = dpi;
                Vector2f32 scale = { static_cast<float>(m_dpi.x) / 96.0f, static_cast<float>(m_dpi.y) / 96.0f };

                OnDpiChange(m_dpi);
                OnScaleChange(scale);

                RECT* rect = reinterpret_cast<RECT*>(lParam);
                if (rect)
                {
                    if (!(::SetWindowPos(m_window, HWND_TOP,
                                            static_cast<int>(rect->left),
                                            static_cast<int>(rect->top),
                                            static_cast<int>(rect->right) - static_cast<int>(rect->left),
                                            static_cast<int>(rect->bottom) - static_cast<int>(rect->top),
                                            0)))
                    {
                        MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to resize window accordingly to new DPI.");
                    }
                }
                   
            }
        } break;
#endif
        case WM_SIZE:
        {
            const Vector2i32 windowSize =
            {
                static_cast<int32_t>(LOWORD(lParam)),
                static_cast<int32_t>(HIWORD(lParam))
            };

            const auto oldSize = m_size;

            m_size =
            {
                windowSize.x < 0 ? 0 : windowSize.x,
                windowSize.y < 0 ? 0 : windowSize.y
            };

            switch (wParam)
            {
            case SIZE_RESTORED:
            {
                if (oldSize == m_size && !m_maximized && !m_minimized)
                {
                    break;
                }

                m_maximized = false;
                m_minimized = false;

                OnResize(m_size);
            } break;
            case SIZE_MAXIMIZED:
            {
                if (m_maximized)
                {
                    break;
                }
                m_maximized = true;
                m_minimized = false;
                OnMaximize(m_size);              
            } break;
            case SIZE_MINIMIZED:
            {
                if (m_minimized)
                {
                    break;
                }
                m_maximized = false;
                m_minimized = true;
                OnMinimize(m_size);
            } break;
            default: break;
            }
        } break;
        case WM_MOVE:
        {
            const Vector2i32 position =
            {
                static_cast<int32_t>(LOWORD(lParam)),
                static_cast<int32_t>(HIWORD(lParam))
            };

            if (position == m_position)
            {
                break;
            }

            m_position = position;
            OnMove(m_position);
        } break;
        case WM_ERASEBKGND: return 0;
        // Keyboard events.
        case WM_KEYDOWN:
        {
            // AltGr breaks this... Control + Alt is being pressed, but only alt is being released.

            uint32_t win32Key = static_cast<uint32_t>(wParam);
            Keyboard::Key key;
            if (m_userInput.ConvertFromWin32Key(win32Key, key))
            {
                ConvertExtendedWin32Key(key, lParam);
                m_userInput.PressKey(key);
                return 0;
            }
        } break;
        case WM_KEYUP:
        {
            uint32_t win32Key = static_cast<uint32_t>(wParam);
            Keyboard::Key key;
            if (m_userInput.ConvertFromWin32Key(win32Key, key))
            {
                ConvertExtendedWin32Key(key, lParam);
                m_userInput.ReleaseKey(key);
                return 0;
            }
        } break;
        // Mouse events.
        case WM_MOUSEMOVE:
        {
            auto position = Vector2i32(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            m_userInput.MoveMouse(position);
            return 0;
        } break;
        case WM_LBUTTONDOWN:
        {
            auto position = Vector2i32(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            m_userInput.PressMouseButton(Mouse::Button::Left, position);
            return 0;
        } break;
        case WM_MBUTTONDOWN:
        {
            auto position = Vector2i32(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            m_userInput.PressMouseButton(Mouse::Button::Middle, position);
            return 0;
        } break;
        case WM_RBUTTONDOWN:
        {
            auto position = Vector2i32(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            m_userInput.PressMouseButton(Mouse::Button::Right, position);
            return 0;
        } break;
        case WM_XBUTTONDOWN:
        {
            auto position = Vector2i32(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            const auto button = HIWORD(wParam);
            switch (button)
            {
            case XBUTTON1: m_userInput.PressMouseButton(Mouse::Button::Backward, position); return 0;
            case XBUTTON2: m_userInput.PressMouseButton(Mouse::Button::Forward, position); return 0;
            default: break;
            }
        } break;
        case WM_LBUTTONUP:
        {
            auto position = Vector2i32(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            m_userInput.ReleaseMouseButton(Mouse::Button::Left, position);
            return 0;
        } break;
        case WM_MBUTTONUP:
        {
            auto position = Vector2i32(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            m_userInput.ReleaseMouseButton(Mouse::Button::Middle, position);
            return 0;
        } break;
        case WM_RBUTTONUP:
        {
            auto position = Vector2i32(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            m_userInput.ReleaseMouseButton(Mouse::Button::Right, position);
            return 0;
        } break;
        case WM_XBUTTONUP:
        {
            auto position = Vector2i32(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            const auto button = HIWORD(wParam);
            switch (button)
            {
            case XBUTTON1: m_userInput.ReleaseMouseButton(Mouse::Button::Backward, position); return 0;
            case XBUTTON2: m_userInput.ReleaseMouseButton(Mouse::Button::Forward, position); return 0;
            default: break;
            }
        } break;
        default: break;
        }

        return ::DefWindowProc(window, message, wParam, lParam);
    }

}

#endif