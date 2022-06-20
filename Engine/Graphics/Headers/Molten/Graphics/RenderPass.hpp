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

#ifndef MOLTEN_GRAPHICS_RENDERPASS_HPP
#define MOLTEN_GRAPHICS_RENDERPASS_HPP

#include "Molten/Graphics/Texture.hpp"
#include "Molten/Graphics/CommandBuffer.hpp"
#include "Molten/Graphics/RenderResource.hpp"
#include "Molten/Math/Bounds.hpp"
#include <vector>
#include <variant>
#include <optional>
#include <functional>

namespace Molten
{

    class RenderPass;
    using RenderPassFunction = std::function<void(CommandBuffer&)>; ///< Typedef of callback function at rendering.


    /** Render pass resource object. */
    class MOLTEN_GRAPHICS_API RenderPass
    {

    public:

        explicit RenderPass(const Vector2ui32& dimensions);

        virtual ~RenderPass() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        RenderPass(const RenderPass&) = delete;
        RenderPass(RenderPass&&) = delete;
        RenderPass& operator = (const RenderPass&) = delete;
        RenderPass& operator = (RenderPass&&) = delete;
        /**@}*/

        /** Get dimensions of render pass. */
        [[nodiscard]] Vector2ui32 GetDimensions() const;

        /** Set current command buffer record function. */
        virtual void SetRecordFunction(RenderPassFunction recordFunction) = 0;

        /** Set current viewport bounds.
         *  Providing an optional without an value causes the render system to use viewport value from renderer. */
        virtual void SetViewport(std::optional<Bounds2i32> bounds) = 0;

        /** Set current scissor bounds.
         *  Providing an optional without an value causes the render system to use scissor value from renderer. */
        virtual void SetScissor(std::optional<Bounds2i32> bounds) = 0;

    protected:

        Vector2ui32 m_dimensions;

    };

    /** Group of render passes, used for sequential render pass rendering, per frame. */
    using RenderPasses = std::vector<SharedRenderResource<RenderPass>>;


    /** Render pass attachment clear value types. */
    /**@{*/
    struct RenderPassAttachmentColorClearValue
    {
        Vector4f32 color = { 0.0f, 0.0f, 0.0f, 0.0f };
    };

    struct RenderPassAttachmentDepthStencilClearValue
    {
        float depth = 1.0f;
        uint8_t stencil = 0;
    };

    using RenderPassAttachmentClearValue = std::variant<
        std::monostate,
        RenderPassAttachmentColorClearValue,
        RenderPassAttachmentDepthStencilClearValue>;
    /**@}*/


    struct RenderPassAttachment
    {
        SharedRenderResource<FramedTexture<2>> texture = {};
        RenderPassAttachmentClearValue clearValue = {};
        TextureType initialType = TextureType::Color;
        TextureUsage initialUsage = TextureUsage::Attachment;
        std::optional<TextureType> finalType = {};
        std::optional<TextureUsage> finalUsage = {};
    };

    using RenderPassAttachments = std::vector<RenderPassAttachment>;

    /** Descriptor class of render pass class. */
    struct MOLTEN_GRAPHICS_API RenderPassDescriptor
    {
        Vector2ui32 dimensions = { 0, 0 };
        RenderPassAttachments attachments = {};
        RenderPassFunction recordFunction = nullptr;
    };


    struct RenderPassUpdateAttachment
    {
        SharedRenderResource<FramedTexture<2>> texture = {};
        TextureType initialType = TextureType::Color;
        TextureUsage initialUsage = TextureUsage::Attachment;
        std::optional<TextureType> finalType = {};
        std::optional<TextureUsage> finalUsage = {};
    };

    using RenderPassUpdateAttachments = std::vector<RenderPassUpdateAttachment>;

    /** Update descriptor class of render pass class. */
    struct MOLTEN_GRAPHICS_API RenderPassUpdateDescriptor
    {
        Vector2ui32 dimensions = { 0, 0 };
        RenderPassUpdateAttachments attachments = {};
    };

}

#endif
