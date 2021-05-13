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

#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryBlock.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryImpl.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    MemoryBlock::MemoryBlock(const VkDeviceSize size) :
        deviceMemory(VK_NULL_HANDLE),
        size(size),
        firstMemory{},
        firstFreeMemory(nullptr),
        lastFreeMemory(nullptr)
    {}

    MemoryBlock::~MemoryBlock()
    {
        // Need this in order to use forward declared Memory for std::unique_ptr<>.
    }

    MemoryBlock::MemoryBlock(MemoryBlock&& memoryBlock) noexcept :
        deviceMemory(memoryBlock.deviceMemory),
        size(memoryBlock.size),
        firstMemory(std::move(memoryBlock.firstMemory)),
        firstFreeMemory(memoryBlock.firstFreeMemory),
        lastFreeMemory(memoryBlock.lastFreeMemory)
    {
        memoryBlock.deviceMemory = VK_NULL_HANDLE;
        memoryBlock.size = 0;
        memoryBlock.firstFreeMemory = nullptr;
        memoryBlock.lastFreeMemory = nullptr;
    }

    MemoryBlock& MemoryBlock::operator = (MemoryBlock&& memoryBlock) noexcept
    {
        deviceMemory = memoryBlock.deviceMemory;
        size = memoryBlock.size;
        firstMemory = std::move(memoryBlock.firstMemory);
        firstFreeMemory = memoryBlock.firstFreeMemory;
        lastFreeMemory = memoryBlock.lastFreeMemory;

        memoryBlock.deviceMemory = VK_NULL_HANDLE;
        memoryBlock.size = 0;
        memoryBlock.firstFreeMemory = nullptr;
        memoryBlock.lastFreeMemory = nullptr;

        return *this;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif