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

#include "Molten/Memory/Reference.hpp"
#include "Molten/Renderer/Framebuffer.hpp"
#include "Molten/Renderer/IndexBuffer.hpp"
#include "Molten/Renderer/Pipeline.hpp"
#include "Molten/Renderer/Texture.hpp"
#include "Molten/Renderer/UniformBlock.hpp"
#include "Molten/Renderer/UniformBuffer.hpp"
#include "Molten/Renderer/VertexBuffer.hpp"
#include "Molten/System/Version.hpp"
#include <functional>

namespace Molten::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Molten
{

    class Window;
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

        /**
         * Static function for creating any renderer by Type.
         * Make sure to open the renderer before using it.
         *
         * @return Pointer to renderer, nullptr if the type of renderer is unavailable.
         */
        static Renderer * Create(const BackendApi renderApi);

        /** Virtual destructor. */
        virtual ~Renderer();

        /**
         * Opens renderer by loading and attaching renderer to provided window.
         *
         * @param window Render target window.
         */
        virtual bool Open(const Window& window, const Version& version = Version::None, Logger * logger = nullptr) = 0;

        /**  Closing renderer. */
        virtual void Close() = 0;

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
        virtual uint32_t GetPushConstantLocation(Pipeline* pipeline, const uint32_t id) = 0;


        /** Create framebuffer object. */
        virtual Framebuffer* CreateFramebuffer(const FramebufferDescriptor& descriptor) = 0;

        /**  Create index buffer object. */
        virtual IndexBuffer* CreateIndexBuffer(const IndexBufferDescriptor& descriptor) = 0;

        /** Create pipeline object. */
        virtual Pipeline* CreatePipeline(const PipelineDescriptor& descriptor) = 0;

        /** Create texture object. */
        virtual Texture* CreateTexture() = 0;

        /** Create uniform buffer object. */
        virtual UniformBlock* CreateUniformBlock(const UniformBlockDescriptor& descriptor) = 0;

        /** Create uniform buffer object. */
        virtual UniformBuffer* CreateUniformBuffer(const UniformBufferDescriptor& descriptor) = 0;

        /** Create vertex buffer object. */
        virtual VertexBuffer* CreateVertexBuffer(const VertexBufferDescriptor& descriptor) = 0;


        /** Destroy framebuffer object. */
        virtual void DestroyFramebuffer(Framebuffer* framebuffer) = 0;

        /** Destroy index buffer object. */
        virtual void DestroyIndexBuffer(IndexBuffer* indexBuffer) = 0;

        /** Destroy pipeline object. */
        virtual void DestroyPipeline(Pipeline* pipeline) = 0;

        /** Destroy texture object. */
        virtual void DestroyTexture(Texture* texture) = 0;

        /** Destroy uniform block object. */
        virtual void DestroyUniformBlock(UniformBlock* uniformBlock) = 0;

        /** Destroy uniform buffer object. */
        virtual void DestroyUniformBuffer(UniformBuffer* uniformBuffer) = 0;

        /** Destroy vertex buffer object. */
        virtual void DestroyVertexBuffer(VertexBuffer* vertexBuffer) = 0;    


        /** Bind pipeline to draw queue. */
        virtual void BindPipeline(Pipeline* pipeline) = 0;

        /** Bind pipeline to draw queue. */
        virtual void BindUniformBlock(UniformBlock* uniformBlock, const uint32_t offset = 0) = 0;


        /** Begin and initialize rendering to framebuffers. */
        virtual void BeginDraw() = 0;

        /** Draw vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(VertexBuffer* vertexBuffer) = 0;

        /** Draw indexed vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(IndexBuffer* indexBuffer, VertexBuffer* vertexBuffer) = 0;

        /** 
         * Push constant values to shader stage.
         * This function call has no effect if provided id argument is greater than the number of push constants in pipeline.
         *
         * @param id Id of push constant to update. This id can be shared between multiple shader stages.
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
        virtual void UpdateUniformBuffer(UniformBuffer* uniformBuffer, const size_t offset, const size_t size, const void* data) = 0;

    };

}

#endif
