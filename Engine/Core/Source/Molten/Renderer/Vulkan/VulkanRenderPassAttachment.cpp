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

#include "Molten/Renderer/Vulkan/VulkanRenderPassAttachment.hpp"


namespace Molten
{

    // Vulkan render pass attachment implementations.
    VulkanRenderPassAttachment::VulkanRenderPassAttachment() :
        type{ RenderPassAttachmentType::Color },
        texture{},
        clearValue{},
        initialLayout{ VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED },
        finalLayout{ VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED }
    {}

    VulkanRenderPassAttachment::VulkanRenderPassAttachment(
        const RenderPassAttachmentType type,
        SharedRenderResource<VulkanFramedTexture<2>> texture,
        std::optional<Vector4f32> clearValue,
        const VkImageLayout initialLayout,
        const VkImageLayout finalLayout
    ) :
        type{ type },
        texture{ std::move(texture) },
        clearValue{ clearValue },
        initialLayout{ initialLayout },
        finalLayout{ finalLayout }
    {}

}
