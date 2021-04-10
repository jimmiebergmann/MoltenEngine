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

#include "Molten/Window/WindowX11.hpp"

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX

#include "Molten/System/Exception.hpp"
#include "Molten/Logger.hpp"
#include <cstring>

#define MOLTEN_WINDOW_LOG(severity, message) if(m_logger){ m_logger->Write(severity, message); }

namespace Molten
{

    WindowX11::WindowX11() :
        m_logger(nullptr),
        m_display(NULL),
        m_screen(0),
        m_window(0),
        m_open(false),
        m_showing(false),
        m_maximized(false),
        m_minimized(false),
        m_size(0, 0),
        m_position(0, 0),
        m_title("")
    {
    }

    WindowX11::WindowX11(const std::string& title, const Vector2ui32 size, Logger* logger) :
        WindowX11()
    {
        Open(title, size, logger);
    }

    WindowX11::~WindowX11()
    {
        Close();
    }

    bool WindowX11::Open(const std::string& title, const Vector2ui32 size, Logger* logger)
    {
        Close();

        m_logger = logger;

        if((m_display = XOpenDisplay(NULL)) == NULL)
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to connect to X server.");
            return false;
        }

        ::XInitThreads( );

        m_screen = DefaultScreen(m_display);

        // Creat the window attributes
        XSetWindowAttributes windowAttributes;
        windowAttributes.colormap = DefaultColormap(m_display, m_screen);
        windowAttributes.event_mask =   KeyPressMask | KeyReleaseMask |
                                        ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask |
                                        EnterWindowMask | LeaveWindowMask | VisibilityChangeMask |
                                        FocusChangeMask | ExposureMask | StructureNotifyMask;

        // Create the window
        m_window = ::XCreateWindow(m_display,
                                 DefaultRootWindow(m_display),
                                 0, 0, size.x, size.y,
                                 0,
                                 DefaultDepth(m_display, m_screen),
                                 InputOutput,
                                 DefaultVisual(m_display, m_screen),
                                 CWBorderPixel | CWEventMask | CWColormap,
                                 &windowAttributes);

        if (!m_window)
        {
            MOLTEN_WINDOW_LOG(Logger::Severity::Error, "Failed to create window.");
            return false;
        }

        Atom wmDeleteMessage = XInternAtom(m_display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(m_display, m_window, &wmDeleteMessage, 1);
        XStoreName(m_display, m_window, title.c_str());



        XMapWindow(m_display, m_window);
        XFlush(m_display);

        m_title = title;
        m_size = size;
        // m_position = ?
        m_open = true;

        return true;
    }

    void WindowX11::Close()
    { 
        if(m_display)
        {
            if(m_display)
            {
                XDestroyWindow(m_display, m_window);
                XFlush(m_display);          
            }

            XCloseDisplay(m_display);
        }

        m_logger = nullptr;
        m_open = false;
        m_display = NULL;
        m_screen = 0;
        m_window = 0;
      
        m_showing = false;
        m_maximized = false;
        m_minimized = false;
        m_size = { 0,0 };
        m_position = { 0,0 };
    }

    void WindowX11::Update()
    {
        // ??
        if(!m_open)
        {
            return;
        }
        // ??

        //m_userInput.Begin();

        XEvent event;
        while(XPending(m_display) > 0)
        {
            XNextEvent(m_display, &event);

            switch (event.type)
            {
                case ClientMessage:
                {                  
                    // This is a hacky way of checking if we closed the window
                    if(std::strcmp(XGetAtomName(m_display, event.xclient.message_type), "WM_PROTOCOLS") == 0)
                    {
                        Close();
                        return;
                    }
                } break;
                default: break;
            }
        }
    }

    bool WindowX11::IsOpen() const
    {
        return m_open;
    }

    bool WindowX11::IsShowing() const
    {
        return m_showing;
    }

    bool WindowX11::IsMaximized() const
    {
        return m_maximized;
    }

    bool WindowX11::IsMinimized() const
    {
        return m_minimized;
    }

    bool WindowX11::IsFocused() const
    {
        return true;
    }

    void WindowX11::Show(const bool /*show*/, const bool /*signal*/)
    {
    }

    void WindowX11::Hide(const bool /*signal*/)
    {
    }

    void WindowX11::Maximize(const bool /*signal*/)
    {
    }

    void WindowX11::Minimize(const bool /*signal*/)
    {
    }

    void WindowX11::Move(const Vector2i32& /*position*/, const bool /*signal*/)
    {
    }
    
    void WindowX11::Resize(const Vector2ui32& /*size*/, const bool /*signal*/)
    {
    }

    void WindowX11::SetTitle(const std::string& /*title*/)
    {
    }

    void WindowX11::SetCursor(const Mouse::Cursor /*cursor*/)
    {
    }

    Vector2ui32 WindowX11::GetDpi() const
    {
        return { 96, 96 };
    }

    Vector2f32 WindowX11::GetScale() const
    {
        return { static_cast<float>(96) / 96.0f, static_cast<float>(96) / 96.0f };
    }

    Vector2ui32 WindowX11::GetSize() const
    {
        return m_size;
    }

    Vector2i32 WindowX11::GetPosition() const
    {
        return m_position;
    }

    std::string WindowX11::GetTitle() const
    {
        return m_title;
    }

    UserInput& WindowX11::GetUserInput()
    {
        return m_userInput;
    }
    const UserInput& WindowX11::GetUserInput() const
    {
        return m_userInput;
    }

    ::Display* WindowX11::GetX11DisplayDevice()
    {
        return m_display;
    }

    ::Window WindowX11::GetX11WindowDevice()
    {
        return m_window;
    }

    int WindowX11::GetX11ScreenDevice()
    {
        return m_screen;
    }

}

#endif