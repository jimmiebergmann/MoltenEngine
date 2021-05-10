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
        m_logicalDevice(nullptr),
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
        Logger* logger)
    {
        Destroy();

        m_logger = logger;

        m_logicalDevice = &logicalDevice;
        if(!m_logicalDevice->GetHandle())
        {
            return VkResult::VK_INCOMPLETE;
        }

        m_physicalDevice = &m_logicalDevice->GetPhysicalDevice();
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
        m_logicalDevice = nullptr;
    }

    Result<> MemoryAllocator::CreateDeviceBuffer(
        DeviceBuffer& deviceBuffer,
        const VkBufferCreateInfo& bufferCreateInfo,
        const uint32_t memoryProperties)
    {
        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        deviceBuffer.buffer = VK_NULL_HANDLE;
        deviceBuffer.memory = nullptr;

        DeviceBufferGuard deviceBufferGuard(*this, deviceBuffer);
        
        Result<> result;
        if (!(result = vkCreateBuffer(logicalDeviceHandle, &bufferCreateInfo, nullptr, &deviceBuffer.buffer)))
        {
            return result;
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(logicalDeviceHandle, deviceBuffer.buffer, &memoryRequirements);

        MemoryType* memoryType = nullptr;
        if (!FindSupportedMemoryType(
            memoryType,
            m_memoryTypes,
            memoryRequirements.memoryTypeBits,
            memoryProperties) || memoryType == nullptr)
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        if(!(result = GetOrCreateFreeMemoryHandle(
            deviceBuffer.memory,
            memoryType->index,
            memoryRequirements.size)))
        {
            return result;
        }

        if (!(result = vkBindBufferMemory(
            logicalDeviceHandle,
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
        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        deviceImage.image = VK_NULL_HANDLE;
        deviceImage.memory = nullptr;

        DeviceImageGuard deviceImageGuard(*this, deviceImage);

        Result<> result;
        if (!(result = vkCreateImage(logicalDeviceHandle, &imageCreateInfo, nullptr, &deviceImage.image)))
        {
            return result;
        }

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(logicalDeviceHandle, deviceImage.image, &memoryRequirements);

        MemoryType* memoryType = nullptr;
        if (!FindSupportedMemoryType(
            memoryType,
            m_memoryTypes,
            memoryRequirements.memoryTypeBits,
            memoryProperties) || memoryType == nullptr)
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        if (!(result = GetOrCreateFreeMemoryHandle(
            deviceImage.memory,
            memoryType->index,
            memoryRequirements.size)))
        {
            return result;
        }

        if (!(result = vkBindImageMemory(
            logicalDeviceHandle,
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
        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        if (deviceBuffer.buffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(logicalDeviceHandle, deviceBuffer.buffer, nullptr);
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
        auto logicalDeviceHandle = m_logicalDevice->GetHandle();

        if (deviceImage.image != VK_NULL_HANDLE)
        {
            vkDestroyImage(logicalDeviceHandle, deviceImage.image, nullptr);
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
    Result<> MemoryAllocator::GetOrCreateFreeMemoryHandle(
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
        if (auto result = CreateMemoryBlock(newMemoryBlock, memoryPool, pagedMemorySize); !result)
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
        if(memoryBlock.freeMemoryHandles.empty())
        {
            return false;
        }

        for(auto it = memoryBlock.freeMemoryHandles.begin(); it != memoryBlock.freeMemoryHandles.end(); ++it)
        {
            if(auto* freeMemoryHandle = *it; freeMemoryHandle->size >= pagedMemorySize)
            {
                memoryHandle = SplitMemoryHandle(freeMemoryHandle, pagedMemorySize);

                if(!memoryHandle)
                {
                    return false;
                }
           
                it = memoryBlock.freeMemoryHandles.erase(it);

                // No new handle, let's use that one.
                if(memoryHandle == freeMemoryHandle)
                {
                    memoryHandle->isFree = false;
                    return true;
                }

                // New handle, use old handle as fetched one and add the new one to free handles list.              
                memoryBlock.freeMemoryHandles.insert(it, memoryHandle);
                memoryHandle = freeMemoryHandle;
                memoryHandle->isFree = false;
                return true;
            }
        }

        return false;
    }

    MemoryHandle MemoryAllocator::SplitMemoryHandle(MemoryHandle memoryHandle, const VkDeviceSize offset)
    {
        if(offset == memoryHandle->size)
        {
            return memoryHandle;
        }
        if(offset > memoryHandle->size)
        {
            return nullptr;
        }

        const auto newSize = memoryHandle->size - offset;
        const auto newOffset = memoryHandle->offset + offset;
        auto* newMemoryHandle = new Memory(memoryHandle->memoryBlock, newSize, newOffset);
        newMemoryHandle->prevMemoryRange = memoryHandle;
        newMemoryHandle->nextMemoryRange = memoryHandle->nextMemoryRange;

        memoryHandle->size = offset;
        memoryHandle->nextMemoryRange = newMemoryHandle;

        return newMemoryHandle;
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

        auto newMemory = std::make_unique<Memory>(newMemoryBlock.get(), newMemoryBlock->size);

        auto logicalDeviceHandle = m_logicalDevice->GetHandle();
        Result<> result;
        if (!(result = vkAllocateMemory(logicalDeviceHandle, &memoryAllocateInfo, nullptr, &newMemoryBlock->deviceMemory)))
        {
            return result;
        }

        newMemoryBlock->firstMemoryHandle = newMemory.get();
        newMemoryBlock->freeMemoryHandles.push_back(newMemory.release());
        memoryBlock = newMemoryBlock.get();
  
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
        // TODO: ...
    }

    void MemoryAllocator::FreeMemoryBlocksInAllPools()
    {
        // TODO: ...

        /*for (size_t i = 0; i < m_memoryPools.size(); i++)
        {
            auto& [memoryBlocks, freeMemoryHandles] = m_memoryPools[i];

            if(memoryBlocks.size() != freeMemoryHandles.size())
            {
                Logger::WriteError(m_logger, "Number of memory blocks in pool " + std::to_string(i) + 
                    " is not equal to number of free memory handles. Make sure that all memory handles before destroying memory allocator.");
            }

            for(auto& memoryBlock : memoryBlocks)
            {
                FreeMemoryBlock(memoryBlock);
            }
        }*/
    }

}

#endif