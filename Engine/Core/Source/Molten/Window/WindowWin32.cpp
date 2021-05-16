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

        return ss.str();
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


    // Win32 drop target implementations.
    DropTargetWin32::DropTargetWin32(WindowWin32& window) :
        m_ref(1),
        m_window(window),
        m_lastDataObject(nullptr)
    {}

    HRESULT DropTargetWin32::Register()
    {
        if(const auto result = ::OleInitialize(0); result != S_OK)
        {
            return result;
        }
        return ::RegisterDragDrop(m_window.GetWin32Window(), this);
    }

    HRESULT STDMETHODCALLTYPE DropTargetWin32::QueryInterface(
        REFIID riid,
        _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject)
    {
        if(riid == IID_IUnknown || riid == IID_IDropTarget)
        {
            *ppvObject = static_cast<IUnknown*>(this);
            AddRef();
            return S_OK;
        }

        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE DropTargetWin32::AddRef()
    {
        return InterlockedIncrement(&m_ref);
    }

    ULONG STDMETHODCALLTYPE DropTargetWin32::Release()
    {
        return InterlockedDecrement(&m_ref);
    }

    HRESULT STDMETHODCALLTYPE DropTargetWin32::DragEnter(
        IDataObject* dataObject,
        DWORD keyboardState,
        POINTL cursorPosition,
        DWORD* effect)
    {
        *effect &= DROPEFFECT_NONE;

        m_lastDataObject = nullptr;

        std::vector<std::filesystem::path> files;
        if (ReadFiles(dataObject, files) != S_OK)
        {
            return S_OK;
        }

        if (files.empty())
        {
            return S_OK;
        }

        if (!m_window.OnFilesDropEnter(files))
        {
            return S_OK;
        }

        *effect &= DROPEFFECT_COPY;
        m_lastDataObject = dataObject;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DropTargetWin32::DragOver(
        DWORD keyboardState,
        POINTL cursorPosition,
        DWORD* effect)
    {
        *effect &= DROPEFFECT_NONE;

        if(!m_lastDataObject)
        {
            return S_OK;
        }

        const auto newPosition = Vector2i32{ cursorPosition.x, cursorPosition.y };
        if(newPosition != m_lastPosition)
        {
            m_window.OnFilesDropMove(newPosition);
            m_lastPosition = newPosition;
        }

        *effect &= DROPEFFECT_COPY;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DropTargetWin32::DragLeave()
    {
        if (!m_lastDataObject)
        {
            return S_OK;
        }

        m_window.OnFilesDropLeave();
        m_lastDataObject = nullptr;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DropTargetWin32::Drop(
        IDataObject* dataObject,
        DWORD keyboardState,
        POINTL cursorPosition,
        DWORD* effect)
    {
        *effect &= DROPEFFECT_NONE;

        if(m_lastDataObject == nullptr)
        {
            return S_OK;
        }

        std::vector<std::filesystem::path> files;
        if(ReadFiles(dataObject, files) != S_OK)
        {       
            return S_OK;
        }

        if(files.empty())
        {
            return S_OK;          
        }

        m_window.OnFilesDrop(files);

        *effect &= DROPEFFECT_COPY;
        m_lastDataObject = nullptr;
        return S_OK;
    }

    HRESULT DropTargetWin32::ReadFiles(
        IDataObject* dataObject,
        std::vector<std::filesystem::path>& files) const
    {
        FORMATETC format = {
            CF_HDROP,
            nullptr,
            DVASPECT_CONTENT,
            -1,
            TYMED_HGLOBAL
        };

        STGMEDIUM medium;
        if(const auto result = dataObject->GetData(&format, &medium); result != S_OK)
        {
            return result;
        }

        const auto drop = static_cast<HDROP>(medium.hGlobal);

        const auto fileCount = DragQueryFile(drop, 0xFFFFFFFF, nullptr, 0);
        files.reserve(fileCount);

        for(UINT i = 0; i < fileCount; i++)
        {
            char filename[MAX_PATH + 1];
            const auto filenameSize = DragQueryFile(drop, i, filename, MAX_PATH);
            if(filenameSize > MAX_PATH)
            {
                // Ignore too long filename.
                continue;
            }

            filename[filenameSize] = 0;
            files.emplace_back(filename);
        }

        return S_OK;
    }
 

    // Dynamic win32 implementations.
    WindowWin32::DynamicFunctions::DynamicFunctions() :
        m_module(nullptr),
    #if (MOLTEN_PLATFORM_WINDOWS_SUPPORT_MULTI_MONITOR_DPI)
        m_setProcessDpiMultiDisplayAwareness(nullptr),
    #endif 
        m_setProcessDpiSingleDisplayAwareness(nullptr)
    {
        // Multi monitor DPI.
    #if (MOLTEN_PLATFORM_WINDOWS_SUPPORT_MULTI_MONITOR_DPI)

        m_module = ::LoadLibrary("Shcore.dll");
        if (m_module != nullptr)
        {
            m_setProcessDpiMultiDisplayAwareness = reinterpret_cast<SetProcessDpiMultiDisplayAwarenessFunc>(::GetProcAddress(m_module, "SetProcessDpiAwareness"));
            if (m_setProcessDpiMultiDisplayAwareness != nullptr)
            {
                return;
            }
        }

    #endif

        // Single monitor DPI.
        m_module = ::LoadLibrary("user32.dll");
        if (m_module != nullptr)
        {
            m_setProcessDpiSingleDisplayAwareness = reinterpret_cast<SetProcessDpiSingleDisplayAwarenessFunc>(::GetProcAddress(m_module, "SetProcessDPIAware"));
        }
    }

    WindowWin32::DynamicFunctions::~DynamicFunctions()
    {
        if (m_module != nullptr)
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
        m_window(nullptr),
        m_instance(nullptr),
        m_deviceContext(nullptr),
        m_style(0),
        m_extendedStyle(0),
        m_windowClassName{},
        m_showing(false),
        m_maximized(false),
        m_minimized(false),
        m_focused(false),
        m_size(0, 0),
        m_position(0, 0),
        m_title{},
        m_dpi(96, 96),
        m_cursor(Mouse::Cursor::Normal),
        m_dropTarget(*this)
    {}

    WindowWin32::WindowWin32(const WindowDescriptor& descriptor) :
        WindowWin32()
    {
        WindowWin32::Open(descriptor);
    }

    WindowWin32::~WindowWin32()
    {
        WindowWin32::Close();
    }

    bool WindowWin32::Open(const WindowDescriptor& descriptor)
    {
        Close();

        m_logger = descriptor.logger;

        m_extendedStyle = WS_EX_APPWINDOW;
        m_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_BORDER;

        auto guid = CreateGuid();
        std::string className = "MoltenEngine_" + guid;

        WNDCLASS winClass;
        HINSTANCE winInstance = ::GetModuleHandle(nullptr);
        winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        winClass.lpfnWndProc = (WNDPROC)WindowWin32::WindowProcStatic;
        winClass.cbClsExtra = 0;
        winClass.cbWndExtra = 0;
        winClass.hInstance = winInstance;
        winClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
        winClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        winClass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
        winClass.lpszClassName = className.c_str();
        winClass.lpszMenuName = nullptr;

        if (!::RegisterClass(&winClass))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to register Window class.");
            return false;
        }
        m_windowClassName = className;
        m_instance = winInstance;

        RECT windowRect;
        windowRect.left = static_cast<LONG>(0);
        windowRect.right = static_cast<LONG>(descriptor.size.x);
        windowRect.top = static_cast<LONG>(0);
        windowRect.bottom = static_cast<LONG>(descriptor.size.y);
        if (!::AdjustWindowRectEx(&windowRect, m_style, FALSE, m_extendedStyle))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to adjust window rect.");
            return false;
        }

        m_window = ::CreateWindowEx(
            m_extendedStyle,
            className.c_str(),
            descriptor.title.c_str(), //stringToWideString(m_title).c_str(),
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | m_style,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,
            nullptr,
            winInstance,
            this);

        if (m_window == nullptr)
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
        const auto dpiX = GetDeviceCaps(m_deviceContext, LOGPIXELSX);
        const auto dpiY = GetDeviceCaps(m_deviceContext, LOGPIXELSY);
        if (dpiX > 0)
        {
            m_dpi.x = static_cast<uint32_t>(dpiX);
        }
        if (dpiY > 0)
        {
            m_dpi.y = static_cast<uint32_t>(dpiY);
        }

        if(descriptor.enableDragAndDrop)
        {
            if(auto result = m_dropTarget.Register(); result != S_OK)
            {
                MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to register dragdrop: " + std::to_string(result));
                return false;
            }
        }

        m_size.x = static_cast<int32_t>(windowRect.right - windowRect.left);
        m_size.y = static_cast<int32_t>(windowRect.bottom - windowRect.top);
        m_position.x = static_cast<int32_t>(windowRect.left);
        m_position.y = static_cast<int32_t>(windowRect.top);      
        m_title = descriptor.title;

        return true;
    }

    void WindowWin32::Close()
    {
        if (m_deviceContext && !::ReleaseDC(m_window, m_deviceContext))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to release window's device context.");
        }
        m_deviceContext = nullptr;

        if (m_window && !::DestroyWindow(m_window))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to destroy window class.");
        }
        m_window = nullptr;
        
        if (m_windowClassName.size() && !::UnregisterClass(m_windowClassName.c_str(), m_instance))
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to unregister window class.");
        }
        m_instance = nullptr;
        
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
        while (::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
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
        return m_window != nullptr;
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

    bool WindowWin32::IsFocused() const
    {
        return m_focused;
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

    void WindowWin32::SetCursor(const Mouse::Cursor cursor)
    {
        m_cursor = cursor;
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

    HWND WindowWin32::GetWin32Window()
    {
        return m_window;
    }

    HDC WindowWin32::GetWin32DeviceContext()
    {
        return m_deviceContext;
    }

    HINSTANCE WindowWin32::GetWin32Instance()
    {
        return m_instance;
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

            if (windowPtr != nullptr)
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
        case WM_SETFOCUS:
        {
            m_focused = true;
        } break;
        case WM_KILLFOCUS:
        {
            m_focused = false;
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
        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT)
            {
                auto cursorName = IDC_ARROW;
                switch (m_cursor)
                {
                    case Mouse::Cursor::SizeLeftRight: cursorName = IDC_SIZEWE; break;
                    case Mouse::Cursor::SizeUpDown: cursorName = IDC_SIZENS; break;
                    case Mouse::Cursor::SizeAll: cursorName = IDC_SIZEALL; break;
                    default: break;
                }

                auto hCursor = ::LoadCursorA(nullptr, cursorName);
                ::SetCursor(hCursor);
                return 0;
            }
           
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