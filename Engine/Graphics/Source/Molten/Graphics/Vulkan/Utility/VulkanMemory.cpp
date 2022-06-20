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

#include "Molten/Graphics/Vulkan/Utility/VulkanMemory.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanMemoryImpl.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanLogicalDevice.hpp"

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

        void* mappedData = nullptr;
        if (const auto result = vkMapMemory(logicalDeviceHandle, deviceMemory, mapOffset, size, 0, &mappedData); result != VK_SUCCESS)
        {
            return result;
        }

        std::memcpy(mappedData, data, size);
        vkUnmapMemory(logicalDeviceHandle, deviceMemory);

        return {};
    }

}

#endif