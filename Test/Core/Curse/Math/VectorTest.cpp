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

    TEST(Math, Vector_Typedefs)
    {
        {
            EXPECT_TRUE((std::is_same<Vector2b::Type, bool>::value));
            EXPECT_TRUE((std::is_same<Vector2i32::Type, int32_t>::value));
            EXPECT_TRUE((std::is_same<Vector2ui32::Type, uint32_t>::value));
            EXPECT_TRUE((std::is_same<Vector2i64::Type, int64_t>::value));
            EXPECT_TRUE((std::is_same<Vector2ui64::Type, uint64_t>::value));
            EXPECT_TRUE((std::is_same<Vector2f32::Type, float>::value));
            EXPECT_TRUE((std::is_same<Vector2f64::Type, double>::value));

            EXPECT_TRUE((std::is_same<Vector3b::Type, bool>::value));
            EXPECT_TRUE((std::is_same<Vector3i32::Type, int32_t>::value));
            EXPECT_TRUE((std::is_same<Vector3ui32::Type, uint32_t>::value));
            EXPECT_TRUE((std::is_same<Vector3i64::Type, int64_t>::value));
            EXPECT_TRUE((std::is_same<Vector3ui64::Type, uint64_t>::value));
            EXPECT_TRUE((std::is_same<Vector3f32::Type, float>::value));
            EXPECT_TRUE((std::is_same<Vector3f64::Type, double>::value));
        }
        {
            EXPECT_EQ(Vector2<int32_t>::Dimensions, size_t(2));
            EXPECT_EQ(Vector2b::Dimensions, size_t(2));
            EXPECT_EQ(Vector2i32::Dimensions, size_t(2));
            EXPECT_EQ(Vector2ui32::Dimensions, size_t(2));
            EXPECT_EQ(Vector2i64::Dimensions, size_t(2));
            EXPECT_EQ(Vector2ui64::Dimensions, size_t(2));
            EXPECT_EQ(Vector2f32::Dimensions, size_t(2));
            EXPECT_EQ(Vector2f64::Dimensions, size_t(2));

            EXPECT_EQ(Vector3<int32_t>::Dimensions, size_t(3));
            EXPECT_EQ(Vector3b::Dimensions, size_t(3));
            EXPECT_EQ(Vector3i32::Dimensions, size_t(3));
            EXPECT_EQ(Vector3ui32::Dimensions, size_t(3));
            EXPECT_EQ(Vector3i64::Dimensions, size_t(3));
            EXPECT_EQ(Vector3ui64::Dimensions, size_t(3));
            EXPECT_EQ(Vector3f32::Dimensions, size_t(3));
            EXPECT_EQ(Vector3f64::Dimensions, size_t(3));
        }
    }

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

    TEST(Math, Vector2_Operators)
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
        {
            EXPECT_EQ(Vector2i32(0, 0) + Vector2i32(0, 0), Vector2i32(0, 0));
            EXPECT_EQ(Vector2i32(0, 0) + Vector2i32(10, 0), Vector2i32(10, 0));
            EXPECT_EQ(Vector2i32(0, 0) + Vector2i32(0, 10), Vector2i32(0, 10));
            EXPECT_EQ(Vector2i32(0, 0) + Vector2i32(10, 10), Vector2i32(10, 10));

            EXPECT_EQ(Vector2i32(120, 300) + Vector2i32(30, 20), Vector2i32(150, 320));
            EXPECT_EQ(Vector2i32(430, 301) + Vector2i32(10, 0), Vector2i32(440, 301));
            EXPECT_EQ(Vector2i32(123, 322) + Vector2i32(50, 10), Vector2i32(173, 332));
            EXPECT_EQ(Vector2i32(333, 444) + Vector2i32(20, 60), Vector2i32(353, 504));

            {
                Vector2i32 vec(100, 200);
                vec += Vector2i32(123, 456);
                EXPECT_EQ(vec, Vector2i32(223, 656));
            }
        }
        {
            EXPECT_EQ(Vector2i32(0, 0) - Vector2i32(0, 0), Vector2i32(0, 0));
            EXPECT_EQ(Vector2i32(0, 0) - Vector2i32(10, 0), Vector2i32(-10, 0));
            EXPECT_EQ(Vector2i32(0, 0) - Vector2i32(0, 10), Vector2i32(0, -10));
            EXPECT_EQ(Vector2i32(0, 0) - Vector2i32(10, -10), Vector2i32(-10, 10));

            EXPECT_EQ(Vector2i32(120, 300) - Vector2i32(30, 20), Vector2i32(90, 280));
            EXPECT_EQ(Vector2i32(430, 301) - Vector2i32(10, 0), Vector2i32(420, 301));
            EXPECT_EQ(Vector2i32(123, 322) - Vector2i32(50, 10), Vector2i32(73, 312));
            EXPECT_EQ(Vector2i32(333, 444) - Vector2i32(20, 60), Vector2i32(313, 384));

            {
                Vector2i32 vec(100, 200);
                vec -= Vector2i32(123, 456);
                EXPECT_EQ(vec, Vector2i32(-23, -256));
            }
        }
        {
            EXPECT_EQ(Vector2i32(0, 0) * Vector2i32(123, 456), Vector2i32(0, 0));
            EXPECT_EQ(Vector2i32(1, 0) * Vector2i32(10, 0), Vector2i32(10, 0));
            EXPECT_EQ(Vector2i32(0, 1) * Vector2i32(0, 10), Vector2i32(0, 10));
            EXPECT_EQ(Vector2i32(1, 2) * Vector2i32(10, -10), Vector2i32(10, -20));

            EXPECT_EQ(Vector2i32(120, 300) * Vector2i32(30, 20), Vector2i32(3600, 6000));
            EXPECT_EQ(Vector2i32(430, 301) * Vector2i32(10, 0), Vector2i32(4300, 0));
            EXPECT_EQ(Vector2i32(123, 322) * Vector2i32(50, 10), Vector2i32(6150, 3220));
            EXPECT_EQ(Vector2i32(333, 444) * Vector2i32(20, 60), Vector2i32(6660, 26640));

            EXPECT_EQ(Vector2i32(120, 300) * int32_t(-1), Vector2i32(-120, -300));
            EXPECT_EQ(Vector2i32(430, 301) * int32_t(1), Vector2i32(430, 301));
            EXPECT_EQ(Vector2i32(123, 322) * int32_t(2), Vector2i32(246, 644));
            EXPECT_EQ(Vector2i32(333, 444) * int32_t(3), Vector2i32(999, 1332));

            {
                Vector2i32 vec(100, 200);
                vec *= Vector2i32(123, 456);
                EXPECT_EQ(vec, Vector2i32(12300, 91200));
            }
            {
                Vector2i32 vec(100, 200);
                vec *= int32_t(300);
                EXPECT_EQ(vec, Vector2i32(30000, 60000));
            }
        }
        {
            EXPECT_EQ(Vector2i32(123, 456) / Vector2i32(10, 20), Vector2i32(12, 22));
            EXPECT_EQ(Vector2i32(100, 0) / Vector2i32(10, 20), Vector2i32(10, 0));
            EXPECT_EQ( Vector2i32(0, 100) / Vector2i32(10, 20), Vector2i32(0, 5));
            EXPECT_EQ(Vector2i32(100, -100) / Vector2i32(10, 20), Vector2i32(10, -5));

            EXPECT_EQ(Vector2i32(120, 300) / Vector2i32(30, 20), Vector2i32(4, 15));
            EXPECT_EQ(Vector2i32(123, 322) / Vector2i32(50, 10), Vector2i32(2, 32));
            EXPECT_EQ(Vector2i32(333, 444) / Vector2i32(20, 60), Vector2i32(16, 7));

            {
                Vector2i32 vec(100, 200);
                vec /= Vector2i32(10, 2);
                EXPECT_EQ(vec, Vector2i32(10, 100));
            }
            {
                Vector2i32 vec(100, 200);
                vec /= int32_t(4);
                EXPECT_EQ(vec, Vector2i32(25, 50));
            }
        }
        {
            EXPECT_TRUE(Vector2i32(0, 0) == Vector2i32(0, 0));
            EXPECT_TRUE(Vector2i32(150, 0) == Vector2i32(150, 0));
            EXPECT_TRUE(Vector2i32(0, 250) == Vector2i32(0, 250));
            EXPECT_TRUE(Vector2i32(150, 250) == Vector2i32(150, 250));

            EXPECT_FALSE(Vector2i32(1, 3) == Vector2i32(1, 6));
            EXPECT_FALSE(Vector2i32(1, 3) == Vector2i32(0, 0));
            EXPECT_FALSE(Vector2i32(1, 0) == Vector2i32(0, 0));
            EXPECT_FALSE(Vector2i32(150, 1) == Vector2i32(150, 0));
            EXPECT_FALSE(Vector2i32(2, 250) == Vector2i32(0, 250));
            EXPECT_FALSE(Vector2i32(150, 250) == Vector2i32(150, 253));
        }
        {
            EXPECT_FALSE(Vector2i32(100, 200) != Vector2i32(100, 200));
            EXPECT_TRUE(Vector2i32(101, 200) != Vector2i32(100, 200));
            EXPECT_TRUE(Vector2i32(100, 201) != Vector2i32(100, 200));
            EXPECT_TRUE(Vector2i32(101, 201) != Vector2i32(100, 200));
            EXPECT_TRUE(Vector2i32(100, 200) != Vector2i32(101, 200));
            EXPECT_TRUE(Vector2i32(100, 200) != Vector2i32(100, 201));
            EXPECT_TRUE(Vector2i32(100, 200) != Vector2i32(101, 201));          
        }
    }

}
