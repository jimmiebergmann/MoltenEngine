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

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDeviceCapabilities.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanSurface.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceQueues.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Physical device surface capabilities implementations.
    PhysicalDeviceSurfaceCapabilities::PhysicalDeviceSurfaceCapabilities() :
        capabilities{},
        formats{},
        presentModes{}
    {}


    // Physical device capabilities implementations.
    PhysicalDeviceCapabilities::PhysicalDeviceCapabilities() :
        properties{},
        features{},
        extensions{},
        hasPresentSupport(false),
        surfaceCapabilities{},
        queueFamilies{}
    {}


    // Static implementations.
    Result<> FetchPhysicalDeviceCapabilities(
        PhysicalDeviceCapabilities& capabilities,
        const VkPhysicalDevice physicalDeviceHandle,
        const VkSurfaceKHR surfaceHandle)
    {
        if (const auto result = FetchPhysicalDeviceSurfaceCapabilities(capabilities.surfaceCapabilities, physicalDeviceHandle, surfaceHandle); !result.IsSuccessful())
        {
            return result;
        }

        Extensions& extensions = capabilities.extensions;
        if (const auto result = FetchDeviceExtensions(extensions, physicalDeviceHandle); !result.IsSuccessful())
        {
            return result;
        }

        VkPhysicalDeviceFeatures& features = capabilities.features;
        vkGetPhysicalDeviceFeatures(physicalDeviceHandle, &features);

        QueueFamilyProperties& queueFamilies = capabilities.queueFamilies;
        FetchQueueFamilyProperties(queueFamilies, physicalDeviceHandle);

        VkPhysicalDeviceProperties& properties = capabilities.properties;
        vkGetPhysicalDeviceProperties(physicalDeviceHandle, &properties);

        return {};
    }

    Result<> FetchPhysicalDeviceSurfaceCapabilities(
        PhysicalDeviceSurfaceCapabilities& surfaceCababilities,
        const VkPhysicalDevice physicalDeviceHandle,
        const VkSurfaceKHR surfaceHandle)
    {
        surfaceCababilities.formats.clear();
        surfaceCababilities.presentModes.clear();

        if (const auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            physicalDeviceHandle,
            surfaceHandle, 
            &surfaceCababilities.capabilities); result != VkResult::VK_SUCCESS)
        {
            return result;
        }

        uint32_t formatCount = 0;
        if (const auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDeviceHandle,
            surfaceHandle,
            &formatCount, 
            nullptr); result != VkResult::VK_SUCCESS)
        {
            return result;
        }
        if (formatCount)
        {
            surfaceCababilities.formats.resize(formatCount);
            if (const auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(
                physicalDeviceHandle,
                surfaceHandle,
                &formatCount,
                surfaceCababilities.formats.data()); result != VkResult::VK_SUCCESS)
            {
                return result;
            }
        }

        uint32_t presentModeCount = 0;
        if (const auto result = vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDeviceHandle,
            surfaceHandle,
            &presentModeCount,
            nullptr); result != VkResult::VK_SUCCESS)
        {
            return result;
        }
        if (presentModeCount)
        {
            surfaceCababilities.presentModes.resize(presentModeCount);
            if (const auto result = vkGetPhysicalDeviceSurfacePresentModesKHR(
                physicalDeviceHandle,
                surfaceHandle,
                &presentModeCount,
                surfaceCababilities.presentModes.data()); result != VkResult::VK_SUCCESS)
            {
                return result;
            }
        }

        return {};
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif