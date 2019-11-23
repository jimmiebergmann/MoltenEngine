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


#include "Curse/Renderer/OpenGL/RendererOpenGLWin32.hpp"

#if defined(CURSE_ENABLE_OPENGL)
#if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS

#include "Curse/Renderer/OpenGL/OpengGLFunctions.hpp"
#include "Curse/Window/WindowBase.hpp"
#include "Curse/System/Exception.hpp"
#include <vector>

namespace Curse
{

    RendererOpenGLWin32::RendererOpenGLWin32() :
        m_deviceContext(NULL),
        m_context(NULL)
    {
    }

    RendererOpenGLWin32::RendererOpenGLWin32(const WindowBase& window, const Version& version, DebugCallback debugCallback) :
        RendererOpenGLWin32()
    {
        Open(window, version, debugCallback);
    }

    RendererOpenGLWin32::~RendererOpenGLWin32()
    {
        Close();
    }

    void RendererOpenGLWin32::Open(const WindowBase& window, const Version& version, DebugCallback /*debugCallback*/)
    {
        HGLRC temporaryContext = NULL;

        auto deviceContext = window.GetWin32DeviceContext();
        if (deviceContext == NULL)
        {
            throw Exception("RendererOpenGLWin32: Device context of parameter \"window\" is null.");
        }

        try
        {
            static PIXELFORMATDESCRIPTOR pixelFormatDescriptor =
            {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW |
                PFD_SUPPORT_OPENGL |
                PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA,
                24,
                0, 0, 0, 0, 0, 0,
                0,
                0,
                0,
                0, 0, 0, 0,
                16, //DepthBits,
                8, //StencilBits,
                0,
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
            };

            // Choose and set the pixel format
            GLuint pixelFormat;

            if ((pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDescriptor)) == 0)
            {
                throw Exception("RendererOpenGLWin32: Failed to choose pixel format for Win32 device context.");
            }
            if ((SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDescriptor)) == false)
            {
                throw Exception("RendererOpenGLWin32: Failed to set pixel format for Win32 device context.");
            }

            temporaryContext = ::wglCreateContext(deviceContext);
            if (temporaryContext == NULL)
            {
                throw Exception("RendererOpenGLWin32: Failed to create primitive Win32 OpenGL context.");
            }

            ::wglMakeCurrent(NULL, NULL);
            ::wglMakeCurrent(deviceContext, temporaryContext);
      
            if (version == Version::None)
            {
                Version openedVersion;
                OpenBestVersion(deviceContext, openedVersion);
                m_version = openedVersion;
            }
            else
            {
                OpenVersion(deviceContext, version);
                m_version = version;
            }
        }
        catch (Exception &)
        {
            if (temporaryContext)
            {
                ::wglDeleteContext(temporaryContext);
            }

            ::wglMakeCurrent(NULL, NULL);

            throw;
        }
        catch (...)
        {

            throw;
        }

        OpenGL::BindOpenGLExtensions();
    }

    void RendererOpenGLWin32::Close()
    {
        if (m_context)
        {
            // Release the context from the current thread
            if (!wglMakeCurrent(NULL, NULL))
            {
                throw Exception("RendererOpenGLWin32: Failed to set current context to null.");
            }

            // Delete the context
            if (!wglDeleteContext(m_context))
            {
                throw Exception("RendererOpenGLWin32: Failed to delete context.");
            }

            m_context = NULL;
        }
    }

    Renderer::BackendApi RendererOpenGLWin32::GetBackendApi() const
    {
        return Renderer::BackendApi::OpenGL;
    }

    Version RendererOpenGLWin32::GetVersion() const
    {
        return m_version;
    }

    void RendererOpenGLWin32::SwapBuffers()
    {
        ::SwapBuffers(m_deviceContext);
    }

    Shader* RendererOpenGLWin32::CreateShader(const ShaderDescriptor& )
    {
        return nullptr;
    }

    void RendererOpenGLWin32::DeleteShader(Shader* )
    {
    }

    Texture* RendererOpenGLWin32::CreateTexture()
    {
        return nullptr;
    }

    void RendererOpenGLWin32::DeleteTexture(Texture* )
    {
    }

    bool RendererOpenGLWin32::OpenVersion(HDC deviceContext, const Version& version)
    {
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
        if ((wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB")) == NULL)
        {
            throw Exception("Cannot get address of wglCreateContextAttribsARB.");
        }

        const int attributes[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, static_cast<int>(version.Major),
            WGL_CONTEXT_MINOR_VERSION_ARB, static_cast<int>(version.Minor),
            0
        };

        if ((m_context = wglCreateContextAttribsARB(deviceContext, 0, attributes)) == NULL)
        {
            throw Exception("Failed to create OpenGL context version " + version.AsString());
        }

        return true;
    }

    void RendererOpenGLWin32::OpenBestVersion(HDC deviceContext, Version& version)
    {
        static const std::vector<Version> versions =
        {
            Version(4, 6),
            Version(4, 5),
            Version(4, 4),
            Version(4, 3),
            Version(4, 2),
            Version(4, 1),
            Version(4, 0),
            Version(3, 3),
            Version(3, 2),
            Version(3, 1),
            Version(3, 0),
            Version(2, 1),
            Version(2, 0)
        };

        version = Version::None;

        for(auto it = versions.begin(); it != versions.end(); it++)
        {
            try
            {
                const Version& ver = *it;
                if (OpenVersion(deviceContext, ver))
                {
                    version = ver;
                    return;
                }
            }
            catch (Exception & e)
            {
                if(std::next(it) != versions.end())
                {
                    continue;
                }
                throw Exception("RendererOpenGLWin32: Failed to create best OpenGL context, last error: " + e.GetMessage());
            }
        }
    }

}

#endif

#endif