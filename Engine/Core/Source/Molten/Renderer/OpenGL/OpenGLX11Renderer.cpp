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


#include "Molten/Renderer/OpenGL/OpenGLX11Renderer.hpp"

#if defined(MOLTEN_ENABLE_OPENGL)
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX

#include "Molten/Renderer/OpenGL/OpengGLFunctions.hpp"
#include "Molten/Renderer/PushConstant.hpp"
#include "Molten/Window/Window.hpp"
#include "Molten/System/Exception.hpp"

namespace Molten
{

    OpenGLX11Renderer::OpenGLX11Renderer()
    {
    }

    OpenGLX11Renderer::OpenGLX11Renderer(RenderTarget& renderTarget, const Version& version, Logger* logger) :
        OpenGLX11Renderer()
    {
        Open(renderTarget, version, logger);
    }

    OpenGLX11Renderer::~OpenGLX11Renderer()
    {
        Close();
    }

    bool OpenGLX11Renderer::Open(RenderTarget& /*renderTarget*/, const Version& /*version*/, Logger* /*logger*/)
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

    bool OpenGLX11Renderer::IsOpen() const
    {
        return false;
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

    const RendererCapabilities& OpenGLX11Renderer::GetCapabilities() const
    {
        static RendererCapabilities tmpCapabilities = {};
        return tmpCapabilities;
    }

    uint32_t OpenGLX11Renderer::GetPushConstantLocation(Pipeline& /*pipeline*/, const uint32_t /*id*/)
    {
        return PushConstantLocation::UnknownLocation;
    }

    RenderResource<DescriptorSet> OpenGLX11Renderer::CreateDescriptorSet(const DescriptorSetDescriptor& /*descriptor*/)
    {
        return { };
    }

    RenderResource<FramedDescriptorSet> OpenGLX11Renderer::CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& /*descriptor*/)
    {
        return { };
    }

    RenderResource<IndexBuffer> OpenGLX11Renderer::CreateIndexBuffer(const IndexBufferDescriptor& /*descriptor*/)
    {
        return { };
    }

    RenderResource<Pipeline> OpenGLX11Renderer::CreatePipeline(const PipelineDescriptor& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<RenderPass> OpenGLX11Renderer::CreateRenderPass(const RenderPassDescriptor& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Sampler1D> OpenGLX11Renderer::CreateSampler(const SamplerDescriptor1D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Sampler2D> OpenGLX11Renderer::CreateSampler(const SamplerDescriptor2D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Sampler3D> OpenGLX11Renderer::CreateSampler(const SamplerDescriptor3D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<ShaderProgram> OpenGLX11Renderer::CreateShaderProgram(const VisualShaderProgramDescriptor& /*descriptor*/ )
    {
        return { };
    }

    SharedRenderResource<Texture1D> OpenGLX11Renderer::CreateTexture(const TextureDescriptor1D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Texture2D> OpenGLX11Renderer::CreateTexture(const TextureDescriptor2D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Texture3D> OpenGLX11Renderer::CreateTexture(const TextureDescriptor3D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<FramedTexture1D> OpenGLX11Renderer::CreateFramedTexture(const TextureDescriptor1D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<FramedTexture2D> OpenGLX11Renderer::CreateFramedTexture(const TextureDescriptor2D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<FramedTexture3D> OpenGLX11Renderer::CreateFramedTexture(const TextureDescriptor3D& /*descriptor*/)
    {
        return { };
    }

    RenderResource<UniformBuffer> OpenGLX11Renderer::CreateUniformBuffer(const UniformBufferDescriptor&)
    {
        return { };
    }

    RenderResource<FramedUniformBuffer> OpenGLX11Renderer::CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& /*descriptor*/)
    {
        return { };
    }

    RenderResource<VertexBuffer> OpenGLX11Renderer::CreateVertexBuffer(const VertexBufferDescriptor&)
    {
        return { };
    }

    bool OpenGLX11Renderer::UpdateRenderPass(RenderPass& /*renderPass*/, const RenderPassUpdateDescriptor& /*descriptor*/)
    {
        return false;
    }

    bool OpenGLX11Renderer::UpdateTexture(Texture1D& /*texture1D*/, const TextureUpdateDescriptor1D& /*descriptor*/)
    {
        return false;
    }

    bool OpenGLX11Renderer::UpdateTexture(Texture2D& /*texture2D*/, const TextureUpdateDescriptor2D& /*descriptor*/)
    {
        return false;
    }

    bool OpenGLX11Renderer::UpdateTexture(Texture3D& /*texture3D*/, const TextureUpdateDescriptor3D& /*descriptor*/)
    {
        return false;
    }

    void OpenGLX11Renderer::UpdateUniformBuffer(RenderResource<UniformBuffer>& /*uniformBuffer*/, const void* /*data*/, const size_t /*size*/, const size_t /*offset*/)
    {
    }

    void OpenGLX11Renderer::UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& /*framedUniformBuffer*/, const void* /*data*/, const size_t /*size*/, const size_t /*offset*/)
    {
    }

    bool OpenGLX11Renderer::DrawFrame(const RenderPasses& /*renderPasses*/)
    {
        return false;
    }

    void OpenGLX11Renderer::Destroy(DescriptorSet& /*descriptorSet*/)
    {
    }

    void OpenGLX11Renderer::Destroy(FramedDescriptorSet& /*framedDescriptorSet*/)
    {
    }

    void OpenGLX11Renderer::Destroy(IndexBuffer& /*indexBuffer*/)
    {
    }

    void OpenGLX11Renderer::Destroy(Pipeline& /*pipeline*/)
    {
    }

    void OpenGLX11Renderer::Destroy(Sampler1D& /*sampler1D*/)
    {
    }

    void OpenGLX11Renderer::Destroy(Sampler2D& /*sampler2D*/)
    {
    }

    void OpenGLX11Renderer::Destroy(Sampler3D& /*sampler3D*/)
    {
    }

    void OpenGLX11Renderer::Destroy(ShaderProgram& /*shaderProgram*/)
    {
    }

    void OpenGLX11Renderer::Destroy(Texture1D& /*texture1D*/)
    {
    }

    void OpenGLX11Renderer::Destroy(Texture2D& /*texture2D*/)
    {
    }

    void OpenGLX11Renderer::Destroy(Texture3D& /*texture3D*/)
    {
    }

    void OpenGLX11Renderer::Destroy(FramedTexture1D& /*framedTexture1D*/)
    {
    }

    void OpenGLX11Renderer::Destroy(FramedTexture2D& /*framedTexture2D*/)
    {
    }

    void OpenGLX11Renderer::Destroy(FramedTexture3D& /*framedTexture3D*/)
    {
    }

    void OpenGLX11Renderer::Destroy(UniformBuffer& /*uniformBuffer*/)
    {
    }

    void OpenGLX11Renderer::Destroy(FramedUniformBuffer& /*framedUniformBuffer*/)
    {
    }

    void OpenGLX11Renderer::Destroy(VertexBuffer& /*vertexBuffer*/)
    {
    }

    void OpenGLX11Renderer::WaitForDevice()
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