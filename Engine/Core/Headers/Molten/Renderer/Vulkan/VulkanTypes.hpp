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
#include "Molten/System/Version.hpp"
#include <vector>
#include <set>
#include <string>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{
    using DescriptorSetLayouts = std::vector<VkDescriptorSetLayout>;
    using DescriptorSets = std::vector<VkDescriptorSet>;
    using Fences = std::vector<VkFence>;
    using Images = std::vector<VkImage>;
    using ImageViews = std::vector<VkImageView>;
    using PhysicalDevices = std::vector<VkPhysicalDevice>;
    using PresentModes = std::vector<VkPresentModeKHR>;
    using QueueFamilyProperties = std::vector<VkQueueFamilyProperties>;
    using Semaphores = std::vector<VkSemaphore>;
    using ShaderModules = std::vector<VkShaderModule>;
    using SurfaceFormats = std::vector<VkSurfaceFormatKHR>;   
    using UniqueQueueFamilyIds = std::set<uint32_t>;
    
    /** A more c++ friendly version of VkExtensionProperties. */
    struct MOLTEN_API Extension
    {
        Extension();
        explicit Extension(const std::string& name);
        Extension(
            const std::string& name,
            const uint32_t version);
        Extension(
            const VkExtensionProperties& extensionProperties);

        Extension(const Extension& extension);
        Extension(Extension&& extension) noexcept;
        Extension& operator =(const Extension& extension);
        Extension& operator =(Extension&& extension) noexcept;

        std::string name;
        uint32_t version;
    };

    using Extensions = std::vector<Extension>;

    /** Single memory type, returned as vector by FilterMemoryTypesByPropertyFlags. */
    struct MOLTEN_API FilteredMemoryType
    {
        FilteredMemoryType();
        FilteredMemoryType(
            const uint32_t index,
            const VkMemoryPropertyFlags propertyFlags);

        uint32_t index; ///< This is the index of this memory propery from the original VkPhysicalDeviceMemoryProperties object.
        VkMemoryPropertyFlags propertyFlags; // Bitmask of supported memory properies for this memory type.
    };

    using FilteredMemoryTypes = std::vector<FilteredMemoryType>;

    /** A more c++ friendly version of VkLayerProperties. */
    struct MOLTEN_API Layer
    {
        Layer();
        explicit Layer(const std::string& name);
        Layer(
            const std::string& name,
            const uint32_t version,
            const Version vulkanVersion);
        Layer(
            const VkLayerProperties& layerProperties);

        Layer(const Layer& layer);
        Layer(Layer&& layer) noexcept;
        Layer& operator =(const Layer& layer);
        Layer& operator =(Layer&& layer) noexcept;

        std::string name;
        uint32_t version;
        Version vulkanVersion;
    };

    using Layers = std::vector<Layer>;

    /** Vulkan instance. */
    struct MOLTEN_API Instance
    {
        Instance();

        VkInstance handle;
        Extensions extensions;
        Layers layers;
    };

    /** Logical device data. */
    struct MOLTEN_API LogicalDevice
    {
        LogicalDevice();

        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
    };

    /** Object for storing surface capabilities. */
    struct MOLTEN_API PhysicalDeviceSurfaceCapabilities
    {
        PhysicalDeviceSurfaceCapabilities();

        VkSurfaceCapabilitiesKHR capabilities;
        SurfaceFormats formats;
        PresentModes presentModes;
    };

    /** Object for storing physical device capabilities. */
    struct MOLTEN_API PhysicalDeviceCapabilities
    {
        PhysicalDeviceCapabilities();

        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        Vulkan::Extensions extensions;
        VkBool32 hasPresentSupport;
        PhysicalDeviceSurfaceCapabilities surfaceCapabilities;
        QueueFamilyProperties queueFamilies;
    };

    struct MOLTEN_API PhysicalDeviceFeatureWithName
    {
        PhysicalDeviceFeatureWithName(
            VkBool32 VkPhysicalDeviceFeatures::* memberPointer,
            const std::string& name);

        VkBool32 VkPhysicalDeviceFeatures::* memberPointer;
        const std::string& name;
    };

    using PhysicalDeviceFeaturesWithName = std::vector<PhysicalDeviceFeatureWithName>;

    /** Physical device with its capabilities. */
    struct MOLTEN_API PhysicalDeviceWithCapabilities
    {
        PhysicalDeviceWithCapabilities();
        PhysicalDeviceWithCapabilities(
            VkPhysicalDevice device);

        VkPhysicalDevice device;
        PhysicalDeviceCapabilities capabilities;      
        uint32_t graphicsQueueIndex;
        uint32_t presentQueueIndex;
    };

    using PhysicalDevicesWithCapabilities = std::vector<PhysicalDeviceWithCapabilities>;

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
