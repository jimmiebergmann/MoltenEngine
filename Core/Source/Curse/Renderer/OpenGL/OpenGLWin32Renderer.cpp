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


#include "Curse/Renderer/OpenGL/OpenGLWin32Renderer.hpp"

#if defined(CURSE_ENABLE_OPENGL)
#if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS

#include "Curse/Renderer/OpenGL/OpengGLFunctions.hpp"
#include "Curse/Window/Window.hpp"
#include "Curse/System/Exception.hpp"
#include <vector>

namespace Curse
{

    OpenGLWin32Renderer::OpenGLWin32Renderer() :
        m_deviceContext(NULL),
        m_context(NULL)
    {
    }

    OpenGLWin32Renderer::OpenGLWin32Renderer(const Window& window, const Version& version, Logger* logger) :
        OpenGLWin32Renderer()
    {
        Open(window, version, logger);
    }

    OpenGLWin32Renderer::~OpenGLWin32Renderer()
    {
        Close();
    }

    bool OpenGLWin32Renderer::Open(const Window& window, const Version& version, Logger* /*logger*/)
    {
        HGLRC temporaryContext = NULL;

        auto deviceContext = window.GetWin32DeviceContext();
        if (deviceContext == NULL)
        {
            throw Exception("OpenGLWin32Renderer: Device context of parameter \"window\" is null.");
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
                throw Exception("OpenGLWin32Renderer: Failed to choose pixel format for Win32 device context.");
            }
            if ((SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDescriptor)) == false)
            {
                throw Exception("OpenGLWin32Renderer: Failed to set pixel format for Win32 device context.");
            }

            temporaryContext = ::wglCreateContext(deviceContext);
            if (temporaryContext == NULL)
            {
                throw Exception("OpenGLWin32Renderer: Failed to create primitive Win32 OpenGL context.");
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

        return false;
    }

    void OpenGLWin32Renderer::Close()
    {
        if (m_context)
        {
            // Release the context from the current thread
            if (!wglMakeCurrent(NULL, NULL))
            {
                throw Exception("OpenGLWin32Renderer: Failed to set current context to null.");
            }

            // Delete the context
            if (!wglDeleteContext(m_context))
            {
                throw Exception("OpenGLWin32Renderer: Failed to delete context.");
            }

            m_context = NULL;
        }
    }

    void OpenGLWin32Renderer::Resize(const Vector2ui32& /*size*/)
    {
    }

    Renderer::BackendApi OpenGLWin32Renderer::GetBackendApi() const
    {
        return Renderer::BackendApi::OpenGL;
    }

    Version OpenGLWin32Renderer::GetVersion() const
    {
        return m_version;
    }

    //std::vector<uint8_t> OpenGLWin32Renderer::CompileShaderProgram(const ShaderFormat /*inputFormat*/, const ShaderType /*inputType*/,
    //                                                              const std::vector<uint8_t>& /*inputData*/, const ShaderFormat /*outputFormat*/)
    //{
    //    return {};
    //}

    Framebuffer* OpenGLWin32Renderer::CreateFramebuffer(const FramebufferDescriptor&)
    {
        return nullptr;
    }

    IndexBuffer* OpenGLWin32Renderer::CreateIndexBuffer(const IndexBufferDescriptor& /*descriptor*/)
    {
        return nullptr;
    }

    Pipeline* OpenGLWin32Renderer::CreatePipeline(const PipelineDescriptor& /*descriptor*/)
    {
        return nullptr;
    }

    Shader::VertexStage* OpenGLWin32Renderer::CreateVertexShaderStage(const Shader::Visual::VertexScript& /*script*/)
    {
        return nullptr;
    }

    Shader::FragmentStage* OpenGLWin32Renderer::CreateFragmentShaderStage(const Shader::Visual::FragmentScript& /*script*/)
    {
        return nullptr;
    }

    Texture* OpenGLWin32Renderer::CreateTexture()
    {
        return nullptr;
    }

    UniformBlock* OpenGLWin32Renderer::CreateUniformBlock(const UniformBlockDescriptor&)
    {
        return nullptr;
    }

    UniformBuffer* OpenGLWin32Renderer::CreateUniformBuffer(const UniformBufferDescriptor&)
    {
        return nullptr;
    }

    VertexBuffer* OpenGLWin32Renderer::CreateVertexBuffer(const VertexBufferDescriptor&)
    {
        return nullptr;
    }

    void OpenGLWin32Renderer::DestroyFramebuffer(Framebuffer*)
    {
    }

    void OpenGLWin32Renderer::DestroyIndexBuffer(IndexBuffer* /*indexBuffer*/)
    {
    }

    void OpenGLWin32Renderer::DestroyPipeline(Pipeline* /*shader*/)
    {
    }

    void OpenGLWin32Renderer::DestroyVertexShaderStage(Shader::VertexStage*)
    {
    }

    void OpenGLWin32Renderer::DestroyFragmentShaderStage(Shader::FragmentStage*)
    {
    }

    void OpenGLWin32Renderer::DestroyTexture(Texture* )
    {
    }

    void OpenGLWin32Renderer::DestroyUniformBlock(UniformBlock* uniformBlock)
    {
    }

    void OpenGLWin32Renderer::DestroyUniformBuffer(UniformBuffer* uniformBuffer)
    {
    }

    void OpenGLWin32Renderer::DestroyVertexBuffer(VertexBuffer* /*vertexBuffer*/)
    {
    }

    void OpenGLWin32Renderer::BindPipeline(Pipeline* /*pipeline*/)
    {
    }

    void OpenGLWin32Renderer::BindUniformBlock(UniformBlock* /*uniformBlock*/, const uint32_t /*offset*/)
    {
    }

    void OpenGLWin32Renderer::BeginDraw()
    {
    }

    void OpenGLWin32Renderer::DrawVertexBuffer(VertexBuffer* /*vertexBuffer*/)
    {
    }

    void OpenGLWin32Renderer::DrawVertexBuffer(IndexBuffer* /*indexBuffer*/, VertexBuffer* /*vertexBuffer*/)
    {
    }

    void OpenGLWin32Renderer::PushShaderConstants(Shader::Type /*stage*/, const uint32_t /*offset*/, const uint32_t size, const void* /*data*/)
    {
    }

    void OpenGLWin32Renderer::EndDraw()
    {
    }

    void OpenGLWin32Renderer::WaitForDevice()
    {
    }

    void OpenGLWin32Renderer::UpdateUniformBuffer(UniformBuffer* /*uniformBuffer*/, const size_t /*offset*/, const size_t /*size*/, const void* /*data*/)
    {
    }

    bool OpenGLWin32Renderer::OpenVersion(HDC deviceContext, const Version& version)
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

    void OpenGLWin32Renderer::OpenBestVersion(HDC deviceContext, Version& version)
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
                throw Exception("OpenGLWin32Renderer: Failed to create best OpenGL context, last error: " + e.GetMessage());
            }
        }
    }

}

#endif

#endif