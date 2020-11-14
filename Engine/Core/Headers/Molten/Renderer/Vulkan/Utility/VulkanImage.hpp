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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANIMAGE_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANIMAGE_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryType.hpp"
#include "Molten/Math/Vector.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class LogicalDevice;
    class DeviceBuffer;


    /** Vulkan image, backed by device memory. */
    class MOLTEN_API Image
    {

    public:

        Image();
        ~Image();

        Image(const Image&) = delete;
        Image& operator = (const Image&) = delete;

        Image(Image&& image) noexcept;
        Image& operator =(Image&& image) noexcept;

        /** Create image. Device memory is allocated but not populated with anything. */
        Result<> Create(
            LogicalDevice& logicalDevice,
            const Vector2ui32& imageDimensions,
            const VkFormat imageFormat,
            const Vulkan::FilteredMemoryTypes& filteredMemoryTypes);

        /** Create image. Device memory is allocated and copied from staging buffer. */
        Result<> Create(
            LogicalDevice& logicalDevice,
            DeviceBuffer& stagingBuffer,
            const VkCommandPool commandPool,
            const Vector2ui32& imageDimensions,
            const VkFormat imageFormat,
            const Vulkan::FilteredMemoryTypes& filteredMemoryTypes);

        void Destroy();

        bool IsCreated() const;

        Result<> CopyFromBuffer(
            const VkCommandPool commandPool,
            const DeviceBuffer& source,
            const Vector2ui32 dimensions,
            bool restoreLayout = false);

        Result<> TransitionToLayout(
            const VkCommandPool commandPool,
            VkImageLayout layout);

        VkBuffer GetHandle() const;

        VkDeviceMemory GetMemory() const;

        VkImageLayout GetLayout() const;

        VkFormat GetFormat() const;

        LogicalDevice& GetLogicalDevice();
        const LogicalDevice& GetLogicalDevice() const;

        bool HasLogicalDevice() const;

    private:

        Result<> LoadImage(
            const Vector2ui32& imageDimensions,
            const VkFormat imageFormat,
            const Vulkan::FilteredMemoryTypes& filteredMemoryTypes);

        VkImage m_handle;
        VkDeviceMemory m_memory;
        VkImageLayout m_layout;
        VkFormat m_format;
        LogicalDevice* m_logicalDevice;

    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif