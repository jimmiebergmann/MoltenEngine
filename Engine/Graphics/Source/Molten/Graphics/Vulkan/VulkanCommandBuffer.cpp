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

#include "Molten/Renderer/Vulkan/VulkanCommandBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanDescriptorSet.hpp"
#include "Molten/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Molten/Renderer/Vulkan/VulkanIndexBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanVertexBuffer.hpp"

namespace Molten
{

    // Global implementations.
    static VkIndexType GetIndexBufferDataType(const IndexBuffer::DataType dataType)
    {
        switch (dataType)
        {
            case IndexBuffer::DataType::Uint16: return VkIndexType::VK_INDEX_TYPE_UINT16;
            case IndexBuffer::DataType::Uint32: return VkIndexType::VK_INDEX_TYPE_UINT32;
        }

        throw Exception("Provided data type is not supported as index buffer data type by the Vulkan renderer.");
    }


    // Vulkan command buffer implementations.
    VulkanCommandBuffer::VulkanCommandBuffer() :
        m_currentFrameIndex(0),
        m_currentCommandBuffer(nullptr),
        m_currentPipeline(nullptr),
        m_commandCount(0)
    {}

    size_t VulkanCommandBuffer::GetCommandCount() const
    {
        return m_commandCount;
    }

    void VulkanCommandBuffer::BindDescriptorSet(DescriptorSet& descriptorSet)
    {
        auto& vulkanDescriptorSet = static_cast<VulkanDescriptorSet&>(descriptorSet);

        vkCmdBindDescriptorSets(
            m_currentCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_currentPipeline->pipelineLayout,
            vulkanDescriptorSet.index,
            1,
            &vulkanDescriptorSet.descriptorSet,
            0,
            nullptr);

        ++m_commandCount;
    }

    void VulkanCommandBuffer::BindFramedDescriptorSet(FramedDescriptorSet& framedDescriptorSet)
    {
        auto& vulkanFramedDescriptorSet = static_cast<VulkanFramedDescriptorSet&>(framedDescriptorSet);

        vkCmdBindDescriptorSets(
            m_currentCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_currentPipeline->pipelineLayout,
            vulkanFramedDescriptorSet.index,
            1,
            &vulkanFramedDescriptorSet.descriptorSets[m_currentFrameIndex],
            0,
            nullptr);

        ++m_commandCount;
    }

    void VulkanCommandBuffer::BindPipeline(Pipeline& pipeline)
    {
        auto& vulkanPipeline = static_cast<VulkanPipeline&>(pipeline);
        vkCmdBindPipeline(m_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.graphicsPipeline);
        m_currentPipeline = &vulkanPipeline;

        ++m_commandCount;
    }
 
    void VulkanCommandBuffer::DrawVertexBuffer(VertexBuffer& vertexBuffer)
    {
        auto& vulkanVertexBuffer = static_cast<VulkanVertexBuffer&>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer.deviceBuffer.buffer };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(m_currentCommandBuffer, vulkanVertexBuffer.vertexCount, 1, 0, 0);

        m_commandCount += 2;
    }

    void VulkanCommandBuffer::DrawVertexBuffer(IndexBuffer& indexBuffer, VertexBuffer& vertexBuffer)
    {
        auto& vulkanIndexBuffer = static_cast<VulkanIndexBuffer&>(indexBuffer);
        auto& vulkanVertexBuffer = static_cast<VulkanVertexBuffer&>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer.deviceBuffer.buffer };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_currentCommandBuffer, vulkanIndexBuffer.deviceBuffer.buffer, 0, GetIndexBufferDataType(vulkanIndexBuffer.dataType));
        vkCmdDrawIndexed(m_currentCommandBuffer, static_cast<uint32_t>(vulkanIndexBuffer.indexCount), 1, 0, 0, 0);

        m_commandCount += 3;
    }

    void VulkanCommandBuffer::PushConstant(const uint32_t location, const bool value)
    {
        vkCmdPushConstants(
            m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);

        ++m_commandCount;
    }
    void VulkanCommandBuffer::PushConstant(const uint32_t location, const int32_t value)
    {
        vkCmdPushConstants(
            m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);

        ++m_commandCount;
    }
    void VulkanCommandBuffer::PushConstant(const uint32_t location, const float value)
    {
        vkCmdPushConstants(
            m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);

        ++m_commandCount;
    }
    void VulkanCommandBuffer::PushConstant(const uint32_t location, const Vector2f32& value)
    {
        vkCmdPushConstants(
            m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);

        ++m_commandCount;
    }
    void VulkanCommandBuffer::PushConstant(const uint32_t location, const Vector3f32& value)
    {
        vkCmdPushConstants(
            m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);

        ++m_commandCount;
    }
    void VulkanCommandBuffer::PushConstant(const uint32_t location, const Vector4f32& value)
    {
        vkCmdPushConstants(
            m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);

        ++m_commandCount;
    }
    void VulkanCommandBuffer::PushConstant(const uint32_t location, const Matrix4x4f32& value)
    {
        vkCmdPushConstants(
            m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);

        ++m_commandCount;
    }

    void VulkanCommandBuffer::UpdateUniformBuffer(UniformBuffer& uniformBuffer, const void* data, const size_t size, const size_t offset)
    {
        /*auto* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer.get());
        auto& deviceBuffer = vulkanUniformBuffer->deviceBuffer;

        Vulkan::Result<> result;
        if (!(result = Vulkan::MapMemory(m_logicalDevice, deviceBuffer, data, size, offset)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map memory of framed uniform buffer");
        }*/
    }

    void VulkanCommandBuffer::UpdateFramedUniformBuffer(FramedUniformBuffer& framedUniformBuffer, const void* data, const size_t size, const size_t offset)
    {
        /*auto* vulkanFramedUniformBuffer = static_cast<VulkanFramedUniformBuffer*>(framedUniformBuffer.get());
        auto& deviceBuffer = vulkanFramedUniformBuffer->deviceBuffers[m_swapChain.GetCurrentImageIndex()];

        Vulkan::Result<> result;
        if (!(result = Vulkan::MapMemory(m_logicalDevice, deviceBuffer, data, size, offset)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map memory of uniform buffer");
        }*/
    }

    void VulkanCommandBuffer::PrepareRecording(
        const size_t frameIndex,
        VkCommandBuffer commandBuffer)
    {
        m_currentFrameIndex = frameIndex;
        m_currentCommandBuffer = commandBuffer;
        m_currentPipeline = nullptr;
        m_commandCount = 0;
    }

    VkCommandBuffer VulkanCommandBuffer::GetCurrentCommandBuffer() const
    {
        return m_currentCommandBuffer;
    }

}

#endif