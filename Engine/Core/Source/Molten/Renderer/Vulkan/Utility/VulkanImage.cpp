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

#include "Molten/Renderer/Vulkan/Utility/VulkanImage.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"
#include "Molten/Utility/SmartFunction.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Image implementations.
    Image::Image() :
        handle(VK_NULL_HANDLE),
        memory(VK_NULL_HANDLE),
        layout(VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED),
        format(VkFormat::VK_FORMAT_UNDEFINED),
        m_logicalDevice(nullptr)
    {}

    Image::~Image()
    {
        Destroy();
    }

    Image::Image(Image&& image) noexcept :
        handle(image.handle),
        memory(image.memory),
        layout(image.layout),
        format(image.format),
        m_logicalDevice(image.m_logicalDevice)
    {
        image.handle = VK_NULL_HANDLE;
        image.memory = VK_NULL_HANDLE;
        image.layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        image.format = VkFormat::VK_FORMAT_UNDEFINED;
        image.m_logicalDevice = nullptr;
    }

    Image& Image::operator =(Image&& image) noexcept
    {
        handle = image.handle;
        memory = image.memory;
        layout = image.layout;
        format = image.format;
        m_logicalDevice = image.m_logicalDevice;
        image.handle = VK_NULL_HANDLE;
        image.memory = VK_NULL_HANDLE;
        image.layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        image.format = VkFormat::VK_FORMAT_UNDEFINED;
        image.m_logicalDevice = nullptr;

        return *this;
    }

    Result<> Image::Create(
        LogicalDevice& logicalDevice,
        const Vector3ui32& imageDimensions,
        const VkImageType imageType,
        const VkFormat imageFormat/*,
        const Vulkan::FilteredMemoryTypes& filteredMemoryTypes*/)
    {
        // Clean up previously created image if any and prepare destroyer.
        Destroy();
        m_logicalDevice = &logicalDevice;

        SmartFunction destroyer = [&]()
        {
            Destroy();
        };

        Result<> result;
        auto logicalDeviceHandle = logicalDevice.GetHandle();

        // Create image.
        if (!(result = LoadImage(
            logicalDeviceHandle,
            imageDimensions,
            imageType,
            imageFormat/*,
            filteredMemoryTypes*/)))
        {
            return result;
        }

        destroyer.Release();
        return result;
    }

    Result<> Image::Create(
        LogicalDevice& logicalDevice,
        VkCommandBuffer& commandBuffer,
        DeviceBuffer& stagingBuffer,
        const Vector3ui32& imageDimensions,
        const VkImageType imageType,
        const VkFormat imageFormat,
        const VkImageLayout imageLayout/*,
        const Vulkan::FilteredMemoryTypes& filteredMemoryTypes*/)
    {
        // Clean up previously created image if any and prepare destroyer.
        Destroy();
        m_logicalDevice = &logicalDevice;

        SmartFunction destroyer = [&]()
        {
            Destroy();
        };

        Result<> result;
        auto logicalDeviceHandle = logicalDevice.GetHandle();

        // Create image.
        if (!(result = LoadImage(
            logicalDeviceHandle,
            imageDimensions,
            imageType,
            imageFormat/*,
            filteredMemoryTypes*/)))
        {
            return result;
        }

        // Copy data from staging buffer and set image layout.
        constexpr VkImageLayout transferLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        if (!Vulkan::TransitionImageLayout(commandBuffer, handle, layout, transferLayout))
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }
        layout = transferLayout;

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent.width = imageDimensions.x;
        region.imageExtent.height = imageDimensions.y;
        region.imageExtent.depth = imageDimensions.z;

        vkCmdCopyBufferToImage(commandBuffer, stagingBuffer.buffer, handle, layout, 1, &region);

        if (!Vulkan::TransitionImageLayout(commandBuffer, handle, layout, imageLayout))
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }
        layout = imageLayout;

        // Finish.
        destroyer.Release();
        return result;
    }

    Result<> Image::Update(
        VkCommandBuffer& commandBuffer,
        DeviceBuffer& stagingBuffer,
        const Vector3ui32& destinationDimensions,
        const Vector3ui32& destinationOffset)
    {
        Result<> result;

        auto oldLayout = layout;

        constexpr VkImageLayout transferLayout = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        if (!Vulkan::TransitionImageLayout(commandBuffer, handle, layout, transferLayout))
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }
        layout = transferLayout;

        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { static_cast<int32_t>(destinationOffset.x), static_cast<int32_t>(destinationOffset.y), static_cast<int32_t>(destinationOffset.z) };
        region.imageExtent.width = destinationDimensions.x;
        region.imageExtent.height = destinationDimensions.y;
        region.imageExtent.depth = destinationDimensions.z;

        vkCmdCopyBufferToImage(commandBuffer, stagingBuffer.buffer, handle, layout, 1, &region);

        if (!Vulkan::TransitionImageLayout(commandBuffer, handle, layout, oldLayout))
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }
        layout = oldLayout;

        return result;
    }

    void Image::Destroy()
    {
        if(m_logicalDevice)
        {
            auto logicalDeviceHandle = m_logicalDevice->GetHandle();

            if (handle != VK_NULL_HANDLE)
            {
                vkDestroyImage(logicalDeviceHandle, handle, nullptr);
                handle = VK_NULL_HANDLE;
            }
            if (memory != VK_NULL_HANDLE)
            {
                vkFreeMemory(logicalDeviceHandle, memory, nullptr);
                memory = VK_NULL_HANDLE;
            }
        }     
    }

    Result<> Image::LoadImage(
        VkDevice& logicalDeviceHandle,
        const Vector3ui32& imageDimensions,
        const VkImageType imageType,
        const VkFormat imageFormat/*,
        const Vulkan::FilteredMemoryTypes& filteredMemoryTypes*/)
    {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.flags = 0;
        imageInfo.imageType = imageType;
        imageInfo.extent.width = static_cast<uint32_t>(imageDimensions.x);
        imageInfo.extent.height = static_cast<uint32_t>(imageDimensions.y);
        imageInfo.extent.depth = static_cast<uint32_t>(imageDimensions.z);
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = imageFormat;
        imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

        Result<> result;
        if (!(result = vkCreateImage(logicalDeviceHandle, &imageInfo, nullptr, &handle)))
        {
            return result;
        }

        VkMemoryRequirements memoryRequirements = {};
        vkGetImageMemoryRequirements(logicalDeviceHandle, handle, &memoryRequirements);

        uint32_t memoryTypeIndex = 0;
        /*if (!Vulkan::FindFilteredMemoryTypeIndex(
            memoryTypeIndex,
            filteredMemoryTypes,
            memoryRequirements.memoryTypeBits))
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }*/

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;
        if (!(result = vkAllocateMemory(logicalDeviceHandle, &allocInfo, nullptr, &memory)))
        {
            return result;
        }

        if (!(result = vkBindImageMemory(logicalDeviceHandle, handle, memory, 0)))
        {
            return result;
        }

        format = imageFormat;

        return result;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif