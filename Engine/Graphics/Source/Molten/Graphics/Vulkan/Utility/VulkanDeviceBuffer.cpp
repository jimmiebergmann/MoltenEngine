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

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Graphics/Vulkan/Utility/VulkanDeviceBuffer.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanMemoryAllocator.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanFunctions.hpp"

namespace Molten::Vulkan
{

    // Device buffer implementations.
    DeviceBuffer::DeviceBuffer() :
        buffer(VK_NULL_HANDLE),
        memory(nullptr)
    {}

    DeviceBuffer::DeviceBuffer(DeviceBuffer&& deviceBuffer) noexcept :
        buffer(deviceBuffer.buffer),
        memory(deviceBuffer.memory)
    {
        deviceBuffer.buffer = VK_NULL_HANDLE;
        deviceBuffer.memory = nullptr;
    }

    DeviceBuffer& DeviceBuffer::operator =(DeviceBuffer&& deviceBuffer) noexcept
    {
        buffer = deviceBuffer.buffer;
        memory = deviceBuffer.memory;
        deviceBuffer.buffer = VK_NULL_HANDLE;
        deviceBuffer.memory = nullptr;
        return *this;
    }

    bool DeviceBuffer::IsEmpty() const
    {
        return buffer == VK_NULL_HANDLE && memory == nullptr;
    }


    // Device buffer guard implementations.
    DeviceBufferGuard::DeviceBufferGuard(
        MemoryAllocator& memoryAllocator,
        DeviceBuffer& deviceBuffer
    ) :
        m_memoryAllocator(memoryAllocator),
        m_deviceBuffer(&deviceBuffer)
    {}

    DeviceBufferGuard::~DeviceBufferGuard()
    {
        if(m_deviceBuffer && !m_deviceBuffer->IsEmpty())
        {
            m_memoryAllocator.FreeDeviceBuffer(*m_deviceBuffer);
        }
    }

    void DeviceBufferGuard::Release()
    {
        m_deviceBuffer = nullptr;
    }


    // Global functions implementations.
    Result<> MapMemory(
        LogicalDevice& logicalDevice,
        DeviceBuffer& deviceBuffer,
        const void* data,
        const VkDeviceSize size,
        const VkDeviceSize offset)
    {
        return MapMemory(logicalDevice, deviceBuffer.memory, data, size, offset);
    }

    Result<> CopyMemory(
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool,
        VkBuffer sourceBuffer,
        VkBuffer destinationBuffer,
        const VkDeviceSize& size)
    {
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        if (const auto result = BeginSingleTimeCommands(commandBuffer, logicalDevice, commandPool); !result.IsSuccessful())
        {
            return result;
        }

        VkBufferCopy copy = {};
        copy.size = size;
        vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &copy);

        if (const auto result = EndSingleTimeCommands(commandBuffer, logicalDevice, commandPool); !result.IsSuccessful())
        {
            return result;
        }

        return {};
    }

    Result<> CopyMemory(
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool,
        DeviceBuffer& sourceBuffer,
        DeviceBuffer& destinationBuffer,
        const VkDeviceSize& size)
    {
        return CopyMemory(logicalDevice, commandPool, sourceBuffer.buffer, destinationBuffer.buffer, size);
    }

}

#endif