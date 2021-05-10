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

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceImage.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryAllocator.hpp"

namespace Molten::Vulkan
{

    // Device image implementations.
    DeviceImage::DeviceImage() :
        image(VK_NULL_HANDLE),
        layout(VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED),
        memory(nullptr)
    {}

    DeviceImage::DeviceImage(DeviceImage&& deviceImage) noexcept :
        image(deviceImage.image),
        layout(deviceImage.layout),
        memory(deviceImage.memory)
    {
        deviceImage.image = VK_NULL_HANDLE;
        layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        deviceImage.memory = nullptr;
    }

    DeviceImage& DeviceImage::operator =(DeviceImage&& deviceImage) noexcept
    {
        image = deviceImage.image;
        layout = deviceImage.layout;
        memory = deviceImage.memory;
        deviceImage.image = VK_NULL_HANDLE;
        deviceImage.memory = nullptr;
        layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        return *this;
    }

    bool DeviceImage::IsEmpty() const
    {
        return image == VK_NULL_HANDLE && memory == nullptr;
    }


    // Device image guard implementations.
    DeviceImageGuard::DeviceImageGuard(
        MemoryAllocator& memoryAllocator,
        DeviceImage& deviceImage
    ) :
        m_memoryAllocator(memoryAllocator),
        m_deviceImage(&deviceImage)
    {}

    DeviceImageGuard::~DeviceImageGuard()
    {
        if (m_deviceImage && !m_deviceImage->IsEmpty())
        {
            m_memoryAllocator.FreeDeviceImage(*m_deviceImage);
        }
    }

    void DeviceImageGuard::Release()
    {
        m_deviceImage = nullptr;
    }

}

#endif