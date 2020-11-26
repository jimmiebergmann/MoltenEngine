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

#ifndef MOLTEN_CORE_RENDERER_OPENGL_OPENGLWIN32RENDERER_HPP
#define MOLTEN_CORE_RENDERER_OPENGL_OPENGLWIN32RENDERER_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_OPENGL)
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Platform/Win32Headers.hpp"

namespace Molten
{

    /**
    * @brief OpenGL renderer class for Win32.
    */
    class MOLTEN_API OpenGLWin32Renderer : public Renderer
    {

    public:

        /** Constructor. */
        OpenGLWin32Renderer();

        /** Constructs and creates renderer. */
        OpenGLWin32Renderer(RenderTarget& renderTarget, const Version& version, Logger* logger = nullptr);

        /** Virtual destructor. */
        virtual ~OpenGLWin32Renderer();

        /** Opens renderer by loading and attaching renderer to provided window. */
        virtual bool Open(RenderTarget& renderTarget, const Version& version = Version::None, Logger* logger = nullptr) override;

        /** Closing renderer. */
        virtual void Close() override;

        /** Checks if renderer is open Same value returned as last call to Open. */
        virtual bool IsOpen() const override;

        /** Resize the framebuffers.
         *  Execute this function as soon as the render target's work area is resized.
         */
        virtual void Resize(const Vector2ui32& size) override;

        /** Get backend API type. */
        virtual BackendApi GetBackendApi() const override;

        /** Get renderer API version. */
        virtual Version GetVersion() const override;

        /** Get location of pipeline push constant by id. Id is set in shader script. */
        virtual uint32_t GetPushConstantLocation(Resource<Pipeline>& pipeline, const uint32_t id) override;


        /** Create descriptor set object. */
        virtual Resource<DescriptorSet> CreateDescriptorSet(const DescriptorSetDescriptor& descriptor) override;

        /** Create framed descriptor set object. */
        virtual Resource<FramedDescriptorSet> CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& descriptor) override;

        /** Create framebuffer object. */
        virtual Resource<Framebuffer> CreateFramebuffer(const FramebufferDescriptor& descriptor) override;

        /**  Create index buffer object. */
        virtual Resource<IndexBuffer> CreateIndexBuffer(const IndexBufferDescriptor& descriptor) override;

        /** Create pipeline object. */
        virtual Resource<Pipeline> CreatePipeline(const PipelineDescriptor& descriptor) override;

        /** Create texture object. */
        virtual Resource<Texture> CreateTexture(const TextureDescriptor& descriptor) override;

        /** Create uniform buffer object. */
        virtual Resource<UniformBuffer> CreateUniformBuffer(const UniformBufferDescriptor& descriptor) override;

        /** Create framed uniform buffer object. */
        virtual Resource<FramedUniformBuffer> CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor) override;

        /** Create vertex buffer object. */
        virtual Resource<VertexBuffer> CreateVertexBuffer(const VertexBufferDescriptor& descriptor) override;


        /** Bind descriptor set to draw queue. */
        virtual void BindDescriptorSet(Resource<DescriptorSet>& descriptorSet) override;

        /** Bind framed descriptor set to draw queue. */
        virtual void BindFramedDescriptorSet(Resource<FramedDescriptorSet>& framedDescriptorSet) override;

        /** Bind pipeline to draw queue. */
        virtual void BindPipeline(Resource<Pipeline>& pipeline) override;


        /** Begin and initialize rendering to framebuffers. */
        virtual void BeginDraw() override;

        /** Draw vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(Resource<VertexBuffer>& vertexBuffer) override;

        /** Draw indexed vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(Resource<IndexBuffer>& indexBuffer, Resource<VertexBuffer>& vertexBuffer) override;

        /** Push constant values to shader stage.
         *  This function call has no effect if provided id argument is greater than the number of push constants in pipeline.
         *
         * @param location Id of push constant to update. This id can be shared between multiple shader stages.
         */
         /**@{*/
        virtual void PushConstant(const uint32_t location, const bool& value) override;
        virtual void PushConstant(const uint32_t location, const int32_t& value) override;
        virtual void PushConstant(const uint32_t location, const float& value) override;
        virtual void PushConstant(const uint32_t location, const Vector2f32& value) override;
        virtual void PushConstant(const uint32_t location, const Vector3f32& value) override;
        virtual void PushConstant(const uint32_t location, const Vector4f32& value) override;
        virtual void PushConstant(const uint32_t location, const Matrix4x4f32& value) override;
        /**@}*/


        /** Finalize and present rendering. */
        virtual void EndDraw() override;


        /** Sleep until the graphical device is ready. */
        virtual void WaitForDevice() override;

        /** Update uniform buffer data. */
        virtual void UpdateUniformBuffer(Resource<UniformBuffer>& uniformBuffer, const size_t offset, const size_t size, const void* data) override;

        /** Update uniform buffer data. */
        virtual void UpdateFramedUniformBuffer(Resource<FramedUniformBuffer>& framedUniformBuffer, const size_t offset, const size_t size, const void* data) override;

    private:

        /** Open opengl context by provided version.
         *
         * @param version[in] Opened version.
         *
         * @throw Exception If failed to open provided version.
         */
        bool OpenVersion(HDC deviceContext, const Version& version);

        /** Open the best available opengl context.
         *
         * @param version[out] Opened version.
         *
         * @throw Exception If failed to open any opengl context.
         */
        void OpenBestVersion(HDC deviceContext, Version& version);

        Version m_version;
        HDC m_deviceContext;
        HGLRC m_context;

    };

}

#endif

#endif

#endif
