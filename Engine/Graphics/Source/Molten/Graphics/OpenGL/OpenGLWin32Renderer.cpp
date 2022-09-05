/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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


#include "Molten/Graphics/OpenGL/OpenGLWin32Renderer.hpp"

#if defined(MOLTEN_ENABLE_OPENGL)
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

#include "Molten/Graphics/OpenGL/OpengGLFunctions.hpp"
#include "Molten/Graphics/Window/Window.hpp"
#include "Molten/Shader/PushConstant.hpp"
#include "Molten/System/Exception.hpp"
#include <array>

namespace Molten
{

    OpenGLWin32Renderer::OpenGLWin32Renderer() :
        m_deviceContext(NULL),
        m_context(NULL)
    {}

    OpenGLWin32Renderer::~OpenGLWin32Renderer()
    {
        Close();
    }

    bool OpenGLWin32Renderer::Open(const RendererDescriptor& descriptor)
    {
        HGLRC temporaryContext = NULL;

        auto deviceContext = descriptor.renderTarget.GetWin32DeviceContext();
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
      
            if (descriptor.version == Version::None)
            {
                Version openedVersion;
                OpenBestVersion(deviceContext, openedVersion);
                m_version = openedVersion;
            }
            else
            {
                OpenVersion(deviceContext, descriptor.version);
                m_version = descriptor.version;
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

    bool OpenGLWin32Renderer::IsOpen() const
    {
        return false;
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

    const RendererCapabilities& OpenGLWin32Renderer::GetCapabilities() const
    {
        static RendererCapabilities tmpCapabilities = {};
        return tmpCapabilities;
    }

    uint32_t OpenGLWin32Renderer::GetPushConstantLocation(Pipeline& /*pipeline*/, const uint32_t /*id*/)
    {
        return PushConstantLocation::UnknownLocation;
    }

    RenderResource<DescriptorSet> OpenGLWin32Renderer::CreateDescriptorSet(const DescriptorSetDescriptor& /*descriptor*/)
    {
        return { };
    }

    RenderResource<FramedDescriptorSet> OpenGLWin32Renderer::CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& /*descriptor*/)
    {
        return { };
    }

    RenderResource<IndexBuffer> OpenGLWin32Renderer::CreateIndexBuffer(const IndexBufferDescriptor& /*descriptor*/)
    {
        return { };
    }

    RenderResource<Pipeline> OpenGLWin32Renderer::CreatePipeline(const PipelineDescriptor& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<RenderPass> OpenGLWin32Renderer::CreateRenderPass(const RenderPassDescriptor& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Sampler1D> OpenGLWin32Renderer::CreateSampler(const SamplerDescriptor1D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Sampler2D> OpenGLWin32Renderer::CreateSampler(const SamplerDescriptor2D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Sampler3D> OpenGLWin32Renderer::CreateSampler(const SamplerDescriptor3D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<ShaderProgram> OpenGLWin32Renderer::CreateShaderProgram(const VisualShaderProgramDescriptor& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Texture1D> OpenGLWin32Renderer::CreateTexture(const TextureDescriptor1D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Texture2D> OpenGLWin32Renderer::CreateTexture(const TextureDescriptor2D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<Texture3D> OpenGLWin32Renderer::CreateTexture(const TextureDescriptor3D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<FramedTexture1D> OpenGLWin32Renderer::CreateFramedTexture(const TextureDescriptor1D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<FramedTexture2D> OpenGLWin32Renderer::CreateFramedTexture(const TextureDescriptor2D& /*descriptor*/)
    {
        return { };
    }

    SharedRenderResource<FramedTexture3D> OpenGLWin32Renderer::CreateFramedTexture(const TextureDescriptor3D& /*descriptor*/)
    {
        return { };
    }

    RenderResource<UniformBuffer> OpenGLWin32Renderer::CreateUniformBuffer(const UniformBufferDescriptor&)
    {
        return { };
    }

    RenderResource<FramedUniformBuffer> OpenGLWin32Renderer::CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& /*descriptor*/)
    {
        return { };
    }

    RenderResource<VertexBuffer> OpenGLWin32Renderer::CreateVertexBuffer(const VertexBufferDescriptor&)
    {
        return { };
    }

    bool OpenGLWin32Renderer::UpdateRenderPass(RenderPass& /*renderPass*/, const RenderPassUpdateDescriptor& /*descriptor*/)
    {
        return false;
    }

    bool OpenGLWin32Renderer::UpdateTexture(Texture1D& /*texture1D*/, const TextureUpdateDescriptor1D& /*descriptor*/)
    {
        return false;
    }

    bool OpenGLWin32Renderer::UpdateTexture(Texture2D& /*texture2D*/, const TextureUpdateDescriptor2D& /*descriptor*/)
    {
        return false;
    }

    bool OpenGLWin32Renderer::UpdateTexture(Texture3D& /*texture3D*/, const TextureUpdateDescriptor3D& /*descriptor*/)
    {
        return false;
    }

    void OpenGLWin32Renderer::UpdateUniformBuffer(RenderResource<UniformBuffer>& /*uniformBuffer*/, const void* /*data*/, const size_t /*size*/, const size_t /*offset*/)
    {
    }

    void OpenGLWin32Renderer::UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& /*framedUniformBuffer*/, const void* /*data*/, const size_t /*size*/, const size_t /*offset*/)
    {
    }

    bool OpenGLWin32Renderer::DrawFrame(const RenderPasses& /*renderPasses*/)
    {
        return false;
    }

    void OpenGLWin32Renderer::Destroy(DescriptorSet& /*descriptorSet*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(FramedDescriptorSet& /*framedDescriptorSet*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(IndexBuffer& /*indexBuffer*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(Pipeline& /*pipeline*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(Sampler1D& /*sampler1D*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(Sampler2D& /*sampler2D*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(Sampler3D& /*sampler3D*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(ShaderProgram& /*shaderProgram*/ )
    {
    }

    void OpenGLWin32Renderer::Destroy(Texture1D& /*texture1D*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(Texture2D& /*texture2D*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(Texture3D& /*texture3D*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(FramedTexture1D& /*framedTexture1D*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(FramedTexture2D& /*framedTexture2D*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(FramedTexture3D& /*framedTexture3D*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(UniformBuffer& /*uniformBuffer*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(FramedUniformBuffer& /*framedUniformBuffer*/)
    {
    }

    void OpenGLWin32Renderer::Destroy(VertexBuffer& /*vertexBuffer*/)
    {
    }

    void OpenGLWin32Renderer::WaitForDevice()
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
            throw Exception("Failed to create OpenGL context version " + ToString(version));
        }

        return true;
    }

    void OpenGLWin32Renderer::OpenBestVersion(HDC deviceContext, Version& version)
    {
        static const std::array versions =
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
