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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANPHYSICALDEVICEFEATURES_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANPHYSICALDEVICEFEATURES_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanHeaders.hpp"
#include "Molten/Math/Vector.hpp"
#include <string>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    using PhysicalDeviceFeaturePointer = VkBool32 (VkPhysicalDeviceFeatures::*);
    using PhysicalDeviceFeaturePointers = std::vector<PhysicalDeviceFeaturePointer>;

    struct MOLTEN_API PhysicalDeviceFeatureWithName
    {
        PhysicalDeviceFeatureWithName();
        PhysicalDeviceFeatureWithName(
            PhysicalDeviceFeaturePointer pointer,
            const std::string& name);

        PhysicalDeviceFeaturePointer pointer;
        const std::string& name;
    };

    using PhysicalDeviceFeaturesWithName = std::vector<PhysicalDeviceFeatureWithName>;


    /** Enabled features in finalFeatures from feature pointer vector. */
    MOLTEN_API void EnablePhysicalDeviceFeatures(
        VkPhysicalDeviceFeatures& finalFeatures,
        const PhysicalDeviceFeaturePointers enableFeatures);

    /** Apply optional features to finalFeatures, if optional feature is present in availableFeatures. */
    MOLTEN_API void EnableOptionalPhysicalDeviceFeatures(
        VkPhysicalDeviceFeatures& finalFeatures,
        const VkPhysicalDeviceFeatures& availableFeatures,
        const PhysicalDeviceFeaturePointers optionalFeatures);

    /** Check if all requiredFeatures are present in availablefeatures. */
    MOLTEN_API bool CheckRequiredPhysicalDeviceFeatures(
        PhysicalDeviceFeaturesWithName& missingFeatures,
        const VkPhysicalDeviceFeatures& availableFeatures,
        const PhysicalDeviceFeaturePointers requiredFeatures);

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
