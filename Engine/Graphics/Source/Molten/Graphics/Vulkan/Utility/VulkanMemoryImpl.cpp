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

#include "Molten/Graphics/Vulkan/Utility/VulkanMemoryImpl.hpp"

namespace Molten::Vulkan
{
    // Memory implementations.
    Memory::Memory(
        MemoryBlock* memoryBlock,
        const VkDeviceSize size,
        const VkDeviceSize offset
    ) :
        isFree(false),
        memoryBlock(memoryBlock),
        size(size),
        offset(offset),
        prevMemory(nullptr),
        nextMemory{},
        freeIterator{}
    {}

    Memory::Memory(Memory&& memory) noexcept :
        isFree(memory.isFree),
        memoryBlock(memory.memoryBlock),
        size(memory.size),
        offset(memory.offset),
        prevMemory(memory.prevMemory),
        nextMemory(std::move(memory.nextMemory)),
        freeIterator(memory.freeIterator)
    {
        memory.isFree = false;
        memory.memoryBlock = nullptr;
        memory.size = 0;
        memory.offset = 0;
        memory.prevMemory = nullptr;
        memory.freeIterator = {};
    }

    Memory& Memory::operator = (Memory&& memory) noexcept
    {
        isFree = memory.isFree;
        memoryBlock = memory.memoryBlock;
        size = memory.size;
        offset = memory.offset;
        prevMemory = memory.prevMemory;
        nextMemory = std::move(memory.nextMemory);
        freeIterator = memory.freeIterator;

        memory.isFree = false;
        memory.memoryBlock = nullptr;
        memory.size = 0;
        memory.offset = 0;
        memory.prevMemory = nullptr;
        memory.freeIterator = {};

        return *this;
    }

}

#endif
