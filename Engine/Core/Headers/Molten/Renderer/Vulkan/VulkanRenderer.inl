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

    template<size_t VDimensions>
    RenderResource<Texture<VDimensions>> VulkanRenderer::CreateTexture(
        const Vector3ui32& dimensions,
        const size_t dataSize,
        const void* data,
        const VkFormat imageFormat,
        const VkFormat internalImageFormat,
        const VkComponentMapping& componentMapping)
    {
        Vulkan::DeviceBuffer stagingBuffer;

        Vulkan::Result<> result;
        if (!(result = stagingBuffer.Create(m_logicalDevice, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_memoryTypesHostVisibleOrCoherent)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for image buffer");
            return { };
        }

        if (!(result = stagingBuffer.MapMemory(0, dataSize, data)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for image buffer");
            return { };
        }

        Vulkan::Image image;
        if (!(result = image.Create(
            m_logicalDevice,
            stagingBuffer,
            m_commandPool,
            dimensions,
            VulkanImageTypeTraits<VDimensions>::type,
            imageFormat,
            m_memoryTypesDeviceLocal)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create image");
            return { };
        }

        if (!(result = image.TransitionToLayout(
            m_commandPool,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed transition image to read only optional layout");
            return { };
        }

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image.GetHandle();
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
    }

    template<size_t VDimensions>
    bool VulkanRenderer::UpdateTexture(
        Vulkan::Image& image,
        const void* data,
        Vector<VDimensions, uint32_t> destinationDimensions,
        Vector<VDimensions, uint32_t> destinationOffset)
    {
        auto getImageSize = [&]() -> std::pair<Vector3ui32, size_t>
        {
            if constexpr (VDimensions == 3)
            {
                return { destinationDimensions, destinationDimensions.x * destinationDimensions.y * destinationDimensions.z };
            }
            else if constexpr (VDimensions == 2)
            {
                return { { destinationDimensions.x, destinationDimensions.y, 1 } , destinationDimensions.x * destinationDimensions.y };
            }
            else if constexpr (VDimensions == 1)
            {
                return { { destinationDimensions.c[0], 1, 1 } , destinationDimensions.c[0] };
            }
        };
        
        auto [copyDimensions, dataSize] = getImageSize();

        Vulkan::Result<> result;

        Vulkan::DeviceBuffer stagingBuffer;
        if (!(result = stagingBuffer.Create(m_logicalDevice, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_memoryTypesHostVisibleOrCoherent)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for image buffer");
            return false;
        }

        if (!(result = stagingBuffer.MapMemory(0, dataSize, data)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for image buffer");
            return false;
        }

        if (!(result = image.CopyFromBuffer(
            m_commandPool,
            stagingBuffer,
            copyDimensions)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to copy from buffer to image.");
            return false;
        }

        if (!(result = image.TransitionToLayout(
            m_commandPool,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed transition image to read only optional layout");
            return false;
        }

        return true;
    }

    template<typename T>
    void VulkanRenderer::InternalPushConstant(const uint32_t location, const T& value)
    {
        vkCmdPushConstants(
            *m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);

    }

}