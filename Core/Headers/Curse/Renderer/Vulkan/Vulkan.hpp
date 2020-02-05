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

#ifndef CURSE_CORE_RENDERER_VULKAN_VULKANHEADERS_HPP
#define CURSE_CORE_RENDERER_VULKAN_VULKANHEADERS_HPP

#include "Curse/Core.hpp"

#if defined(CURSE_ENABLE_VULKAN)

#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#include "vulkan/vulkan.h"

#if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
#include "Curse/Platform/Win32Headers.hpp"
#include "vulkan/vulkan_win32.h"
#elif CURSE_PLATFORM == CURSE_PLATFORM_LINUX
#include "Curse/Platform/X11Headers.hpp"
#include "vulkan/vulkan_xlib.h"
#endif

#include <vector>

namespace Curse
{

    namespace Vulkan
    {

       VkSwapchainKHR CURSE_API CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice,
                                               VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode,
                                               const VkSurfaceCapabilitiesKHR & capabilities, uint32_t imageCount,
                                               uint32_t graphicsQueueIndex, uint32_t presentQueueIndex,
                                               VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);


         void CURSE_API DestroySemaphores(VkDevice logicalDevice, std::vector<VkSemaphore>& semaphores);

         void CURSE_API DestroyFences(VkDevice logicalDevice, std::vector<VkFence>& fences);

         void CURSE_API DestroyImageViews(VkDevice logicalDevice, std::vector<VkImageView>& imageViews);

         void CURSE_API GetSwapchainImages(VkDevice logicalDevice, VkSwapchainKHR swapchain, std::vector<VkImage>& images);

    }

}

#endif

#endif
