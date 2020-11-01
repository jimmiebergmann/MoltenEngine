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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANBUFFER_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANBUFFER_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryType.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class LogicalDevice;


    /** Vulkan buffer, backed by device memory. */
    class MOLTEN_API DeviceBuffer
    {

    public:

        DeviceBuffer();
        ~DeviceBuffer();

        DeviceBuffer(const DeviceBuffer&) = delete;
        DeviceBuffer& operator = (const DeviceBuffer&) = delete;
        
        DeviceBuffer(DeviceBuffer&& deviceBuffer) noexcept;
        DeviceBuffer& operator =(DeviceBuffer&& deviceBuffer) noexcept;

        Result<> Create(
            LogicalDevice& logicalDevice,
            const VkDeviceSize size,
            const VkBufferUsageFlags usage,
            const Vulkan::FilteredMemoryTypes& filteredMemoryTypes);

        void Destroy();

        bool IsCreated() const;

        Result<> MapMemory(
            VkDeviceSize offset,
            VkDeviceSize size,
            const void* data,
            VkMemoryMapFlags flags = 0);

        Result<> Copy(
            const VkCommandPool commandPool,
            DeviceBuffer& destination,
            VkDeviceSize size) const;

        VkBuffer GetHandle() const;

        VkDeviceMemory GetMemory() const;

        LogicalDevice& GetLogicalDevice();
        const LogicalDevice& GetLogicalDevice() const;

        bool HasLogicalDevice() const;

    private:

        VkBuffer m_handle;
        VkDeviceMemory m_memory;
        LogicalDevice* m_logicalDevice;
    
    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
