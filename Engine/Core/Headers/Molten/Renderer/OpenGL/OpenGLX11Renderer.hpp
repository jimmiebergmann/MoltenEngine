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

#ifndef MOLTEN_CORE_RENDERER_OPENGL_OPENGLX11RENDERER_HPP
#define MOLTEN_CORE_RENDERER_OPENGL_OPENGLX11RENDERER_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_OPENGL)
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX

#include "Molten/Renderer/Renderer.hpp"


namespace Molten
{

    /**
    * @brief OpenGL renderer class for Win32.
    */
    class MOLTEN_API OpenGLX11Renderer : public Renderer
    {

    public:

        /** Constructor. */
        OpenGLX11Renderer();

        /**
         * Constructs and creates renderer.
         *
         * @param window[in] Render target window.
         */
        OpenGLX11Renderer(RenderTarget& renderTarget, const Version& version, Logger* logger = nullptr);

        /** Virtual destructor. */
        ~OpenGLX11Renderer() override;

        /**
         * Opens renderer by loading and attaching renderer to provided window.
         *
         * @param window Render target window.
         */
        bool Open(RenderTarget& renderTarget, const Version& version = Version::None, Logger* logger = nullptr) override;

        /** Closing renderer. */
        void Close() override;

        /** Checks if renderer is open Same value returned as last call to Open. */
        bool IsOpen() const override;

        /**
         * Resize the framebuffers.
         * Execute this function as soon as the render target's work area is resized.
         */
        void Resize(const Vector2ui32& size) override;

        /** Get backend API type. */
        BackendApi GetBackendApi() const override;

        /** Get renderer API version. */
        Version GetVersion() const override;

        /** Get supported capabilities and features of renderer. */
        const RendererCapabilities& GetCapabilities() const override;

        /** Get location of pipeline push constant by id. Id is set in shader script. */
        uint32_t GetPushConstantLocation(Pipeline& pipeline, const uint32_t id) override;


        /** Create descriptor set object. */
        RenderResource<DescriptorSet> CreateDescriptorSet(const DescriptorSetDescriptor& descriptor) override;

        /** Create framed descriptor set object. */
        RenderResource<FramedDescriptorSet> CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& descriptor) override;

        /** Create framebuffer object. */
        RenderResource<Framebuffer> CreateFramebuffer(const FramebufferDescriptor& descriptor) override;

        /**  Create index buffer object. */
        RenderResource<IndexBuffer> CreateIndexBuffer(const IndexBufferDescriptor& descriptor) override;

        /** Create pipeline object. */
        RenderResource<Pipeline> CreatePipeline(const PipelineDescriptor& descriptor) override;

        /** Create sampler object. */
        /**@{*/
        RenderResource<Sampler1D> CreateSampler(const SamplerDescriptor1D& descriptor) override;
        RenderResource<Sampler2D> CreateSampler(const SamplerDescriptor2D& descriptor) override;
        RenderResource<Sampler3D> CreateSampler(const SamplerDescriptor3D& descriptor) override;
        /**@}*/

        /** Create shader module object. */
        RenderResource<ShaderProgram> CreateShaderProgram(const VisualShaderProgramDescriptor& descriptor) override;

        /** Create texture object. */
        /**@{*/
        RenderResource<Texture1D> CreateTexture(const TextureDescriptor1D& descriptor) override;
        RenderResource<Texture2D> CreateTexture(const TextureDescriptor2D& descriptor) override;
        RenderResource<Texture3D> CreateTexture(const TextureDescriptor3D& descriptor) override;
        /**@}*/

        /** Create uniform buffer object. */
        RenderResource<UniformBuffer> CreateUniformBuffer(const UniformBufferDescriptor& descriptor) override;

        /** Create framed uniform buffer object. */
        RenderResource<FramedUniformBuffer> CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor) override;

        /** Create vertex buffer object. */
        RenderResource<VertexBuffer> CreateVertexBuffer(const VertexBufferDescriptor& descriptor) override;


        /** Update texture data. */
        /**@{*/
        bool UpdateTexture(Texture1D& texture1D, const TextureUpdateDescriptor1D& descriptor) override;
        bool UpdateTexture(Texture2D& texture2D, const TextureUpdateDescriptor2D& descriptor) override;
        bool UpdateTexture(Texture3D& texture3D, const TextureUpdateDescriptor3D& descriptor) override;
        /**@}*/


        /** Destroys render resource. */
        /**@{*/
        void Destroy(DescriptorSet& descriptorSet) override;
        void Destroy(FramedDescriptorSet& framedDescriptorSet) override;
        void Destroy(Framebuffer& framebuffer) override;
        void Destroy(IndexBuffer& indexBuffer) override;
        void Destroy(Pipeline& pipeline) override;
        void Destroy(Sampler1D& sampler1D) override;
        void Destroy(Sampler2D& sampler2D) override;
        void Destroy(Sampler3D& sampler3D) override;
        void Destroy(ShaderProgram& shaderProgram) override;
        void Destroy(Texture1D& texture1D) override;
        void Destroy(Texture2D& texture2D) override;
        void Destroy(Texture3D& texture3D) override;
        void Destroy(UniformBuffer& uniformBuffer) override;
        void Destroy(FramedUniformBuffer& framedUniformBuffer) override;
        void Destroy(VertexBuffer& vertexBuffer) override;
        /**@}*/


        /** Bind descriptor set to draw queue. */
        void BindDescriptorSet(DescriptorSet& descriptorSet) override;

        /** Bind framed descriptor set to draw queue. */
        void BindFramedDescriptorSet(FramedDescriptorSet& framedDescriptorSet) override;

        /** Bind pipeline to draw queue. */
        void BindPipeline(Pipeline& pipeline) override;


        /** Begin and initialize rendering to framebuffers. */
        void BeginDraw() override;

        /** Draw vertex buffer, using the current bound pipeline. */
        void DrawVertexBuffer(VertexBuffer& vertexBuffer) override;

        /** Draw indexed vertex buffer, using the current bound pipeline. */
        void DrawVertexBuffer(IndexBuffer& indexBuffer, VertexBuffer& vertexBuffer) override;

        /** Push constant values to shader stage.
         *  This function call has no effect if provided id argument is greater than the number of push constants in pipeline.
         *
         * @param location Id of push constant to update. This id can be shared between multiple shader stages.
         */
        /**@{*/
        void PushConstant(const uint32_t location, const bool& value) override;
        void PushConstant(const uint32_t location, const int32_t& value) override;
        void PushConstant(const uint32_t location, const float& value) override;
        void PushConstant(const uint32_t location, const Vector2f32& value) override;
        void PushConstant(const uint32_t location, const Vector3f32& value) override;
        void PushConstant(const uint32_t location, const Vector4f32& value) override;
        void PushConstant(const uint32_t location, const Matrix4x4f32& value) override;
        /**@}*/


        /** Finalize and present rendering. */
        void EndDraw() override;


        /** Sleep until the graphical device is ready. */
        void WaitForDevice() override;

        /** Update uniform buffer data. */
        void UpdateUniformBuffer(RenderResource<UniformBuffer>& uniformBuffer, const void* data, const size_t size, const size_t offset) override;

        /** Update uniform buffer data. */
        void UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& framedUniformBuffer, const void* data, const size_t size, const size_t offset) override;

    private:

        /**
        * @brief Open opengl context by provided version.
        *
        * @param version[in] Opened version.
        *
        * @throw Exception If failed to open provided version.
        */
       // bool OpenVersion(HDC deviceContext, const Version& version);

        /**
        * @brief Open the best available opengl context.
        *
        * @param version[out] Opened version.
        *
        * @throw Exception If failed to open any opengl context.
        */
        //void OpenBestVersion(HDC deviceContext, Version& version);

        Version m_version;

    };

}

#endif

#endif

#endif
