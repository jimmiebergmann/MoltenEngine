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
#include "Molten/Renderer/Framebuffer.hpp"
#include "Molten/Renderer/IndexBuffer.hpp"
#include "Molten/Renderer/Pipeline.hpp"
#include "Molten/Renderer/Texture.hpp"
#include "Molten/Renderer/UniformBuffer.hpp"
#include "Molten/Renderer/VertexBuffer.hpp"
#include "Molten/Renderer/DescriptorSet.hpp"
#include "Molten/System/Version.hpp"
#include "Molten/System/Resource.hpp"

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
        virtual uint32_t GetPushConstantLocation(Resource<Pipeline>& pipeline, const uint32_t id) = 0;


        /** Create descriptor set object. */
        virtual Resource<DescriptorSet> CreateDescriptorSet(const DescriptorSetDescriptor& descriptor) = 0;

        /** Create framed descriptor set object. */
        virtual Resource<FramedDescriptorSet> CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& descriptor) = 0;

        /** Create framebuffer object. */
        virtual Resource<Framebuffer> CreateFramebuffer(const FramebufferDescriptor& descriptor) = 0;

        /**  Create index buffer object. */
        virtual Resource<IndexBuffer> CreateIndexBuffer(const IndexBufferDescriptor& descriptor) = 0;

        /** Create pipeline object. */
        virtual Resource<Pipeline> CreatePipeline(const PipelineDescriptor& descriptor) = 0;

        /** Create texture object. */
        virtual Resource<Texture> CreateTexture(const TextureDescriptor& descriptor) = 0;

        /** Create uniform buffer object. */
        virtual Resource<UniformBuffer> CreateUniformBuffer(const UniformBufferDescriptor& descriptor) = 0;

        /** Create framed uniform buffer object. */
        virtual Resource<FramedUniformBuffer> CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor) = 0;

        /** Create vertex buffer object. */
        virtual Resource<VertexBuffer> CreateVertexBuffer(const VertexBufferDescriptor& descriptor) = 0;


        /** Bind descriptor set to draw queue. */
        virtual void BindDescriptorSet(Resource<DescriptorSet>& descriptorSet) = 0;

        /** Bind framed descriptor set to draw queue. */
        virtual void BindFramedDescriptorSet(Resource<FramedDescriptorSet>& framedDescriptorSet) = 0;

        /** Bind pipeline to draw queue. */
        virtual void BindPipeline(Resource<Pipeline>& pipeline) = 0;


        /** Begin and initialize rendering to framebuffers. */
        virtual void BeginDraw() = 0;

        /** Draw vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(Resource<VertexBuffer>& vertexBuffer) = 0;

        /** Draw indexed vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(Resource<IndexBuffer>& indexBuffer, Resource<VertexBuffer>& vertexBuffer) = 0;

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
        virtual void UpdateUniformBuffer(Resource<UniformBuffer>& uniformBuffer, const size_t offset, const size_t size, const void* data) = 0;

        /** Update framed uniform buffer data. */
        virtual void UpdateFramedUniformBuffer(Resource<FramedUniformBuffer>& framedUniformBuffer, const size_t offset, const size_t size, const void* data) = 0;

    };

}

#endif
