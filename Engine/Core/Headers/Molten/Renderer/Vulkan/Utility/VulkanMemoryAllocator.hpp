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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORYALLOCATOR_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANMEMORYALLOCATOR_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemory.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryType.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryBlock.hpp"
#include <vector>
#include <list>
#include <memory>

namespace Molten
{
    class Logger;
}

namespace Molten::Vulkan
{

    /** Forward declarations. */
    struct DeviceBuffer;
    struct DeviceImage;


    /** Memory allocator for logical device. */
    class MOLTEN_API MemoryAllocator
    {

    public:

        /** Default constructor. */
        MemoryAllocator();

        /** Destructor. Frees all allocated memory blocks. */
        ~MemoryAllocator();

        /** Deleted copy, move constructors and assignment operators. */
        /**@{*/
        MemoryAllocator(const MemoryAllocator&) = delete;
        MemoryAllocator(MemoryAllocator&&) = delete;
        MemoryAllocator& operator = (const MemoryAllocator&) = delete;
        MemoryAllocator& operator = (MemoryAllocator&&) = delete;
        /**@}*/

        /** Load memory allocator.*/
        Result<> Load(
            LogicalDevice& logicalDevice,
            VkDeviceSize blockAllocationSize,
            Logger* logger = nullptr);

        /** Destroys memory allocator and frees all allocations.
         * Allocator must be loaded again after a call to Destroy() before re-use.
         */
        void Destroy();

        /** Creates buffer by create info, allocates memory and binds it to the created buffer. */
        Result<> CreateDeviceBuffer(
            DeviceBuffer& deviceBuffer,
            const VkBufferCreateInfo& bufferCreateInfo,
            const VkMemoryPropertyFlags memoryProperties);

        /** Creates buffer by size and usage, allocates memory and binds it to the created buffer. */
        Result<> CreateDeviceBuffer(
            DeviceBuffer& deviceBuffer,
            const VkDeviceSize size,
            const VkBufferUsageFlags usage,
            const VkMemoryPropertyFlags memoryProperties);


        /** Creates image by create info, allocates memory and binds it to the created image. */
        Result<> CreateDeviceImage(
            DeviceImage& deviceImage,
            const VkImageCreateInfo& imageCreateInfo,
            const VkMemoryPropertyFlags memoryProperties);

        /** Creates image by size, allocates memory and binds it to the created image. */
        /*Result<> CreateDeviceImage(
            DeviceImage& deviceImage,
            const VkImageCreateInfo& imageCreateInfo,
            const VkMemoryPropertyFlags memoryProperties);*/

        /** Free device buffer. */
        void FreeDeviceBuffer(DeviceBuffer& deviceBuffer);

        /** Free device image. */
        void FreeDeviceImage(DeviceImage& deviceImage);

    private:

        struct MemoryPool
        {
            explicit MemoryPool(uint32_t physicalDeviceMemoryTypeIndex);
            ~MemoryPool() = default;

            MemoryPool(MemoryPool&& memoryPool) noexcept;
            MemoryPool& operator = (MemoryPool&& memoryPool) noexcept;

            MemoryPool(const MemoryPool&) = delete;
            MemoryPool& operator = (const MemoryPool&) = delete;

            const uint32_t physicalDeviceMemoryTypeIndex;
            std::list<std::unique_ptr<MemoryBlock>> memoryBlocks;
        };

        Result<> GetOrCreateFreeMemoryHandle(
            MemoryHandle& memoryHandle,
            const uint32_t memoryTypeIndex,
            const VkDeviceSize memorySize);

        /** Returns false if no free memory of size equal or larger than pagedMemorySize, else true. */
        bool FetchFreeMemoryFromBlock(
            MemoryHandle& memoryHandle,
            MemoryBlock& memoryBlock,
            const VkDeviceSize pagedMemorySize);

        /** Slips provided memory handle by an offset.
         *  Returns new memory handle if offset is less than provided memory handle size.
         *  Returns provided memory handle if its size is equal to offset.
         *  Return nullptr if offset is larger than provided memory handle size.
         */
        static [[nodiscard]] MemoryHandle SplitMemoryHandle(MemoryHandle memoryHandle, const VkDeviceSize offset);

        /** Created a new memory block for memory pool.
         *  Block size is determined by using max of provided pagedMemorySize or block allocation size provided at allocator loading.
         */
        Result<> CreateMemoryBlock(
            MemoryBlock*& memoryBlock,
            MemoryPool& memoryPool,
            const VkDeviceSize pagedMemorySize);

        [[nodiscard]] VkDeviceSize GetPagedSize(VkDeviceSize size) const;

        void FreeMemoryHandle(MemoryHandle memoryHandle);
        void FreeMemoryBlock(MemoryBlock& memoryBlock);
        void FreeMemoryBlocksInAllPools();

        Logger* m_logger;
        LogicalDevice* m_logicalDevice;
        PhysicalDevice* m_physicalDevice;
        VkDeviceSize m_pageSize;
        VkDeviceSize m_blockAllocationSize;
        MemoryTypes m_memoryTypes;
        std::vector<MemoryPool> m_memoryPools;

    };

}

#endif

#endif
