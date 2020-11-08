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
#include "Molten/Renderer/Vulkan/Utility/VulkanBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"
#include "Molten/Utility/SmartFunction.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Image implementations.
    Image::Image() :
        m_handle(VK_NULL_HANDLE),
        m_memory(VK_NULL_HANDLE),
        m_layout(VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED),
        m_format(VkFormat::VK_FORMAT_UNDEFINED),
        m_logicalDevice(nullptr)
    {}

    Image::~Image()
    {
        Destroy();
    }

    Image::Image(Image&& image) noexcept :
        m_handle(image.m_handle),
        m_memory(image.m_memory),
        m_layout(image.m_layout),
        m_format(image.m_format),
        m_logicalDevice(image.m_logicalDevice)
    {
        image.m_handle = VK_NULL_HANDLE;
        image.m_memory = VK_NULL_HANDLE;
        image.m_layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        image.m_format = VkFormat::VK_FORMAT_UNDEFINED;
        image.m_logicalDevice = nullptr;
    }

    Image& Image::operator =(Image&& image) noexcept
    {
        m_handle = image.m_handle;
        m_memory = image.m_memory;
        m_layout = image.m_layout;
        m_format = image.m_format;
        m_logicalDevice = image.m_logicalDevice;
        image.m_handle = VK_NULL_HANDLE;
        image.m_memory = VK_NULL_HANDLE;
        image.m_layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        image.m_format = VkFormat::VK_FORMAT_UNDEFINED;
        image.m_logicalDevice = nullptr;
        return *this;
    }

    Result<> Image::Create(
        LogicalDevice& logicalDevice,
        const Vector2ui32& imageDimensions,
        const VkFormat imageFormat,
        const Vulkan::FilteredMemoryTypes& filteredMemoryTypes)
    {
        Destroy();

        m_logicalDevice = &logicalDevice;

        SmartFunction destroyer = [&]()
        {
            Destroy();
        };

        Result<> result;
        if (!(result = LoadImage(
            imageDimensions,
            imageFormat,
            filteredMemoryTypes)))
        {
            return result;
        }

        destroyer.Release();
        return result;    
    }

    Result<> Image::Create(
        LogicalDevice& logicalDevice,
        DeviceBuffer& stagingBuffer,
        const VkCommandPool commandPool,
        const Vector2ui32& imageDimensions,
        const VkFormat imageFormat,
        const Vulkan::FilteredMemoryTypes& filteredMemoryTypes)
    {
        Destroy();

        m_logicalDevice = &logicalDevice;

        SmartFunction destroyer = [&]()
        {
            Destroy();
        };

        Result<> result;
        if (!(result = LoadImage(
            imageDimensions,
            imageFormat,
            filteredMemoryTypes)))
        {
            return result;
        }

        if (!(result = CopyFromBuffer(
            commandPool, 
            stagingBuffer, 
            imageDimensions)))
        {
            return result;
        }

        destroyer.Release();
        return result;
    }

    void Image::Destroy()
    {
        if (m_logicalDevice)
        {
            auto logicalDeviceHandle = m_logicalDevice->GetHandle();

            if (m_handle != VK_NULL_HANDLE)
            {
                vkDestroyImage(logicalDeviceHandle, m_handle, nullptr);
                m_handle = VK_NULL_HANDLE;
            }
            if (m_memory != VK_NULL_HANDLE)
            {
                vkFreeMemory(logicalDeviceHandle, m_memory, nullptr);
                m_memory = VK_NULL_HANDLE;
            }

            m_logicalDevice = nullptr;
        }
    }

    bool Image::IsCreated() const
    {
        return m_handle != VK_NULL_HANDLE;
    }

    Result<> Image::CopyFromBuffer(
        const VkCommandPool commandPool,
        const DeviceBuffer& source,
        const Vector2ui32 dimensions,
        bool restoreLayout)
    {
        Result<> result;

        // Transition layout to destination optional.
        auto oldLayout = m_layout;
        if (oldLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            if (!(result = TransitionToLayout(commandPool, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)))
            {
                return result;
            }
        }

        // Copy buffer.
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        if (!(result = BeginSingleTimeCommands(commandBuffer, *m_logicalDevice, commandPool)))
        {
            return result;
        }

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent.width = dimensions.x;
        region.imageExtent.height = dimensions.y;
        region.imageExtent.depth = 1;

        vkCmdCopyBufferToImage(commandBuffer, source.GetHandle(), m_handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        if (!(result = Vulkan::EndSingleTimeCommands(commandBuffer, *m_logicalDevice, commandPool)))
        {
            return result;
        }

        // Restore layout of asked to.
        if (restoreLayout && m_layout != oldLayout)
        {
            if (!(result = TransitionToLayout(commandPool, oldLayout)))
            {
                return result;
            }
        }

        return result;
    }

    Result<> Image::TransitionToLayout(
        const VkCommandPool commandPool,
        VkImageLayout layout)
    {
        Result<> result;

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        if (!(result = Vulkan::BeginSingleTimeCommands(commandBuffer, *m_logicalDevice, commandPool)))
        {
            return result;
        }

        if (!Vulkan::TransitionImageLayout(commandBuffer, *m_logicalDevice, m_handle, m_format, m_layout, layout))
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        if (!(result = Vulkan::EndSingleTimeCommands(commandBuffer, *m_logicalDevice, commandPool)))
        {
            return result;
        }

        m_layout = layout;

        return result;
    }

    VkBuffer Image::GetHandle() const
    {
        return m_handle;
    }

    VkDeviceMemory Image::GetMemory() const
    {
        return m_memory;
    }

    VkImageLayout Image::GetLayout() const
    {
        return m_layout;
    }

    VkFormat Image::GetFormat() const
    {
        return m_format;
    }

    LogicalDevice& Image::GetLogicalDevice()
    {
        return *m_logicalDevice;
    }
    const LogicalDevice& Image::GetLogicalDevice() const
    {
        return *m_logicalDevice;
    }

    bool Image::HasLogicalDevice() const
    {
        return m_logicalDevice != nullptr;
    }

    Result<> Image::LoadImage(
        const Vector2ui32& imageDimensions,
        const VkFormat imageFormat,
        const Vulkan::FilteredMemoryTypes& filteredMemoryTypes)
    {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.flags = 0;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(imageDimensions.x);
        imageInfo.extent.height = static_cast<uint32_t>(imageDimensions.y);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = imageFormat;
        imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        Result<> result;
        if (!(result = vkCreateImage(logicalDeviceHandle, &imageInfo, nullptr, &m_handle)))
        {
            return result;
        }

        VkMemoryRequirements memoryRequirements = {};
        vkGetImageMemoryRequirements(logicalDeviceHandle, m_handle, &memoryRequirements);

        uint32_t memoryTypeIndex = 0;
        if (!Vulkan::FindFilteredMemoryTypeIndex(
            memoryTypeIndex,
            filteredMemoryTypes,
            memoryRequirements.memoryTypeBits))
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;
        if (!(result = vkAllocateMemory(logicalDeviceHandle, &allocInfo, nullptr, &m_memory)))
        {
            return result;
        }

        if (!(result = vkBindImageMemory(logicalDeviceHandle, m_handle, m_memory, 0)))
        {
            return result;
        }

        m_format = imageFormat;

        return result;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif