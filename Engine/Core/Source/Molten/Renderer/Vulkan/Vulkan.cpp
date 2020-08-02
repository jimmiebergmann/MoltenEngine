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

#include "Molten/Renderer/Vulkan/Vulkan.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include <algorithm>

namespace Molten
{

    namespace Vulkan
    {

        VkSwapchainKHR CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
            VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode,
            const VkSurfaceCapabilitiesKHR& capabilities, uint32_t imageCount,
            uint32_t graphicsQueueIndex, uint32_t presentQueueIndex,
            VkSwapchainKHR oldSwapchain)
        {
            VkSwapchainCreateInfoKHR swapchainInfo = {};
            swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainInfo.surface = surface;
            swapchainInfo.presentMode = presentMode;
            swapchainInfo.clipped = VK_TRUE;
            swapchainInfo.minImageCount = imageCount;
            swapchainInfo.imageFormat = surfaceFormat.format;
            swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
            swapchainInfo.imageExtent = capabilities.currentExtent;
            swapchainInfo.imageArrayLayers = 1;
            swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            swapchainInfo.preTransform = capabilities.currentTransform;
            swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchainInfo.oldSwapchain = oldSwapchain;

            const uint32_t queueFamilies[2] = { graphicsQueueIndex, presentQueueIndex };
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

            VkSwapchainKHR swapchain;
            if (vkCreateSwapchainKHR(logicalDevice, &swapchainInfo, nullptr, &swapchain) != VK_SUCCESS)
            {
                return VK_NULL_HANDLE;
            }

            if (oldSwapchain)
            {
                vkDestroySwapchainKHR(logicalDevice, oldSwapchain, nullptr);
            }

            return swapchain;
        }

        void DestroySemaphores(VkDevice logicalDevice, std::vector<VkSemaphore>& semaphores)
        {
            for (auto semaphore : semaphores)
            {
                vkDestroySemaphore(logicalDevice, semaphore, nullptr);
            }
        }

        void DestroyFences(VkDevice logicalDevice, std::vector<VkFence>& fences)
        {
            for (auto fence : fences)
            {
                vkDestroyFence(logicalDevice, fence, nullptr);
            }
        }

        void DestroyImageViews(VkDevice logicalDevice, std::vector<VkImageView>& imageViews)
        {
            for (auto& imageView : imageViews)
            {
                vkDestroyImageView(logicalDevice, imageView, nullptr);
            }
        }

        void GetSwapchainImages(VkDevice logicalDevice, VkSwapchainKHR swapchain, std::vector<VkImage>& images)
        {
            uint32_t imageCount;
            vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr);
            images.resize(imageCount);
            vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, images.data());
        }

    }

}

#endif