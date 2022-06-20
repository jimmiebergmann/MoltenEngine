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

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Graphics/Vulkan/Utility/VulkanMemoryType.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    void GetPhysicalDeviceMemoryTypes(
        MemoryTypes& memoryTypes,
        VkPhysicalDevice physicalDeviceHandle,
        const bool ignoreEmptyProperties)
    {
        memoryTypes.clear();

        VkPhysicalDeviceMemoryProperties memoryProperies;
        vkGetPhysicalDeviceMemoryProperties(physicalDeviceHandle, &memoryProperies);

        uint32_t index = 0;
        for (uint32_t i = 0; i < memoryProperies.memoryTypeCount; i++)
        {
            if (ignoreEmptyProperties && memoryProperies.memoryTypes[i].propertyFlags == 0)
            {
                continue;
            }

            memoryTypes.push_back({ index++, i, memoryProperies.memoryTypes[i].propertyFlags });
        }
    }

    bool FindSupportedMemoryType(
        MemoryType*& foundMemoryType,
        MemoryTypes& availableMemoryTypes,
        const uint32_t memoryTypeBits,
        const VkMemoryPropertyFlags memoryProperties)
    {
        for (auto& availableMemoryType : availableMemoryTypes)
        {
            if ((memoryTypeBits & (1 << availableMemoryType.physicalDeviceMemoryTypeIndex)) && 
                (availableMemoryType.propertyFlags & memoryProperties) == memoryProperties)
            {
                foundMemoryType = &availableMemoryType;
                return true;
            }
        }

        return false;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif