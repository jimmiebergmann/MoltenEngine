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

#include "Test.hpp"
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
            EXPECT_EQ(Matrix4x4<int32_t>::Rows, size_t(4));
            EXPECT_EQ(Matrix4x4<int32_t>::Columns, size_t(4));
            EXPECT_EQ(Matrix4x4<int32_t>::Components, size_t(16));

            EXPECT_EQ(Matrix4x4i32::Rows, size_t(4));
            EXPECT_EQ(Matrix4x4i32::Columns, size_t(4));
            EXPECT_EQ(Matrix4x4i32::Components, size_t(16));

            EXPECT_EQ(Matrix4x4i64::Rows, size_t(4));
            EXPECT_EQ(Matrix4x4i64::Columns, size_t(4));
            EXPECT_EQ(Matrix4x4i64::Components, size_t(16));

            EXPECT_EQ(Matrix4x4f32::Rows, size_t(4));
            EXPECT_EQ(Matrix4x4f32::Columns, size_t(4));
            EXPECT_EQ(Matrix4x4f32::Components, size_t(16));

            EXPECT_EQ(Matrix4x4f64::Rows, size_t(4));
            EXPECT_EQ(Matrix4x4f64::Columns, size_t(4));
            EXPECT_EQ(Matrix4x4f64::Components, size_t(16));
        }
    }

    TEST(Math, Matrix)
    {
        const int32_t rows = 6;
        const int32_t columns = 10;
        Matrix<rows, columns, int32_t> matrix;

        for (int32_t i = 0; i < rows * columns; i++)
        {
            matrix.e[i] = i;
        }

        for (int32_t y = 0; y < rows; y++)
        {
            for (int32_t x = 0; x < columns; x++)
            {
                int32_t i = (y * columns) + x;
                EXPECT_EQ(matrix.e[i], static_cast<int32_t>(i));
                EXPECT_EQ(matrix.row[y][x], static_cast<int32_t>(i));
                EXPECT_EQ(matrix.row[y].c[x], static_cast<int32_t>(i));
            }
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

            EXPECT_EQ(mat.e[0], int32_t(1));
            EXPECT_EQ(mat.e[1], int32_t(2));
            EXPECT_EQ(mat.e[2], int32_t(3));
            EXPECT_EQ(mat.e[3], int32_t(4));
            EXPECT_EQ(mat.e[4], int32_t(5));
            EXPECT_EQ(mat.e[5], int32_t(6));
            EXPECT_EQ(mat.e[6], int32_t(7));
            EXPECT_EQ(mat.e[7], int32_t(8));
            EXPECT_EQ(mat.e[8], int32_t(9));
            EXPECT_EQ(mat.e[9], int32_t(10));
            EXPECT_EQ(mat.e[10], int32_t(11));
            EXPECT_EQ(mat.e[11], int32_t(12));
            EXPECT_EQ(mat.e[12], int32_t(13));
            EXPECT_EQ(mat.e[13], int32_t(14));
            EXPECT_EQ(mat.e[14], int32_t(15));
            EXPECT_EQ(mat.e[15], int32_t(16));

            EXPECT_EQ(mat.row[0][0], int32_t(1));
            EXPECT_EQ(mat.row[0][1], int32_t(2));
            EXPECT_EQ(mat.row[0][2], int32_t(3));
            EXPECT_EQ(mat.row[0][3], int32_t(4));
            EXPECT_EQ(mat.row[1][0], int32_t(5));
            EXPECT_EQ(mat.row[1][1], int32_t(6));
            EXPECT_EQ(mat.row[1][2], int32_t(7));
            EXPECT_EQ(mat.row[1][3], int32_t(8));
            EXPECT_EQ(mat.row[2][0], int32_t(9));
            EXPECT_EQ(mat.row[2][1], int32_t(10));
            EXPECT_EQ(mat.row[2][2], int32_t(11));
            EXPECT_EQ(mat.row[2][3], int32_t(12));
            EXPECT_EQ(mat.row[3][0], int32_t(13));
            EXPECT_EQ(mat.row[3][1], int32_t(14));
            EXPECT_EQ(mat.row[3][2], int32_t(15));
            EXPECT_EQ(mat.row[3][3], int32_t(16));

            EXPECT_EQ(mat.row[0].x, int32_t(1));
            EXPECT_EQ(mat.row[0].y, int32_t(2));
            EXPECT_EQ(mat.row[0].z, int32_t(3));
            EXPECT_EQ(mat.row[0].w, int32_t(4));
            EXPECT_EQ(mat.row[1].x, int32_t(5));
            EXPECT_EQ(mat.row[1].y, int32_t(6));
            EXPECT_EQ(mat.row[1].z, int32_t(7));
            EXPECT_EQ(mat.row[1].w, int32_t(8));
            EXPECT_EQ(mat.row[2].x, int32_t(9));
            EXPECT_EQ(mat.row[2].y, int32_t(10));
            EXPECT_EQ(mat.row[2].z, int32_t(11));
            EXPECT_EQ(mat.row[2].w, int32_t(12));
            EXPECT_EQ(mat.row[3].x, int32_t(13));
            EXPECT_EQ(mat.row[3].y, int32_t(14));
            EXPECT_EQ(mat.row[3].z, int32_t(15));
            EXPECT_EQ(mat.row[3].w, int32_t(16));
        }
    }

    TEST(Math, Matrix4x4_Perspective)
    {
        EXPECT_NO_THROW(Matrix4x4f32::Perspective(60.0f, 1.0f, 1.0f, 0.0f));
    }

}