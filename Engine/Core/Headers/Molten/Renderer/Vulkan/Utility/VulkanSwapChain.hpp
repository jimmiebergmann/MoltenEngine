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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANSWAPCHAIN_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANSWAPCHAIN_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include <vector>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class LogicalDevice;

    class MOLTEN_API SwapChain
    {

    public:

        SwapChain();
        ~SwapChain();

        /*SwapChain(const SwapChain&) = delete;
        SwapChain(SwapChain&&) = delete;
        SwapChain& operator = (const SwapChain&) = delete;
        SwapChain& operator = (SwapChain&&) = delete;*/

        Result<> Create(
            LogicalDevice& logicalDevice,
            const VkSurfaceFormatKHR& surfaceFormat,
            const VkPresentModeKHR presentMode,
            const uint32_t imageCount);

        Result<> Recreate();

        void Destroy();

        [[nodiscard]] bool IsCreated();

        /** This function is blocking until next image is available.
         *  @return Status VK_SUCCESSFUL if next image successfully was acquired.
         *          Recreate must be called before calling AcquireNextImage again, if VK_ERROR_OUT_OF_DATE_KHR is returned.
         *          It's also recommended call Recreate() if VK_SUBOPTIMAL_KHR is returned.
         */
        [[nodiscard]] Result<> AcquireNextImage();

        /** This function is blocking until waitSemaphore is signaled. */
        [[nodiscard]] Result<> PresentImage(VkSemaphore waitSemaphore);

        [[nodiscard]] VkSwapchainKHR GetHandle() const;

        [[nodiscard]] VkExtent2D GetExtent() const;

        [[nodiscard]] VkPresentModeKHR GetPresentMode() const;

        [[nodiscard]] VkSurfaceFormatKHR GetSurfaceFormat() const;

        [[nodiscard]] const Images& GetImages() const;

        [[nodiscard]] uint32_t GetCurrentImageIndex() const;

        [[nodiscard]] VkFence GetCurrentFrameFence() const;

        [[nodiscard]] VkSemaphore GetCurrentImageAvailableSemaphore() const;  

        [[nodiscard]] LogicalDevice& GetLogicalDevice();
        [[nodiscard]] const LogicalDevice& GetLogicalDevice() const;

        [[nodiscard]] bool HasLogicalDevice() const;

    private:

        Result<> Load(const uint32_t imageCount);

        Result<> LoadAssociatedObjects();
        void UnloadAssociatedObjects();

        Result<> FetchwapchainImages(Images& images);

        VkSwapchainKHR m_handle;
        LogicalDevice* m_logicalDevice;
        VkExtent2D m_extent;
        VkPresentModeKHR m_presentMode;
        VkSurfaceFormatKHR m_surfaceFormat;
        Images m_images;
        Semaphores m_imageAvailableSemaphores;
        Fences m_frameFences;    
        uint32_t m_currentImageIndex;
        uint32_t m_currentFrameIndex;

    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif