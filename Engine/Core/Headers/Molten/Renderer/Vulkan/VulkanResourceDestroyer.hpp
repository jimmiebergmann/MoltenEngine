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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_VULKANRESOURCEDESTROYER_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_VULKANRESOURCEDESTROYER_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanShaderModule.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanImage.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanImageSampler.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanBuffer.hpp"
#include <variant>
#include <queue>

namespace Molten
{
    // TODO: All those resources will be moved to Vulkan namespace later...
    class VulkanDescriptorSet;
    class VulkanFramedDescriptorSet;
    class VulkanFramebuffer;
    class VulkanIndexBuffer;
    class VulkanPipeline;
    template<size_t VDimensions> class VulkanSampler;
    template<size_t VDimensions> class VulkanTexture;
    class VulkanUniformBuffer;
    class VulkanFramedUniformBuffer;
    class VulkanVertexBuffer;
}

namespace Molten::Vulkan
{

    /**
    * @brief Vulkan renderer class.
    */
    class MOLTEN_API ResourceDestroyer
    {

    public:

        explicit ResourceDestroyer(LogicalDevice& logicalDevice);
        ~ResourceDestroyer();

        ResourceDestroyer(const ResourceDestroyer&) = delete;
        ResourceDestroyer(ResourceDestroyer&&) = delete;
        ResourceDestroyer& operator = (const ResourceDestroyer&) = delete;
        ResourceDestroyer& operator = (ResourceDestroyer&&) = delete;

        void Process(const uint32_t currentFrameIndex);
        void ProcessAll();

        void Add(const uint32_t cleanupFrameIndex, VulkanDescriptorSet& descriptorSet);
        void Add(const uint32_t cleanupFrameIndex, VulkanFramedDescriptorSet& descriptorSet);
        void Add(const uint32_t cleanupFrameIndex, VulkanFramebuffer& framebuffer);
        void Add(const uint32_t cleanupFrameIndex, VulkanIndexBuffer& indexBuffer);
        void Add(const uint32_t cleanupFrameIndex, VulkanPipeline& pipeline);
        void Add(const uint32_t cleanupFrameIndex, VulkanSampler<1>& sampler1D);
        void Add(const uint32_t cleanupFrameIndex, VulkanSampler<2>& sampler2D);
        void Add(const uint32_t cleanupFrameIndex, VulkanSampler<3>& sampler3D);
        void Add(const uint32_t cleanupFrameIndex, VulkanTexture<1>& texture1D);
        void Add(const uint32_t cleanupFrameIndex, VulkanTexture<2>& texture2D);
        void Add(const uint32_t cleanupFrameIndex, VulkanTexture<3>& texture3D);
        void Add(const uint32_t cleanupFrameIndex, VulkanUniformBuffer& uniformBuffer);
        void Add(const uint32_t cleanupFrameIndex, VulkanFramedUniformBuffer& framedUniformBuffer);
        void Add(const uint32_t cleanupFrameIndex, VulkanVertexBuffer& vertexBuffer);

    private:

        struct DescriptorSetCleanup
        {
            VkDescriptorSet descriptorSet;
            VkDescriptorPool descriptorPool;
        };

        struct FramedDescriptorSetCleanup
        {
            std::vector<VkDescriptorSet> descriptorSets;
            VkDescriptorPool descriptorPool;
        };

        struct FramebufferCleanup
        {
            VkFramebuffer framebuffer;
        };

        struct IndexBufferCleanup
        {
            DeviceBuffer buffer;
        };

        struct PipelineCleanup
        {
            VkPipeline graphicsPipeline;
            VkPipelineLayout pipelineLayout;
            DescriptorSetLayouts descriptionSetLayouts;
            ShaderModules shaderModules;
        };

        struct SamplerCleanup
        {
            ImageSampler imageSampler;
        };

        struct TextureCleanup
        {
            Image image;
            VkImageView imageView;
        };

        struct UniformBufferCleanup
        {
            DeviceBuffer deviceBuffer;
        };

        struct FramedUniformBufferCleanup
        {
            std::vector<DeviceBuffer> deviceBuffers;
        };

        struct VertexBufferCleanup
        {
            DeviceBuffer buffer;
        };

        using CleanupVariant = std::variant<
            DescriptorSetCleanup,
            FramedDescriptorSetCleanup,
            FramebufferCleanup,
            IndexBufferCleanup,
            PipelineCleanup,
            SamplerCleanup,
            TextureCleanup,
            UniformBufferCleanup,
            FramedUniformBufferCleanup,
            VertexBufferCleanup
        >;

        struct CleanupData
        {
            uint32_t cleanupFrameIndex;
            CleanupVariant data;
        };

        using CleanupQueue = std::queue<CleanupData>;
        
        void Process(CleanupVariant& cleanupVariant);

        void Process(DescriptorSetCleanup& data);
        void Process(FramedDescriptorSetCleanup& data);
        void Process(FramebufferCleanup& data);
        void Process(IndexBufferCleanup& data);
        void Process(PipelineCleanup& data);      
        void Process(SamplerCleanup& data);
        void Process(TextureCleanup& data);
        void Process(UniformBufferCleanup& data);
        void Process(FramedUniformBufferCleanup& data);
        void Process(VertexBufferCleanup& data);

        LogicalDevice& m_logicalDevice;
        CleanupQueue m_cleanupQueue;

    };

}

#endif

#endif