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
#include "Curse/Math/Matrix.hpp"

namespace Curse
{

    TEST(Math, Matrix_Typedefs)
    {
        {
            EXPECT_TRUE((std::is_same<Matrix4x4i32::Type, int32_t>::value));
            EXPECT_TRUE((std::is_same<Matrix4x4i64::Type, int64_t>::value));
            EXPECT_TRUE((std::is_same<Matrix4x4f32::Type, float>::value));
            EXPECT_TRUE((std::is_same<Matrix4x4f64::Type, double>::value));
        }
        {
            EXPECT_EQ(Matrix4x4<int32_t>::Width, size_t(4));
            EXPECT_EQ(Matrix4x4<int32_t>::Height, size_t(4));
            EXPECT_EQ(Matrix4x4<int32_t>::ComponentCount, size_t(16));

            EXPECT_EQ(Matrix4x4i32::Width, size_t(4));
            EXPECT_EQ(Matrix4x4i32::Height, size_t(4));
            EXPECT_EQ(Matrix4x4i32::ComponentCount, size_t(16));

            EXPECT_EQ(Matrix4x4i64::Width, size_t(4));
            EXPECT_EQ(Matrix4x4i64::Height, size_t(4));
            EXPECT_EQ(Matrix4x4i64::ComponentCount, size_t(16));

            EXPECT_EQ(Matrix4x4f32::Width, size_t(4));
            EXPECT_EQ(Matrix4x4f32::Height, size_t(4));
            EXPECT_EQ(Matrix4x4f32::ComponentCount, size_t(16));

            EXPECT_EQ(Matrix4x4f64::Width, size_t(4));
            EXPECT_EQ(Matrix4x4f64::Height, size_t(4));
            EXPECT_EQ(Matrix4x4f64::ComponentCount, size_t(16));
        }
    }

    TEST(Math, Matrix)
    {
        const size_t x = 6;
        const size_t y = 10;
        Matrix<x, y, int32_t> matrix;

        int32_t counter = 0;
        for (size_t xx = 0; xx < x; xx++)
        {
            for (size_t yy = 0; yy < y; yy++)
            {
                matrix.v[xx][yy] = counter;
                ++counter;
            }
        }

        for (size_t i = 0; i < x * y; i++)
        {
            EXPECT_EQ(matrix.c[i], static_cast<int32_t>(i));
        }
    }

    TEST(Math, Matrix4x4)
    {
        {
            Matrix4x4i32 mat(
                1, 2, 3, 4,
                5, 6, 7, 8,
                9, 10, 11, 12,
                13, 14, 15, 16);

            EXPECT_EQ(mat.c[0], int32_t(1));
            EXPECT_EQ(mat.c[1], int32_t(5));
            EXPECT_EQ(mat.c[2], int32_t(9));
            EXPECT_EQ(mat.c[3], int32_t(13));
            EXPECT_EQ(mat.c[4], int32_t(2));
            EXPECT_EQ(mat.c[5], int32_t(6));
            EXPECT_EQ(mat.c[6], int32_t(10));
            EXPECT_EQ(mat.c[7], int32_t(14));
            EXPECT_EQ(mat.c[8], int32_t(3));
            EXPECT_EQ(mat.c[9], int32_t(7));
            EXPECT_EQ(mat.c[10], int32_t(11));
            EXPECT_EQ(mat.c[11], int32_t(15));
            EXPECT_EQ(mat.c[12], int32_t(4));
            EXPECT_EQ(mat.c[13], int32_t(8));
            EXPECT_EQ(mat.c[14], int32_t(12));
            EXPECT_EQ(mat.c[15], int32_t(16));
        }
    }

}