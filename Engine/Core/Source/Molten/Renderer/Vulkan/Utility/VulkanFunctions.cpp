/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include <algorithm>

MOLTEN_UNSCOPED_ENUM_BEGIN
/*
#define CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME_QUOTE(x) #x
#define CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(feature) {  &VkPhysicalDeviceFeatures::##feature, CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME_QUOTE(##feature) }
*/
namespace Molten::Vulkan
{  

    /*struct InternalPhysicalDeviceFeatureWithName
    {
        VkBool32 VkPhysicalDeviceFeatures::* memberPointer;
        const std::string name;
    };

    // Static data
    static const std::vector<InternalPhysicalDeviceFeatureWithName> g_allPhysicalDeviceFeatures = {
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(robustBufferAccess),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(fullDrawIndexUint32),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(imageCubeArray),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(independentBlend),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(geometryShader),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(tessellationShader),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sampleRateShading),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(dualSrcBlend),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(logicOp),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(multiDrawIndirect),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(drawIndirectFirstInstance),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(depthClamp),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(depthBiasClamp),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(fillModeNonSolid),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(depthBounds),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(wideLines),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(largePoints),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(alphaToOne),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(multiViewport),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(samplerAnisotropy),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(textureCompressionETC2),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(textureCompressionASTC_LDR),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(textureCompressionBC),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(occlusionQueryPrecise),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(pipelineStatisticsQuery),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(vertexPipelineStoresAndAtomics),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(fragmentStoresAndAtomics),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderTessellationAndGeometryPointSize),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderImageGatherExtended),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageExtendedFormats),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageMultisample),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageReadWithoutFormat),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageWriteWithoutFormat),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderUniformBufferArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderSampledImageArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageBufferArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderClipDistance),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderCullDistance),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderFloat64),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderInt64),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderInt16),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderResourceResidency),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderResourceMinLod),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseBinding),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyBuffer),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyImage2D),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyImage3D),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency2Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency4Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency8Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency16Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyAliased),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(variableMultisampleRate),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(inheritedQueries)
    };



    // Vulkan functions implementations.
    bool CheckRequiredExtensions(
        Extensions& missingExtensions,
        const Extensions& requiredExtensions,
        const Extensions& availableExtensions)
    {
        missingExtensions = requiredExtensions;

        for (const auto& availableExtension : availableExtensions)
        {
            auto it = std::find_if(missingExtensions.begin(), missingExtensions.end(),
                [&](auto& extension) {return availableExtension.name == extension.name && availableExtension.version >= extension.version; });
            if (it != missingExtensions.end())
            {
                missingExtensions.erase(it);
            }
        }

        return missingExtensions.size() == 0;
    }

    bool CheckRequiredDeviceFeatures(
        PhysicalDeviceFeaturesWithName& missingFeatures,
        const VkPhysicalDeviceFeatures& requiredFeatures,
        const VkPhysicalDeviceFeatures& availableFeatures)
    {
        bool failed = false;

        for (auto& feature : g_allPhysicalDeviceFeatures)
        {
            if (requiredFeatures.*(feature.memberPointer) &&
                requiredFeatures.*(feature.memberPointer) != availableFeatures.*(feature.memberPointer))
            {
                failed = true;
                missingFeatures.push_back({ feature.memberPointer, feature.name }); 
            }
        }

        return !failed;
    }*/

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
        Result<> result;

        commandBuffer = VK_NULL_HANDLE;

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandPool = commandPool;
        commandBufferInfo.commandBufferCount = 1;

        auto logicalDeviceHandle = logicalDevice.GetHandle();
        if (!(result = vkAllocateCommandBuffers(logicalDeviceHandle, &commandBufferInfo, &commandBuffer)))
        {
            return result;
        }

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if (!(result = vkBeginCommandBuffer(commandBuffer, &beginInfo)))
        {
            return result;
        }

        return result;
    }

    Result<> EndSingleTimeCommands(
        VkCommandBuffer commandBuffer,
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool)
    {
        Result<> result;

        if (!(result = vkEndCommandBuffer(commandBuffer)))
        {
            return result;
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        // Should we use a fence here? No? Yes?
        auto& deviceQueues = logicalDevice.GetDeviceQueues();
        if (!(result = vkQueueSubmit(deviceQueues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE)))
        {
            return result;
        }
        if (!(result = vkQueueWaitIdle(deviceQueues.graphicsQueue)))
        {
            return result;
        }
        
        vkFreeCommandBuffers(logicalDevice.GetHandle(), commandPool, 1, &commandBuffer);

        return result;
    }

    bool TransitionImageLayout(
        VkCommandBuffer commandBuffer,
        LogicalDevice& logicalDevice,
        VkImage image,
        VkFormat format,
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

}

MOLTEN_UNSCOPED_ENUM_END

#endif