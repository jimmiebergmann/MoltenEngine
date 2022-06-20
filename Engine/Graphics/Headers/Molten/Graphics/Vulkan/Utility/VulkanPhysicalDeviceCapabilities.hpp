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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANPHYSICALDEVICECAPABILITIES_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANPHYSICALDEVICECAPABILITIES_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanExtension.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class Surface;
  

    /** Vulkan physical device.*/
    struct MOLTEN_API PhysicalDeviceSurfaceCapabilities
    {
        PhysicalDeviceSurfaceCapabilities();

        VkSurfaceCapabilitiesKHR capabilities;
        SurfaceFormats formats;
        PresentModes presentModes;

    };

    using PhysicalDeviceSurfacesCapabilities = std::vector<PhysicalDeviceSurfaceCapabilities>;

    /** Vulkan physical device.*/
    struct MOLTEN_API PhysicalDeviceCapabilities
    {
        PhysicalDeviceCapabilities();

        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        Extensions extensions;
        VkBool32 hasPresentSupport;
        PhysicalDeviceSurfaceCapabilities surfaceCapabilities;
        QueueFamilyProperties queueFamilies;

    };


    /** Common function for fetching capabilities of physical device by handle and surface. 
     * THis function also fetches surface capabilities.
     */
    MOLTEN_API Result<> FetchPhysicalDeviceCapabilities(
        PhysicalDeviceCapabilities& capabilities,
        const VkPhysicalDevice physicalDeviceHandle,
        const VkSurfaceKHR surfaceHandle);

    /** Common function for fetching surface capabilities of physical device by handle and surface. */
    MOLTEN_API Result<> FetchPhysicalDeviceSurfaceCapabilities(
        PhysicalDeviceSurfaceCapabilities& surfaceCababilities,
        const VkPhysicalDevice physicalDeviceHandle,
        const VkSurfaceKHR surfaceHandle);

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif