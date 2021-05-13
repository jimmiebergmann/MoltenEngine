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

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryAllocator.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryImpl.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceImage.hpp"
#include "Molten/Utility/SmartFunction.hpp"
#include "Molten/Logger.hpp"

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
        if(memoryBlock.freeMemories.empty())
        {
            return false;
        }

        for(auto it = memoryBlock.freeMemories.begin(); it != memoryBlock.freeMemories.end(); ++it)
        {
            if(auto* freeMemory = *it; freeMemory->size >= pagedMemorySize)
            {
                memoryHandle = SplitFreeMemory(it, pagedMemorySize);
                return memoryHandle != nullptr;
            }
        }

        return false;
    }

    void MemoryAllocator::TryMergeMemory(MemoryHandle& memoryHandle)
    {
        auto* memoryBlock = memoryHandle->memoryBlock;
        //std::array<Memory, 2> 
        MemoryHandle mergeFrom = memoryHandle;
        MemoryHandle mergeTo = memoryHandle;


        if (auto* prevMemory = memoryHandle->prevMemory)
        {
            if (prevMemory->isFree && prevMemory->offset + prevMemory->size == memoryHandle->offset)
            {
                mergeFrom = prevMemory;
            }
        }
        if (auto* nextMemory = memoryHandle->nextMemory.get())
        {
            if (nextMemory->isFree && memoryHandle->offset + memoryHandle->size == nextMemory->offset)
            {
                mergeTo = nextMemory;
            }
        }

        if(mergeFrom == mergeTo)
        {
            return;
        }

        const auto newSize = (mergeTo->offset + mergeTo->size) - mergeFrom->offset;
        auto newNextMemory = std::move(mergeTo->nextMemory);

        auto* currentMemory = mergeTo;
        while(currentMemory != mergeFrom)
        {
            memoryBlock->freeMemories.erase(currentMemory->freeIterator);
            currentMemory->nextMemory.reset();
            currentMemory = currentMemory->prevMemory;
        }

        mergeFrom->size = newSize;
        mergeFrom->nextMemory = std::move(newNextMemory);
        if(mergeFrom->nextMemory)
        {
            mergeFrom->nextMemory->prevMemory = mergeFrom;
        }
    }

    MemoryHandle MemoryAllocator::SplitFreeMemory(MemoryBlock::FeeMemoryList::iterator it, const VkDeviceSize size)
    {
        auto* memoryHandle = *it;
        auto* memoryBlock = memoryHandle->memoryBlock;

        if (size == memoryHandle->size)
        {
            MarkMemoryAsUsed(it);
            return memoryHandle;
        }
        if (size > memoryHandle->size)
        {
            return nullptr;
        }

        const auto newSize = memoryHandle->size - size;
        const auto newOffset = memoryHandle->offset + size;
        auto newMemory = std::make_unique<Memory>(memoryBlock, newSize, newOffset);
        MarkMemoryAsFree(newMemory.get());
        newMemory->prevMemory = memoryHandle;
        memoryBlock->freeMemories.erase(it);

        if (auto oldNextMemory = std::move(memoryHandle->nextMemory))
        {
            oldNextMemory->prevMemory = newMemory.get();
            newMemory->nextMemory = std::move(oldNextMemory);
        }

        memoryHandle->isFree = false;
        memoryHandle->freeIterator = {};
        memoryHandle->size = size;
        memoryHandle->nextMemory = std::move(newMemory);

        return memoryHandle;
    }

    void MemoryAllocator::MarkMemoryAsFree(MemoryHandle memoryHandle)
    {
        memoryHandle->isFree = true;

        auto* memoryBlock = memoryHandle->memoryBlock;
        memoryHandle->freeIterator = memoryBlock->freeMemories.insert(memoryBlock->freeMemories.end(), memoryHandle);
    }

    void MemoryAllocator::MarkMemoryAsUsed(MemoryBlock::FeeMemoryList::iterator& it)
    {
        auto* memoryHandle = *it;
        auto* memoryBlock = memoryHandle->memoryBlock;

        memoryHandle->isFree = false;
        memoryHandle->freeIterator = {};
        it = memoryBlock->freeMemories.erase(it);
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
        MarkMemoryAsFree(newMemoryBlock->firstMemory.get());

        memoryPool.memoryBlocks.push_back(std::move(newMemoryBlock));
          
        return result;
    }

    VkDeviceSize MemoryAllocator::GetPagedSize(VkDeviceSize size) const
    {
        return (((size - 1) / m_pageSize) + 1) * m_pageSize;
    }

    void MemoryAllocator::FreeMemoryHandle(MemoryHandle memoryHandle)
    {
        MarkMemoryAsFree(memoryHandle);
        TryMergeMemory(memoryHandle);
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
                    logMemoryBlockWarning(poolIndex, blockIndex, "First memory handle is missing, it's nullptr.");
                }
                else if (memoryBlock->freeMemories.size() != 1)
                {
                    logMemoryBlockWarning(poolIndex, blockIndex, "Number of free memory handles are not equal to 1, it's " +
                        std::to_string(memoryBlock->freeMemories.size()));
                }
                else if (const auto firstMemory = memoryBlock->freeMemories.front(); firstMemory->offset != 0)
                {
                    logMemoryBlockWarning(poolIndex, blockIndex, "Offset of free memory handle is not 0, it's " +
                        std::to_string(firstMemory->offset));
                }
                else if (firstMemory->size != memoryBlock->size)
                {
                    logMemoryBlockWarning(poolIndex, blockIndex, "Size of free memory is " + std::to_string(firstMemory->size) +
                        ", block's size is " + std::to_string(memoryBlock->size));
                }
                else if(memoryBlock->firstMemory.get() != memoryBlock->freeMemories.front())
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