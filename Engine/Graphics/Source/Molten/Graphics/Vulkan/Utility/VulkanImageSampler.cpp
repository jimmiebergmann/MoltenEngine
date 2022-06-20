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

#include "Molten/Graphics/Vulkan/Utility/VulkanImageSampler.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanPhysicalDevice.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Image sampler implementations.
    ImageSampler::ImageSampler() :
        m_handle(VK_NULL_HANDLE),
        m_logicalDevice(nullptr)
    {}

    ImageSampler::~ImageSampler()
    {
        Destroy();
    }

    ImageSampler::ImageSampler(ImageSampler&& imageSampler) noexcept :
        m_handle(imageSampler.m_handle),
        m_logicalDevice(imageSampler.m_logicalDevice)
    {
        imageSampler.m_handle = VK_NULL_HANDLE;
        imageSampler.m_logicalDevice = nullptr;
    }

    ImageSampler& ImageSampler::operator =(ImageSampler&& imageSampler) noexcept
    {
        m_handle = imageSampler.m_handle;
        m_logicalDevice = imageSampler.m_logicalDevice;
        imageSampler.m_handle = VK_NULL_HANDLE;
        imageSampler.m_logicalDevice = nullptr;
        return *this;
    }

    Result<> ImageSampler::Create(
        LogicalDevice& logicalDevice,
        VkFilter magFilter,
        VkFilter minFilter,
        VkSamplerAddressMode addressModeU,
        VkSamplerAddressMode addressModeV,
        VkSamplerAddressMode addressModeW,
        VkBool32 anisotropyEnable,
        float maxAnisotropy)
    {
        m_logicalDevice = &logicalDevice;

        auto logicalDeviceHandle = logicalDevice.GetHandle();
        auto& physicalDevice = logicalDevice.GetPhysicalDevice();
        auto& physicalDeviceLimits = physicalDevice.GetCapabilities().properties.limits;

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = magFilter;
        samplerInfo.minFilter = minFilter;
        samplerInfo.addressModeU = addressModeU;
        samplerInfo.addressModeV = addressModeV;
        samplerInfo.addressModeW = addressModeW;
        samplerInfo.anisotropyEnable = anisotropyEnable & logicalDevice.GetEnabledFeatures().samplerAnisotropy;
        samplerInfo.maxAnisotropy = std::min(maxAnisotropy, physicalDeviceLimits.maxSamplerAnisotropy);
        samplerInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        return vkCreateSampler(logicalDeviceHandle, &samplerInfo, nullptr, &m_handle);
    }

    VkSampler ImageSampler::GetHandle() const
    {
        return m_handle;
    }

    void ImageSampler::Destroy()
    {
        if (m_logicalDevice)
        {
            if (m_handle != VK_NULL_HANDLE)
            {
                vkDestroySampler(m_logicalDevice->GetHandle(), m_handle, nullptr);
                m_handle = VK_NULL_HANDLE;
            }
        }
    }

    LogicalDevice& ImageSampler::GetLogicalDevice()
    {
        return *m_logicalDevice;
    }
    const LogicalDevice& ImageSampler::GetLogicalDevice() const
    {
        return *m_logicalDevice;
    }

    bool ImageSampler::HasLogicalDevice() const
    {
        return m_logicalDevice != nullptr;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif