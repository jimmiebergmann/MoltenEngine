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

#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceQueues.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Device queue indices implemenetations.
    DeviceQueueIndices::DeviceQueueIndices() :
        graphicsQueue{},
        presentQueue{}
    {}


    // Device queues implementations.
    DeviceQueues::DeviceQueues() :
        graphicsQueue(VK_NULL_HANDLE),
        presentQueue(VK_NULL_HANDLE),
        graphicsQueueIndex(0),
        presentQueueIndex(0)
    {}


    // Static implementations.
    void FetchQueueFamilyProperties(
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


    bool FindRenderableDeviceQueueIndices(
        DeviceQueueIndices& queueIndices,
        VkPhysicalDevice physicalDevice,
        const VkSurfaceKHR surface,
        const QueueFamilyProperties& queueFamilies)
    {
        queueIndices.graphicsQueue.reset();
        queueIndices.presentQueue.reset();

        uint32_t finalGraphicsQueueIndex = 0;
        uint32_t finalPresentQueueIndex = 0;

        bool graphicsFamilySupport = false;
        bool presentFamilySupport = false;

        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            auto& queueFamily = queueFamilies[i];

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
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

        queueIndices.graphicsQueue = finalGraphicsQueueIndex;
        queueIndices.presentQueue = finalPresentQueueIndex;
        return true;
    }


}

MOLTEN_UNSCOPED_ENUM_END

#endif