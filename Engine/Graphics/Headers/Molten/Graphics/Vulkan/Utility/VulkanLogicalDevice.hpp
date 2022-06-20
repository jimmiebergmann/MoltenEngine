/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#ifndef MOLTEN_GRAPHICS_VULKAN_UTILITY_VULKANLOGICALDEVICE_HPP
#define MOLTEN_GRAPHICS_VULKAN_UTILITY_VULKANLOGICALDEVICE_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Graphics/Vulkan/Utility/VulkanDeviceQueues.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanLayer.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanExtension.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanResult.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class PhysicalDevice;

    class MOLTEN_GRAPHICS_API LogicalDevice
    {

    public:

        LogicalDevice();
        ~LogicalDevice();

        Result<> Create(
            PhysicalDevice& physicalDevice,
            const Layers& enabledInstanceLayers,
            const Extensions& enabledDeviceExtensions,
            const VkPhysicalDeviceFeatures& enabledDeviceFeatures);

        void Destroy();

        void WaitIdle();

        bool IsCreated() const;

        VkDevice& GetHandle();
        const VkDevice& GetHandle() const;

        const VkPhysicalDeviceFeatures& GetEnabledFeatures() const;

        DeviceQueues& GetDeviceQueues();
        const DeviceQueues& GetDeviceQueues() const;

        PhysicalDevice& GetPhysicalDevice();
        const PhysicalDevice& GetPhysicalDevice() const;

        bool HasPhysicalDevice() const;

    private:

        VkDevice m_handle;
        VkPhysicalDeviceFeatures m_enabledFeatures;
        DeviceQueues m_deviceQueues;
        PhysicalDevice* m_physicalDevice;

    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif