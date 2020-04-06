/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#ifndef CURSE_CORE_ECS_ECSALLOCATOR_HPP
#define CURSE_CORE_ECS_ECSALLOCATOR_HPP

#include "Curse/Ecs/Ecs.hpp"

namespace Curse
{

    namespace Ecs
    {

        /**
        * @brief Memory allocator class, providing the ECS with blocks of memory.
        * The allocator internally stored blocks of data.
        * The user can request any amount of memory, less or equal to GetBlockSize().
        *
        */
        class CURSE_API Allocator
        {

        public:

            /**
            * @brief Constructor.
            *
            * @param blockSize Memory block size in bytes.
            *
            * @throw Exception if blockSize is 0.
            */
            explicit Allocator(const size_t blockSize);

            /**
            * @brief Destructor. Cleaning up all allocated memory blocks.
            *
            */
            ~Allocator();  

            /**
            * @brief Get data pointer to provided block index.
            */
            /**@{*/ 
            Byte* GetBlock(const size_t block);
            const Byte* GetBlock(const size_t block) const;
            /**@}*/

            /**
            * @brief Get number of allocated memory blocks.
            */
            size_t GetBlockCount() const;

            /**
            * @brief Get block size in bytes of each block.
            */
            size_t GetBlockSize() const;

            /**
            * @brief Get current block index in use for further memory requests.
            */
            size_t GetCurrentBlockIndex() const;

            /**
            * @brief Get current data index in use for further memory requests.
            */
            size_t GetCurrentDataIndex() const;

            /*
            * @brief Request memory from the allocator. 
            *
            * @size Size of data to request.
            * @block blockIndex Index of block index, where the returned data is located.
            * @index dataIndex Index what data position, where the returned data is located.
            *
            * @throw Exception if system is out of memory, requested size is 0 or > GetBlockSize().
            */
            Byte* RequestMemory(const size_t size, size_t& blockIndex, size_t& dataIndex);

        private:

            /**
            * @brief Append new memory block.
            *
            * @return Index of new memory block.
            */
            size_t AppendNewBlock();

            size_t m_blockSize;
            std::vector<Byte*> m_blocks;
            size_t m_freeBlockIndex;
            size_t m_freeDataIndex;

        };

    }

}

#endif