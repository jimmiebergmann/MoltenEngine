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

#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryAllocator.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryImpl.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceImage.hpp"
#include "Molten/Utility/SmartFunction.hpp"
#include "Molten/Logger.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

namespace Molten::Vulkan
{

    // Vulkan memory allocator implementations.
    MemoryAllocator::MemoryAllocator() :
        m_logger(nullptr),
        m_logicalDeviceHandle(VK_NULL_HANDLE),
        m_physicalDevice(nullptr),
        m_pageSize(0),
        m_blockAllocationSize(0)
    {}

    MemoryAllocator::~MemoryAllocator()
    {
        Destroy();
    }

    Result<> MemoryAllocator::Load(
        LogicalDevice& logicalDevice,
        VkDeviceSize blockAllocationSize,
        Molten::Logger* logger)
    {
        Destroy();

        m_logger = logger;

        m_logicalDeviceHandle = logicalDevice.GetHandle();
        if(!m_logicalDeviceHandle)
        {
            return VkResult::VK_INCOMPLETE;
        }

        m_physicalDevice = &logicalDevice.GetPhysicalDevice();
        auto& physicalDeviceHandle = m_physicalDevice->GetHandle();
        if (!physicalDeviceHandle)
        {
            return VkResult::VK_INCOMPLETE;
        }
       
        GetPhysicalDeviceMemoryTypes(m_memoryTypes, physicalDeviceHandle, true);

        m_pageSize = m_physicalDevice->GetCapabilities().properties.limits.bufferImageGranularity;
        m_blockAllocationSize = GetPagedSize(blockAllocationSize);

        m_memoryPools.reserve(m_memoryTypes.size());
        for(size_t i = 0; i < m_memoryTypes.size(); i++)
        {
            m_memoryPools.emplace_back(m_memoryTypes[i].physicalDeviceMemoryTypeIndex);
        }

        return VkResult::VK_SUCCESS;
    }
 
    void MemoryAllocator::Destroy()
    {
        FreeMemoryBlocksInAllPools();
        m_memoryPools.clear();
        m_memoryTypes.clear();
        m_logicalDeviceHandle = VK_NULL_HANDLE;
    }

    Result<> MemoryAllocator::CreateDeviceBuffer(
        DeviceBuffer& deviceBuffer,
        const VkBufferCreateInfo& bufferCreateInfo,
        const VkMemoryPropertyFlags memoryProperties)
    {
        deviceBuffer.buffer = VK_NULL_HANDLE;
        deviceBuffer.memory = nullptr;

        DeviceBufferGuard deviceBufferGuard(*this, deviceBuffer);
        
        Result<> result;
        if (!(result = vkCreateBuffer(m_logicalDeviceHandle, &bufferCreateInfo, nullptr, &deviceBuffer.buffer)))
        {
            return result;
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_logicalDeviceHandle, deviceBuffer.buffer, &memoryRequirements);

        MemoryType* memoryType = nullptr;
        if (!FindSupportedMemoryType(
            memoryType,
            m_memoryTypes,
            memoryRequirements.memoryTypeBits,
            memoryProperties) || memoryType == nullptr)
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        if(!(result = GetOrCreateMemory(
            deviceBuffer.memory,
            memoryType->index,
            memoryRequirements.size)))
        {
            return result;
        }

        if (!(result = vkBindBufferMemory(
            m_logicalDeviceHandle,
            deviceBuffer.buffer,
            deviceBuffer.memory->memoryBlock->deviceMemory,
            deviceBuffer.memory->offset)))
        {
            return result;
        }

        deviceBufferGuard.Release();
        return result;
    }

    Result<> MemoryAllocator::CreateDeviceBuffer(
        DeviceBuffer& deviceBuffer,
        const VkDeviceSize size,
        const VkBufferUsageFlags usage,
        const VkMemoryPropertyFlags memoryProperties)
    {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        return CreateDeviceBuffer(deviceBuffer, bufferCreateInfo, memoryProperties);
    }

    Result<> MemoryAllocator::CreateDeviceImage(
        DeviceImage& deviceImage,
        const VkImageCreateInfo& imageCreateInfo,
        const VkMemoryPropertyFlags memoryProperties)
    {
        deviceImage.image = VK_NULL_HANDLE;
        deviceImage.memory = nullptr;

        DeviceImageGuard deviceImageGuard(*this, deviceImage);

        Result<> result;
        if (!(result = vkCreateImage(m_logicalDeviceHandle, &imageCreateInfo, nullptr, &deviceImage.image)))
        {
            return result;
        }

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(m_logicalDeviceHandle, deviceImage.image, &memoryRequirements);

        MemoryType* memoryType = nullptr;
        if (!FindSupportedMemoryType(
            memoryType,
            m_memoryTypes,
            memoryRequirements.memoryTypeBits,
            memoryProperties) || memoryType == nullptr)
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        if (!(result = GetOrCreateMemory(
            deviceImage.memory,
            memoryType->index,
            memoryRequirements.size)))
        {
            return result;
        }

        if (!(result = vkBindImageMemory(
            m_logicalDeviceHandle,
            deviceImage.image,
            deviceImage.memory->memoryBlock->deviceMemory,
            deviceImage.memory->offset)))
        {
            return result;
        }

        deviceImageGuard.Release();

        return result;
    }

    void MemoryAllocator::FreeDeviceBuffer(DeviceBuffer& deviceBuffer)
    {
        if (deviceBuffer.buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(m_logicalDeviceHandle, deviceBuffer.buffer, nullptr);
            deviceBuffer.buffer = VK_NULL_HANDLE;
        }
        if(deviceBuffer.memory)
        {
            FreeMemoryHandle(deviceBuffer.memory);
            deviceBuffer.memory = nullptr;
        }
    }

    void MemoryAllocator::FreeDeviceImage(DeviceImage& deviceImage)
    {
        if (deviceImage.image != VK_NULL_HANDLE)
        {
            vkDestroyImage(m_logicalDeviceHandle, deviceImage.image, nullptr);
            deviceImage.image = VK_NULL_HANDLE;
        }
        if (deviceImage.memory)
        {
            FreeMemoryHandle(deviceImage.memory);
            deviceImage.memory = nullptr;
        }
    }


    // Memory pool of memory allocator implemenations.
    MemoryAllocator::MemoryPool::MemoryPool(uint32_t physicalDeviceMemoryTypeIndex) :
        physicalDeviceMemoryTypeIndex(physicalDeviceMemoryTypeIndex),
        memoryBlocks{}
    {}

    MemoryAllocator::MemoryPool::MemoryPool(MemoryPool && memoryPool) noexcept :
        physicalDeviceMemoryTypeIndex(memoryPool.physicalDeviceMemoryTypeIndex),
        memoryBlocks(std::move(memoryPool.memoryBlocks))
    {}

    MemoryAllocator::MemoryPool& MemoryAllocator::MemoryPool::operator = (MemoryPool && memoryPool) noexcept
    {
        memoryBlocks = std::move(memoryPool.memoryBlocks);
        return *this;
    }


    // Private memory allocator implementations.
    Result<> MemoryAllocator::GetOrCreateMemory(
        MemoryHandle& memoryHandle,
        const uint32_t memoryTypeIndex,
        const VkDeviceSize memorySize)
    {      
        auto& memoryPool = m_memoryPools[memoryTypeIndex];

        const auto pagedMemorySize = GetPagedSize(memorySize);

        for (auto& memoryBlock : memoryPool.memoryBlocks)
        {
            if(FetchFreeMemoryFromBlock(memoryHandle, *memoryBlock, pagedMemorySize))
            {
                return VkResult::VK_SUCCESS;
            }
        }

        MemoryBlock* newMemoryBlock = nullptr;
        if (const auto result = CreateMemoryBlock(newMemoryBlock, memoryPool, pagedMemorySize); !result)
        {
            return result;
        }

        if (FetchFreeMemoryFromBlock(memoryHandle, *newMemoryBlock, pagedMemorySize))
        {
            return VkResult::VK_SUCCESS;
        }

        return VkResult::VK_ERROR_UNKNOWN;
    }

    bool MemoryAllocator::FetchFreeMemoryFromBlock(
        MemoryHandle& memoryHandle,
        MemoryBlock& memoryBlock,
        const VkDeviceSize pagedMemorySize)
    {
        if(!memoryBlock.firstFreeMemory)
        {
            return false;
        }

        auto* freeMemory = memoryBlock.firstFreeMemory;
        while(freeMemory)
        {
            if(freeMemory->size >= pagedMemorySize)
            {
                if(SplitAndFetchMemory(freeMemory, pagedMemorySize))
                {
                    memoryHandle = freeMemory;
                    return true;
                }
                return false;
            }
        }

        return false;
    }

    bool MemoryAllocator::SplitAndFetchMemory(MemoryHandle memoryHandle, const VkDeviceSize size)
    {
        if(size == memoryHandle->size)
        {
            MarkMemoryAsUsed(memoryHandle);
            return true;
        }
        if(size > memoryHandle->size)
        {
            return false;
        }

        auto oldNextMemory = std::move(memoryHandle->nextMemory);
        
        const auto newSize = memoryHandle->size - size;
        const auto newOffset = memoryHandle->offset + size;
        auto newMemory = std::make_unique<Memory>(memoryHandle->memoryBlock, newSize, newOffset);
        auto* newMemoryPtr = newMemory.get();

        newMemory->prevMemory = memoryHandle;
        memoryHandle->size = size;
        memoryHandle->nextMemory = std::move(newMemory);

        if(oldNextMemory)
        {
            oldNextMemory->prevMemory = newMemoryPtr;
            newMemoryPtr->nextMemory = std::move(oldNextMemory);
        }

        ReplaceFreeMemory(memoryHandle, newMemoryPtr);

        return true;
    }

    void MemoryAllocator::MarkMemoryAsUsed(MemoryHandle memoryHandle)
    {
        auto* memoryBlock = memoryHandle->memoryBlock;

        auto* prevFreeMemory = memoryHandle->prevFreeMemory;
        auto* nextFreeMemory = memoryHandle->nextFreeMemory;
        

        if (prevFreeMemory)
        {
            prevFreeMemory->nextFreeMemory = nextFreeMemory;
        }
        else
        {
            memoryBlock->firstFreeMemory = nextFreeMemory;
        }

        if (nextFreeMemory)
        {
            nextFreeMemory->prevFreeMemory = prevFreeMemory;     
        }
        else
        {
            memoryBlock->lastFreeMemory = prevFreeMemory;
        }

        memoryHandle->isFree = false;
        memoryHandle->prevFreeMemory = nullptr;
        memoryHandle->nextFreeMemory = nullptr;
    }

    void MemoryAllocator::ReplaceFreeMemory(MemoryHandle oldMemoryHandle, MemoryHandle newMemoryHandle)
    {
        auto* memoryBlock = oldMemoryHandle->memoryBlock;
        if (memoryBlock->firstFreeMemory == oldMemoryHandle)
        {
            memoryBlock->firstFreeMemory = newMemoryHandle;
        }
        if (memoryBlock->lastFreeMemory == oldMemoryHandle)
        {
            memoryBlock->lastFreeMemory = newMemoryHandle;
        }

        auto* prevFreeMemory = oldMemoryHandle->prevFreeMemory;
        auto* nextFreeMemory = oldMemoryHandle->nextFreeMemory;

        if (prevFreeMemory)
        {
            prevFreeMemory->nextFreeMemory = newMemoryHandle;
        }
        if (nextFreeMemory)
        {
            nextFreeMemory->prevFreeMemory = newMemoryHandle;
        }

        newMemoryHandle->prevFreeMemory = oldMemoryHandle->prevFreeMemory;
        newMemoryHandle->nextFreeMemory = oldMemoryHandle->nextFreeMemory;

        oldMemoryHandle->prevFreeMemory = nullptr;
        oldMemoryHandle->nextFreeMemory = nullptr;
        oldMemoryHandle->isFree = false;
    }

    Result<> MemoryAllocator::CreateMemoryBlock(
        MemoryBlock*& memoryBlock,
        MemoryPool& memoryPool,
        const VkDeviceSize pagedMemorySize)
    {
        memoryBlock = nullptr;

        auto newMemoryBlock = std::make_unique<MemoryBlock>(std::max(m_blockAllocationSize, pagedMemorySize));

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = newMemoryBlock->size;
        memoryAllocateInfo.memoryTypeIndex = memoryPool.physicalDeviceMemoryTypeIndex;         

        Result<> result;
        if (!(result = vkAllocateMemory(m_logicalDeviceHandle, &memoryAllocateInfo, nullptr, &newMemoryBlock->deviceMemory)))
        {
            return result;
        }

        memoryBlock = newMemoryBlock.get();

        newMemoryBlock->firstMemory = std::make_unique<Memory>(newMemoryBlock.get(), newMemoryBlock->size);
        newMemoryBlock->firstFreeMemory = newMemoryBlock->firstMemory.get();
        newMemoryBlock->lastFreeMemory = newMemoryBlock->firstMemory.get();
 
        memoryPool.memoryBlocks.push_back(std::move(newMemoryBlock));
          
        return result;
    }

    VkDeviceSize MemoryAllocator::GetPagedSize(VkDeviceSize size) const
    {
        return (((size - 1) / m_pageSize) + 1) * m_pageSize;
    }

    void MemoryAllocator::FreeMemoryHandle(MemoryHandle memoryHandle)
    {
        // TODO: ...
    }

    void MemoryAllocator::FreeMemoryBlock(MemoryBlock& memoryBlock)
    {
        vkFreeMemory(m_logicalDeviceHandle, memoryBlock.deviceMemory, nullptr);
    }

    void MemoryAllocator::FreeMemoryBlocksInAllPools()
    {
        if(!m_logicalDeviceHandle)
        {
            return;
        }

        auto logMemoryBlockWarning = [&](const size_t poolIndex, const size_t blockIndex, const std::string& message)
        {
            Logger::WriteWarning(m_logger, "Memory allocator free - Pool(" + std::to_string(poolIndex) + ") block(" +
                std::to_string(blockIndex) + "): " + message);
        };

        for (size_t poolIndex = 0; poolIndex < m_memoryPools.size(); poolIndex++)
        {
            auto& memoryPool = m_memoryPools[poolIndex];

            size_t blockIndex = 0;
            for(auto& memoryBlock : memoryPool.memoryBlocks)
            {
                if (memoryBlock->firstMemory == nullptr)
                {
                    logMemoryBlockWarning(poolIndex, blockIndex, "First memory handle is missing, it's nullptr..");
                }
                else if(memoryBlock->firstMemory.get() != memoryBlock->firstFreeMemory)
                {
                    logMemoryBlockWarning(poolIndex, blockIndex, "First memory handle is not equal to first free memory block.");
                }
                else if(memoryBlock->firstMemory->nextMemory != nullptr)
                {
                    logMemoryBlockWarning(poolIndex, blockIndex, "Found more than one memory handle in block.");
                }

                FreeMemoryBlock(*memoryBlock);

                ++blockIndex;
            }
        }

        m_memoryPools.clear();
    }

}

#endif