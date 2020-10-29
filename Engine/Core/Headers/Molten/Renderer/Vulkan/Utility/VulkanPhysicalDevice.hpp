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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANPHYSICALDEVICE_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANPHYSICALDEVICE_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDeviceCapabilities.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceQueues.hpp"
#include <vector>
#include <functional>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class Instance;

   /** Vulkan physical device.*/
   class MOLTEN_API PhysicalDevice
   {

   public:

       PhysicalDevice();

       Result<> Create(VkPhysicalDevice physicalDeviceHandle);

       VkPhysicalDevice& GetHandle();
       const VkPhysicalDevice& GetHandle() const;

       PhysicalDeviceCapabilities& GetCapabilities();
       const PhysicalDeviceCapabilities& GetCapabilities() const;

       DeviceQueues& GetDeviceQueues();
       const DeviceQueues& GetDeviceQueues() const;

   private:

       VkPhysicalDevice m_handle;
       PhysicalDeviceCapabilities m_capabilities;
       DeviceQueues m_deviceQueues;

   };

   using PhysicalDevices = std::vector<PhysicalDevice>;


   /** Filter for fetching and creating physical devices. */
   using PhysicalDeviceFilter = std::function<bool(const PhysicalDeviceCapabilities&)>;
   using PhysicalDeviceFilters = std::vector<PhysicalDeviceFilter>;

   /** Fetch and create physical devices from vulkan instance.
    * Provide filters to ignore certain physical devices.
    */
   MOLTEN_API Result<> FetchAndCreatePhysicalDevices(
       PhysicalDevices& physicalDevices,
       Instance& instance,
       PhysicalDeviceFilters filters = {});

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif