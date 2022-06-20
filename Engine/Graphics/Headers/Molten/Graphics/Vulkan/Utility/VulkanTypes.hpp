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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANTYPES_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANTYPES_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanHeaders.hpp"
#include <vector>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    using CommandBuffers = std::vector<VkCommandBuffer>;
    using DescriptorSetLayouts = std::vector<VkDescriptorSetLayout>;
    using DescriptorSets = std::vector<VkDescriptorSet>;
    using Fences = std::vector<VkFence>;
    using FrameBuffers = std::vector<VkFramebuffer>;
    using Images = std::vector<VkImage>;
    using ImageViews = std::vector<VkImageView>;
    using PipelineShaderStageCreateInfos = std::vector<VkPipelineShaderStageCreateInfo>;
    using PresentModes = std::vector<VkPresentModeKHR>;
    using QueueFamilyProperties = std::vector<VkQueueFamilyProperties>;
    using Semaphores = std::vector<VkSemaphore>;
    using ShaderModules = std::vector<VkShaderModule>;
    using SurfaceFormats = std::vector<VkSurfaceFormatKHR>;

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif