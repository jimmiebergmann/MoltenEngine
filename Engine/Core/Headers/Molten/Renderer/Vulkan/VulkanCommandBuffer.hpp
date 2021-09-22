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

#ifndef MOLTEN_CORE_RENDERER_VULKANCOMMANDBUFFER_HPP
#define MOLTEN_CORE_RENDERER_VULKANCOMMANDBUFFER_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/CommandBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"

namespace Molten
{

    /** Forward declarations.*/
    namespace Vulkan
    {
        class ResourceDestroyer;
    }
    class VulkanPipeline;
    class VulkanRenderer;
    class VulkanRenderPass;
     

    /** Vulkan command buffer. */
    class MOLTEN_API VulkanCommandBuffer final : public CommandBuffer
    {

    public:

        using Base = CommandBuffer;

        VulkanCommandBuffer();

        /** Get number of recorded commands of this buffer. */
        [[nodiscard]] size_t GetCommandCount() const;


        /** Bind descriptor set to draw queue. */
        void BindDescriptorSet(DescriptorSet& descriptorSet) override;

        /** Bind framed descriptor set to draw queue. */
        void BindFramedDescriptorSet(FramedDescriptorSet& framedDescriptorSet) override;

        /** Bind pipeline to draw queue. */
        void BindPipeline(Pipeline& pipeline) override;


        /** Draw vertex buffer, using the current bound pipeline. */
        void DrawVertexBuffer(VertexBuffer& vertexBuffer) override;

        /** Draw indexed vertex buffer, using the current bound pipeline. */
        void DrawVertexBuffer(IndexBuffer& indexBuffer, VertexBuffer& vertexBuffer) override;


        /** Push constant values to currently bound pipeline.
         *
         * @param location Id of push constant to update. This id can be shared between multiple shader stages.
         */
         /**@{*/
        void PushConstant(const uint32_t location, const bool value) override;
        void PushConstant(const uint32_t location, const int32_t value) override;
        void PushConstant(const uint32_t location, const float value) override;
        void PushConstant(const uint32_t location, const Vector2f32& value) override;
        void PushConstant(const uint32_t location, const Vector3f32& value) override;
        void PushConstant(const uint32_t location, const Vector4f32& value) override;
        void PushConstant(const uint32_t location, const Matrix4x4f32& value) override;
        /**@}*/


        /** Update uniform buffer data. */
        void UpdateUniformBuffer(UniformBuffer& uniformBuffer, const void* data, const size_t size, const size_t offset) override;

        /** Update uniform buffer data. */
        void UpdateFramedUniformBuffer(FramedUniformBuffer& framedUniformBuffer, const void* data, const size_t size, const size_t offset) override;

    private:

        friend class VulkanRenderPass;
        friend class VulkanRenderer;
        friend class Vulkan::ResourceDestroyer;

        void PrepareRecording(
            const size_t frameIndex,
            VkCommandBuffer commandBuffer);

        [[nodiscard]] VkCommandBuffer GetCurrentCommandBuffer() const;

        size_t m_currentFrameIndex;
        VkCommandBuffer m_currentCommandBuffer;
        VulkanPipeline* m_currentPipeline;
        size_t m_commandCount;

    };

}

#endif

#endif
