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

#include "Molten/Renderer/Vulkan/VulkanFunctions.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanResult.hpp"
#include "Molten/Logger.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{  

    // Vulkan functions implementations.
    bool CheckRequiredExtensions(
        std::vector<std::string>& missingExtensionNames,
        const std::vector<std::string>& requiredExtensionNames,
        const ExtensionProperties availableExtensions)
    {
        missingExtensionNames = requiredExtensionNames;

        for (const auto& availableExtension : availableExtensions)
        {
            auto it = std::find(missingExtensionNames.begin(), missingExtensionNames.end(), availableExtension.extensionName);
            if (it != missingExtensionNames.end())
            {
                missingExtensionNames.erase(it);
            }
        }

        return missingExtensionNames.size() == 0;
    }

    VkResult CreateBuffer(
        VkBuffer& buffer,
        VkDeviceMemory& memory,
        VkDevice logicalDevice,
        const VkDeviceSize size,
        const VkBufferUsageFlags usage,
        const VkDeviceSize allocationSize,
        const uint32_t memoryTypeIndex)
    {
        VkResult result = VkResult::VK_SUCCESS;

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if ((result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer)) != VK_SUCCESS)
        {
            return result;
        }

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = allocationSize;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        if ((result = vkAllocateMemory(logicalDevice, &memoryAllocateInfo, nullptr, &memory)) != VK_SUCCESS)
        {
            vkDestroyBuffer(logicalDevice, buffer, nullptr);
            return result;
        }

        if ((result = vkBindBufferMemory(logicalDevice, buffer, memory, 0)) != VK_SUCCESS)
        {
            vkDestroyBuffer(logicalDevice, buffer, nullptr);
            vkFreeMemory(logicalDevice, memory, nullptr);
            return result;
        }

        return result;
    }

    VkResult CreateFences(
        Fences& fences,
        VkDevice logicalDevice,
        const VkFenceCreateFlagBits createFlags,
        const size_t count)
    {
        VkResult result = VkResult::VK_SUCCESS;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = createFlags;

        fences.resize(count, VK_NULL_HANDLE);
        for (size_t i = 0; i < count; i++)
        {
            if ((result = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fences[i])) != VkResult::VK_SUCCESS)
            {
                for (size_t j = 0; j <= i; j++)
                {
                    vkDestroyFence(logicalDevice, fences[j], nullptr);
                }
                return result;
            }
        }

        return result;
    }

    VkResult CreateInstance(
        VkInstance& instance,
        const Version& vulkanVersion,
        const std::string& engineName,
        const Version& engineVersion,
        const std::string& applicationName,
        const Version& applicationVersion,
        const std::vector<std::string>& enabledExtensions,
        const VkInstanceCreateInfo& preparedInstanceInfo)
    {
        VkResult result = VkResult::VK_SUCCESS;

        auto engineVersionVulkan = CreateVersion(engineVersion);
        auto vulkanVersionVulkan = CreateVersion(vulkanVersion);

        std::vector<const char*> ptrExtensions(enabledExtensions.size(), nullptr);
        for (size_t i = 0; i < enabledExtensions.size(); i++)
        {
            ptrExtensions[i] = enabledExtensions[i].c_str();
        }

        // Create instance.
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = engineName.c_str();
        appInfo.engineVersion = engineVersionVulkan;
        appInfo.apiVersion = vulkanVersionVulkan;

        VkInstanceCreateInfo instanceInfo = preparedInstanceInfo;
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr; 
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(ptrExtensions.size());
        instanceInfo.ppEnabledExtensionNames = ptrExtensions.data();

        if ((result = vkCreateInstance(&instanceInfo, nullptr, &instance)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        return result;
    }

    VkResult CreateLogicalDevice(
        VkDevice& logicalDevice,
        UniqueQueueFamilyIds familyIds,
        VkPhysicalDevice physicalDevice,
        const VkPhysicalDeviceFeatures enabledPhysicalDeviceFeatures,
        const std::vector<std::string> enabledExtensions,
        const std::vector<std::string> enabledLayers)
    {
        VkResult result = VkResult::VK_SUCCESS;

        const float queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        for (auto familyId : familyIds)
        {
            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = familyId;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;
            queueInfos.push_back(queueInfo);
        }

        std::vector<const char*> ptrEnabledExtensions(enabledExtensions.size(), nullptr);
        for(size_t i = 0; i < enabledExtensions.size() ; i++)
        {
            ptrEnabledExtensions[i] = enabledExtensions[i].c_str();
        }
        std::vector<const char*> ptrEnabledLayers(enabledLayers.size(), nullptr);
        for (size_t i = 0; i < enabledLayers.size(); i++)
        {
            ptrEnabledLayers[i] = enabledLayers[i].c_str();
        }

        VkDeviceCreateInfo deviceInfo = {};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.pEnabledFeatures = &enabledPhysicalDeviceFeatures;
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(ptrEnabledExtensions.size());
        deviceInfo.ppEnabledExtensionNames = ptrEnabledExtensions.data();
        deviceInfo.enabledLayerCount = static_cast<uint32_t>(ptrEnabledLayers.size());
        deviceInfo.ppEnabledLayerNames = ptrEnabledLayers.data();
        
        if ((result = vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &logicalDevice)) != VK_SUCCESS)
        {
            return result;
        }
       
        return result;
    }

    VkResult CreatePlatformSurface(
        VkSurfaceKHR& surface,
        VkInstance& instance,
        RenderTarget& renderTarget)
    {
    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

        VkResult result = VkResult::VK_SUCCESS;

        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.hwnd = renderTarget.GetWin32Window();
        surfaceInfo.hinstance = renderTarget.GetWin32Instance();

        if ((result = vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface)) != VK_SUCCESS)
        {
            return result;
        }

        return result;

    #else
        
        return VkResult::VK_ERROR_FEATURE_NOT_PRESENT;

    #endif
    }

    VkResult CreateSemaphores(
        Semaphores& semaphores,
        VkDevice logicalDevice,
        const size_t count)
    {
        VkResult result = VkResult::VK_SUCCESS;

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        semaphores.resize(count, VK_NULL_HANDLE);
        for (size_t i = 0; i < count; i++)
        {
            if ((result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &semaphores[i])) != VkResult::VK_SUCCESS)
            {
                for (size_t j = 0; j <= i; j++)
                {
                    vkDestroySemaphore(logicalDevice, semaphores[j], nullptr);
                }
                return result;
            }
        }

        return result;
    }

    VkResult CreateSwapchain(
        VkSwapchainKHR& swapchain,
        VkDevice logicalDevice,
        const VkSurfaceKHR surface,
        const VkSurfaceFormatKHR& surfaceFormat,
        const VkPresentModeKHR presentMode,
        const VkSurfaceCapabilitiesKHR& capabilities,
        const uint32_t imageCount,
        const uint32_t graphicsQueueIndex,
        const uint32_t presentQueueIndex,
        VkSwapchainKHR oldSwapchain)
    {
        VkResult result = VkResult::VK_SUCCESS;

        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = surface;
        swapchainInfo.presentMode = presentMode;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.minImageCount = imageCount;
        swapchainInfo.imageFormat = surfaceFormat.format;
        swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainInfo.imageExtent = capabilities.currentExtent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.preTransform = capabilities.currentTransform;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.oldSwapchain = oldSwapchain;

        const uint32_t queueFamilies[2] = { graphicsQueueIndex, presentQueueIndex };
        if (queueFamilies[0] != queueFamilies[1])
        {
            swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainInfo.queueFamilyIndexCount = 2;
            swapchainInfo.pQueueFamilyIndices = queueFamilies;
        }
        else
        {
            swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainInfo.queueFamilyIndexCount = 0;
            swapchainInfo.pQueueFamilyIndices = nullptr;
        }

        if ((result = vkCreateSwapchainKHR(logicalDevice, &swapchainInfo, nullptr, &swapchain)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        if (oldSwapchain)
        {
            vkDestroySwapchainKHR(logicalDevice, oldSwapchain, nullptr);
        }

        return result;
    }


    uint32_t CreateVersion(const Version& version)
    {
        return VK_MAKE_VERSION(version.Major, version.Minor, version.Patch);
    }

    void DestroyFences(VkDevice logicalDevice, Fences& fences)
    {
        for (auto fence : fences)
        {
            vkDestroyFence(logicalDevice, fence, nullptr);
        }
    }

    void DestroyImageViews(VkDevice logicalDevice, ImageViews& imageViews)
    {
        for (auto& imageView : imageViews)
        {
            vkDestroyImageView(logicalDevice, imageView, nullptr);
        }
    }

    void DestroySemaphores(VkDevice logicalDevice, Semaphores& semaphores)
    {
        for (auto semaphore : semaphores)
        {
            vkDestroySemaphore(logicalDevice, semaphore, nullptr);
        }
    }


    VkResult FetchDeviceExtensionProperties(
        ExtensionProperties& extensionProperties,
        VkPhysicalDevice physicalDevice)
    {
        extensionProperties.clear();

        VkResult result = VkResult::VK_SUCCESS;

        uint32_t extensionCount = 0;
        if ((result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        if (!extensionCount)
        {
            return result;
        }

        extensionProperties.resize(extensionCount);
        if ((result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        return result;
    }

    VkResult FetchInstanceExtensionProperties(
        ExtensionProperties& extensionProperties)
    {
        extensionProperties.clear();

        VkResult result = VkResult::VK_SUCCESS;

        uint32_t extensionCount = 0;
        if ((result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        if (!extensionCount)
        {
            return result;
        }

        extensionProperties.resize(extensionCount);
        if ((result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }
            
        return result;
    }

    VkResult FetchPhysicalDevices(
        PhysicalDevices& physicalDevices,
        VkInstance instance)
    {
        VkResult result = VkResult::VK_SUCCESS;

        uint32_t deviceCount = 0;
        if ((result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }
            
        if (!deviceCount)
        {
            return VkResult::VK_SUCCESS;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        if ((result = vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        for (uint32_t i = 0; i < deviceCount; i++)
        {
            auto& physicalDevice = physicalDevices[i];
            auto& device = devices[i];
            physicalDevice.device = device;
        }

        return result;
    }

    VkResult FetchSurfaceCapabilities(
        SurfaceCapabilities& surfaceCababilities,
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface)
    {
        VkResult result = VkResult::VK_SUCCESS;

        if ((result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCababilities.capabilities)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        uint32_t formatCount = 0;
        if ((result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }
        if (formatCount)
        {
            surfaceCababilities.formats.resize(formatCount);
            if ((result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceCababilities.formats.data())) != VkResult::VK_SUCCESS)
            {
                return result;
            }
        }

        uint32_t presentModeCount = 0;
        if ((result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }
        if (presentModeCount)
        {
            surfaceCababilities.presentModes.resize(presentModeCount);
            if ((result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, surfaceCababilities.presentModes.data())) != VkResult::VK_SUCCESS)
            {
                return result;
            }
        }

        return result;
    }

    const std::string& GetPlatformSurfaceExtensionName()
    {
    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
        static const std::string surfaceExtensionName = "VK_KHR_win32_surface";
    #elif MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX
        static const std::string surfaceExtensionName = "VK_KHR_xlib_surface";
    #else
        static const std::string surfaceExtensionName = "";
    #endif
        return surfaceExtensionName;
    }

    VkResult GetSwapchainImages(
        Images& images,
        VkDevice logicalDevice,
        const VkSwapchainKHR swapchain)
    {
        VkResult result = VkResult::VK_SUCCESS;

        uint32_t imageCount = 0;
        if ((result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        images.resize(imageCount);
        if ((result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, images.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        return result;
    }

    FilteredMemoryTypes FilterMemoryTypesByPropertyFlags(
        const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperies,
        const uint32_t propertyFlags)
    {
        FilteredMemoryTypes filteredMemorytypes;

        for (uint32_t i = 0; i < physicalDeviceMemoryProperies.memoryTypeCount; i++)
        {
            auto& memoryType = physicalDeviceMemoryProperies.memoryTypes[i];
            if ((memoryType.propertyFlags & propertyFlags) == propertyFlags)
            {
                filteredMemorytypes.push_back(FilteredMemoryType{ i, memoryType.propertyFlags });
            }
        }
            
        return filteredMemorytypes;
    }

    bool FindMemoryTypeIndex(
        const FilteredMemoryTypes& filteredMemoryTypes,
        const uint32_t requiredMemoryTypeFlags,
        uint32_t& index)
    {
        for (auto& filteredMemoryType : filteredMemoryTypes)
        {
            if (requiredMemoryTypeFlags & (1 << filteredMemoryType.index))
            {
                index = filteredMemoryType.index;
                return true;
            }
        }

        return false;
    }

    const VkBaseInStructure& FindLastBaseInStructure(VkBaseInStructure& baseInStructure)
    {
        const auto* currentStructure = &baseInStructure;
        while (currentStructure->pNext != nullptr)
        {
            currentStructure = currentStructure->pNext;
        }

        return *currentStructure;
    }

    VkBaseOutStructure& FindLastBaseOutStructure(VkBaseOutStructure& baseOutStructure)
    {
        auto* currentStructure = &baseOutStructure;
        while (currentStructure->pNext != nullptr)
        {
            currentStructure = currentStructure->pNext;
        }

        return *currentStructure;
    }

    void Log(Logger* logger, const std::string& message, VkResult result)
    {
        if (logger == nullptr)
        {
            return;
        }

        VulkanResult parsedResult(result);

        Logger::WriteError(logger, message + ": (" + parsedResult.name + "): " + parsedResult.description);
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif