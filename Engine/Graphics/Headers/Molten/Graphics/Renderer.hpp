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

#ifndef MOLTEN_GRAPHICS_RENDERER_HPP
#define MOLTEN_GRAPHICS_RENDERER_HPP

#include "Molten/Graphics/RenderTarget.hpp"
#include "Molten/Graphics/DescriptorSet.hpp"
#include "Molten/Graphics/IndexBuffer.hpp"
#include "Molten/Graphics/Pipeline.hpp"
#include "Molten/Graphics/RenderPass.hpp"
#include "Molten/Graphics/Sampler.hpp"
#include "Molten/Graphics/ShaderProgram.hpp"
#include "Molten/Graphics/Texture.hpp"
#include "Molten/Graphics/UniformBuffer.hpp"
#include "Molten/Graphics/VertexBuffer.hpp"
#include "Molten/Graphics/RenderResource.hpp"
#include "Molten/System/Version.hpp"
#include <memory>

namespace Molten::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Molten
{

    class Logger;

    /** Structure for storing and presenting capabilities and features supported by renderer.
     *  Each renderer implementation must present its capabilities via Renderer::GetCapabilities().
     */
    struct RendererCapabilities
    {
        bool textureSwizzle;
    };


    /** Renderer creation descriptor. */
    struct MOLTEN_GRAPHICS_API RendererDescriptor
    {
        RenderTarget& renderTarget;
        Version version = Version::None;
        Logger* logger = nullptr;
    };


    /** Base class of renderer. */
    class MOLTEN_GRAPHICS_API Renderer
    {

    public:

        /** Types of renderers. */
        enum class BackendApi
        {
            OpenGL,
            Vulkan
        };

        /** Static function for creating any renderer by Type.
         *
         * @return Pointer to renderer, nullptr if the type of renderer is unavailable.
         */
        static std::unique_ptr<Renderer> Create(
            const BackendApi backendApi,
            [[maybe_unused]] const RendererDescriptor& descriptor);

        /** Virtual destructor. */
        virtual ~Renderer() = default;

        /** Opens renderer by loading and attaching renderer to provided render target. */
        virtual bool Open(const RendererDescriptor& descriptor) = 0;

        /** Closing renderer. */
        virtual void Close() = 0;

        /** Checks if renderer is open Same value returned as last call to Open. */
        [[nodiscard]] virtual bool IsOpen() const = 0;

        /**
         * Resize the framebuffers.
         * Execute this function as soon as the render target's work area is resized.
         */
        virtual void Resize(const Vector2ui32& size) = 0;

        /** Get backend API type. */
        [[nodiscard]] virtual BackendApi GetBackendApi() const = 0;

        /** Get renderer API version. */
        [[nodiscard]] virtual Version GetVersion() const = 0;

        /** Get supported capabilities and features of renderer. */
        [[nodiscard]] virtual const RendererCapabilities& GetCapabilities() const = 0;

        /** Get location of pipeline push constant by id. Id is set in shader script. */
        [[nodiscard]] virtual uint32_t GetPushConstantLocation(Pipeline& pipeline, const uint32_t id) = 0;

        [[nodiscard]] virtual SharedRenderResource<RenderPass> GetSwapChainRenderPass() { return {}; }


        /** Create descriptor set object. */
        [[nodiscard]] virtual RenderResource<DescriptorSet> CreateDescriptorSet(const DescriptorSetDescriptor& descriptor) = 0;

        /** Create framed descriptor set object. */
        [[nodiscard]] virtual RenderResource<FramedDescriptorSet> CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& descriptor) = 0;

        /**  Create index buffer object. */
        [[nodiscard]] virtual RenderResource<IndexBuffer> CreateIndexBuffer(const IndexBufferDescriptor& descriptor) = 0;

        /** Create pipeline object. */
        [[nodiscard]] virtual RenderResource<Pipeline> CreatePipeline(const PipelineDescriptor& descriptor) = 0;

        /** Create render pass object. */
        [[nodiscard]] virtual SharedRenderResource<RenderPass> CreateRenderPass(const RenderPassDescriptor& descriptor) = 0;

        /** Create sampler object. */
        /**@{*/
        [[nodiscard]] virtual SharedRenderResource<Sampler1D> CreateSampler(const SamplerDescriptor1D& descriptor) = 0;
        [[nodiscard]] virtual SharedRenderResource<Sampler2D> CreateSampler(const SamplerDescriptor2D& descriptor) = 0;
        [[nodiscard]] virtual SharedRenderResource<Sampler3D> CreateSampler(const SamplerDescriptor3D& descriptor) = 0;
        /**@}*/

        /** Create shader module object. */
        [[nodiscard]] virtual SharedRenderResource<ShaderProgram> CreateShaderProgram(const VisualShaderProgramDescriptor& descriptor) = 0;

        /** Create texture object. */
        /**@{*/
        [[nodiscard]] virtual SharedRenderResource<Texture1D> CreateTexture(const TextureDescriptor1D& descriptor) = 0;
        [[nodiscard]] virtual SharedRenderResource<Texture2D> CreateTexture(const TextureDescriptor2D& descriptor) = 0;
        [[nodiscard]] virtual SharedRenderResource<Texture3D> CreateTexture(const TextureDescriptor3D& descriptor) = 0;
        /**@}*/

        /** Create framed texture object. */
        /**@{*/
        [[nodiscard]] virtual SharedRenderResource<FramedTexture1D> CreateFramedTexture(const TextureDescriptor1D& descriptor) = 0;
        [[nodiscard]] virtual SharedRenderResource<FramedTexture2D> CreateFramedTexture(const TextureDescriptor2D& descriptor) = 0;
        [[nodiscard]] virtual SharedRenderResource<FramedTexture3D> CreateFramedTexture(const TextureDescriptor3D& descriptor) = 0;
        /**@}*/

        /** Create uniform buffer object. */
        [[nodiscard]] virtual RenderResource<UniformBuffer> CreateUniformBuffer(const UniformBufferDescriptor& descriptor) = 0;

        /** Create framed uniform buffer object. */
        [[nodiscard]] virtual RenderResource<FramedUniformBuffer> CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor) = 0;

        /** Create vertex buffer object. */
        [[nodiscard]] virtual RenderResource<VertexBuffer> CreateVertexBuffer(const VertexBufferDescriptor& descriptor) = 0;


        /** Update render pass object. */
        [[nodiscard]] virtual bool UpdateRenderPass(RenderPass& renderPass, const RenderPassUpdateDescriptor& descriptor) = 0;

        /** Update texture data. */
        /**@{*/
        virtual bool UpdateTexture(Texture1D& texture1D, const TextureUpdateDescriptor1D& descriptor) = 0;
        virtual bool UpdateTexture(Texture2D& texture2D, const TextureUpdateDescriptor2D& descriptor) = 0;
        virtual bool UpdateTexture(Texture3D& texture3D, const TextureUpdateDescriptor3D& descriptor) = 0;
        /**@}*/

        /** Update uniform buffer data. */
        virtual void UpdateUniformBuffer(RenderResource<UniformBuffer>& uniformBuffer, const void* data, const size_t size, const size_t offset) = 0;

        /** Update framed uniform buffer data. */
        virtual void UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& framedUniformBuffer, const void* data, const size_t size, const size_t offset) = 0;


        /** Draw next frame by one or multiple render passes. */
        virtual bool DrawFrame(const RenderPasses& renderPasses) = 0;


        /** Destroys render resource.
         *  Some renderer implementations are not destroying the resource right away, but puts them in a cleanup queue.
         */
        /**@{*/
        virtual void Destroy(DescriptorSet& descriptorSet) = 0;
        virtual void Destroy(FramedDescriptorSet& framedDescriptorSet) = 0;
        virtual void Destroy(IndexBuffer& indexBuffer) = 0;
        virtual void Destroy(Pipeline& pipeline) = 0;
        virtual void Destroy(RenderPass& renderPass) = 0;
        virtual void Destroy(Sampler1D& sampler1D) = 0;
        virtual void Destroy(Sampler2D& sampler2D) = 0;
        virtual void Destroy(Sampler3D& sampler3D) = 0;
        virtual void Destroy(ShaderProgram& shaderProgram) = 0;
        virtual void Destroy(Texture1D& texture1D) = 0;
        virtual void Destroy(Texture2D& texture2D) = 0;
        virtual void Destroy(Texture3D& texture3D) = 0;
        virtual void Destroy(FramedTexture1D& framedTexture1D) = 0;
        virtual void Destroy(FramedTexture2D& framedTexture2D) = 0;
        virtual void Destroy(FramedTexture3D& framedTexture3D) = 0;
        virtual void Destroy(UniformBuffer& uniformBuffer) = 0;
        virtual void Destroy(FramedUniformBuffer& framedUniformBuffer) = 0;
        virtual void Destroy(VertexBuffer& vertexBuffer) = 0;
        /**@}*/


        /** Sleep until the graphical device is ready. */
        virtual void WaitForDevice() = 0;

    };

}

#endif
