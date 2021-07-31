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

#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceImage.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include <algorithm>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{  

    VkResult CreateFences(
        Fences& fences,
        VkDevice logicalDevice,
        const VkFenceCreateFlagBits createFlags,
        const size_t count)
    {
        VkResult result = VkResult::VK_SUCCESS;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = createFlags;

        fences.resize(count, VK_NULL_HANDLE);
        for (size_t i = 0; i < count; i++)
        {
            if ((result = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fences[i])) != VkResult::VK_SUCCESS)
            {
                for (size_t j = 0; j <= i; j++)
                {
                    vkDestroyFence(logicalDevice, fences[j], nullptr);
                }
                return result;
            }
        }

        return result;
    }


    VkFramebuffer CreateFramebuffer(
        VkDevice logicalDevice,
        VkRenderPass renderpass,
        const VkImageView& imageView,
        const Vector2ui32 size)
    {
        VkImageView attachments[] = { imageView };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderpass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = size.x;
        framebufferInfo.height = size.y;
        framebufferInfo.layers = 1;

        VkFramebuffer framebuffer;
        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
        {
            return VK_NULL_HANDLE;
        }

        return framebuffer;
    }

    VkResult CreateSemaphores(
        Semaphores& semaphores,
        VkDevice logicalDevice,
        const size_t count)
    {
        VkResult result = VkResult::VK_SUCCESS;

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        semaphores.resize(count, VK_NULL_HANDLE);
        for (size_t i = 0; i < count; i++)
        {
            if ((result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &semaphores[i])) != VkResult::VK_SUCCESS)
            {
                for (size_t j = 0; j <= i; j++)
                {
                    vkDestroySemaphore(logicalDevice, semaphores[j], nullptr);
                }
                return result;
            }
        }

        return result;
    }

    uint32_t CreateVersion(const Version& version)
    {
        return VK_MAKE_VERSION(version.Major, version.Minor, version.Patch);
    }

    void DestroyFences(VkDevice logicalDevice, Fences& fences)
    {
        for (auto fence : fences)
        {
            vkDestroyFence(logicalDevice, fence, nullptr);
        }
    }

    void DestroyImageViews(VkDevice logicalDevice, ImageViews& imageViews)
    {
        for (auto& imageView : imageViews)
        {
            vkDestroyImageView(logicalDevice, imageView, nullptr);
        }
    }

    void DestroySemaphores(VkDevice logicalDevice, Semaphores& semaphores)
    {
        for (auto semaphore : semaphores)
        {
            vkDestroySemaphore(logicalDevice, semaphore, nullptr);
        }
    }

    const VkBaseInStructure& FindLastBaseInStructure(VkBaseInStructure& baseInStructure)
    {
        const auto* currentStructure = &baseInStructure;
        while (currentStructure->pNext != nullptr)
        {
            currentStructure = currentStructure->pNext;
        }

        return *currentStructure;
    }

    VkBaseOutStructure& FindLastBaseOutStructure(VkBaseOutStructure& baseOutStructure)
    {
        auto* currentStructure = &baseOutStructure;
        while (currentStructure->pNext != nullptr)
        {
            currentStructure = currentStructure->pNext;
        }

        return *currentStructure;
    }

    Layers::iterator FindLayer(Layers& layers, const std::string& name)
    {
        return std::find_if(layers.begin(), layers.end(), 
            [&](Layer& layer)
        {
            return layer.name == name;
        });
    }

    Extensions::iterator FindExtension(Extensions& extensions, const std::string& name)
    {
        return std::find_if(extensions.begin(), extensions.end(),
            [&](Extension& extension)
        {
            return extension.name == name;
        });
    }

    void RemoveLayers(Layers& layers, const Layers& excludes)
    {
        auto findLayer = [](const Layers& layers, const Layer& layer)
        {
            for (auto& currentLayer : layers)
            {
                if (currentLayer.name == layer.name)
                {
                    return true;
                }
            }
            return false;
        };
        auto it = std::remove_if(layers.begin(), layers.end(),
            [&](auto& extension)
        { 
            return findLayer(excludes, extension);
        });

        layers.erase(it);
    }

    void RemoveExtensions(Extensions& extensions, const Extensions& excludes)
    {
        auto findExtension = [](const Extensions& extensions, const Extension& extension)
        {
            for (auto& currentExtension : extensions)
            {
                if (currentExtension.name == extension.name)
                {
                    return true;
                }
            }
            return false;
        };
        auto it = std::remove_if(extensions.begin(), extensions.end(),
            [&](auto& extension)
        {
            return findExtension(excludes, extension);
        });

        extensions.erase(it);
    }

    Result<> BeginSingleTimeCommands(
        VkCommandBuffer& commandBuffer,
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool)
    {
        commandBuffer = VK_NULL_HANDLE;

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandPool = commandPool;
        commandBufferInfo.commandBufferCount = 1;

        auto logicalDeviceHandle = logicalDevice.GetHandle();
        if (const auto result = vkAllocateCommandBuffers(logicalDeviceHandle, &commandBufferInfo, &commandBuffer); result != VK_SUCCESS)
        {
            return result;
        }

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if (const auto result = vkBeginCommandBuffer(commandBuffer, &beginInfo); result != VK_SUCCESS)
        {
            return result;
        }

        return {};
    }

    Result<> EndSingleTimeCommands(
        VkCommandBuffer commandBuffer,
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool)
    {
        if (const auto result = vkEndCommandBuffer(commandBuffer); result != VK_SUCCESS)
        {
            return result;
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        // Should we use a fence here? No? Yes?
        auto& deviceQueues = logicalDevice.GetDeviceQueues();
        if (const auto result = vkQueueSubmit(deviceQueues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE); result != VK_SUCCESS)
        {
            return result;
        }
        if (const auto result = vkQueueWaitIdle(deviceQueues.graphicsQueue); result != VK_SUCCESS)
        {
            return result;
        }
        
        vkFreeCommandBuffers(logicalDevice.GetHandle(), commandPool, 1, &commandBuffer);

        return {};
    }

    bool TransitionImageLayout(
        VkCommandBuffer commandBuffer,
        VkImage image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout)
    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStageMask;
        VkPipelineStageFlags destStageMask;

        if (oldLayout == VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED && 
            newLayout == VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (
            oldLayout == VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
            newLayout == VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
            destStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if(
            oldLayout == VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
            newLayout == VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else
        {
            return false;
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStageMask,
            destStageMask, 
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        return true;
    }

    bool TransitionImageLayout(
        VkCommandBuffer commandBuffer,
        DeviceImage& deviceImage,
        VkImageLayout newLayout)
    {
        if(!TransitionImageLayout(commandBuffer, deviceImage.image, deviceImage.layout, newLayout))
        {
            return false;
        }

        deviceImage.layout = newLayout;
        return true;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif