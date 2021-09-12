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

#include "Molten/Renderer/Vulkan/Utility/VulkanSwapChain.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanSurface.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"
#include "Molten/Utility/SmartFunction.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Swap chain implementations.
    SwapChain::SwapChain() :
        m_handle(VK_NULL_HANDLE),
        m_logicalDevice(nullptr),
        m_extent{ 0, 0 },
        m_presentMode(VkPresentModeKHR::VK_PRESENT_MODE_MAX_ENUM_KHR),
        m_surfaceFormat{},
        m_images{},     
        m_imageAvailableSemaphores{},
        m_frameFences{},
        m_currentImageIndex(0),
        m_currentFrameIndex(0)
    {}

    SwapChain::~SwapChain()
    {
        Destroy();
    }

    Result<> SwapChain::Create(
        LogicalDevice& logicalDevice,
        const VkSurfaceFormatKHR& surfaceFormat,
        const VkPresentModeKHR presentMode,
        const uint32_t imageCount)
    {
        Destroy();

        m_logicalDevice = &logicalDevice;
        m_presentMode = presentMode;
        m_surfaceFormat = surfaceFormat;

        return Load(imageCount);
    }

    void SwapChain::Destroy()
    {
        if (m_logicalDevice)
        {
            if (m_handle != VK_NULL_HANDLE)
            {
                UnloadAssociatedObjects();
                vkDestroySwapchainKHR(m_logicalDevice->GetHandle(), m_handle, nullptr);
                m_handle = VK_NULL_HANDLE;
                
            }
            m_logicalDevice = nullptr;
        }
        
    }

    Result<> SwapChain::Recreate()
    {
        const auto imageCount = static_cast<uint32_t>(m_images.size());
        m_logicalDevice->WaitIdle();
        UnloadAssociatedObjects();
        return Load(imageCount);
    }

    bool SwapChain::IsCreated()
    {
        return m_handle != VK_NULL_HANDLE;
    }

    Result<> SwapChain::AcquireNextImage()
    {
        const auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        vkWaitForFences(logicalDeviceHandle, 1, &m_frameFences[m_currentFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());

        if (const auto acquireImageResult = vkAcquireNextImageKHR(
            logicalDeviceHandle,
            m_handle,
            std::numeric_limits<uint64_t>::max(),
            m_imageAvailableSemaphores[m_currentFrameIndex],
            VK_NULL_HANDLE,
            &m_currentImageIndex); acquireImageResult != VK_SUCCESS)
        {
            return acquireImageResult;
        }

        if (m_currentImageIndex >= m_images.size())
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        if (const auto result = vkResetFences(logicalDeviceHandle, 1, &m_frameFences[m_currentFrameIndex]); result != VK_SUCCESS)
        {
            return result;
        }

        return VkResult::VK_SUCCESS;
    }

    Result<> SwapChain::PresentImage(VkSemaphore waitSemaphore)
    {
        const uint32_t waitSemaphoreCount = waitSemaphore != VK_NULL_HANDLE ? 1 : 0;
        const VkSemaphore* waitSemaphorePtr = waitSemaphoreCount ? &waitSemaphore : nullptr;

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = waitSemaphoreCount;
        presentInfo.pWaitSemaphores = waitSemaphorePtr;

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_handle;
        presentInfo.pImageIndices = &m_currentImageIndex;

        auto& deviceQueues = m_logicalDevice->GetDeviceQueues();
        if(const Result<> result = vkQueuePresentKHR(deviceQueues.presentQueue, &presentInfo); !result.IsSuccessful())
        {
            return result;
        }

        m_currentFrameIndex = (m_currentFrameIndex + 1) % static_cast<uint32_t>(m_images.size());

        return VkResult::VK_SUCCESS;
    }

    VkSwapchainKHR SwapChain::GetHandle() const
    {
        return m_handle;
    }

    VkExtent2D SwapChain::GetExtent() const
    {
        return m_extent;
    }

    VkPresentModeKHR SwapChain::GetPresentMode() const
    {
        return m_presentMode;
    }

    VkSurfaceFormatKHR SwapChain::GetSurfaceFormat() const
    {
        return m_surfaceFormat;
    }

    const Images& SwapChain::GetImages() const
    {
        return m_images;
    }

    uint32_t SwapChain::GetCurrentImageIndex() const
    {
        return m_currentImageIndex;
    }

    VkFence SwapChain::GetCurrentFrameFence() const
    {
        return m_frameFences[m_currentFrameIndex];
    }

    VkSemaphore SwapChain::GetCurrentImageAvailableSemaphore() const
    {
        return m_imageAvailableSemaphores[m_currentFrameIndex];
    }

    LogicalDevice& SwapChain::GetLogicalDevice()
    {
        return *m_logicalDevice;
    }
    const LogicalDevice& SwapChain::GetLogicalDevice() const
    {
        return *m_logicalDevice;
    }

    bool SwapChain::HasLogicalDevice() const
    {
        return m_logicalDevice != nullptr;
    }

    Result<> SwapChain::Load(const uint32_t imageCount)
    {
        const auto oldHandle = m_handle;

        SmartFunction destroyer([&]()
        {
            Destroy();
        });

        auto& physicalDevice = m_logicalDevice->GetPhysicalDevice();
        auto& surface = physicalDevice.GetSurface();
        auto& surfaceCapabilities = physicalDevice.GetCapabilities().surfaceCapabilities.capabilities;

        m_extent = surfaceCapabilities.currentExtent;

        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = surface.GetHandle();
        swapchainInfo.presentMode = m_presentMode;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.minImageCount = imageCount;
        swapchainInfo.imageFormat = m_surfaceFormat.format;
        swapchainInfo.imageColorSpace = m_surfaceFormat.colorSpace;
        swapchainInfo.imageExtent = m_extent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.oldSwapchain = oldHandle;

        auto& deviceQueues = m_logicalDevice->GetDeviceQueues();
        const uint32_t queueFamilies[2] = {
            deviceQueues.graphicsQueueIndex,
            deviceQueues.presentQueueIndex
        };

        if (queueFamilies[0] != queueFamilies[1])
        {
            swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainInfo.queueFamilyIndexCount = 2;
            swapchainInfo.pQueueFamilyIndices = queueFamilies;
        }
        else
        {
            swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainInfo.queueFamilyIndexCount = 0;
            swapchainInfo.pQueueFamilyIndices = nullptr;
        }

        const auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        if (const auto result = vkCreateSwapchainKHR(logicalDeviceHandle, &swapchainInfo, nullptr, &m_handle); result != VkResult::VK_SUCCESS)
        {
            return result;
        }
        if (m_handle == VK_NULL_HANDLE)
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        if (oldHandle)
        {
            vkDestroySwapchainKHR(logicalDeviceHandle, oldHandle, nullptr);
        }

        if (const auto result = LoadAssociatedObjects(); !result.IsSuccessful())
        {
            return result;
        }

        // Finalize.
        destroyer.Release();

        return {};
    }


    Result<> SwapChain::LoadAssociatedObjects()
    {
        const auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        SmartFunction destroyer([&]()
        {
            auto& logicalDevice = m_logicalDevice->GetHandle();

            for (auto& fence : m_frameFences)
            {
                vkDestroyFence(logicalDevice, fence, nullptr);
            }

            for (auto& semaphore : m_imageAvailableSemaphores)
            {
                vkDestroySemaphore(logicalDevice, semaphore, nullptr);
            }
        });

        // Get images from the swap chain.
        if (const auto result = FetchwapchainImages(m_images); !result.IsSuccessful())
        {
            return result;
        }

        // Load sync objects.
        if (const auto result = CreateSemaphores(m_imageAvailableSemaphores, logicalDeviceHandle, m_images.size()); result != VK_SUCCESS)
        {
            return result;
        }

        if (const auto result = CreateFences(m_frameFences, logicalDeviceHandle, VK_FENCE_CREATE_SIGNALED_BIT, m_images.size()); result != VK_SUCCESS)
        {
            return result;
        }

        destroyer.Release();

        return {};
    }

    void SwapChain::UnloadAssociatedObjects()
    {
        const auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        DestroySemaphores(logicalDeviceHandle, m_imageAvailableSemaphores);
        m_imageAvailableSemaphores.clear();
        DestroyFences(logicalDeviceHandle, m_frameFences);
        m_frameFences.clear();
    }

    Result<> SwapChain::FetchwapchainImages(Images& images)
    {
        const auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        uint32_t imageCount = 0;
        if (const auto result = vkGetSwapchainImagesKHR(logicalDeviceHandle, m_handle, &imageCount, nullptr); result != VkResult::VK_SUCCESS)
        {
            return result;
        }

        images.resize(imageCount);
        if (const auto result = vkGetSwapchainImagesKHR(logicalDeviceHandle, m_handle, &imageCount, images.data()); result != VkResult::VK_SUCCESS)
        {
            return result;
        }

        return VkResult::VK_SUCCESS;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif