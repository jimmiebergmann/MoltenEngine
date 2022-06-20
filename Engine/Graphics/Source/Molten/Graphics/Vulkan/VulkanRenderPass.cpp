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

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanRenderPass.hpp"
#include "Molten/Renderer/Vulkan/VulkanTexture.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResultLogger.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"

namespace Molten
{

    // Vulkan render pass attachment implementations.
    VulkanRenderPassAttachment::VulkanRenderPassAttachment() :
        texture{},
        initialType{ TextureType::Color },
        initialLayout{ VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED },
        finalLayout{ VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED }
    {}

    VulkanRenderPassAttachment::VulkanRenderPassAttachment(
        SharedRenderResource<VulkanFramedTexture<2>> texture,
        const TextureType initialType,
        const VkImageLayout initialLayout,
        const VkImageLayout finalLayout
    ) :
        texture{ std::move(texture) },
        initialType{ initialType },
        initialLayout{ initialLayout },
        finalLayout{ finalLayout }
    {}


    // Vulkan render pass implementations.
    VulkanRenderPass::VulkanRenderPass(
        Logger* logger,
        VkRenderPass renderPass,
        const VkCommandPool commandPool,
        const Vector2ui32& dimensions,
        VulkanRenderPassFrames&& frames,
        VulkanRenderPassAttachments&& attachments,
        const bool hasDepthStencilAttachment,
        std::vector<VkClearValue>&& clearValues,
        RenderPassFunction recordFunction
    ) :
        RenderPass(dimensions),
        m_logger(logger),
        m_renderPass(renderPass),
        m_commandPool(commandPool),
        m_frames(std::move(frames)),
        m_attachments(std::move(attachments)),
        m_hasDepthStencilAttachment(hasDepthStencilAttachment),
        m_clearValues(std::move(clearValues)),
        m_currentFrameIndex(0),
        m_recordFunction(std::move(recordFunction))
    {}

    void VulkanRenderPass::SetRecordFunction(RenderPassFunction recordFunction)
    {
        m_recordFunction = recordFunction;
    }

    void VulkanRenderPass::SetViewport(std::optional<Bounds2i32> bounds)
    {
        m_viewportBounds = bounds;
    }

    void VulkanRenderPass::SetScissor(std::optional<Bounds2i32> bounds)
    {
        m_scissorBounds = bounds;
    }

    VulkanRenderPassFrame& VulkanRenderPass::GetCurrentFrame()
    {
        return m_frames[m_currentFrameIndex];
    }

    size_t VulkanRenderPass::GetCommandCount() const
    {
        return m_commandBuffer.GetCommandCount();
    }

    void VulkanRenderPass::SetDimensions(const Vector2ui32& dimensions)
    {
        m_dimensions = dimensions;
    }

    Vulkan::Result<> VulkanRenderPass::Record(const size_t frameIndex)
    {
        if(frameIndex >= m_frames.size())
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }
        m_currentFrameIndex = frameIndex;

        if(!m_recordFunction)
        {
            return {};
        }

        const auto& currentFrame = m_frames[m_currentFrameIndex];
        auto currentCommandBuffer = currentFrame.commandBuffer;
        auto currentFramebuffer = currentFrame.framebuffer;

        m_commandBuffer.PrepareRecording(m_currentFrameIndex, currentCommandBuffer);

        // Begin command buffer.
        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.pNext = nullptr;
        commandBufferBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (auto result = vkBeginCommandBuffer(currentCommandBuffer, &commandBufferBeginInfo); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to begin recording command buffer.");
            return result;
        }

        for(auto& attachment : m_attachments)
        {
            auto& texture = attachment.texture;
            auto& textureFrame = texture->frames[m_currentFrameIndex];

            if(textureFrame.deviceImage.layout != attachment.initialLayout)
            {
                if (!Vulkan::TransitionImageLayout(currentCommandBuffer, textureFrame.deviceImage, attachment.initialLayout))
                {
                    Logger::WriteError(m_logger, "Failed to transition image layout of framed texture: " +
                        std::to_string(static_cast<size_t>(attachment.initialLayout)) + ".");
                    return VkResult::VK_ERROR_UNKNOWN;
                }
            }
        }

        const auto bounds = Bounds2i32{ { 0.0f, 0.0f }, m_dimensions };

        const auto viewportBounds = m_viewportBounds.value_or(Bounds2i32{ bounds });
        const auto viewportSize = viewportBounds.GetSize();     
        VkViewport viewport = {};
        viewport.x = static_cast<float>(viewportBounds.low.x);
        viewport.y = static_cast<float>(viewportBounds.low.y);
        viewport.width = static_cast<float>(viewportSize.x);
        viewport.height = static_cast<float>(viewportSize.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        const auto scissorBounds = m_scissorBounds.value_or(Bounds2i32{ bounds });
        const auto scissorSize = scissorBounds.GetSize();
        VkRect2D scissor = {};
        scissor.offset.x = scissorBounds.low.x;
        scissor.offset.y = scissorBounds.low.y;
        scissor.extent.width = static_cast<uint32_t>(scissorSize.x);
        scissor.extent.height = static_cast<uint32_t>(scissorSize.y);

        // Begin render pass.
        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = currentFramebuffer;
        renderPassBeginInfo.renderArea.offset = scissor.offset;
        renderPassBeginInfo.renderArea.extent = scissor.extent;
        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(m_clearValues.size());
        renderPassBeginInfo.pClearValues = m_clearValues.data();

        vkCmdBeginRenderPass(currentCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);

        // Execute render function.
        m_recordFunction(m_commandBuffer);

        // End recoding.
        vkCmdEndRenderPass(currentCommandBuffer);
        
        if (auto result = vkEndCommandBuffer(currentCommandBuffer); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to end command buffer");
            return result;
        }

        return {};
    }

    Vulkan::Result<> VulkanRenderPass::Submit(
        Vulkan::LogicalDevice& logicalDevice,
        const VkSemaphore waitSemaphore,
        const VkFence submitFence)
    {
        const auto& currentFrame = m_frames[m_currentFrameIndex];
        auto currentCommandBuffer = currentFrame.commandBuffer;

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags pipelineWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = waitSemaphore != VK_NULL_HANDLE ? 1 : 0;
        submitInfo.pWaitSemaphores = waitSemaphore != VK_NULL_HANDLE ? &waitSemaphore : nullptr;
        submitInfo.pWaitDstStageMask = pipelineWaitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &currentCommandBuffer;

        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &currentFrame.finishSemaphore;

        auto& deviceQueues = logicalDevice.GetDeviceQueues();
        if (const auto result = vkQueueSubmit(deviceQueues.graphicsQueue, 1, &submitInfo, submitFence); result != VK_SUCCESS)
        {
            return result;
        }

        for (auto& attachment : m_attachments)
        {
            auto& texture = attachment.texture;
            auto& textureFrame = texture->frames[m_currentFrameIndex];
            textureFrame.deviceImage.layout = attachment.finalLayout;
        }

        return {};
    }

}

#endif