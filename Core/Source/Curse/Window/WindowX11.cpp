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

#include "Curse/Window/WindowX11.hpp"

#if CURSE_PLATFORM == CURSE_PLATFORM_LINUX

#include "Curse/System/Exception.hpp"
#include <cstring>

namespace Curse
{

    WindowX11::WindowX11() :
        m_display(NULL),
        m_screen(0),
        m_window(0),
        m_open(false),
        m_initialSize(0, 0),
        m_currentSize(0, 0),
        m_title("")
    {
    }

    WindowX11::WindowX11(const std::string& title, const Vector2ui32 size) :
        WindowX11()
    {
        Open(title, size);
    }

    WindowX11::~WindowX11()
    {
        Close();
    }

    void WindowX11::Open(const std::string& title, const Vector2ui32 size)
    {
        Close();

        if((m_display = XOpenDisplay(NULL)) == NULL)
        {
            throw Exception("X11: Failed to connect to X server.");
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
            throw Exception("X11: Failed to create window.");
        }

        Atom wmDeleteMessage = XInternAtom(m_display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(m_display, m_window, &wmDeleteMessage, 1);
        XStoreName(m_display, m_window, title.c_str());



        XMapWindow(m_display, m_window);
        XFlush(m_display);

        m_title = title;
        m_initialSize = m_currentSize = size;
        m_open = true;
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

        m_open = false;
        m_display = NULL;
        m_screen = 0;
        m_window = 0;
    }

    void WindowX11::Update()
    {
        if(!m_open)
        {
            return;
        }


        XEvent event;
        while(XPending(m_display) > 0)
        {
            XNextEvent(m_display, &event);

            switch(event.type)
            {
                case ClientMessage:
                {                  
                    // This is ahacky way of checking if we closed the window
                    if(std::strcmp(XGetAtomName(m_display, event.xclient.message_type), "WM_PROTOCOLS") == 0)
                    {

                        Close();
                        return;
                    }
                }
                break;
                default:
                    break;
            }
        }
    }

    void WindowX11::Show(const bool /*show*/)
    {
    }

    void WindowX11::Hide()
    {
    }

    bool WindowX11::IsOpen() const
    {
        return m_open;
    }

    Vector2ui32 WindowX11::GetInitialSize() const
    {
        return m_initialSize;
    }

    Vector2ui32 WindowX11::GetCurrentSize() const
    {
        return m_currentSize;
    }

    ::Display * WindowX11::GetX11DisplayDevice() const
    {
        return m_display;
    }

    ::Window WindowX11::GetX11WindowDevice() const
    {
        return m_window;
    }

    int WindowX11::GetX11ScreenDevice() const
    {
        return m_screen;
    }


}

#endif