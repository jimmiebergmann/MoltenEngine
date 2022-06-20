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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANDEVICEBUFFER_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANDEVICEBUFFER_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanMemory.hpp"
#include <vector>

namespace Molten::Vulkan
{

    /** Forward declarations. */
    class LogicalDevice;
    class MemoryAllocator;


    /** Vulkan device buffer, backed by device memory. */
    struct MOLTEN_API DeviceBuffer
    {
        DeviceBuffer();

        ~DeviceBuffer() = default;

        DeviceBuffer(DeviceBuffer&& deviceBuffer) noexcept;
        DeviceBuffer& operator =(DeviceBuffer&& deviceBuffer) noexcept;

        DeviceBuffer(const DeviceBuffer&) = delete;
        DeviceBuffer& operator =(const DeviceBuffer&) = delete;

        /** Returns true if buffer == VK_NULL_HANDLE and memory == nullptr. */
        [[nodiscard]] bool IsEmpty() const;

        VkBuffer buffer;
        MemoryHandle memory;
    };


    /** External device buffer guard.
     *  DeviceBuffer object is not returned if DeviceBuffer::IsEmpty is true
     *  or Release is called before destruction.
     */
    class MOLTEN_API DeviceBufferGuard
    {

    public:

        [[nodiscard]] DeviceBufferGuard(
            MemoryAllocator& memoryAllocator,
            DeviceBuffer& deviceBuffer);

        ~DeviceBufferGuard();

        DeviceBufferGuard(const DeviceBufferGuard&) = delete;
        DeviceBufferGuard(DeviceBufferGuard&&) = delete;
        DeviceBufferGuard& operator =(const DeviceBufferGuard&) = delete;
        DeviceBufferGuard& operator =(DeviceBufferGuard&&) = delete;

        /** Provided DeviceBuffer object is released from guard.*/
        void Release();

    private:

        MemoryAllocator& m_memoryAllocator;
        DeviceBuffer* m_deviceBuffer;

    };


    /** Map and copy data to device buffer's memory. */
    MOLTEN_API Result<> MapMemory(
        LogicalDevice& logicalDevice,
        DeviceBuffer& deviceBuffer,
        const void* data,
        const VkDeviceSize size,
        const VkDeviceSize offset);

    /** Copy data between two device buffers. */
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
