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

#include "Test.hpp"
#include "Curse/Ecs/EcsAllocator.hpp"
#include "Curse/System/Exception.hpp"
#include <algorithm>

namespace Curse
{

    namespace Ecs
    {

        TEST(ECS, Allocator)
        {
            {
                EXPECT_THROW(Allocator(0), Curse::Exception);
            }
            {
                Allocator allocator(100);

                EXPECT_EQ(allocator.GetBlockCount(), size_t(1));
                EXPECT_EQ(allocator.GetBlockSize(), size_t(100));
                EXPECT_EQ(allocator.GetCurrentBlockIndex(), size_t(0));
                EXPECT_EQ(allocator.GetCurrentDataIndex(), size_t(0));
            }
            {
                Allocator allocator(100);
                size_t blockIndex = 0;
                size_t dataIndex = 0;
                ASSERT_NO_THROW(allocator.RequestMemory(100, blockIndex, dataIndex));
            }
            {
                Allocator allocator(100);
                size_t blockIndex = 0;
                size_t dataIndex = 0;
                Byte* data = allocator.RequestMemory(100, blockIndex, dataIndex);
                EXPECT_EQ(blockIndex, size_t(0));
                EXPECT_EQ(dataIndex, size_t(0));
                EXPECT_NE(data, nullptr);
                EXPECT_EQ(allocator.GetBlock(0), data);

                EXPECT_EQ(allocator.GetBlockCount(), size_t(1));
                EXPECT_EQ(allocator.GetCurrentBlockIndex(), size_t(0));
                EXPECT_EQ(allocator.GetCurrentDataIndex(), size_t(100));
            }
            {
                Allocator allocator(100);
                size_t blockIndex = 0;
                size_t dataIndex = 0;
                EXPECT_THROW(allocator.RequestMemory(101, blockIndex, dataIndex), Curse::Exception);
            }
            {
                Allocator allocator(100);

                const size_t requests = 10;
                size_t blockIndex[requests] = { 0 };
                size_t dataIndex[requests] = { 0 };
                Byte* data[requests] = { nullptr };

                for (size_t i = 0; i < requests; i++)
                {
                    const size_t expectCurrentBlock = static_cast<size_t>(std::max((static_cast<int32_t>(i) - 1) / 2, 0));
                    const size_t expectNextBlock = i / 2;

                    EXPECT_EQ(allocator.GetCurrentBlockIndex(), expectCurrentBlock);
                    data[i] = allocator.RequestMemory(50, blockIndex[i], dataIndex[i]);
                    EXPECT_EQ(allocator.GetCurrentBlockIndex(), expectNextBlock);
                  
                    EXPECT_EQ(blockIndex[i], i / 2);
                    EXPECT_EQ(dataIndex[i], size_t(50) * (i % 2));
                    EXPECT_EQ(data[i], (allocator.GetBlock(blockIndex[i]) + dataIndex[i]));
                }
            }

        }

    }

}