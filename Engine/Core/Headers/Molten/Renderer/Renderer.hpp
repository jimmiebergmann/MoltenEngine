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

namespace Molten::Shader
{
    class VertexStage;
    class FragmentStage;
}

namespace Molten::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Molten
{

    class Window;
    class Logger;


    /**
    * @brief Base class of renderer.
    */
    class MOLTEN_API Renderer
    {

    public:

        /**
        * @brief Types of renderers.
        */
        enum class BackendApi
        {
            OpenGL,
            Vulkan
        };

        /**
        * @brief Static function for creating any renderer by Type.
        *        Make sure to open the renderer before using it.
        *
        * @return Pointer to renderer, nullptr if the type of renderer is unavailable.
        */
        static Renderer * Create(const BackendApi renderApi);

        /**
        * @brief Virtual destructor.
        */
        virtual ~Renderer();

        /**
        * @brief Opens renderer by loading and attaching renderer to provided window.
        *
        * @param window Render target window.
        */
        virtual bool Open(const Window& window, const Version& version = Version::None, Logger * logger = nullptr) = 0;

        /**
        * @brief Closing renderer.
        */
        virtual void Close() = 0;

        /**
        * @brief Resize the framebuffers.
        *        Execute this function as soon as the render target's work area is resized.
        */
        virtual void Resize(const Vector2ui32& size) = 0;

        /**
        * @brief Get backend API type.
        */
        virtual BackendApi GetBackendApi() const = 0;

        /**
        * @brief Get renderer API version.
        */
        virtual Version GetVersion() const = 0;


        /**
        * @brief Create framebuffer object.
        */
        virtual Framebuffer* CreateFramebuffer(const FramebufferDescriptor& descriptor) = 0;

        /**
        * @brief Create index buffer object.
        */
        virtual IndexBuffer* CreateIndexBuffer(const IndexBufferDescriptor& descriptor) = 0;

        /**
        * @brief Create pipeline object.
        */
        virtual Pipeline* CreatePipeline(const PipelineDescriptor& descriptor) = 0;

        /**
        * @brief Create vertex shader stage object out of vertex script.
        */
        virtual Shader::VertexStage* CreateVertexShaderStage(const Shader::Visual::VertexScript& script) = 0;

        /**
        * @brief Create fragment shader stage object out of fragment script.
        */
        virtual Shader::FragmentStage* CreateFragmentShaderStage(const Shader::Visual::FragmentScript& script) = 0;

        /**
        * @brief Create texture object.
        */
        virtual Texture* CreateTexture() = 0;

        /**
        * @brief Create uniform buffer object.
        */
        virtual UniformBlock* CreateUniformBlock(const UniformBlockDescriptor& descriptor) = 0;

        /**
        * @brief Create uniform buffer object.
        */
        virtual UniformBuffer* CreateUniformBuffer(const UniformBufferDescriptor& descriptor) = 0;

        /**
        * @brief Create vertex buffer object.
        */
        virtual VertexBuffer* CreateVertexBuffer(const VertexBufferDescriptor& descriptor) = 0;


        /**
        * @brief Destroy framebuffer object.
        */
        virtual void DestroyFramebuffer(Framebuffer* framebuffer) = 0;

        /**
        * @brief Destroy index buffer object.
        */
        virtual void DestroyIndexBuffer(IndexBuffer* indexBuffer) = 0;

        /**
        * @brief Destroy pipeline object.
        */
        virtual void DestroyPipeline(Pipeline* pipeline) = 0;

        /**
        * @brief Destroy vertex shader stage object.
        */
        virtual void DestroyVertexShaderStage(Shader::VertexStage* shader) = 0;

        /**
        * @brief Destroy fragment shader stage object.
        */
        virtual void DestroyFragmentShaderStage(Shader::FragmentStage* shader) = 0;

        /**
        * @brief Destroy texture object.
        */
        virtual void DestroyTexture(Texture* texture) = 0;

        /**
        * @brief Destroy uniform block object.
        */
        virtual void DestroyUniformBlock(UniformBlock* uniformBlock) = 0;

        /**
        * @brief Destroy uniform buffer object.
        */
        virtual void DestroyUniformBuffer(UniformBuffer* uniformBuffer) = 0;

        /**
        * @brief Destroy vertex buffer object.
        */
        virtual void DestroyVertexBuffer(VertexBuffer* vertexBuffer) = 0;    


        /**
        * @brief Bind pipeline to draw queue.
        */
        virtual void BindPipeline(Pipeline* pipeline) = 0;

        /**
        * @brief Bind pipeline to draw queue.
        */
        virtual void BindUniformBlock(UniformBlock* uniformBlock, const uint32_t offset = 0) = 0;


        /**
        * @brief Begin and initialize rendering to framebuffers.
        */
        virtual void BeginDraw() = 0;

        /**
        * @brief Draw vertex buffer, using the current bound pipeline.
        */
        virtual void DrawVertexBuffer(VertexBuffer* vertexBuffer) = 0;

        /**
        *  @brief Draw indexed vertex buffer, using the current bound pipeline.
        */
        virtual void DrawVertexBuffer(IndexBuffer* indexBuffer, VertexBuffer* vertexBuffer) = 0;

        /**
        * @brie Push constant values to shader stage.
        */
        virtual void PushShaderConstants(Shader::Type stage, const uint32_t offset, const uint32_t size, const void* data) = 0;

        /**
        * @brief Finalize and present rendering.
        */
        virtual void EndDraw() = 0;


        /**
        * @brief Sleep until the graphical device is ready.
        */
        virtual void WaitForDevice() = 0;

        /**
        * @brief Update uniform buffer data.
        */
        virtual void UpdateUniformBuffer(UniformBuffer* uniformBuffer, const size_t offset, const size_t size, const void* data) = 0;

    };

}

#endif