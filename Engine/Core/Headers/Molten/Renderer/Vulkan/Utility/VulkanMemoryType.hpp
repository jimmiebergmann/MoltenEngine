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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORYTYPE_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORYTYPE_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    /** Single memory type, returned as vector by FilterMemoryTypesByPropertyFlags. */
    struct MOLTEN_API FilteredMemoryType
    {
        FilteredMemoryType();
        FilteredMemoryType(
            const uint32_t index,
            const VkMemoryPropertyFlags propertyFlags);

        uint32_t index; ///< This is the index of this memory propery from the original VkPhysicalDeviceMemoryProperties object.
        VkMemoryPropertyFlags propertyFlags; // Bitmask of supported memory properies for this memory type.
    };

    using FilteredMemoryTypes = std::vector<FilteredMemoryType>;


    /** Filter provided physical device memory properies by filter flags.
      *  Returns vector of provided memory properties that contains all filter flags.
      */
    MOLTEN_API void FilterMemoryTypesByPropertyFlags(
        FilteredMemoryTypes& filteredMemorytypes,
        const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperies,
        const uint32_t propertyFlags);

    /** Find first index of device memory propery with required memory type flags. */
    MOLTEN_API bool FindFilteredMemoryTypeIndex(
        uint32_t& index,
        const FilteredMemoryTypes& filteredMemoryTypes,
        const uint32_t requiredMemoryTypeFlags);

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
