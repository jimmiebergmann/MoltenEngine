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

#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDevice.hpp"
#include <set>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Logical device implementations.
    LogicalDevice::LogicalDevice() :
        m_handle(VK_NULL_HANDLE),
        m_enabledFeatures{},
        m_deviceQueues{},
        m_physicalDevice(nullptr)
    {}

    LogicalDevice::~LogicalDevice()
    {
        Destroy();
    }

    Result<> LogicalDevice::Create(
        PhysicalDevice& physicalDevice,
        const Layers& enabledInstanceLayers,
        const Extensions& enabledDeviceExtensions,
        const VkPhysicalDeviceFeatures& enabledDeviceFeatures)
    {
        Destroy();

        Result<> result;

        auto& deviceQueueIndices = physicalDevice.GetDeviceQueueIndices();

        if (!deviceQueueIndices.graphicsQueue.has_value() ||
            !deviceQueueIndices.presentQueue.has_value())
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        m_deviceQueues.graphicsQueueIndex = deviceQueueIndices.graphicsQueue.value();
        m_deviceQueues.presentQueueIndex = deviceQueueIndices.presentQueue.value();

        const std::set<uint32_t> uniqueFamilyIds = {
            m_deviceQueues.graphicsQueueIndex,
            m_deviceQueues.presentQueueIndex
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
        
        if (!(result = vkCreateDevice(physicalDevice.GetHandle(), &deviceInfo, nullptr, &m_handle)))
        {
            return result;
        }
        if (m_handle == VK_NULL_HANDLE)
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        vkGetDeviceQueue(m_handle, m_deviceQueues.graphicsQueueIndex, 0, &m_deviceQueues.graphicsQueue);
        vkGetDeviceQueue(m_handle, m_deviceQueues.presentQueueIndex, 0, &m_deviceQueues.presentQueue);
       
        m_physicalDevice = &physicalDevice;
        m_enabledFeatures = enabledDeviceFeatures;

        return result;
    }

    void LogicalDevice::Destroy()
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_handle, nullptr);
            m_handle = VK_NULL_HANDLE;
            m_physicalDevice = nullptr;
        }
    }

    void LogicalDevice::WaitIdle()
    {
        vkDeviceWaitIdle(m_handle);
    }

    bool LogicalDevice::IsCreated() const
    {
        return m_handle != VK_NULL_HANDLE;
    }

    VkDevice& LogicalDevice::GetHandle()
    {
        return m_handle;
    }
    const VkDevice& LogicalDevice::GetHandle() const
    {
        return m_handle;
    }

    const VkPhysicalDeviceFeatures& LogicalDevice::GetEnabledFeatures() const
    {
        return m_enabledFeatures;
    }

    DeviceQueues& LogicalDevice::GetDeviceQueues()
    {
        return m_deviceQueues;
    }
    const DeviceQueues& LogicalDevice::GetDeviceQueues() const
    {
        return m_deviceQueues;
    }

    PhysicalDevice& LogicalDevice::GetPhysicalDevice()
    {
        return *m_physicalDevice;
    }
    const PhysicalDevice& LogicalDevice::GetPhysicalDevice() const
    {
        return *m_physicalDevice;
    }

    bool LogicalDevice::HasPhysicalDevice() const
    {
        return m_physicalDevice != nullptr;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif