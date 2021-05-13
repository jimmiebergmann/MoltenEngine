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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORYTYPE_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORYTYPE_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    struct MemoryType
    {
        uint32_t index; ///< Index of parent MemoryTypes.
        uint32_t physicalDeviceMemoryTypeIndex; ///< This is the index of this memory propery from the original VkPhysicalDeviceMemoryProperties object.    
        VkMemoryPropertyFlags propertyFlags; ///< Bitmask of supported memory properties for this memory type.
    };

    using MemoryTypes = std::vector<MemoryType>;


    MOLTEN_API void GetPhysicalDeviceMemoryTypes(
        MemoryTypes& memoryTypes,
        VkPhysicalDevice physicalDeviceHandle,
        const bool ignoreEmptyProperties = false);

    MOLTEN_API bool FindSupportedMemoryType(
        MemoryType*& foundMemoryType,
        MemoryTypes& availableMemoryTypes,
        const uint32_t memoryTypeBits,
        const VkMemoryPropertyFlags memoryProperties);

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
