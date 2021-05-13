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

#include "Molten/Renderer/Vulkan/Utility/VulkanResourceDestroyer.hpp"
#include "Molten/Renderer/Vulkan/VulkanDescriptorSet.hpp"
#include "Molten/Renderer/Vulkan/VulkanFramebuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanIndexBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Molten/Renderer/Vulkan/VulkanSampler.hpp"
#include "Molten/Renderer/Vulkan/VulkanTexture.hpp"
#include "Molten/Renderer/Vulkan/VulkanUniformBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanVertexBuffer.hpp"
#include <limits>

namespace Molten::Vulkan
{

    // Global implementations.
    static const uint32_t g_lowerQuarterOfMaxUnit32 = (std::numeric_limits<uint32_t>::max() / 4);
    static const uint32_t g_upperQuarterOfMaxUnit32 = (std::numeric_limits<uint32_t>::max() / 4) * 3;

    static bool ShouldProcessCleanup(const uint32_t currentFrameIndex, const uint32_t cleanupFrameIndex)
    {
        if (currentFrameIndex <= g_lowerQuarterOfMaxUnit32)
        {
            return (currentFrameIndex + g_lowerQuarterOfMaxUnit32) >= (cleanupFrameIndex + g_lowerQuarterOfMaxUnit32);
        }
        else if (currentFrameIndex >= g_upperQuarterOfMaxUnit32)
        {
            return (currentFrameIndex - g_lowerQuarterOfMaxUnit32) >= (cleanupFrameIndex - g_lowerQuarterOfMaxUnit32);
        }

        return currentFrameIndex >= cleanupFrameIndex;
    }


    // Vulkan resource destroyer implementations.
    ResourceDestroyer::ResourceDestroyer(
        LogicalDevice& logicalDevice,
        MemoryAllocator& memoryAllocator
    ) :
        m_logicalDevice(logicalDevice),
        m_memoryAllocator(memoryAllocator)
    {}

    ResourceDestroyer::~ResourceDestroyer()
    {
        ProcessAll();
    }

    void ResourceDestroyer::Process(const uint32_t currentFrameIndex)
    {
        if( m_cleanupQueue.empty() || 
            !ShouldProcessCleanup(currentFrameIndex, m_cleanupQueue.front().cleanupFrameIndex))
        {
            return;
        }

        uint32_t prevCleanupFrameIndex = m_cleanupQueue.front().cleanupFrameIndex;

        while(!m_cleanupQueue.empty())
        {
            auto& cleanupData = m_cleanupQueue.front();

            if(prevCleanupFrameIndex != cleanupData.cleanupFrameIndex)
            {
                if(!ShouldProcessCleanup(currentFrameIndex, cleanupData.cleanupFrameIndex))
                {
                    return;
                }
                prevCleanupFrameIndex = cleanupData.cleanupFrameIndex;
            }

            Process(cleanupData.data);
            m_cleanupQueue.pop();
        }
    }

    void ResourceDestroyer::ProcessAll()
    {
        if(!m_logicalDevice.IsCreated())
        {
            return;
        }

        while(!m_cleanupQueue.empty())
        {
            Process(m_cleanupQueue.front().data);
            m_cleanupQueue.pop();
        }
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanDescriptorSet& descriptorSet)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, DescriptorSetCleanup{
            descriptorSet.descriptorSet,
            descriptorSet.descriptorPool
        } });

        descriptorSet.descriptorSet = VK_NULL_HANDLE;
        descriptorSet.descriptorPool = VK_NULL_HANDLE;
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanFramedDescriptorSet& descriptorSet)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, FramedDescriptorSetCleanup{
            std::move(descriptorSet.descriptorSets),
            descriptorSet.descriptorPool
        } });

        descriptorSet.descriptorPool = VK_NULL_HANDLE;
    }

    void ResourceDestroyer::Add(const uint32_t /*cleanupFrameIndex*/, VulkanFramebuffer& /*ramebuffer*/)
    {
        // Not implemented yet, due to missing creation implementations.
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanIndexBuffer& indexBuffer)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, IndexBufferCleanup{
           std::move(indexBuffer.deviceBuffer)
        } });
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanPipeline& pipeline)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, PipelineCleanup{
            pipeline.graphicsPipeline,
            pipeline.pipelineLayout,
            std::move(pipeline.descriptionSetLayouts),
            std::move(pipeline.shaderModules)
        } });

        pipeline.graphicsPipeline = VK_NULL_HANDLE;
        pipeline.pipelineLayout = VK_NULL_HANDLE;
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanSampler<1>& sampler1D)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, SamplerCleanup{
           std::move(sampler1D.imageSampler)
        } });
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanSampler<2>& sampler2D)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, SamplerCleanup{
           std::move(sampler2D.imageSampler)
        } });
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanSampler<3>& sampler3D)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, SamplerCleanup{
           std::move(sampler3D.imageSampler)
        } });
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanTexture<1>& texture1D)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, TextureCleanup{
            std::move(texture1D.deviceImage),
            texture1D.imageView
        } });

        texture1D.imageView = VK_NULL_HANDLE;
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanTexture<2>& texture2D)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, TextureCleanup{
            std::move(texture2D.deviceImage),
            texture2D.imageView
        } });

        texture2D.imageView = VK_NULL_HANDLE;
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanTexture<3>& texture3D)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, TextureCleanup{
            std::move(texture3D.deviceImage),
            texture3D.imageView
        } });

        texture3D.imageView = VK_NULL_HANDLE;
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanUniformBuffer& uniformBuffer)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, UniformBufferCleanup{
            std::move(uniformBuffer.deviceBuffer)
        } });
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanFramedUniformBuffer& framedUniformBuffer)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, FramedUniformBufferCleanup{
            std::move(framedUniformBuffer.deviceBuffers)
        } });
    }

    void ResourceDestroyer::Add(const uint32_t cleanupFrameIndex, VulkanVertexBuffer& vertexBuffer)
    {
        m_cleanupQueue.emplace<CleanupData>({ cleanupFrameIndex, VertexBufferCleanup{
            std::move(vertexBuffer.deviceBuffer)
        } });

        vertexBuffer.vertexCount = 0;
        vertexBuffer.vertexSize = 0;
    }

    void ResourceDestroyer::Process(CleanupVariant& cleanupVariant)
    {
        std::visit([&](auto& data)
        {
            Process(data);
        }, cleanupVariant);
    }

    void ResourceDestroyer::Process(DescriptorSetCleanup& data)
    {
        vkDestroyDescriptorPool(m_logicalDevice.GetHandle(), data.descriptorPool, nullptr);
    }

    void ResourceDestroyer::Process(FramedDescriptorSetCleanup& data)
    {
        vkDestroyDescriptorPool(m_logicalDevice.GetHandle(), data.descriptorPool, nullptr);
    }

    void ResourceDestroyer::Process(FramebufferCleanup& /*data*/)
    {
        // Not implemented yet, due to missing creation implementations.
    }

    void ResourceDestroyer::Process(IndexBufferCleanup& data)
    {
        m_memoryAllocator.FreeDeviceBuffer(data.deviceBuffer);
    }

    void ResourceDestroyer::Process(PipelineCleanup& data)
    {
        if (data.graphicsPipeline)
        {
            vkDeviceWaitIdle(m_logicalDevice.GetHandle());
            vkDestroyPipeline(m_logicalDevice.GetHandle(), data.graphicsPipeline, nullptr);
        }
        if (data.pipelineLayout)
        {
            vkDestroyPipelineLayout(m_logicalDevice.GetHandle(), data.pipelineLayout, nullptr);
        }
        for (auto& setLayout : data.descriptionSetLayouts)
        {
            vkDestroyDescriptorSetLayout(m_logicalDevice.GetHandle(), setLayout, nullptr);
        }
        for (auto& shaderModule : data.shaderModules)
        {
            shaderModule.Destroy();
        }
    }

    void ResourceDestroyer::Process(SamplerCleanup& data)
    {
        data.imageSampler.Destroy();
    }

    void ResourceDestroyer::Process(TextureCleanup& data)
    {
        vkDestroyImageView(m_logicalDevice.GetHandle(), data.imageView, nullptr);
        m_memoryAllocator.FreeDeviceImage(data.deviceImage);
    }

    void ResourceDestroyer::Process(UniformBufferCleanup& data)
    {
        m_memoryAllocator.FreeDeviceBuffer(data.deviceBuffer);
    }

    void ResourceDestroyer::Process(FramedUniformBufferCleanup& data)
    {
        for(auto& deviceBuffer : data.deviceBuffers)
        {
            m_memoryAllocator.FreeDeviceBuffer(deviceBuffer);
        }
    }

    void ResourceDestroyer::Process(VertexBufferCleanup& data)
    {
        m_memoryAllocator.FreeDeviceBuffer(data.deviceBuffer);
    }

}

#endif