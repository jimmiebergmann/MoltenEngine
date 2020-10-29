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


#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDevice.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanInstance.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Physical device implementations.
    PhysicalDevice::PhysicalDevice() :
        m_handle{ VK_NULL_HANDLE },
        m_capabilities{},
        m_deviceQueues{}
    {}

    /*Result<>*/ VkResult PhysicalDevice::Create(VkPhysicalDevice physicalDeviceHandle)
    {
        return VkResult::VK_ERROR_FRAGMENTATION;
    }

    VkPhysicalDevice& PhysicalDevice::GetHandle()
    {
        return m_handle;
    }
    const VkPhysicalDevice& PhysicalDevice::GetHandle() const
    {
        return m_handle;
    }

    PhysicalDeviceCapabilities& PhysicalDevice::GetCapabilities()
    {
        return m_capabilities;
    }
    const PhysicalDeviceCapabilities& PhysicalDevice::GetCapabilities() const
    {
        return m_capabilities;
    }

    DeviceQueues& PhysicalDevice::GetDeviceQueues()
    {
        return m_deviceQueues;
    }
    const DeviceQueues& PhysicalDevice::GetDeviceQueues() const
    {
        return m_deviceQueues;
    }

    // Static functions.
    /*Result<>*/VkResult FetchAndCreatePhysicalDevices(
        PhysicalDevices& physicalDevices,
        Instance& instance,
        PhysicalDeviceFilters filters)
    {
        physicalDevices.clear();

        auto& instanceHandle = instance.GetHandle();

        uint32_t handleCount = 0;
        VkResult result = VkResult::VK_SUCCESS;
        if ((result = vkEnumeratePhysicalDevices(instanceHandle, &handleCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        if (!handleCount)
        {
            return VkResult::VK_SUCCESS;
        }

        std::vector<VkPhysicalDevice> handles(handleCount, VK_NULL_HANDLE);
        if ((result = vkEnumeratePhysicalDevices(instanceHandle, &handleCount, handles.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        auto checkFilters = [&](const PhysicalDeviceCapabilities& capabilities) -> bool
        {
            for (auto& filter : filters)
            {
                if (!filter(capabilities))
                {
                    return false;
                }
            }
            return true;
        };

        physicalDevices.resize(handles.size());
        auto it = physicalDevices.begin();

        for(auto& handle : handles)
        {
            auto& physicalDevice = *it;

            bool createSucceeded = physicalDevice.Create(handle);

            if (!createSucceeded || !checkFilters(physicalDevice.GetCapabilities()))
            {
                it = physicalDevices.erase(it);
                break;
            }
            else
            {
                ++it;
            }
        }

        return VkResult::VK_SUCCESS;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif