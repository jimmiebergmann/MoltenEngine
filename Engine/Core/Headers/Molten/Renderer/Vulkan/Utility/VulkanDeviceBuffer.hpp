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

#include "Molten/Core.hpp"

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


    /** Device buffer guard. Returning DeviceBuffer member to allocator at destruction.
     *  DeviceBuffer object is not returned if DeviceBuffer::IsEmpty is true.
     *  Another approach to not return DeviceBuffer to allocator is to move it before destruction.
     */
    class MOLTEN_API GuardedDeviceBuffer
    {

    public:

        [[nodiscard]] explicit GuardedDeviceBuffer(MemoryAllocator& memoryAllocator);

        ~GuardedDeviceBuffer();

        GuardedDeviceBuffer(const GuardedDeviceBuffer&) = delete;
        GuardedDeviceBuffer(GuardedDeviceBuffer&&) = delete;
        GuardedDeviceBuffer& operator =(const GuardedDeviceBuffer&) = delete;
        GuardedDeviceBuffer& operator =(GuardedDeviceBuffer&&) = delete;

        DeviceBuffer deviceBuffer;

    private:
 
        MemoryAllocator& m_memoryAllocator;

    };

    /** External device buffer guard.
     *  Works the same way as GuardedDeviceBuffer, but for an external DeviceBuffer object, passed by the user.
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


    /** External device buffer guards.
     *  Works the same way as DeviceBufferGuard, but guards multiple DeviceBuffers.
     */
    class MOLTEN_API DeviceBufferGuards
    {

    public:

        [[nodiscard]] DeviceBufferGuards(
            MemoryAllocator& memoryAllocator,
            std::vector<DeviceBuffer>& deviceBuffers);

        ~DeviceBufferGuards();

        DeviceBufferGuards(const DeviceBufferGuards&) = delete;
        DeviceBufferGuards(DeviceBufferGuards&&) = delete;
        DeviceBufferGuards& operator =(const DeviceBufferGuards&) = delete;
        DeviceBufferGuards& operator =(DeviceBufferGuards&&) = delete;

        /** Provided DeviceBuffer vector object is released from guard.*/
        void Release();

    private:

        MemoryAllocator& m_memoryAllocator;
        std::vector<DeviceBuffer>* m_deviceBuffers;

    };
    
}

#endif

#endif
