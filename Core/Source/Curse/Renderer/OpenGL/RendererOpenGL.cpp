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


#include "Curse/Renderer/OpenGL/RendererOpenGL.hpp"

#if CURSE_OPENGL_IS_AVAILABLE

#include "Curse/Window/WindowBase.hpp"
#include "Curse/System/Exception.hpp"

namespace Curse
{

    RendererOpenGL::RendererOpenGL() :
#if defined(CURSE_PLATFORM_WINDOWS)
        m_deviceContext(NULL),
        m_context(NULL)
#elif defined(CURSE_PLATFORM_LINUX)

#endif
    {
    }

    RendererOpenGL::RendererOpenGL(const WindowBase& window) :
        RendererOpenGL()
    {
        Create(window);
    }

 
    RendererOpenGL::~RendererOpenGL()
    {
    }

    void RendererOpenGL::Create(const WindowBase& window)
    {
        window.GetWin32DeviceContext();
    }

    void RendererOpenGL::Destroy()
    {
    #if defined(CURSE_PLATFORM_WINDOWS)

        if (m_context)
        {
            // Release the context from the current thread
            if (!wglMakeCurrent(NULL, NULL))
            {
                throw Exception("RendererOpenGL: Failed to set current context to null.");
            }

            // Delete the context
            if (!wglDeleteContext(m_context))
            {
                throw Exception("RendererOpenGL: Failed to delete context.");
            }

            m_context = NULL;
        }

    #elif defined(CURSE_PLATFORM_LINUX)

    #endif
    }

}

#endif