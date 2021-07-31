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
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResultLogger.hpp"

namespace Molten
{

    // Vulkan render pass implementations.
    VulkanRenderPass::VulkanRenderPass(
        Logger* logger,
        const VkCommandPool commandPool,
        Vulkan::CommandBuffers&& commandBuffers,
        Vulkan::Semaphores&& finishSemaphores,
        VkRenderPass renderPass,
        RenderPassFunction recordFunction
    ) :
        m_logger(logger),
        m_renderPass(renderPass),
        m_recordFunction(std::move(recordFunction)),
        m_commandPool(commandPool),
        m_commandBuffers(std::move(commandBuffers)),
        m_finishSemaphores(std::move(finishSemaphores)),
        m_commandBuffer{}
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

    Vulkan::Result<> VulkanRenderPass::Record(
        const size_t commandBufferIndex,
        const Bounds2i32& defaultViewportBounds,
        const Bounds2i32& defaultScissorBounds)
    {
        if(!m_recordFunction)
        {
            return {};
        }

        auto currentCommandBuffer = m_commandBuffers[commandBufferIndex];
        //auto currentFinishSemaphore = m_finishSemaphores[commandBufferIndex];

        m_commandBuffer.PrepareRecording(commandBufferIndex, currentCommandBuffer);

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

        const auto viewportBounds = m_viewportBounds.value_or(defaultViewportBounds);
        const auto viewportSize = viewportBounds.GetSize();     
        VkViewport viewport = {};
        viewport.x = static_cast<float>(viewportBounds.low.x);
        viewport.y = static_cast<float>(viewportBounds.low.y);
        viewport.width = static_cast<float>(viewportSize.x);
        viewport.height = static_cast<float>(viewportSize.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 0.0f;

        const auto scissorBounds = m_scissorBounds.value_or(defaultScissorBounds);
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
        // TODO: renderPassBeginInfo.framebuffer = currentFramebuffer;
        renderPassBeginInfo.renderArea.offset = scissor.offset;
        renderPassBeginInfo.renderArea.extent = scissor.extent;

        const VkClearValue clearColor = { 0.3f, 0.0f, 0.0f, 0.0f }; // TODO: Invalid initialization.
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

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

    void VulkanRenderPass::Submit()
    {
        /*
        auto& deviceQueues = m_logicalDevice->GetDeviceQueues();
        if (!(result = vkQueueSubmit(deviceQueues.graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrameIndex])))
        {
            return result;
        }
        */
    }

}

#endif