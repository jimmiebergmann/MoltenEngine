/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_VULKANTYPES_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_VULKANTYPES_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include "Molten/Renderer/Vulkan/VulkanHeaders.hpp"
#include <vector>
#include <set>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{
    using DescriptorSetLayouts = std::vector<VkDescriptorSetLayout>;
    using DescriptorSets = std::vector<VkDescriptorSet>;
    using ExtensionProperties = std::vector<VkExtensionProperties>;
    using Fences = std::vector<VkFence>;
    using Images = std::vector<VkImage>;
    using ImageViews = std::vector<VkImageView>;
    using PresentModes = std::vector<VkPresentModeKHR>;
    using Semaphores = std::vector<VkSemaphore>;
    using ShaderModules = std::vector<VkShaderModule>;
    using SurfaceFormats = std::vector<VkSurfaceFormatKHR>;   
    using UniqueQueueFamilyIds = std::set<uint32_t>;

    /** Single memory type, returned as vector by FilterMemoryTypesByPropertyFlags. */
    struct FilteredMemoryType
    {
        uint32_t index; ///< This is the index of this memory propery from the original VkPhysicalDeviceMemoryProperties object.
        VkMemoryPropertyFlags propertyFlags; // Bitmask of supported memory properies for this memory type.
    };

    using FilteredMemoryTypes = std::vector<FilteredMemoryType>;

    /** Object for storing physical device capabilities. */
    struct PhysicalDeviceCapabilities
    {

    };

    /** Object for storing swap chain capabilities. */
    /* struct SwapChainCapabilities
    {
        //ExtensionProperties extensions;
        PresentModes presentModes;
    };*/

    /** Object for storing surface capabilities. */
    struct SurfaceCapabilities
    {
        VkSurfaceCapabilitiesKHR capabilities;
        SurfaceFormats formats;
        PresentModes presentModes;
    };

    /** Object for storing surface data. */
    struct Surface
    {
        Surface();

        VkSurfaceKHR handle;
        SurfaceCapabilities capabilities;

    };

    /** Physical device. */
    struct PhysicalDevice
    {
        PhysicalDevice();

        PhysicalDeviceCapabilities capabilities;
        VkPhysicalDevice device;
        uint32_t graphicsQueueIndex;
        uint32_t presentQueueIndex;
        //SwapChainCapabilities swapChainCapabilities;          
    };

    using PhysicalDevices = std::vector< PhysicalDevice>;

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
