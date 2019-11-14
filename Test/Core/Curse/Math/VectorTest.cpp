/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#include "gtest/gtest.h"
#include "Curse/Math/Vector.hpp"

namespace Curse
{
    TEST(Math, VectorD)
    {
        Vector<5, int32_t> vec5i;
        EXPECT_EQ(vec5i.Dimensions, size_t(5));
        EXPECT_TRUE((std::is_same<Vector<5, int32_t>::Type, int32_t>::value));

        for (size_t i = 0; i < vec5i.Dimensions; i++)
        {
            vec5i.c[i] = static_cast<int>(i);
        }

        EXPECT_EQ(vec5i.c[0], int32_t(0));
        EXPECT_EQ(vec5i.c[1], int32_t(1));
        EXPECT_EQ(vec5i.c[2], int32_t(2));
        EXPECT_EQ(vec5i.c[3], int32_t(3));
        EXPECT_EQ(vec5i.c[4], int32_t(4));
    }
    TEST(Math, Vector2)
    {
        {
            Vector<2, int32_t> vec2i;
            EXPECT_EQ(vec2i.Dimensions, size_t(2));
            EXPECT_TRUE((std::is_same<Vector<2, int32_t>::Type, int32_t>::value));
        }
        {
            Vector<2, int32_t> vec2i(int32_t(100));
            EXPECT_EQ(vec2i.x, int32_t(100));
            EXPECT_EQ(vec2i.y, int32_t(100));
            EXPECT_EQ(vec2i.c[0], int32_t(100));
            EXPECT_EQ(vec2i.c[1], int32_t(100));
        }
        {
            Vector<2, int32_t> vec2i(int32_t(100), int32_t(200));
            EXPECT_EQ(vec2i.x, int32_t(100));
            EXPECT_EQ(vec2i.y, int32_t(200));
            EXPECT_EQ(vec2i.c[0], int32_t(100));
            EXPECT_EQ(vec2i.c[1], int32_t(200));
        }
    }

}
