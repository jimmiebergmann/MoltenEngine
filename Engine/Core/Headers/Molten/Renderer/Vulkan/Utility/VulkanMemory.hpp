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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORY_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORY_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanHeaders.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"

namespace Molten::Vulkan
{

    /** Forward declarations. */
    class LogicalDevice;
    struct DeviceBuffer;


    /* Forward declaration. Declared in VulkanMemoryDeclaration.hpp.
     * @see MemoryHandle for usage.
     */
    struct Memory;

    /* Opaque pointer to memory object. Use this for referencing allocated memory.
     * Never call delete on a MemoryHandle, but instead return it to the MemoryAllocator through its public interface.
     */
    using MemoryHandle = Memory*;


    /** Map and copy data to memory. */
    /**@{*/
    MOLTEN_API Result<> MapMemory(
        LogicalDevice& logicalDevice,
        MemoryHandle memoryHandle,
        const void* data,
        const VkDeviceSize size,
        const VkDeviceSize offset);

    MOLTEN_API Result<> MapMemory(
        LogicalDevice& logicalDevice,
        DeviceBuffer& deviceBuffer,
        const void* data,
        const VkDeviceSize size,
        const VkDeviceSize offset);
    /**@}*/

    /** Copy data between two buffers. */
    /**@{*/
    MOLTEN_API Result<> CopyMemory(
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool,
        VkBuffer sourceBuffer,
        VkBuffer destinationBuffer,
        const VkDeviceSize& size);

    MOLTEN_API Result<> CopyMemory(
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool,
        DeviceBuffer& sourceBuffer,
        DeviceBuffer& destinationBuffer,
        const VkDeviceSize& size);
    /**@}*/

}

#endif

#endif