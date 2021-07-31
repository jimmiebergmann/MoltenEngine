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

#ifndef MOLTEN_CORE_RENDERER_VULKANRENDERPASS_HPP
#define MOLTEN_CORE_RENDERER_VULKANRENDERPASS_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/RenderPass.hpp"
#include "Molten/Renderer/Vulkan/VulkanCommandBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"

namespace Molten
{
    /** Forward declarations. */
    namespace Vulkan
    {
        class ResourceDestroyer;
    }
    class VulkanRenderer;
    class Logger;


    /** Vulkan Render pass. */
    class MOLTEN_API VulkanRenderPass : public RenderPass
    {

    public:

        using Base = RenderPass;

        VulkanRenderPass() = delete;
        VulkanRenderPass(
            Logger* logger,
            const VkCommandPool commandPool,
            Vulkan::CommandBuffers&& commandBuffers,
            Vulkan::Semaphores&& finishSemaphores,
            VkRenderPass renderPass,
            RenderPassFunction recordFunction);

        /** Set current command buffer record function. */
        void SetRecordFunction(RenderPassFunction recordFunction) override;

        /** Set current viewport bounds.
         *  Providing an optional without an value causes the render system to use viewport value from renderer. */
        void SetViewport(std::optional<Bounds2i32> bounds) override;

        /** Set current scissor bounds.
         *  Providing an optional without an value causes the render system to use scissor value from renderer. */
        void SetScissor(std::optional<Bounds2i32> bounds) override;

    protected:

        friend class VulkanRenderer;
        friend class Vulkan::ResourceDestroyer;

        [[nodiscard]] Vulkan::Result<> Record(
            const size_t commandBufferIndex,
            const Bounds2i32& defaultViewportBounds,
            const Bounds2i32& defaultScissorBounds);

        void Submit();

    private:

        Logger* m_logger;
        VkRenderPass m_renderPass;
        RenderPassFunction m_recordFunction;
        VkCommandPool m_commandPool;
        Vulkan::CommandBuffers m_commandBuffers;
        Vulkan::Semaphores m_finishSemaphores;
        VulkanCommandBuffer m_commandBuffer;
        std::optional<Bounds2i32> m_viewportBounds;
        std::optional<Bounds2i32> m_scissorBounds;

    };

}

#endif

#endif
