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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORYBLOCK_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORYBLOCK_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanMemory.hpp"
#include <memory>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    /** Memory block struct, used by memory allocator. */
    struct MOLTEN_API MemoryBlock
    {
        explicit MemoryBlock(const VkDeviceSize size);

        ~MemoryBlock(); 

        MemoryBlock(MemoryBlock&& memoryBlock) noexcept;
        MemoryBlock& operator = (MemoryBlock&& memoryBlock) noexcept;

        MemoryBlock(const MemoryBlock&) = delete;
        MemoryBlock& operator = (const MemoryBlock&) = delete;
        
        VkDeviceMemory deviceMemory;
        VkDeviceSize size;
        std::unique_ptr<Memory> firstMemory;
        Memory* firstFreeMemory;
        Memory* lastFreeMemory;
    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif