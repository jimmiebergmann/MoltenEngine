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
#include "Molten/System/Result.hpp"

struct TestData
{
    int a;
    float b;
};

Molten::Result<TestData, int> CreateTestError()
{
    return Molten::Result<TestData, int>::Error(-2);
}

namespace Molten
{
    TEST(System, Result)
    {
        {
            Result<int, int> result;

            EXPECT_FALSE(result);
            if (result)
            {
                EXPECT_TRUE(result);
            }
        }

        {
            Result<int, int> result = Result<int, int>::Value(123);
            EXPECT_TRUE(result);
            EXPECT_EQ(result.Value(), int(123));

            [[maybe_unused]] auto value = std::move(result);
        }
        {
            Result<int, int> result = Result<int, int>::Error(404);
            EXPECT_FALSE(result);
            EXPECT_EQ(result.Error(), int(404));
        }
        {
            auto result = CreateTestError();
            EXPECT_FALSE(result);
            EXPECT_EQ(result.Error(), int(-2));
        }
        
    }
}