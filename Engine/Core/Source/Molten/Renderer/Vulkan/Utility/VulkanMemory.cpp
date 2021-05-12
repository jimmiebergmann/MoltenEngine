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

#include "Molten/Renderer/Vulkan/Utility/VulkanMemory.hpp"

#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryImpl.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

namespace Molten::Vulkan
{

    // Global functions implementations.
    Result<> MapMemory(
        LogicalDevice& logicalDevice,
        MemoryHandle memoryHandle,
        const void* data,
        const VkDeviceSize size,
        const VkDeviceSize offset)
    {
        auto deviceMemory = memoryHandle->memoryBlock->deviceMemory;      

        const auto mapOffset = memoryHandle->offset + offset;
        if(mapOffset + size > memoryHandle->offset + memoryHandle->size)
        {
            return VkResult::VK_ERROR_MEMORY_MAP_FAILED;
        }

        auto logicalDeviceHandle = logicalDevice.GetHandle();

        Result<> result;
        void* mappedData = nullptr;
        if (!(result = vkMapMemory(logicalDeviceHandle, deviceMemory, mapOffset, size, 0, &mappedData)))
        {
            return result;
        }

        std::memcpy(mappedData, data, size);
        vkUnmapMemory(logicalDeviceHandle, deviceMemory);

        return result;
    }

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
        Result<> result;

        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        if (!(result = BeginSingleTimeCommands(commandBuffer, logicalDevice, commandPool)))
        {
            return result;
        }

        VkBufferCopy copy = {};
        copy.size = size;
        vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &copy);

        if (!(result = EndSingleTimeCommands(commandBuffer, logicalDevice, commandPool)))
        {
            return result;
        }

        return result;
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