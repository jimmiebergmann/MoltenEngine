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

namespace Molten
{


    template<size_t VDimensions>
    VulkanTexture<VDimensions>::VulkanTexture() :
        deviceImage{},
        imageView(VK_NULL_HANDLE),
        bytesPerPixel(0)
    {}

    template<size_t VDimensions>
    VulkanTexture<VDimensions>::VulkanTexture(
        Vulkan::DeviceImage&& deviceImage,
        VkImageView imageView,
        const uint8_t bytesPerPixel
    ) :
        deviceImage(std::move(deviceImage)),
        imageView(imageView),
        bytesPerPixel(bytesPerPixel)
    {}


    template<size_t VDimensions>
    VulkanTexture<VDimensions>::VulkanTexture(VulkanTexture&& vulkanTexture) noexcept :
        deviceImage(std::move(vulkanTexture.deviceImage)),
        imageView(vulkanTexture.imageView),
        bytesPerPixel(vulkanTexture.bytesPerPixel)
    {
        vulkanTexture.imageView = VK_NULL_HANDLE;
        vulkanTexture.bytesPerPixel = 0;
    }

    template<size_t VDimensions>
    VulkanTexture<VDimensions>& VulkanTexture<VDimensions>::operator = (VulkanTexture&& vulkanTexture) noexcept
    {
        deviceImage = std::move(vulkanTexture.deviceImage);
        imageView = vulkanTexture.imageView;
        bytesPerPixel = vulkanTexture.bytesPerPixel;

        vulkanTexture.imageView = VK_NULL_HANDLE;
        vulkanTexture.bytesPerPixel = 0;
        return *this;
    }

}