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
#include "Molten/Renderer/Vulkan/VulkanDebugMessenger.hpp"
#include "Molten/Renderer/RenderTarget.hpp"
#include "Molten/System/Version.hpp"
#include "Molten/Logger.hpp"
#include <string>
#include <functional>
#include <memory>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten
{
    class Logger;
}

namespace Molten::Vulkan
{

    /** Check if all required extensions are present in provided extension container.. */
    MOLTEN_API bool CheckRequiredExtensions(
        Extensions& missingExtensions,
        const Extensions& requiredExtensions,
        const Extensions& availableExtensions);

    MOLTEN_API bool CheckRequiredDeviceFeatures(
        PhysicalDeviceFeaturesWithName& missingFeatures,
        const VkPhysicalDeviceFeatures& requiredFeatures,
        const VkPhysicalDeviceFeatures& availableFeatures);


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

    /** Create instance, by optionally providing prepared instanceInfo, which should be done by debug messenger. 
     *  Make sure to call PrepareInstance before CreateInstance.
     */
    MOLTEN_API VkResult CreateInstance(
        Instance& instance,
        const Version& vulkanVersion,
        const std::string& engineName,
        const Version& engineVersion,
        const std::string& applicationName,
        const Version& applicationVersion,
        const Extensions& enabledExtensions,
        const Layers& enabledLayers,
        DebugMessenger* debugMessenger = nullptr);

    /** Create logical device from physical device with other provided properties. */
    MOLTEN_API VkResult CreateLogicalDevice(
        LogicalDevice& logicalDevice,
        PhysicalDevice& physicalDeviceWithCapabilities,
        const Layers& enabledInstanceLayers = {},
        const Extensions& enabledDeviceExtensions = {},
        const VkPhysicalDeviceFeatures& enabledDeviceFeatures = {});

    /** Create platform specific surface. */
    MOLTEN_API VkResult CreatePlatformSurface(
        VkSurfaceKHR& surface,
        Instance& instance,
        RenderTarget& renderTarget);

    /** Create N number of semaphores. */
    MOLTEN_API VkResult CreateSemaphores(
        Semaphores& semaphores,
        VkDevice logicalDevice,
        const size_t count);


    /** Create swapchain with a surface, graphics and present queue. */
    MOLTEN_API VkResult CreateSwapChain(
        SwapChain& swapChain,
        LogicalDevice& logicalDevice,
        PhysicalDevice& physicalDeviceWithCapabilities,
        const VkSurfaceKHR surface, 
        const VkSurfaceFormatKHR& surfaceFormat,
        const VkPresentModeKHR presentMode,
        const VkSurfaceCapabilitiesKHR& surfaceCapabilities, 
        const uint32_t imageCount);
    

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
    MOLTEN_API VkResult FetchDeviceExtensions(
        Extensions& extensions,
        VkPhysicalDevice physicalDevice);

    /** Fetch instance extension names. */
    /*MOLTEN_API VkResult FetchInstanceExtensions(
        Extensions& extensions);*/

    /** Fetch instance layers. */
    MOLTEN_API VkResult FetchInstanceLayers(
        Layers& layers);

    MOLTEN_API void FetchQueueFamilyProperties(
        QueueFamilyProperties& queueFamilyProperties,
        VkPhysicalDevice physicalDevice);

    /** Fetch physical devices from vulkan instance.
     * Provide filters to ignore certain physical devices. 
     */

    MOLTEN_API VkResult FetchPhysicalDevices(
        PhysicalDevices& physicalDevices,
        Instance& instance);

    /** Fetch swap chain capabilities from physical device. */
    MOLTEN_API VkResult FetchPhysicalDeviceSurfaceCapabilities(
        PhysicalDeviceSurfaceCapabilities& surfaceCababilities,
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface);

    /** Filter provided physical device memory properies by filter flags.
      *  Returns vector of provided memory properties that contains all filter flags.
      */
    MOLTEN_API void FilterMemoryTypesByPropertyFlags(
        FilteredMemoryTypes& filteredMemorytypes,
        const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperies,
        const uint32_t propertyFlags);

    /***/
    MOLTEN_API void FilterPhysicalDevicesWithRenderCapabilities(
        PhysicalDevices& filteredPhysicalDeviceWithCapabilities,
        PhysicalDevices& physicalDeviceWithCapabilities,
        const Vulkan::Extensions& requiredExtensions,
        const VkPhysicalDeviceFeatures& requiredDeviceFeatures,
        VkSurfaceKHR surface,
        Logger* logger = nullptr
    );


    /** Search for the last in structure, where pNext == nullptr. */
    MOLTEN_API const VkBaseInStructure& FindLastBaseInStructure(VkBaseInStructure& baseInStructure);

    /** Search for the last out structure, where pNext == nullptr. */
    MOLTEN_API VkBaseOutStructure& FindLastBaseOutStructure(VkBaseOutStructure& baseOutStructure);

    /** Find layer in vector of layers, by name. Returns iterator of found layer, else layers.end(). */
    MOLTEN_API Layers::iterator FindLayer(Layers& layers, const std::string& name);

    /** Find layer in vector of extensions, by name. Returns iterator of found extension, else extensions.end(). */
    MOLTEN_API Extensions::iterator FindExtension(Extensions& extensions, const std::string& name);

    /** Find first index of device memory propery with required memory type flags. */
    MOLTEN_API bool FindMemoryTypeIndex(
        uint32_t& index,
        const FilteredMemoryTypes& filteredMemoryTypes,
        const uint32_t requiredMemoryTypeFlags);

    /** Find suitable pair of queue indices for rendering to a surface.
     * 
     * @requiredQueueFlags VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT is explicitly required.
     */
    MOLTEN_API bool FindSuitableQueueIndices(
        uint32_t& graphicsQueueIndex,
        uint32_t& presentQueueIndex,
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        const QueueFamilyProperties& queueFamilies,
        const VkQueueFlags requiredQueueFlags = 0);


    /** Get platform specific surface extension name. */
    MOLTEN_API const std::string& GetPlatformSurfaceExtensionName();

    /** Get vector of images in swapchain. */
    MOLTEN_API VkResult GetSwapchainImages(
        Images& images,
        VkDevice logicalDevice, 
        const VkSwapchainKHR swapchain);


    /** Log vulkan result */
    /**@{*/
    MOLTEN_API void LogInfo(Logger* logger, const std::string& message, VkResult result);
    MOLTEN_API void LogDebug(Logger* logger, const std::string& message, VkResult result);
    MOLTEN_API void LogWarning(Logger* logger, const std::string& message, VkResult result);
    MOLTEN_API void LogError(Logger* logger, const std::string& message, VkResult result);
    /**@}*/


    /** Preparing instance for creation.
     *  Make sure to call PrepareInstance before CreateInstance.
     */
    MOLTEN_API VkResult PrepareInstance(
        Instance& instance);


    /** Removing layers by an exluding list of other layers.*/
    MOLTEN_API void RemoveLayers(
        Layers& layers,
        const Layers& excludes);

    /** Removing extensions by an exluding list of other extensions.*/
    MOLTEN_API void RemoveExtensions(
        Extensions& extensions,
        const Extensions& excludes);


    /** Score physical device by iterating a list of physical devices and returns the device with highest score. 
     *  Callback function should return a positive interger if the device is considered as suitable.
     * 
     * @return true if any device with positive score is found, else false.
     */
    /**@{*/
    using ScorePhysicalDevicesCallback = std::function<int32_t(const PhysicalDevice&)>;
    
    MOLTEN_API bool ScorePhysicalDevices(
        PhysicalDevice& winningPhysicalDeviceWithCapabilities,
        const PhysicalDevices& physicalDevicesWithCapabilities,
        const ScorePhysicalDevicesCallback& callback);
    /**@}*/

}

MOLTEN_UNSCOPED_ENUM_END

#include "Molten/Renderer/Vulkan/VulkanFunctions.inl"

#endif

#endif
