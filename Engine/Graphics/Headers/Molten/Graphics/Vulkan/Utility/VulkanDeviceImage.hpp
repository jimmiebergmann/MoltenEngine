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

#ifndef MOLTEN_GRAPHICS_VULKAN_UTILITY_VULKANDEVICEIMAGE_HPP
#define MOLTEN_GRAPHICS_VULKAN_UTILITY_VULKANDEVICEIMAGE_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Graphics/Vulkan/Utility/VulkanMemory.hpp"

namespace Molten::Vulkan
{

    /** Forward declarations. */
    class MemoryAllocator;
    struct DeviceBuffer;


    /** Vulkan device image, backed by device memory. */
    struct MOLTEN_GRAPHICS_API DeviceImage
    {
        DeviceImage();

        ~DeviceImage() = default;

        DeviceImage(DeviceImage&& deviceImage) noexcept;
        DeviceImage& operator =(DeviceImage&& deviceImage) noexcept;

        DeviceImage(const DeviceImage&) = delete;
        DeviceImage& operator =(const DeviceImage&) = delete;

        /** Returns true if image == VK_NULL_HANDLE and memory == nullptr. */
        [[nodiscard]] bool IsEmpty() const;

        VkImage image;
        VkImageLayout layout;
        MemoryHandle memory;
    };


    /** External device image guard.
     *  Destroys device image, by returning it to memory allocation, at destruction.
     *  Call Release before guard destruction to prevent image device from being destroyed.
     */
    class MOLTEN_GRAPHICS_API DeviceImageGuard
    {

    public:

        [[nodiscard]] DeviceImageGuard(
            MemoryAllocator& memoryAllocator,
            DeviceImage& deviceImage);

        ~DeviceImageGuard();

        DeviceImageGuard(const DeviceImageGuard&) = delete;
        DeviceImageGuard(DeviceImageGuard&&) = delete;
        DeviceImageGuard& operator =(const DeviceImageGuard&) = delete;
        DeviceImageGuard& operator =(DeviceImageGuard&&) = delete;

        /** Provided DeviceImage object is released from guard and prevents destruction at guard destruction.*/
        void Release();

    private:

        MemoryAllocator& m_memoryAllocator;
        DeviceImage* m_deviceImage;

    };


    /** Copy device buffer to device image.
     *  Image layout is transitioned to finalImageLayout at completion.
     *  Provide deviceImage.layout to restore it at completion.
     */
    MOLTEN_GRAPHICS_API bool CopyDeviceBufferToDeviceImage(
        DeviceBuffer& deviceBuffer,
        DeviceImage& deviceImage,
        VkCommandBuffer commandBuffer,
        const VkBufferImageCopy& bufferImageCopy,
        const VkImageLayout finalImageLayout);

}

#endif

#endif