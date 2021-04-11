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

#ifndef MOLTEN_CORE_RENDERER_RENDERER_HPP
#define MOLTEN_CORE_RENDERER_RENDERER_HPP

#include "Molten/Renderer/RenderTarget.hpp"
#include "Molten/Renderer/DescriptorSet.hpp"
#include "Molten/Renderer/Framebuffer.hpp"
#include "Molten/Renderer/IndexBuffer.hpp"
#include "Molten/Renderer/Pipeline.hpp"
#include "Molten/Renderer/Sampler.hpp"
#include "Molten/Renderer/Texture.hpp"
#include "Molten/Renderer/UniformBuffer.hpp"
#include "Molten/Renderer/VertexBuffer.hpp"
#include "Molten/Renderer/RenderResource.hpp"
#include "Molten/System/Version.hpp"

namespace Molten::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Molten
{

    class Logger;


    /** Base class of renderer. */
    class MOLTEN_API Renderer
    {

    public:

        /** Types of renderers. */
        enum class BackendApi
        {
            OpenGL,
            Vulkan
        };

        /** Static function for creating any renderer by Type.
         *  Make sure to open the renderer before using it.
         *
         * @return Pointer to renderer, nullptr if the type of renderer is unavailable.
         */
        static Renderer * Create(const BackendApi backendApi);

        /** Virtual destructor. */
        virtual ~Renderer() = default;

        /** Opens renderer by loading and attaching renderer to provided window. */
        virtual bool Open(RenderTarget& renderTarget, const Version& version = Version::None, Logger * logger = nullptr) = 0;

        /** Closing renderer. */
        virtual void Close() = 0;

        /** Checks if renderer is open Same value returned as last call to Open. */
        virtual bool IsOpen() const = 0;

        /**
         * Resize the framebuffers.
         * Execute this function as soon as the render target's work area is resized.
         */
        virtual void Resize(const Vector2ui32& size) = 0;

        /** Get backend API type. */
        virtual BackendApi GetBackendApi() const = 0;

        /** Get renderer API version. */
        virtual Version GetVersion() const = 0;

        /** Get location of pipeline push constant by id. Id is set in shader script. */
        virtual uint32_t GetPushConstantLocation(Pipeline& pipeline, const uint32_t id) = 0;


        /** Create descriptor set object. */
        virtual RenderResource<DescriptorSet> CreateDescriptorSet(const DescriptorSetDescriptor& descriptor) = 0;

        /** Create framed descriptor set object. */
        virtual RenderResource<FramedDescriptorSet> CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& descriptor) = 0;

        /** Create framebuffer object. */
        virtual RenderResource<Framebuffer> CreateFramebuffer(const FramebufferDescriptor& descriptor) = 0;

        /**  Create index buffer object. */
        virtual RenderResource<IndexBuffer> CreateIndexBuffer(const IndexBufferDescriptor& descriptor) = 0;

        /** Create pipeline object. */
        virtual RenderResource<Pipeline> CreatePipeline(const PipelineDescriptor& descriptor) = 0;

        /** Create sampler object. */
        /**@{*/
        virtual RenderResource<Sampler1D> CreateSampler(const SamplerDescriptor1D& descriptor) = 0;
        virtual RenderResource<Sampler2D> CreateSampler(const SamplerDescriptor2D& descriptor) = 0;
        virtual RenderResource<Sampler3D> CreateSampler(const SamplerDescriptor3D& descriptor) = 0;
        /**@}*/

        /** Create texture object. */
        virtual RenderResource<Texture> CreateTexture(const TextureDescriptor& descriptor) = 0;

        /** Create uniform buffer object. */
        virtual RenderResource<UniformBuffer> CreateUniformBuffer(const UniformBufferDescriptor& descriptor) = 0;

        /** Create framed uniform buffer object. */
        virtual RenderResource<FramedUniformBuffer> CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor) = 0;

        /** Create vertex buffer object. */
        virtual RenderResource<VertexBuffer> CreateVertexBuffer(const VertexBufferDescriptor& descriptor) = 0;


        /** Destroys render resource.
         *  Some renderer implementations are not destroying the resource right away, but puts them in a cleanup queue.
         */
        /**@{*/
        virtual void Destroy(DescriptorSet& descriptorSet) = 0;
        virtual void Destroy(FramedDescriptorSet& framedDescriptorSet) = 0;
        virtual void Destroy(Framebuffer& framebuffer) = 0;
        virtual void Destroy(IndexBuffer& indexBuffer) = 0;
        virtual void Destroy(Pipeline& pipeline) = 0;
        virtual void Destroy(Sampler1D& sampler1D) = 0;
        virtual void Destroy(Sampler2D& sampler2D) = 0;
        virtual void Destroy(Sampler3D& sampler3D) = 0;
        virtual void Destroy(Texture& texture) = 0;
        virtual void Destroy(UniformBuffer& uniformBuffer) = 0;
        virtual void Destroy(FramedUniformBuffer& framedUniformBuffer) = 0;
        virtual void Destroy(VertexBuffer& vertexBuffer) = 0;
        /**@}*/


        /** Bind descriptor set to draw queue. */
        virtual void BindDescriptorSet(DescriptorSet& descriptorSet) = 0;

        /** Bind framed descriptor set to draw queue. */
        virtual void BindFramedDescriptorSet(FramedDescriptorSet& framedDescriptorSet) = 0;

        /** Bind pipeline to draw queue. */
        virtual void BindPipeline(Pipeline& pipeline) = 0;


        /** Begin and initialize rendering to framebuffers. */
        virtual void BeginDraw() = 0;

        /** Draw vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(VertexBuffer& vertexBuffer) = 0;

        /** Draw indexed vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(IndexBuffer& indexBuffer, VertexBuffer& vertexBuffer) = 0;

        /** Push constant values to shader stage.
         *  This function call has no effect if provided id argument is greater than the number of push constants in pipeline.
         *
         * @param location Id of push constant to update. This id can be shared between multiple shader stages.
         */
        /**@{*/
        virtual void PushConstant(const uint32_t location, const bool& value) = 0;
        virtual void PushConstant(const uint32_t location, const int32_t& value) = 0;
        virtual void PushConstant(const uint32_t location, const float& value) = 0;
        virtual void PushConstant(const uint32_t location, const Vector2f32& value) = 0;
        virtual void PushConstant(const uint32_t location, const Vector3f32& value) = 0;
        virtual void PushConstant(const uint32_t location, const Vector4f32& value) = 0;
        virtual void PushConstant(const uint32_t location, const Matrix4x4f32& value) = 0;
        /**@}*/


        /** Finalize and present rendering. */
        virtual void EndDraw() = 0;


        /** Sleep until the graphical device is ready. */
        virtual void WaitForDevice() = 0;

        /** Update uniform buffer data. */
        virtual void UpdateUniformBuffer(RenderResource<UniformBuffer>& uniformBuffer, const size_t offset, const size_t size, const void* data) = 0;

        /** Update framed uniform buffer data. */
        virtual void UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& framedUniformBuffer, const size_t offset, const size_t size, const void* data) = 0;

    };

}

#endif
