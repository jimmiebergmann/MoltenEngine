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

namespace Molten
{

    /*template<size_t VDimensions>
    RenderResource<Texture<VDimensions>> VulkanRenderer::CreateTexture(
        const Vector3ui32& dimensions,
        const size_t dataSize,
        const void* data,
        const VkFormat imageFormat,
        const VkFormat internalImageFormat,
        const VkComponentMapping& componentMapping)
    {
        Vulkan::GuardedDeviceBuffer stagingBuffer(m_memoryAllocator);

        Vulkan::Result<> result;
        if (!(result = m_memoryAllocator.CreateDeviceBuffer(
            stagingBuffer.deviceBuffer,
            dataSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for texture");
            return { };
        }

        if (!(result = Vulkan::MapMemory(m_logicalDevice, stagingBuffer.deviceBuffer, data, dataSize, 0)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for texture");
            return { };
        }

        VkCommandBuffer commandBuffer = nullptr;
        if (!(result = Vulkan::BeginSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to begin single time command.");
            return { };
        }

        Vulkan::Image image;

        // TODO: Use memory allocator for this!
        //if (!(result = image.Create(
        //    m_logicalDevice,
        //    commandBuffer,
        //    stagingBuffer.deviceBuffer,
        //    dimensions,
        //    VulkanImageTypeTraits<VDimensions>::type,
        //    imageFormat,
        //    VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        //    m_memoryTypesDeviceLocal)))
        //{
        //    Vulkan::Logger::WriteError(m_logger, result, "Failed to create image");
        //    return { };
        //}

        if (!(result = Vulkan::EndSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to end single time command.");
            return { };
        }

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image.handle;
        viewInfo.viewType = VulkanImageViewTypeTraits<VDimensions>::type;
        viewInfo.format = internalImageFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.components = componentMapping;

        VkImageView imageView;
        if (vkCreateImageView(m_logicalDevice.GetHandle(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create image view for texture");
            return { };
        }

        return RenderResource<Texture<VDimensions>>(new VulkanTexture<VDimensions>{
            std::move(image),
            imageView
        }, RenderResourceDeleter<Texture<VDimensions>>{ this });

        return {};
    }*/

    /*template<size_t VDimensions>
    bool VulkanRenderer::UpdateTexture(
        Vulkan::Image& image,
        const void* data,
        const size_t dataSize,
        const Vector3ui32& destinationDimensions,
        const Vector3ui32& destinationOffset)
    {
        
        Vulkan::GuardedDeviceBuffer stagingBuffer(m_memoryAllocator);

        Vulkan::Result<> result;
        if (!(result = m_memoryAllocator.CreateDeviceBuffer(
            stagingBuffer.deviceBuffer,
            dataSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for texture update");
            return { };
        }

        if (!(result = Vulkan::MapMemory(m_logicalDevice, stagingBuffer.deviceBuffer, data, dataSize, 0)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for texture update");
            return { };
        }

        VkCommandBuffer commandBuffer = nullptr;
        if (!(result = Vulkan::BeginSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to begin single time command.");
            return { };
        }

        if (!(result = image.Update(
            commandBuffer,
            stagingBuffer.deviceBuffer,
            destinationDimensions,
            destinationOffset)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create image");
            return { };
        }

        if (!(result = Vulkan::EndSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to end single time command.");
            return { };
        }
       

        return false;
    } */

    /*template<typename T>
    void VulkanRenderer::InternalPushConstant(const uint32_t location, const T& value)
    {
        vkCmdPushConstants(
            *m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);

    }*/

}