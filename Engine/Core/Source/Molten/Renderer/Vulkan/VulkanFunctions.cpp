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
#include <algorithm>
#include <map>
#include <set>

MOLTEN_UNSCOPED_ENUM_BEGIN

#define CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME_QUOTE(x) #x
#define CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(feature) {  &VkPhysicalDeviceFeatures::##feature, CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME_QUOTE(##feature) }

namespace Molten::Vulkan
{  

    struct InternalPhysicalDeviceFeatureWithName
    {
        VkBool32 VkPhysicalDeviceFeatures::* memberPointer;
        const std::string name;
    };

    // Static data
    static const std::vector<InternalPhysicalDeviceFeatureWithName> g_allPhysicalDeviceFeatures = {
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(robustBufferAccess),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(fullDrawIndexUint32),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(imageCubeArray),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(independentBlend),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(geometryShader),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(tessellationShader),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sampleRateShading),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(dualSrcBlend),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(logicOp),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(multiDrawIndirect),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(drawIndirectFirstInstance),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(depthClamp),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(depthBiasClamp),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(fillModeNonSolid),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(depthBounds),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(wideLines),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(largePoints),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(alphaToOne),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(multiViewport),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(samplerAnisotropy),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(textureCompressionETC2),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(textureCompressionASTC_LDR),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(textureCompressionBC),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(occlusionQueryPrecise),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(pipelineStatisticsQuery),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(vertexPipelineStoresAndAtomics),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(fragmentStoresAndAtomics),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderTessellationAndGeometryPointSize),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderImageGatherExtended),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageExtendedFormats),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageMultisample),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageReadWithoutFormat),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageWriteWithoutFormat),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderUniformBufferArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderSampledImageArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageBufferArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderClipDistance),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderCullDistance),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderFloat64),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderInt64),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderInt16),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderResourceResidency),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderResourceMinLod),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseBinding),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyBuffer),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyImage2D),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyImage3D),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency2Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency4Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency8Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency16Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyAliased),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(variableMultisampleRate),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(inheritedQueries)
    };



    // Vulkan functions implementations.
    bool CheckRequiredExtensions(
        Extensions& missingExtensions,
        const Extensions& requiredExtensions,
        const Extensions& availableExtensions)
    {
        missingExtensions = requiredExtensions;

        for (const auto& availableExtension : availableExtensions)
        {
            auto it = std::find_if(missingExtensions.begin(), missingExtensions.end(),
                [&](auto& extension) {return availableExtension.name == extension.name && availableExtension.version >= extension.version; });
            if (it != missingExtensions.end())
            {
                missingExtensions.erase(it);
            }
        }

        return missingExtensions.size() == 0;
    }

    bool CheckRequiredDeviceFeatures(
        PhysicalDeviceFeaturesWithName& missingFeatures,
        const VkPhysicalDeviceFeatures& requiredFeatures,
        const VkPhysicalDeviceFeatures& availableFeatures)
    {
        bool failed = false;

        for (auto& feature : g_allPhysicalDeviceFeatures)
        {
            if (requiredFeatures.*(feature.memberPointer) &&
                requiredFeatures.*(feature.memberPointer) != availableFeatures.*(feature.memberPointer))
            {
                failed = true;
                missingFeatures.push_back({ feature.memberPointer, feature.name }); 
            }
        }

        return !failed;
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
        Instance& instance,
        const Version& vulkanVersion,
        const std::string& engineName,
        const Version& engineVersion,
        const std::string& applicationName,
        const Version& applicationVersion,
        const Extensions& enabledExtensions,
        const Layers& enabledLayers,
        DebugMessenger* debugMessenger)
    {
        VkResult result = VkResult::VK_SUCCESS;

        auto engineVersionVulkan = CreateVersion(engineVersion);
        auto vulkanVersionVulkan = CreateVersion(vulkanVersion);

        std::vector<const char*> ptrExtensions(enabledExtensions.size(), nullptr);
        for (size_t i = 0; i < enabledExtensions.size(); i++)
        {
            ptrExtensions[i] = enabledExtensions[i].name.c_str();
        }

        std::vector<const char*> ptrEnabledLayers(enabledLayers.size(), nullptr);
        for (size_t i = 0; i < enabledLayers.size(); i++)
        {
            ptrEnabledLayers[i] = enabledLayers[i].name.c_str();
        }

        // Create instance.
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = engineName.c_str();
        appInfo.engineVersion = engineVersionVulkan;
        appInfo.apiVersion = vulkanVersionVulkan;

        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr; 
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(ptrExtensions.size());
        instanceInfo.ppEnabledExtensionNames = ptrExtensions.data();
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(ptrEnabledLayers.size());
        instanceInfo.ppEnabledLayerNames = ptrEnabledLayers.data();

        if (debugMessenger)
        {
            instanceInfo.pNext = reinterpret_cast<const VkBaseOutStructure*>(&debugMessenger->GetCreateInfo());
        }

        if ((result = vkCreateInstance(&instanceInfo, nullptr, &instance.handle)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        /*if ((result = Vulkan::FetchInstanceExtensions(instance.extensions)) != VK_SUCCESS)
        {
            return result;
        }

        if ((result = Vulkan::FetchInstanceLayers(instance.layers)) != VK_SUCCESS)
        {
            return result;
        }*/

        return result;
    }

    VkResult CreateLogicalDevice(
        LogicalDevice& logicalDevice,
        PhysicalDeviceWithCapabilities& physicalDeviceWithCapabilities,
        const Layers& enabledInstanceLayers,
        const Extensions& enabledDeviceExtensions,
        const VkPhysicalDeviceFeatures& enabledDeviceFeatures)
    {
        VkResult result = VkResult::VK_SUCCESS;

        const std::set<uint32_t> uniqueFamilyIds = {
            physicalDeviceWithCapabilities.graphicsQueueIndex,
            physicalDeviceWithCapabilities.presentQueueIndex
        };
        
        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        const float queuePriority = 1.0f;

        for (auto familyId : uniqueFamilyIds)
        {
            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = familyId;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;
            queueInfos.push_back(queueInfo);
        }

        std::vector<const char*> ptrEnabledExtensions(enabledDeviceExtensions.size(), nullptr);
        for(size_t i = 0; i < enabledDeviceExtensions.size() ; i++)
        {
            ptrEnabledExtensions[i] = enabledDeviceExtensions[i].name.c_str();
        }
        std::vector<const char*> ptrEnabledLayers(enabledInstanceLayers.size(), nullptr);
        for (size_t i = 0; i < enabledInstanceLayers.size(); i++)
        {
            ptrEnabledLayers[i] = enabledInstanceLayers[i].name.c_str();
        }

        VkDeviceCreateInfo deviceInfo = {};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.pEnabledFeatures = &enabledDeviceFeatures;
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(ptrEnabledExtensions.size());
        deviceInfo.ppEnabledExtensionNames = ptrEnabledExtensions.data();
        deviceInfo.enabledLayerCount = static_cast<uint32_t>(ptrEnabledLayers.size());
        deviceInfo.ppEnabledLayerNames = ptrEnabledLayers.data();
        
        auto& physicalDevice = physicalDeviceWithCapabilities.device;
        if ((result = vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &logicalDevice.device)) != VK_SUCCESS)
        {
            return result;
        }

        vkGetDeviceQueue(logicalDevice.device, physicalDeviceWithCapabilities.graphicsQueueIndex, 0, &logicalDevice.graphicsQueue);
        vkGetDeviceQueue(logicalDevice.device, physicalDeviceWithCapabilities.presentQueueIndex, 0, &logicalDevice.presentQueue);
       
        return result;
    }

    VkResult CreatePlatformSurface(
        VkSurfaceKHR& surface,
        Instance& instance,
        RenderTarget& renderTarget)
    {
    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

        VkResult result = VkResult::VK_SUCCESS;

        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.hwnd = renderTarget.GetWin32Window();
        surfaceInfo.hinstance = renderTarget.GetWin32Instance();

        if ((result = vkCreateWin32SurfaceKHR(instance.handle, &surfaceInfo, nullptr, &surface)) != VK_SUCCESS)
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

    VkResult FetchDeviceExtensions(
        Extensions& extensions,
        VkPhysicalDevice physicalDevice)
    {
        extensions.clear();

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

        std::vector<VkExtensionProperties> extensionProperties(extensionCount, VkExtensionProperties{});
        if ((result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        std::copy(extensionProperties.begin(), extensionProperties.end(), std::back_inserter(extensions));
        return result;
    }

    VkResult FetchInstanceExtensions(
        Extensions& extensions)
    {
        extensions.clear();

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

        std::vector<VkExtensionProperties> extensionProperties(extensionCount, VkExtensionProperties{});
        if ((result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }
        
        std::copy(extensionProperties.begin(), extensionProperties.end(), std::back_inserter(extensions));
        return result;
    }

    VkResult FetchInstanceLayers(
        Layers& layers)
    {
        layers.clear();

        VkResult result = VkResult::VK_SUCCESS;

        uint32_t layerCount = 0;
        if ((result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        if (!layerCount)
        {
            return result;
        }

        std::vector<VkLayerProperties> layerProperties(layerCount, VkLayerProperties{});
        if ((result = vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        std::copy(layerProperties.begin(), layerProperties.end(), std::back_inserter(layers));
        return result;
    }


    MOLTEN_API void FetchQueueFamilyProperties(
        QueueFamilyProperties& queueFamilyProperties,
        VkPhysicalDevice physicalDevice)
    {
        queueFamilyProperties.clear();

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        if (!queueFamilyCount)
        {
            return;
        }

        queueFamilyProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
    }


    VkResult FetchPhysicalDevices(
        PhysicalDevices& physicalDevices,
        Instance& instance)
    {
        physicalDevices.clear();

        VkResult result = VkResult::VK_SUCCESS;

        uint32_t physicalDeviceCount = 0;
        if ((result = vkEnumeratePhysicalDevices(instance.handle, &physicalDeviceCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }
            
        if (!physicalDeviceCount)
        {
            return VkResult::VK_SUCCESS;
        }

        physicalDevices.resize(physicalDeviceCount);
        if ((result = vkEnumeratePhysicalDevices(instance.handle, &physicalDeviceCount, physicalDevices.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        return result;
    }

    VkResult FetchPhysicalDeviceSurfaceCapabilities(
        PhysicalDeviceSurfaceCapabilities& surfaceCababilities,
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

    void FilterMemoryTypesByPropertyFlags(
        FilteredMemoryTypes& filteredMemorytypes,
        const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperies,
        const uint32_t propertyFlags)
    {
        filteredMemorytypes.clear();

        for (uint32_t i = 0; i < physicalDeviceMemoryProperies.memoryTypeCount; i++)
        {
            auto& memoryType = physicalDeviceMemoryProperies.memoryTypes[i];
            if ((memoryType.propertyFlags & propertyFlags) == propertyFlags)
            {
                filteredMemorytypes.push_back(FilteredMemoryType{ i, memoryType.propertyFlags });
            }
        }
    }

    void FilterPhysicalDevicesWithRenderCapabilities(
        PhysicalDevicesWithCapabilities& filteredPhysicalDeviceWithCapabilities,
        PhysicalDevicesWithCapabilities& physicalDeviceWithCapabilities,
        const Vulkan::Extensions& requiredExtensions,
        const VkPhysicalDeviceFeatures& requiredDeviceFeatures,
        VkSurfaceKHR surface,
        Logger* logger)
    {
        filteredPhysicalDeviceWithCapabilities.clear();

        PhysicalDevicesWithCapabilities physicalDevicesCandidates;
        std::copy_if(physicalDeviceWithCapabilities.begin(), physicalDeviceWithCapabilities.end(), std::back_inserter(filteredPhysicalDeviceWithCapabilities),
            [&](PhysicalDeviceWithCapabilities& physicalDevice)
        {
            VkResult result = VkResult::VK_SUCCESS;

            VkPhysicalDevice& device = physicalDevice.device;

            Extensions& availableExtensions = physicalDevice.capabilities.extensions;
            if ((result = FetchDeviceExtensions(availableExtensions, physicalDevice.device)) != VkResult::VK_SUCCESS)
            {
                LogError(logger, "Failed to fetch device extensions", result);
                return false;
            }


            Extensions missingExtensions;
            if (!CheckRequiredExtensions(missingExtensions, requiredExtensions, availableExtensions))
            {
                return false;
            }

            VkPhysicalDeviceFeatures& features = physicalDevice.capabilities.features;
            vkGetPhysicalDeviceFeatures(device, &features);
            Vulkan::PhysicalDeviceFeaturesWithName missingFeatures;
            if(!Vulkan::CheckRequiredDeviceFeatures(missingFeatures, requiredDeviceFeatures, features))
            {
                return false;
            }

            PhysicalDeviceSurfaceCapabilities& surfaceCapabilities = physicalDevice.capabilities.surfaceCapabilities;
            if ((result = FetchPhysicalDeviceSurfaceCapabilities(surfaceCapabilities, physicalDevice.device, surface)) != VkResult::VK_SUCCESS)
            {
                LogError(logger, "Failed to fetch surface capabilities", result);
                return false;
            }

            if (!surfaceCapabilities.formats.size() ||
                !surfaceCapabilities.presentModes.size())
            {
                return false;
            }

            QueueFamilyProperties& queueFamilies = physicalDevice.capabilities.queueFamilies;
            FetchQueueFamilyProperties(queueFamilies, device);

            bool foundQueueIndicies = FindSuitableQueueIndices(
                physicalDevice.graphicsQueueIndex,
                physicalDevice.presentQueueIndex,
                device,
                surface,
                queueFamilies);

            if (!foundQueueIndicies)
            {
                return false;
            }

            VkPhysicalDeviceProperties& properties = physicalDevice.capabilities.properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            return true;
        });
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

    Layers::iterator FindLayer(Layers& layers, const std::string& name)
    {
        return std::find_if(layers.begin(), layers.end(), 
            [&](Layer& layer)
        {
            return layer.name == name;
        });
    }

    Extensions::iterator FindExtension(Extensions& extensions, const std::string& name)
    {
        return std::find_if(extensions.begin(), extensions.end(),
            [&](Extension& extension)
        {
            return extension.name == name;
        });
    }

    bool FindMemoryTypeIndex(
        uint32_t& index,
        const FilteredMemoryTypes& filteredMemoryTypes,
        const uint32_t requiredMemoryTypeFlags)
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

    bool FindSuitableQueueIndices(
        uint32_t& graphicsQueueIndex,
        uint32_t& presentQueueIndex,
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        const QueueFamilyProperties& queueFamilies,
        const VkQueueFlags requiredQueueFlags)
    {
        graphicsQueueIndex = std::numeric_limits<uint32_t>::max();
        presentQueueIndex = std::numeric_limits<uint32_t>::max();

        uint32_t finalGraphicsQueueIndex = 0;
        uint32_t finalPresentQueueIndex = 0;

        bool graphicsFamilySupport = false;
        bool presentFamilySupport = false;

        for(uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            auto& queueFamily = queueFamilies[i];

            if (queueFamily.queueFlags & (VK_QUEUE_GRAPHICS_BIT | requiredQueueFlags))
            {
                graphicsFamilySupport = true;
                finalGraphicsQueueIndex = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport)
            {
                presentFamilySupport = true;
                finalPresentQueueIndex = i;
            }

            if (graphicsFamilySupport && presentFamilySupport)
            {
                break;
            }
        }
        if (!graphicsFamilySupport || !presentFamilySupport)
        {
            return false;
        }

        graphicsQueueIndex = finalGraphicsQueueIndex;
        presentQueueIndex = finalPresentQueueIndex;
        return true;
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

    void LogInfo(Logger* logger, const std::string& message, VkResult result)
    {
        if (logger == nullptr)
        {
            return;
        }

        VulkanResult parsedResult(result);

        Logger::WriteInfo(logger, message + ": (" + parsedResult.name + "): " + parsedResult.description);
    }
    void LogDebug(Logger* logger, const std::string& message, VkResult result)
    {
        if (logger == nullptr)
        {
            return;
        }

        VulkanResult parsedResult(result);

        Logger::WriteDebug(logger, message + ": (" + parsedResult.name + "): " + parsedResult.description);
    }
    void LogWarning(Logger* logger, const std::string& message, VkResult result)
    {
        if (logger == nullptr)
        {
            return;
        }

        VulkanResult parsedResult(result);

        Logger::WriteWarning(logger, message + ": (" + parsedResult.name + "): " + parsedResult.description);
    }
    void LogError(Logger* logger, const std::string& message, VkResult result)
    {
        if (logger == nullptr)
        {
            return;
        }

        VulkanResult parsedResult(result);

        Logger::WriteError(logger, message + ": (" + parsedResult.name + "): " + parsedResult.description);
    }

    VkResult PrepareInstance(
        Instance& instance)
    {
        VkResult result = VkResult::VK_SUCCESS;

        if ((result = Vulkan::FetchInstanceExtensions(instance.extensions)) != VK_SUCCESS)
        {
            return result;
        }

        if ((result = Vulkan::FetchInstanceLayers(instance.layers)) != VK_SUCCESS)
        {
            return result;
        }

        return result;
    }

    void RemoveLayers(Layers& layers, const Layers& excludes)
    {
        auto findLayer = [](const Layers& layers, const Layer& layer)
        {
            for (auto& currentLayer : layers)
            {
                if (currentLayer.name == layer.name)
                {
                    return true;
                }
            }
            return false;
        };
        auto it = std::remove_if(layers.begin(), layers.end(),
            [&](auto& extension)
        { 
            return findLayer(excludes, extension);
        });

        layers.erase(it);
    }

    void RemoveExtensions(Extensions& extensions, const Extensions& excludes)
    {
        auto findExtension = [](const Extensions& extensions, const Extension& extension)
        {
            for (auto& currentExtension : extensions)
            {
                if (currentExtension.name == extension.name)
                {
                    return true;
                }
            }
            return false;
        };
        auto it = std::remove_if(extensions.begin(), extensions.end(),
            [&](auto& extension)
        {
            return findExtension(excludes, extension);
        });

        extensions.erase(it);
    }

    bool ScorePhysicalDevices(
        PhysicalDeviceWithCapabilities& winningPhysicalDeviceWithCapabilities,
        const PhysicalDevicesWithCapabilities& physicalDevicesWithCapabilities,
        const ScorePhysicalDevicesCallback& callback)
    {
        std::multimap< int32_t, size_t> scoredDevicesIndices;

        for (size_t i = 0; i < physicalDevicesWithCapabilities.size(); i++)
        {
            auto& physicalDevice = physicalDevicesWithCapabilities[i];

            const int32_t score = callback(physicalDevice);

            if (score >= 0)
            {
                scoredDevicesIndices.insert({ score, i });
            }
        }

        if (!scoredDevicesIndices.size())
        {
            return false;
        }

        auto& highestScoreIndex = scoredDevicesIndices.rbegin()->second;
        if (highestScoreIndex < 0)
        {
            return false;
        }

        winningPhysicalDeviceWithCapabilities = physicalDevicesWithCapabilities[highestScoreIndex];
        return true;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif