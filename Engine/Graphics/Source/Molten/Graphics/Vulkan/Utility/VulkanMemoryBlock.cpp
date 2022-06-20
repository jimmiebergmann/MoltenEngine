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

#include "Molten/Graphics/Vulkan/Utility/VulkanMemoryBlock.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanMemoryImpl.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    MemoryBlock::MemoryBlock(const VkDeviceSize size) :
        deviceMemory(VK_NULL_HANDLE),
        size(size),
        firstMemory{},
        freeMemories{}
    {}

    MemoryBlock::~MemoryBlock()
    {
        for (auto memory = std::move(firstMemory); memory;)
        {
            memory = std::move(memory->nextMemory);
        }
    }

    MemoryBlock::MemoryBlock(MemoryBlock&& memoryBlock) noexcept :
        deviceMemory(memoryBlock.deviceMemory),
        size(memoryBlock.size),
        firstMemory(std::move(memoryBlock.firstMemory)),
        freeMemories(std::move(memoryBlock.freeMemories))
    {
        memoryBlock.deviceMemory = VK_NULL_HANDLE;
        memoryBlock.size = 0;
    }

    MemoryBlock& MemoryBlock::operator = (MemoryBlock&& memoryBlock) noexcept
    {
        deviceMemory = memoryBlock.deviceMemory;
        size = memoryBlock.size;
        firstMemory = std::move(memoryBlock.firstMemory);

        memoryBlock.deviceMemory = VK_NULL_HANDLE;
        memoryBlock.size = 0;

        return *this;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif