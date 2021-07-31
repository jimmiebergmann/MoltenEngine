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

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class LogicalDevice;

    class MOLTEN_API SwapChain
    {

    public:

        SwapChain();
        ~SwapChain();

        Result<> Create(
            LogicalDevice& logicalDevice,
            const VkRenderPass renderPass,
            const VkSurfaceFormatKHR& surfaceFormat,
            const VkPresentModeKHR presentMode,
            const uint32_t imageCount);

        Result<> Recreate();

        void Destroy();

        bool IsCreated();

        Result<> BeginDraw();
        Result<> EndDraw(VkCommandBuffer& commandBuffer);

        VkSwapchainKHR GetHandle() const;

        VkExtent2D GetExtent() const;

        VkPresentModeKHR GetPresentMode() const;

        VkSurfaceFormatKHR GetSurfaceFormat() const;

        uint32_t GetImageCount() const;
        uint32_t GetMaxFramesInFlight() const;

        uint32_t GetCurrentImageIndex() const;
        uint32_t GetCurrentFrameIndex()  const;
        VkFramebuffer GetCurrentFramebuffer()  const;

        LogicalDevice& GetLogicalDevice();
        const LogicalDevice& GetLogicalDevice() const;

        bool HasLogicalDevice() const;

        void SetExtent(VkExtent2D extent);

    private:

        Result<> Load();

        Result<> LoadAssociatedObjects();
        void UnloadAssociatedObjects();

        Result<> GetSwapchainImages(Images& images);

        VkSwapchainKHR m_handle;
        LogicalDevice* m_logicalDevice;
        VkRenderPass m_renderPass;
        VkExtent2D m_extent;
        VkPresentModeKHR m_presentMode;
        VkSurfaceFormatKHR m_surfaceFormat;
        uint32_t m_imageCount; // WHY?
        Images m_images; /// Group together with image view?
        ImageViews m_imageViews;
        FrameBuffers m_framebuffers;
        uint32_t m_maxFramesInFlight;
        Semaphores m_imageAvailableSemaphores;
        Semaphores m_renderFinishedSemaphores;
        Fences m_inFlightFences;
        Fences m_imagesInFlight;
        uint32_t m_currentFrameIndex;
        uint32_t m_currentImageIndex;
        bool m_resize;
        
    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif