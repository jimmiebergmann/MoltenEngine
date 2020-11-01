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

#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryType.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Filtered memory type implementations.
    FilteredMemoryType::FilteredMemoryType() :
        index(0),
        propertyFlags(0)
    {}

    FilteredMemoryType::FilteredMemoryType(
        const uint32_t index,
        const VkMemoryPropertyFlags propertyFlags
    ) :
        index(index),
        propertyFlags(propertyFlags)
    {}


    // Static implementations.
    void FilterMemoryTypesByPropertyFlags(
        FilteredMemoryTypes& filteredMemorytypes,
        const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperies,
        const uint32_t propertyFlags)
    {
        filteredMemorytypes.clear();

        for (uint32_t i = 0; i < physicalDeviceMemoryProperies.memoryTypeCount; i++)
        {
            auto& memoryType = physicalDeviceMemoryProperies.memoryTypes[i];
            if ((memoryType.propertyFlags & propertyFlags) == propertyFlags)
            {
                filteredMemorytypes.push_back(FilteredMemoryType{ i, memoryType.propertyFlags });
            }
        }
    }

    bool FindFilteredMemoryTypeIndex(
        uint32_t& index,
        const FilteredMemoryTypes& filteredMemoryTypes,
        const uint32_t requiredMemoryTypeFlags)
    {
        for (auto& filteredMemoryType : filteredMemoryTypes)
        {
            if (requiredMemoryTypeFlags & (1 << filteredMemoryType.index))
            {
                index = filteredMemoryType.index;
                return true;
            }
        }

        return false;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif