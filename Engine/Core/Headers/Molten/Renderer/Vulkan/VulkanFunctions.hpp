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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_VULKANFUNCTIONS_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_VULKANFUNCTIONS_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanTypes.hpp"
#include "Molten/Renderer/RenderTarget.hpp"
#include "Molten/System/Version.hpp"
#include "Molten/Logger.hpp"
#include <string>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten
{
    class Logger;
}

namespace Molten::Vulkan
{

    /** Check if all required extensions are present in provided extension container.. */
    MOLTEN_API bool CheckRequiredExtensions(
        std::vector<std::string>& missingExtensionNames,
        const std::vector<std::string>& requiredExtensionNames,
        const ExtensionProperties availableExtensions);


    /** Create buffer with given size and usage. */
    MOLTEN_API VkResult CreateBuffer(
        VkBuffer& buffer,
        VkDeviceMemory& memory,
        VkDevice logicalDevice,
        const VkDeviceSize size,
        const VkBufferUsageFlags usage,
        const VkDeviceSize allocationSize,
        const uint32_t memoryTypeIndex);

    /** Create N number of fences. */
    MOLTEN_API VkResult CreateFences(
        Fences& fences,
        VkDevice logicalDevice,
        const VkFenceCreateFlagBits createFlags,
        const size_t count);

    /** Create instance, by optionally providing prepared instanceInfo, which should be done by debug messenger. */
    MOLTEN_API VkResult CreateInstance(
        VkInstance& instance,
        const Version& vulkanVersion,
        const std::string& engineName,
        const Version& engineVersion,
        const std::string& applicationName,
        const Version& applicationVersion,
        const std::vector<std::string>& enabledExtensions,
        const VkInstanceCreateInfo& preparedInstanceInfo = {});

    /** Create logical device from physical device with other provided properties. */
    MOLTEN_API VkResult CreateLogicalDevice(
        VkDevice& logicalDevice,
        UniqueQueueFamilyIds familyIds,      
        VkPhysicalDevice physicalDevice,
        const VkPhysicalDeviceFeatures enabledPhysicalDeviceFeatures = {},
        const std::vector<std::string> enabledExtensions = {},
        const std::vector<std::string> enabledLayers = {});

    /** Create platform specific surface. */
    MOLTEN_API VkResult CreatePlatformSurface(
        VkSurfaceKHR& surface,
        VkInstance& instance,
        RenderTarget& renderTarget);

    /** Create N number of semaphores. */
    MOLTEN_API VkResult CreateSemaphores(
        Semaphores& semaphores,
        VkDevice logicalDevice,
        const size_t count);

    /** Create swapchain with a surface, graphics and present queue. */
    MOLTEN_API VkResult CreateSwapchain(
        VkSwapchainKHR& swapchain,
        VkDevice logicalDevice,                                            
        const VkSurfaceKHR surface, 
        const VkSurfaceFormatKHR& surfaceFormat,
        const VkPresentModeKHR presentMode,
        const VkSurfaceCapabilitiesKHR & capabilities, 
        const uint32_t imageCount,
        const uint32_t graphicsQueueIndex,
        const uint32_t presentQueueIndex,
        VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);

    /** Create Vulkan version(uint32_t) from Molten version,*/
    MOLTEN_API uint32_t CreateVersion(const Version& version);


    /** Destroy all fences in vector. */
    MOLTEN_API void DestroyFences(
        VkDevice logicalDevice,
        Fences& fences);

    /** Destroy all image vies in vector. */
    MOLTEN_API void DestroyImageViews(
        VkDevice logicalDevice,
        ImageViews& imageViews);

    /** Destroy all semaphores in vector. */
    MOLTEN_API void DestroySemaphores(
        VkDevice logicalDevice,
        Semaphores& semaphores);


    /** Fetch avilable extension properties from physical device. */
    MOLTEN_API VkResult FetchDeviceExtensionProperties(
        ExtensionProperties& extensionProperties,
        VkPhysicalDevice physicalDevice);

    /** Fetch extension capabilities from physical device. */
    MOLTEN_API VkResult FetchInstanceExtensionProperties(
        ExtensionProperties& extensionProperties);

    /** Fetch physical devices from vulkan instance. */
    MOLTEN_API VkResult FetchPhysicalDevices(
        PhysicalDevices& physicalDevices,
        VkInstance instance);

    /** Fetch swap chain capabilities from physical device. */
    MOLTEN_API VkResult FetchSurfaceCapabilities(
        SurfaceCapabilities& surfaceCababilities,
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface);


    /** Get platform specific surface extension name. */
    MOLTEN_API const std::string& GetPlatformSurfaceExtensionName();

    /** Get vector of images in swapchain. */
    MOLTEN_API VkResult GetSwapchainImages(
        Images& images,
        VkDevice logicalDevice, 
        const VkSwapchainKHR swapchain);


    /** Filter provided physical device memory properies by filter flags.
        *  Returns vector of provided memory properties that contains all filter flags.
        */
    MOLTEN_API FilteredMemoryTypes FilterMemoryTypesByPropertyFlags(
        const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperies, 
        const uint32_t propertyFlags);

    /** Find first index of device memory propery with required memory type flags. */
    MOLTEN_API bool FindMemoryTypeIndex(
        const FilteredMemoryTypes& filteredMemoryTypes,
        const uint32_t requiredMemoryTypeFlags, 
        uint32_t& index);


    /** Search for the last in structure, where pNext == nullptr. */
    MOLTEN_API const VkBaseInStructure& FindLastBaseInStructure(VkBaseInStructure& baseInStructure);

    /** Search for the last out structure, where pNext == nullptr. */
    MOLTEN_API VkBaseOutStructure& FindLastBaseOutStructure(VkBaseOutStructure& baseOutStructure);


    /** Log vulkan result */
    MOLTEN_API void Log(Logger* logger, const std::string& message, VkResult result);

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
