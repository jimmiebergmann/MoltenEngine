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

#include "Curse/Ecs/EcsAllocator.hpp"
#include "Curse/System/Exception.hpp"
#include <cstring>

namespace Curse
{

    namespace Ecs
    {


        Allocator::Allocator(const size_t blockSize) :
            m_blockSize(blockSize),
            m_blocks{},
            m_freeBlockIndex(0),
            m_freeDataIndex(0)
        {
            if (!blockSize)
            {
                throw Exception("Block size of 0 is not allowed.");
            }
            AppendNewBlock();
        }

        Allocator::~Allocator()
        {
            for (auto* block : m_blocks)
            {
                delete[] block;
            }
        }

        Byte* Allocator::GetBlock(const size_t block)
        {
            return m_blocks[block];
        }
        const Byte* Allocator::GetBlock(const size_t block) const
        {
            return m_blocks[block];
        }

        size_t Allocator::GetBlockCount() const
        {
            return m_blocks.size();
        }

        size_t Allocator::GetBlockSize() const
        {
            return m_blockSize;
        }

        size_t Allocator::GetCurrentBlockIndex() const
        {
            return m_freeBlockIndex;
        }

        size_t Allocator::GetCurrentDataIndex() const
        {
            return m_freeDataIndex;
        }

        Byte* Allocator::RequestMemory(const size_t size, size_t& blockIndex, size_t& dataIndex)
        {
            blockIndex = m_freeBlockIndex;
            dataIndex = m_freeDataIndex;

            if (!size)
            {
                throw Exception("Requested 0 bytes of data from allocator.");
            }

            if (dataIndex + size > m_blockSize)
            {
                if (size > m_blockSize)
                {
                    throw Exception("Requested " + std::to_string(size) + " bytes of data from allocator, " +
                                    std::to_string(m_blockSize) + " is the maximum allowed data size request.");
                }

                blockIndex = AppendNewBlock();
                dataIndex = 0;
            }

            m_freeDataIndex += size;
            return m_blocks[blockIndex] + dataIndex;
        }

        size_t Allocator::AppendNewBlock()
        {
            m_blocks.push_back(new Byte[m_blockSize]);
           
            size_t index = m_blocks.size() - 1;
            m_freeBlockIndex = index;
            m_freeDataIndex = 0;

            std::memset(m_blocks[index], 0, m_blockSize);

            return index;
        }

    }

}