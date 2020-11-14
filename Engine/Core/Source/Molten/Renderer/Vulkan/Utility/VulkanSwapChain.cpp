/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#include "Molten/Renderer/Vulkan/Utility/VulkanSwapChain.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
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
        m_renderPass{VK_NULL_HANDLE},
        m_extent{ 0, 0 },
        m_presentMode(VkPresentModeKHR::VK_PRESENT_MODE_MAX_ENUM_KHR),
        m_surfaceFormat{},
        m_imageCount(0),
        m_images{},
        m_imageViews{},
        m_framebuffers{},
        m_maxFramesInFlight(0),
        m_imageAvailableSemaphores{},
        m_renderFinishedSemaphores{},
        m_inFlightFences{},
        m_imagesInFlight{},
        m_currentFrameIndex(0),
        m_currentImageIndex(0),
        m_resize(false)
    {}

    SwapChain::~SwapChain()
    {
        Destroy();
    }

    Result<> SwapChain::Create(
        LogicalDevice& logicalDevice,
        const VkRenderPass renderPass,
        const VkSurfaceFormatKHR& surfaceFormat,
        const VkPresentModeKHR presentMode,
        const uint32_t imageCount)
    {
        Destroy();

        auto& physicalDevice = logicalDevice.GetPhysicalDevice();
        auto& surfaceCapabilities = physicalDevice.GetCapabilities().surfaceCapabilities.capabilities;

        m_logicalDevice = &logicalDevice;
        m_renderPass = renderPass;
        m_extent = surfaceCapabilities.currentExtent;
        m_presentMode = presentMode;
        m_surfaceFormat = surfaceFormat;
        m_imageCount = imageCount;

        return Load();
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
        m_logicalDevice->WaitIdle();
        UnloadAssociatedObjects();
        return Load();
    }

    bool SwapChain::IsCreated()
    {
        return m_handle != VK_NULL_HANDLE;
    }

    Result<> SwapChain::BeginDraw()
    {
        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        vkWaitForFences(logicalDeviceHandle, 1, &m_inFlightFences[m_currentFrameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());

        Result<> result = vkAcquireNextImageKHR(
            logicalDeviceHandle,
            m_handle,
            std::numeric_limits<uint64_t>::max(),
            m_imageAvailableSemaphores[m_currentFrameIndex],
            VK_NULL_HANDLE,
            &m_currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_resize)
        {
            if (!(result = Recreate()))
            {
                return result;
            }

            if (!(result = BeginDraw()))
            {
                return result;
            }

            return VkResult::VK_SUCCESS;
        }
        else if (result != VK_SUCCESS)
        {
            return result;
        }

        if (m_imagesInFlight[m_currentImageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(logicalDeviceHandle, 1, &m_imagesInFlight[m_currentImageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
        }
        m_imagesInFlight[m_currentImageIndex] = m_inFlightFences[m_currentFrameIndex];

        if (m_currentImageIndex >= static_cast<uint32_t>(m_images.size()))
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        return VkResult::VK_SUCCESS;
    }

    Result<> SwapChain::EndDraw(VkCommandBuffer& commandBuffer)
    {
        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore imageSemaphores[] = { m_imageAvailableSemaphores[m_currentFrameIndex] };
        VkPipelineStageFlags pipelineWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = imageSemaphores;
        submitInfo.pWaitDstStageMask = pipelineWaitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkSemaphore renderSemaphores[] = { m_renderFinishedSemaphores[m_currentFrameIndex] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = renderSemaphores;

        Result<> result;

        if (!(result = vkResetFences(logicalDeviceHandle, 1, &m_inFlightFences[m_currentFrameIndex])))
        {
            return result;
        }

        auto& deviceQueues = m_logicalDevice->GetDeviceQueues();
        if (!(result = vkQueueSubmit(deviceQueues.graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrameIndex])))
        {
            return result;
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = renderSemaphores;

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_handle;
        presentInfo.pImageIndices = &m_currentImageIndex;

        vkQueuePresentKHR(deviceQueues.presentQueue, &presentInfo);

        m_currentFrameIndex = (m_currentFrameIndex + 1) % m_maxFramesInFlight;

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

    uint32_t SwapChain::GetImageCount() const
    {
        return m_imageCount;
    }

    uint32_t SwapChain::GetCurrentImageIndex() const
    {
        return m_currentFrameIndex;
    }
    uint32_t SwapChain::GetCurrentFrameIndex() const
    {
        return m_currentFrameIndex;
    }

    VkFramebuffer SwapChain::GetCurrentFramebuffer() const
    {
        return m_framebuffers[m_currentImageIndex];
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

    void SwapChain::SetExtent(VkExtent2D extent)
    {
        if (extent.width != m_extent.width || extent.height != m_extent.height)
        {
            m_resize = true;
        }
        m_extent = extent;
    }

    Result<> SwapChain::Load()
    {
        Result<> result;
        VkResult vkResult = VkResult::VK_SUCCESS;

        auto oldHandle = m_handle;

        SmartFunction destroyer = [&]()
        {
            Destroy();
        };

        auto& physicalDevice = m_logicalDevice->GetPhysicalDevice();
        auto& surface = physicalDevice.GetSurface();
        auto& surfaceCapabilities = physicalDevice.GetCapabilities().surfaceCapabilities.capabilities;

        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = surface.GetHandle();
        swapchainInfo.presentMode = m_presentMode;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.minImageCount = m_imageCount;
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

        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        if ((vkResult = vkCreateSwapchainKHR(logicalDeviceHandle, &swapchainInfo, nullptr, &m_handle)) != VkResult::VK_SUCCESS)
        {
            return vkResult;
        }
        if (m_handle == VK_NULL_HANDLE)
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        if (oldHandle)
        {
            vkDestroySwapchainKHR(logicalDeviceHandle, oldHandle, nullptr);
        }

        if (!(result = LoadAssociatedObjects()))
        {
            return result;
        }

        // Finalize.
        m_resize = false;
        destroyer.Release();
        return result;
    }


    Result<> SwapChain::LoadAssociatedObjects()
    {
        Result<> result;

        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        // Get images from the swap chain.
        if (!(result = GetSwapchainImages(m_images)))
        {
            return result;
        }
        if (m_images.size() != m_imageCount)
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        // Create image views.
        m_imageViews.resize(m_images.size(), VK_NULL_HANDLE);
        for (size_t i = 0; i < m_imageViews.size(); i++)
        {
            VkImageViewCreateInfo imageViewInfo = {};
            imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewInfo.image = m_images[i];
            imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewInfo.format = m_surfaceFormat.format;
            imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewInfo.subresourceRange.baseMipLevel = 0;
            imageViewInfo.subresourceRange.levelCount = 1;
            imageViewInfo.subresourceRange.baseArrayLayer = 0;
            imageViewInfo.subresourceRange.layerCount = 1;

            if (!(result = vkCreateImageView(logicalDeviceHandle, &imageViewInfo, nullptr, &m_imageViews[i])))
            {
                return result;
            }
        }

        // Load framebuffers.
        for (auto& imageView : m_imageViews)
        {
            auto framebuffer = CreateFramebuffer(logicalDeviceHandle, m_renderPass, imageView, { m_extent.width, m_extent.height });
            m_framebuffers.push_back(framebuffer);
        }
        if (!m_framebuffers.size())
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        m_maxFramesInFlight = static_cast<uint32_t>(m_framebuffers.size()) - 1;      

        // Load sync objects.
        m_imagesInFlight.resize(m_images.size(), VK_NULL_HANDLE);

        if (!(result = Vulkan::CreateSemaphores(m_imageAvailableSemaphores, logicalDeviceHandle, m_maxFramesInFlight)) ||
            !(result = Vulkan::CreateSemaphores(m_renderFinishedSemaphores, logicalDeviceHandle, m_maxFramesInFlight)) ||
            !(result = Vulkan::CreateFences(m_inFlightFences, logicalDeviceHandle, VK_FENCE_CREATE_SIGNALED_BIT, m_maxFramesInFlight)))
        {
            return result;
        }

        return result;
    }

    void SwapChain::UnloadAssociatedObjects()
    {
        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        Vulkan::DestroySemaphores(logicalDeviceHandle, m_imageAvailableSemaphores);
        m_imageAvailableSemaphores.clear();
        Vulkan::DestroySemaphores(logicalDeviceHandle, m_renderFinishedSemaphores);
        m_renderFinishedSemaphores.clear();
        Vulkan::DestroyFences(logicalDeviceHandle, m_inFlightFences);
        m_inFlightFences.clear();

        m_imagesInFlight.clear();

        for (auto& framebuffer : m_framebuffers)
        {
            vkDestroyFramebuffer(logicalDeviceHandle, framebuffer, nullptr);
        }
        m_framebuffers.clear();

        Vulkan::DestroyImageViews(logicalDeviceHandle, m_imageViews);
    }

    Result<> SwapChain::GetSwapchainImages(Images& images)
    {
        Result<> result;

        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        uint32_t imageCount = 0;
        if ((result = vkGetSwapchainImagesKHR(logicalDeviceHandle, m_handle, &imageCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        images.resize(imageCount);
        if ((result = vkGetSwapchainImagesKHR(logicalDeviceHandle, m_handle, &imageCount, images.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        return result;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif