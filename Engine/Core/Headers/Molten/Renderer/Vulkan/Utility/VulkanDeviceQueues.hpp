/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANDEVICEQUEUE_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANDEVICEQUEUE_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"
#include <optional>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class Surface;

    struct MOLTEN_API DeviceQueueIndices
    {
        DeviceQueueIndices();

        std::optional<uint32_t> graphicsQueue;
        std::optional<uint32_t> presentQueue;

    };

    struct MOLTEN_API DeviceQueues
    {
        DeviceQueues();

        VkQueue graphicsQueue;
        VkQueue presentQueue;
        uint32_t graphicsQueueIndex;
        uint32_t presentQueueIndex;
    };


    MOLTEN_API void FetchQueueFamilyProperties(
        QueueFamilyProperties& queueFamilyProperties,
        VkPhysicalDevice physicalDevice);


    MOLTEN_API bool FindRenderableDeviceQueueIndices(
        DeviceQueueIndices& queueIndices,
        VkPhysicalDevice physicalDevice,
        const VkSurfaceKHR surface,
        const QueueFamilyProperties& queueFamilies);

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif