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

        /** Create image. Device memory is allocated but not copied from anything and no layout is set. */
        Result<> Create(
            LogicalDevice& logicalDevice,
            const Vector3ui32& imageDimensions,
            const VkImageType imageType,
            const VkFormat imageFormat,
            const Vulkan::FilteredMemoryTypes& filteredMemoryTypes);

        /** Create image. Device memory is allocated and copied from staging buffer. */
        Result<> Create(
            LogicalDevice& logicalDevice,
            VkCommandBuffer& commandBuffer,
            DeviceBuffer& stagingBuffer,
            const Vector3ui32& imageDimensions,
            const VkImageType imageType,
            const VkFormat imageFormat,
            const VkImageLayout imageLayout,
            const Vulkan::FilteredMemoryTypes& filteredMemoryTypes);

        /** Copy data from staging buffer to image. */
        Result<> Update(
            VkCommandBuffer& commandBuffer,
            DeviceBuffer& stagingBuffer,
            const Vector3ui32& destinationDimensions,
            const Vector3ui32& destinationOffset);

        void Destroy();

        VkImage handle;
        VkDeviceMemory memory;
        VkImageLayout layout;
        VkFormat format;

    private:

        Result<> LoadImage(
            VkDevice& logicalDeviceHandle,
            const Vector3ui32& imageDimensions,
            const VkImageType imageType,
            const VkFormat imageFormat,
            const Vulkan::FilteredMemoryTypes& filteredMemoryTypes);

        LogicalDevice* m_logicalDevice;

    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
