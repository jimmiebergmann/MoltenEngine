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

#ifndef MOLTEN_CORE_RENDERER_VULKANRENDERPASS_HPP
#define MOLTEN_CORE_RENDERER_VULKANRENDERPASS_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/RenderPass.hpp"
#include "Molten/Renderer/Vulkan/VulkanRenderPassFrame.hpp"
#include "Molten/Renderer/Vulkan/VulkanCommandBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"

namespace Molten
{
    /** Forward declarations. */
    namespace Vulkan
    {
        class ResourceDestroyer;
        class LogicalDevice;
    }
    class VulkanRenderer;
    template<size_t VDimensions> class VulkanFramedTexture;
    class Logger;


    /** Vulkan render pass attachment. */
    struct MOLTEN_API VulkanRenderPassAttachment
    {
        VulkanRenderPassAttachment();
        VulkanRenderPassAttachment(
            SharedRenderResource<VulkanFramedTexture<2>> texture,
            std::optional<Vector4f32> clearValue,
            const TextureType initialType,
            const VkImageLayout initialLayout,
            const VkImageLayout finalLayout);
        ~VulkanRenderPassAttachment() = default;

        VulkanRenderPassAttachment(const VulkanRenderPassAttachment&) = default;
        VulkanRenderPassAttachment(VulkanRenderPassAttachment&& attachment) noexcept = default;
        VulkanRenderPassAttachment& operator = (const VulkanRenderPassAttachment&) = default;
        VulkanRenderPassAttachment& operator = (VulkanRenderPassAttachment&& attachment) noexcept = default;

        SharedRenderResource<VulkanFramedTexture<2>> texture;
        std::optional<Vector4f32> clearValue;
        TextureType initialType;
        VkImageLayout initialLayout;
        VkImageLayout finalLayout;
    };

    using VulkanRenderPassAttachments = std::vector<VulkanRenderPassAttachment>;


    /** Vulkan Render pass. */
    class MOLTEN_API VulkanRenderPass : public RenderPass
    {

    public:

        using Base = RenderPass;


        VulkanRenderPass() = delete;
        VulkanRenderPass(
            Logger* logger,
            VkRenderPass renderPass,
            const VkCommandPool commandPool,
            const Vector2ui32& dimensions,
            VulkanRenderPassFrames&& frames,
            VulkanRenderPassAttachments&& attachments,
            const bool hasDepthStencilAttachment,
            std::vector<VkClearValue>&& clearValues,
            RenderPassFunction recordFunction);

        /** Set current command buffer record function. */
        void SetRecordFunction(RenderPassFunction recordFunction) override;

        /** Set current viewport bounds.
         *  Providing an optional without an value causes the render system to use viewport value from renderer. */
        void SetViewport(std::optional<Bounds2i32> bounds) override;

        /** Set current scissor bounds.
         *  Providing an optional without an value causes the render system to use scissor value from renderer. */
        void SetScissor(std::optional<Bounds2i32> bounds) override;

    protected:

        friend class VulkanRenderer;
        friend class Vulkan::ResourceDestroyer;

        [[nodiscard]] VulkanRenderPassFrame& GetCurrentFrame();

        [[nodiscard]] size_t GetCommandCount() const;

        void SetDimensions(const Vector2ui32& dimensions);

        [[nodiscard]] Vulkan::Result<> Record(const size_t frameIndex);

        [[nodiscard]] Vulkan::Result<> Submit(
            Vulkan::LogicalDevice& logicalDevice,
            const VkSemaphore waitSemaphore,
            const VkFence submitFence);

    private:

        Logger* m_logger;
        VkRenderPass m_renderPass;     
        VkCommandPool m_commandPool;
        VulkanCommandBuffer m_commandBuffer;
        VulkanRenderPassFrames m_frames;
        VulkanRenderPassAttachments m_attachments;
        bool m_hasDepthStencilAttachment;
        std::vector<VkClearValue> m_clearValues;
        size_t m_currentFrameIndex;
        RenderPassFunction m_recordFunction;
        std::optional<Bounds2i32> m_viewportBounds;
        std::optional<Bounds2i32> m_scissorBounds;

    };

}

#endif

#endif
