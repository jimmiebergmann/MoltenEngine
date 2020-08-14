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


#include "Molten/Renderer/OpenGL/OpenGLX11Renderer.hpp"

#if defined(MOLTEN_ENABLE_OPENGL)
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX

#include "Molten/Renderer/OpenGL/OpengGLFunctions.hpp"
#include "Molten/Window/Window.hpp"
#include "Molten/System/Exception.hpp"

namespace Molten
{

    OpenGLX11Renderer::OpenGLX11Renderer()
    {
    }

    OpenGLX11Renderer::OpenGLX11Renderer(const Window& window, const Version& version, Logger* logger) :
        OpenGLX11Renderer()
    {
        Open(window, version, logger);
    }

    OpenGLX11Renderer::~OpenGLX11Renderer()
    {
        Close();
    }

    bool OpenGLX11Renderer::Open(const Window& /*window*/, const Version& /*version*/, Logger* /*logger*/)
    {
        /*HGLRC temporaryContext = NULL;

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

        OpenGL::BindOpenGLExtensions();*/

        return false;
    }

    void OpenGLX11Renderer::Close()
    {
        /*if (m_context)
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
        }*/
    }

    void OpenGLX11Renderer::Resize(const Vector2ui32& /*size*/)
    {
    }

    Renderer::BackendApi OpenGLX11Renderer::GetBackendApi() const
    {
        return Renderer::BackendApi::OpenGL;
    }

    Version OpenGLX11Renderer::GetVersion() const
    {
        return m_version;
    }

    uint32_t OpenGLX11Renderer::GetPushConstantLocation(Pipeline* /*pipeline*/, const uint32_t /*id*/)
    {
        return 0;
    }

    //std::vector<uint8_t> OpenGLX11Renderer::CompileShader(const ShaderFormat /*inputFormat*/, const ShaderType /*inputType*/,
    //                                                      const std::vector<uint8_t>& /*inputData*/, const ShaderFormat /*outputFormat*/)
    //{
    //    return {};
    //}

    Framebuffer* OpenGLX11Renderer::CreateFramebuffer(const FramebufferDescriptor&)
    {
        return nullptr;
    }

    IndexBuffer* OpenGLX11Renderer::CreateIndexBuffer(const IndexBufferDescriptor& /*descriptor*/)
    {
        return nullptr;
    }

    Pipeline* OpenGLX11Renderer::CreatePipeline(const PipelineDescriptor& /*descriptor*/)
    {
        return nullptr;
    }

    Texture* OpenGLX11Renderer::CreateTexture()
    {
        return nullptr;
    }

    UniformBlock* OpenGLX11Renderer::CreateUniformBlock(const UniformBlockDescriptor&)
    {
        return nullptr;
    }

    UniformBuffer* OpenGLX11Renderer::CreateUniformBuffer(const UniformBufferDescriptor&)
    {
        return nullptr;
    }

    VertexBuffer* OpenGLX11Renderer::CreateVertexBuffer(const VertexBufferDescriptor&)
    {
        return nullptr;
    }

    void OpenGLX11Renderer::DestroyFramebuffer(Framebuffer*)
    {
    }

    void OpenGLX11Renderer::DestroyIndexBuffer(IndexBuffer* /*indexBuffer*/)
    {
    }

    void OpenGLX11Renderer::DestroyPipeline(Pipeline* /*shader*/)
    {
    }

    void OpenGLX11Renderer::DestroyTexture(Texture*)
    {
    }

    void OpenGLX11Renderer::DestroyUniformBlock(UniformBlock* /*uniformBlock*/)
    {
    }

    void OpenGLX11Renderer::DestroyUniformBuffer(UniformBuffer* /*uniformBuffer*/)
    {
    }

    void OpenGLX11Renderer::DestroyVertexBuffer(VertexBuffer* /*vertexBuffer*/)
    {
    }

    void OpenGLX11Renderer::BindPipeline(Pipeline* /*pipeline*/)
    {
    }

    void OpenGLX11Renderer::BindUniformBlock(UniformBlock* /*uniformBlock*/, const uint32_t /*offset*/)
    {
    }

    void OpenGLX11Renderer::BeginDraw()
    {
    }

    void OpenGLX11Renderer::DrawVertexBuffer(VertexBuffer* /*vertexBuffer*/)
    {
    }

    void OpenGLX11Renderer::DrawVertexBuffer(IndexBuffer* /*indexBuffer*/, VertexBuffer* /*vertexBuffer*/)
    {
    }

    void OpenGLX11Renderer::PushConstant(const uint32_t /*location*/, const bool& /*value*/)
    {
    }
    void OpenGLX11Renderer::PushConstant(const uint32_t /*location*/, const int32_t& /*value*/)
    {
    }
    void OpenGLX11Renderer::PushConstant(const uint32_t /*location*/, const float& /*value*/)
    {
    }
    void OpenGLX11Renderer::PushConstant(const uint32_t /*location*/, const Vector2f32& /*value*/)
    {
    }
    void OpenGLX11Renderer::PushConstant(const uint32_t /*location*/, const Vector3f32& /*value*/)
    {
    }
    void OpenGLX11Renderer::PushConstant(const uint32_t /*location*/, const Vector4f32& /*value*/)
    {
    }
    void OpenGLX11Renderer::PushConstant(const uint32_t /*location*/, const Matrix4x4f32& /*value*/)
    {
    }

    void OpenGLX11Renderer::EndDraw()
    {
    }

    void OpenGLX11Renderer::WaitForDevice()
    {
    }

    void OpenGLX11Renderer::UpdateUniformBuffer(UniformBuffer* /*uniformBuffer*/, const size_t /*offset*/, const size_t /*size*/, const void* /*data*/)
    {
    }

    /*bool RendererOpenGLWin32::OpenVersion(HDC deviceContext, const Version& version)
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
    }*/

    /*void RendererOpenGLWin32::OpenBestVersion(HDC deviceContext, Version& version)
    {

        // USE VECTOR AND USE FOR EACH LOOP!

        static const size_t versionCount = 13;
        static const Version versions[versionCount] =
        {
            Version(4, 9),
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

        for (size_t i = 0; i < versionCount; i++)
        {
            try
            {
                if (OpenVersion(deviceContext, versions[i]))
                {
                    version = versions[i];
                    return;
                }
            }
            catch (Exception & e)
            {
                if (i < versionCount - 1)
                {
                    continue;
                }
                throw Exception("RendererOpenGLWin32: Failed to create best OpenGL context, last error: " + e.GetMessage());
            }
        }
    }*/

}

#endif

#endif