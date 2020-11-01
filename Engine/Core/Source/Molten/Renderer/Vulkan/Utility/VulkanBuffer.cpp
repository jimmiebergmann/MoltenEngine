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


#include "Molten/Renderer/Vulkan/Utility/VulkanBuffer.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"
#include "Molten/Utility/SmartFunction.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Device buffer implementations.
    DeviceBuffer::DeviceBuffer() :
        m_handle(VK_NULL_HANDLE),
        m_memory(VK_NULL_HANDLE),
        m_logicalDevice(nullptr)
    {}

    DeviceBuffer::~DeviceBuffer()
    {
        Destroy();
    }

    DeviceBuffer::DeviceBuffer(DeviceBuffer&& deviceBuffer) noexcept :
        m_handle(deviceBuffer.m_handle),
        m_memory(deviceBuffer.m_memory),
        m_logicalDevice(deviceBuffer.m_logicalDevice)
    {
        deviceBuffer.m_handle = VK_NULL_HANDLE;
        deviceBuffer.m_memory = VK_NULL_HANDLE;
        deviceBuffer.m_logicalDevice = nullptr;
    }

    DeviceBuffer& DeviceBuffer::operator =(DeviceBuffer&& deviceBuffer) noexcept
    {
        m_handle = deviceBuffer.m_handle;
        m_memory = deviceBuffer.m_memory;
        m_logicalDevice = deviceBuffer.m_logicalDevice;
        deviceBuffer.m_handle = VK_NULL_HANDLE;
        deviceBuffer.m_memory = VK_NULL_HANDLE;
        deviceBuffer.m_logicalDevice = nullptr;
        return *this;
    }

    Result<> DeviceBuffer::Create(
        LogicalDevice& logicalDevice,
        const VkDeviceSize size,
        const VkBufferUsageFlags usage,
        const Vulkan::FilteredMemoryTypes& filteredMemoryTypes)
    {
        Destroy();

        m_logicalDevice = &logicalDevice;
        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        Result<> result;
        if (!(result = vkCreateBuffer(logicalDeviceHandle, &bufferInfo, nullptr, &m_handle)))
        {
            return result;
        }

        SmartFunction destroyer = [&]()
        {
            Destroy();
        };

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(logicalDeviceHandle, m_handle, &memoryRequirements);

        uint32_t memoryTypeIndex = 0;
        if (!Vulkan::FindFilteredMemoryTypeIndex(
            memoryTypeIndex,
            filteredMemoryTypes,
            memoryRequirements.memoryTypeBits))
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        if (!(result = vkAllocateMemory(logicalDeviceHandle, &memoryAllocateInfo, nullptr, &m_memory)))
        {
            return result;
        }

        if (!(result = vkBindBufferMemory(logicalDeviceHandle, m_handle, m_memory, 0)))
        {
            return result;
        }

        destroyer.Release();
        return result;
    }

    void DeviceBuffer::Destroy()
    {
        if (m_logicalDevice)
        {
            auto logicalDeviceHandle = m_logicalDevice->GetHandle();

            if (m_handle != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(logicalDeviceHandle, m_handle, nullptr);
                m_handle = VK_NULL_HANDLE;
            }
            if (m_memory != VK_NULL_HANDLE)
            {
                vkFreeMemory(logicalDeviceHandle, m_memory, nullptr);
                m_memory = VK_NULL_HANDLE;
            }
        }
    }

    bool DeviceBuffer::IsCreated() const
    {
        return m_handle != VK_NULL_HANDLE;
    }

    Result<> DeviceBuffer::MapMemory(
        VkDeviceSize offset,
        VkDeviceSize size,
        const void* data,
        VkMemoryMapFlags flags)
    {
        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        Result<> result;
        void* mappedData = nullptr;
        if (!(result = vkMapMemory(logicalDeviceHandle, m_memory, 0, size, 0, &mappedData)))
        {
            return result;
        }
        std::memcpy(mappedData, data, (size_t)size);
        vkUnmapMemory(logicalDeviceHandle, m_memory);

        return result;
    }

    Result<> DeviceBuffer::CopyFromBuffer(
        const VkCommandPool commandPool,
        const DeviceBuffer& source,
        const VkDeviceSize& size)
    {
        Result<> result;

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        if (!(result = Vulkan::BeginSingleTimeCommands(commandBuffer, *m_logicalDevice, commandPool)))
        {
            return result;
        }

        VkBufferCopy copy = {};
        copy.size = size;
        vkCmdCopyBuffer(commandBuffer, source.m_handle, m_handle, 1, &copy);

        if (!(result = Vulkan::EndSingleTimeCommands(commandBuffer, *m_logicalDevice, commandPool)))
        {
            return result;
        }

        return result;
    }

    VkBuffer DeviceBuffer::GetHandle() const
    {
        return m_handle;
    }

    VkDeviceMemory DeviceBuffer::GetMemory() const
    {
        return m_memory;
    }

    LogicalDevice& DeviceBuffer::GetLogicalDevice()
    {
        return *m_logicalDevice;
    }
    const LogicalDevice& DeviceBuffer::GetLogicalDevice() const
    {
        return *m_logicalDevice;
    }

    bool DeviceBuffer::HasLogicalDevice() const
    {
        return m_logicalDevice != nullptr;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif