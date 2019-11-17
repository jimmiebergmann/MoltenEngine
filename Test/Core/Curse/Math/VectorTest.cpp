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
#include "Curse/Math/Vector.hpp"

namespace Curse
{

    // Vector tests.
    TEST(Math, Vector_Typedefs)
    {
        {
            EXPECT_TRUE((std::is_same<Vector<11, bool>::Type, bool>::value));
            EXPECT_TRUE((std::is_same<Vector<11, float>::Type, float>::value));

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

            EXPECT_TRUE((std::is_same<Vector4b::Type, bool>::value));
            EXPECT_TRUE((std::is_same<Vector4i32::Type, int32_t>::value));
            EXPECT_TRUE((std::is_same<Vector4ui32::Type, uint32_t>::value));
            EXPECT_TRUE((std::is_same<Vector4i64::Type, int64_t>::value));
            EXPECT_TRUE((std::is_same<Vector4ui64::Type, uint64_t>::value));
            EXPECT_TRUE((std::is_same<Vector4f32::Type, float>::value));
            EXPECT_TRUE((std::is_same<Vector4f64::Type, double>::value));
        }
        {
            EXPECT_EQ((Vector<5, int32_t>::Dimensions), size_t(5));
            EXPECT_EQ((Vector<11, float>::Dimensions), size_t(11));

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

            EXPECT_EQ(Vector4<int32_t>::Dimensions, size_t(4));
            EXPECT_EQ(Vector4b::Dimensions, size_t(4));
            EXPECT_EQ(Vector4i32::Dimensions, size_t(4));
            EXPECT_EQ(Vector4ui32::Dimensions, size_t(4));
            EXPECT_EQ(Vector4i64::Dimensions, size_t(4));
            EXPECT_EQ(Vector4ui64::Dimensions, size_t(4));
            EXPECT_EQ(Vector4f32::Dimensions, size_t(4));
            EXPECT_EQ(Vector4f64::Dimensions, size_t(4));
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


    // 2D Vector tests.
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

    TEST(Math, Vector2_Length)
    {
        {
            Vector2i32 vec(3, 4);
            EXPECT_EQ(vec.Length(), int32_t(5));
        }
        {
            Vector2f32 vec(50.0f, 0.0f);
            EXPECT_NEAR(vec.Length(), float(50.0f), 1e-5);
        }
        {
            Vector2f32 vec(0.0f, -60.0f);
            EXPECT_NEAR(vec.Length(), float(60.0f), 1e-5);
        }
    }
    TEST(Math, Vector2_Normal)
    {
        {
            {
                Vector2i32 vec1(100, 0);
                EXPECT_EQ(vec1.Normal(), Vector2i32(1, 0));
                Vector2i32 vec2(-200, 0);
                EXPECT_EQ(vec2.Normal(), Vector2i32(-1, 0));
                Vector2i32 vec3(0, 300);
                EXPECT_EQ(vec3.Normal(), Vector2i32(0, 1));
                Vector2i32 vec4(0, -400);
                EXPECT_EQ(vec4.Normal(), Vector2i32(0, -1));

                Vector2i32 vec_zero(0, 0);
                EXPECT_EQ(vec_zero.Normal(), Vector2i32(0, 0));
            }
            {
                Vector2f32 vec1(100.0f, 0.0f);
                EXPECT_EQ(vec1.Normal(), Vector2f32(1.0f, 0.0f));
                Vector2f32 vec2(-200.0f, 0.0f);
                EXPECT_EQ(vec2.Normal(), Vector2f32(-1.0f, 0.0f));
                Vector2f32 vec3(0.0f, 300.0f);
                EXPECT_EQ(vec3.Normal(), Vector2f32(0.0f, 1.0f));
                Vector2f32 vec4(0.0f, -400.0f);
                EXPECT_EQ(vec4.Normal(), Vector2f32(0.0f, -1.0f));

                Vector2f32 vec_zero(0.0f, 0.0f);
                EXPECT_EQ(vec_zero.Normal(), Vector2f32(0.0f, 0.0f));
            }
            {
                Vector2f64 vec1(100.0f, 0.0f);
                EXPECT_EQ(vec1.Normal(), Vector2f64(1.0f, 0.0f));
                Vector2f64 vec2(-200.0f, 0.0f);
                EXPECT_EQ(vec2.Normal(), Vector2f64(-1.0f, 0.0f));
                Vector2f64 vec3(0.0f, 300.0f);
                EXPECT_EQ(vec3.Normal(), Vector2f64(0.0f, 1.0f));
                Vector2f64 vec4(0.0f, -400.0f);
                EXPECT_EQ(vec4.Normal(), Vector2f64(0.0f, -1.0f));

                Vector2f64 vec_zero(0.0f, 0.0f);
                EXPECT_EQ(vec_zero.Normal(), Vector2f64(0.0f, 0.0f));
            }
            {
                Vector2<long double> vec1(100.0f, 0.0f);
                EXPECT_EQ(vec1.Normal(), Vector2<long double>(1.0f, 0.0f));
                Vector2<long double> vec2(-200.0f, 0.0f);
                EXPECT_EQ(vec2.Normal(), Vector2<long double>(-1.0f, 0.0f));
                Vector2<long double> vec3(0.0f, 300.0f);
                EXPECT_EQ(vec3.Normal(), Vector2<long double>(0.0f, 1.0f));
                Vector2<long double> vec4(0.0f, -400.0f);
                EXPECT_EQ(vec4.Normal(), Vector2<long double>(0.0f, -1.0f));

                Vector2<long double> vec_zero(0.0f, 0.0f);
                EXPECT_EQ(vec_zero.Normal(), Vector2<long double>(0.0f, 0.0f));
            }
        }
        {
            {
                Vector2i32 vec(100, 0);
                vec.Normalize();
                EXPECT_EQ(vec, Vector2i32(1, 0));
            }
            {
                Vector2i32 vec(-200, 0);
                vec.Normalize();
                EXPECT_EQ(vec, Vector2i32(-1, 0));
            }
            {
                Vector2i32 vec(0, 300);
                vec.Normalize();
                EXPECT_EQ(vec, Vector2i32(0, 1));
            }
            {
                Vector2i32 vec(0, -400);
                vec.Normalize();
                EXPECT_EQ(vec, Vector2i32(0, -1));
            }
        }
    }

    TEST(Math, Vector2_Dot)
    {
        {
            Vector2f32 vec1(1.0f, 0.0f);
            Vector2f32 vec2(1.0f, 0.0f);
            EXPECT_NEAR(vec1.Dot(vec2), float(1.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot<double>(vec2), double(1.0f), 1e-3);
        }
        {
            Vector2f32 vec1(1.0f, 0.0f);
            Vector2f32 vec2(-1.0f, 0.0f);
            EXPECT_NEAR(vec1.Dot(vec2), float(-1.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot<double>(vec2), double(-1.0f), 1e-3);
        }
        {
            Vector2f32 vec1(1.0f, 0.0f);
            Vector2f32 vec2(0.0f, 1.0f);
            Vector2f32 vec3(0.0f, -1.0f);
            EXPECT_NEAR(vec1.Dot(vec2), float(0.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot<double>(vec2), double(0.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot(vec3), float(0.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot<double>(vec3), double(0.0f), 1e-3);
        }
    }


    // 3D Vector tests.
    TEST(Math, Vector3)
    {
        {
            Vector<3, int32_t> vec3i;
            EXPECT_EQ(vec3i.Dimensions, size_t(3));
            EXPECT_TRUE((std::is_same<Vector<3, int32_t>::Type, int32_t>::value));
        }
        {
            Vector<3, int32_t> vec3i(int32_t(100));
            EXPECT_EQ(vec3i.x, int32_t(100));
            EXPECT_EQ(vec3i.y, int32_t(100));
            EXPECT_EQ(vec3i.z, int32_t(100));
            EXPECT_EQ(vec3i.c[0], int32_t(100));
            EXPECT_EQ(vec3i.c[1], int32_t(100));
            EXPECT_EQ(vec3i.c[2], int32_t(100));
        }
        {
            Vector<3, int32_t> vec3i(int32_t(100), int32_t(200), int32_t(300));
            EXPECT_EQ(vec3i.x, int32_t(100));
            EXPECT_EQ(vec3i.y, int32_t(200));
            EXPECT_EQ(vec3i.z, int32_t(300));
            EXPECT_EQ(vec3i.c[0], int32_t(100));
            EXPECT_EQ(vec3i.c[1], int32_t(200));
            EXPECT_EQ(vec3i.c[2], int32_t(300));
        }
    }

    TEST(Math, Vector3_Operators)
    {
        {
            EXPECT_EQ(Vector3i32(0, 0, 0) + Vector3i32(0, 0, 0), Vector3i32(0, 0, 0));
            EXPECT_EQ(Vector3i32(0, 0, 0) + Vector3i32(10, 0, 0), Vector3i32(10, 0, 0));
            EXPECT_EQ(Vector3i32(0, 0, 0) + Vector3i32(0, 10, 0), Vector3i32(0, 10, 0));
            EXPECT_EQ(Vector3i32(0, 0, 0) + Vector3i32(10, 10, 10), Vector3i32(10, 10, 10));

            EXPECT_EQ(Vector3i32(120, 300, 1) + Vector3i32(30, 20, 5), Vector3i32(150, 320, 6));
            EXPECT_EQ(Vector3i32(430, 301, 2) + Vector3i32(10, 0, 6), Vector3i32(440, 301, 8));
            EXPECT_EQ(Vector3i32(123, 322, 3) + Vector3i32(50, 10, 7), Vector3i32(173, 332, 10));
            EXPECT_EQ(Vector3i32(333, 444, 4) + Vector3i32(20, 60, 8), Vector3i32(353, 504, 12));

            {
                Vector3i32 vec(100, 200, 300);
                vec += Vector3i32(123, 456, 432);
                EXPECT_EQ(vec, Vector3i32(223, 656, 732));
            }
        }
        {
            EXPECT_EQ(Vector3i32(0, 0, 0) - Vector3i32(0, 0, 0), Vector3i32(0, 0, 0));
            EXPECT_EQ(Vector3i32(0, 0, 0) - Vector3i32(10, 0, 5), Vector3i32(-10, 0, -5));
            EXPECT_EQ(Vector3i32(0, 0, 6) - Vector3i32(0, 10, 1), Vector3i32(0, -10, 5));
            EXPECT_EQ(Vector3i32(0, 0, 10) - Vector3i32(10, -10, -10), Vector3i32(-10, 10, 20));
        }
        {
            EXPECT_FALSE(Vector3i32(100, 200, 300) != Vector3i32(100, 200, 300));
            EXPECT_TRUE(Vector3i32(101, 200, 300) != Vector3i32(100, 200, 300));
            EXPECT_TRUE(Vector3i32(100, 201, 300) != Vector3i32(100, 200, 300));
            EXPECT_TRUE(Vector3i32(101, 201, 300) != Vector3i32(100, 200, 300));
            EXPECT_TRUE(Vector3i32(100, 200, 300) != Vector3i32(100, 200, 301));
            EXPECT_TRUE(Vector3i32(100, 200, 301) != Vector3i32(100, 200, 300));
            EXPECT_TRUE(Vector3i32(100, 200, 300) != Vector3i32(101, 201, 300));
        }
    }

    TEST(Math, Vector3_Length)
    {
        {
            Vector3f32 vec(1, 2, 3);
            EXPECT_NEAR(vec.Length(), float(3.74165), 1e-3);
        }
        {
            Vector3f32 vec(50.0f, 0.0f, 0.0f);
            EXPECT_NEAR(vec.Length(), float(50.0f), 1e-5);
        }
        {
            Vector3f32 vec(0.0f, -60.0f, 0.0f);
            EXPECT_NEAR(vec.Length(), float(60.0f), 1e-5);
        }
        {
            Vector3f32 vec(0.0f, 0.0f, 125.0f);
            EXPECT_NEAR(vec.Length(), float(125.0f), 1e-5);
        }
    }
    TEST(Math, Vector3_Normal)
    {
        {
            {
                Vector3i32 vec1(100, 0, 0);
                EXPECT_EQ(vec1.Normal(), Vector3i32(1, 0, 0));
                Vector3i32 vec2(-200, 0, 0);
                EXPECT_EQ(vec2.Normal(), Vector3i32(-1, 0, 0));
                Vector3i32 vec3(0, 300, 0);
                EXPECT_EQ(vec3.Normal(), Vector3i32(0, 1, 0));
                Vector3i32 vec4(0, -400, 0);
                EXPECT_EQ(vec4.Normal(), Vector3i32(0, -1, 0));

                Vector3i32 vec5(0, 0, 500);
                EXPECT_EQ(vec5.Normal(), Vector3i32(0, 0, 1));
                Vector3i32 vec6(0, 0, -600);
                EXPECT_EQ(vec6.Normal(), Vector3i32(0, 0, -1));

                Vector3i32 vec_zero(0, 0, 0);
                EXPECT_EQ(vec_zero.Normal(), Vector3i32(0, 0, 0));
            }
            {
                Vector3f32 vec1(100.0f, 0.0f, 0.0f);
                EXPECT_EQ(vec1.Normal(), Vector3f32(1.0f, 0.0f, 0.0f));
                Vector3f32 vec2(-200.0f, 0.0f, 0.0f);
                EXPECT_EQ(vec2.Normal(), Vector3f32(-1.0f, 0.0f, 0.0f));
                Vector3f32 vec3(0.0f, 300.0f, 0.0f);
                EXPECT_EQ(vec3.Normal(), Vector3f32(0.0f, 1.0f, 0.0f));
                Vector3f32 vec4(0.0f, -400.0f, 0.0f);
                EXPECT_EQ(vec4.Normal(), Vector3f32(0.0f, -1.0f, 0.0f));

                Vector3f32 vec5(0.0f, 0.0f, 500.0f);
                EXPECT_EQ(vec5.Normal(), Vector3f32(0.0f, 0.0f, 1.0f));
                Vector3f32 vec6(0.0f, 0.0f, -600.0f);
                EXPECT_EQ(vec6.Normal(), Vector3i32(0.0f, 0.0f, -1.0f));

                Vector3f32 vec_zero(0.0f, 0.0f, 0.0f);
                EXPECT_EQ(vec_zero.Normal(), Vector3f32(0.0f, 0.0f, 0.0f));
            }
            {
                Vector3f64 vec1(100.0f, 0.0f, 0.0f);
                vec1 = vec1.Normal();
                EXPECT_NEAR(vec1.x, double(1.0f), 1e-3);
                EXPECT_NEAR(vec1.y, double(0.0f), 1e-3);
                EXPECT_NEAR(vec1.z, double(0.0f), 1e-3);
                
                Vector3f64 vec2(-200.0f, 0.0f, 0.0f);
                vec2 = vec2.Normal();
                EXPECT_NEAR(vec2.x, double(-1.0f), 1e-3);
                EXPECT_NEAR(vec2.y, double(0.0f), 1e-3);
                EXPECT_NEAR(vec2.z, double(0.0f), 1e-3);
                
                Vector3f64 vec3(0.0f, 300.0f, 0.0f);
                vec3 = vec3.Normal();
                EXPECT_NEAR(vec3.x, double(0.0f), 1e-3);
                EXPECT_NEAR(vec3.y, double(1.0f), 1e-3);
                EXPECT_NEAR(vec3.z, double(0.0f), 1e-3);
                
                Vector3f64 vec4(0.0f, -400.0f, 0.0f);
                vec4 = vec4.Normal();
                EXPECT_NEAR(vec4.x, double(0.0f), 1e-3);
                EXPECT_NEAR(vec4.y, double(-1.0f), 1e-3);
                EXPECT_NEAR(vec3.z, double(0.0f), 1e-3);

                Vector3f64 vec5(0.0f, 0.0f, 500.0f);
                vec5 = vec5.Normal();
                EXPECT_NEAR(vec5.x, double(0.0f), 1e-3);
                EXPECT_NEAR(vec5.y, double(0.0f), 1e-3);
                EXPECT_NEAR(vec5.z, double(1.0f), 1e-3);
                
                Vector3f64 vec6(0.0f, 0.0f, -600.0f);
                vec6 = vec6.Normal();
                EXPECT_NEAR(vec6.x, double(0.0f), 1e-3);
                EXPECT_NEAR(vec6.y, double(0.0f), 1e-3);
                EXPECT_NEAR(vec6.z, double(-1.0f), 1e-3);

                Vector3f64 vec_zero(0.0f, 0.0f, 0.0f);
                vec_zero = vec_zero.Normal();
                EXPECT_NEAR(vec_zero.x, double(0.0f), 1e-3);
                EXPECT_NEAR(vec_zero.y, double(0.0f), 1e-3);
                EXPECT_NEAR(vec_zero.z, double(0.0f), 1e-3);
            }
        }
        {
            {
                Vector3i32 vec(100, 0, 0);
                vec.Normalize();
                EXPECT_EQ(vec, Vector3i32(1, 0, 0));
            }
            {
                Vector3i32 vec(-200, 0, 0);
                vec.Normalize();
                EXPECT_EQ(vec, Vector3i32(-1, 0, 0));
            }
            {
                Vector3i32 vec(0, 300, 0);
                vec.Normalize();
                EXPECT_EQ(vec, Vector3i32(0, 1, 0));
            }
            {
                Vector3i32 vec(0, -400, 0);
                vec.Normalize();
                EXPECT_EQ(vec, Vector3i32(0, -1, 0));
            }
            {
                Vector3i32 vec(0, 0, 500);
                vec.Normalize();
                EXPECT_EQ(vec, Vector3i32(0, 0, 1));
            }
            {
                Vector3i32 vec(0, 0, -600);
                vec.Normalize();
                EXPECT_EQ(vec, Vector3i32(0, 0, -1));
            }
        }
    }

    TEST(Math, Vector3_Dot)
    {
        {
            Vector3f32 vec1(1.0f, 0.0f, 0.0f);
            Vector3f32 vec2(1.0f, 0.0f, 0.0f);
            EXPECT_NEAR(vec1.Dot(vec2), float(1.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot<double>(vec2), double(1.0f), 1e-3);
        }
        {
            Vector3f32 vec1(1.0f, 0.0f, 0.0f);
            Vector3f32 vec2(-1.0f, 0.0f, 0.0f);
            EXPECT_NEAR(vec1.Dot(vec2), float(-1.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot<double>(vec2), double(-1.0f), 1e-3);
        }
        {
            Vector3f32 vec1(1.0f, 0.0f, 0.0f);
            Vector3f32 vec2(0.0f, 1.0f, 0.0f);
            Vector3f32 vec3(0.0f, -1.0f, 0.0f);
            EXPECT_NEAR(vec1.Dot(vec2), float(0.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot<double>(vec2), double(0.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot(vec3), float(0.0f), 1e-3);
            EXPECT_NEAR(vec1.Dot<double>(vec3), double(0.0f), 1e-3);
        }
    }

    TEST(Math, Vector3_Cross)
    {
        {
            Vector3f32 vec1(100.0f, 200.0f, 300.0f);
            Vector3f32 vec2(100.0f, 200.0f, 300.0f);
            Vector3f32 cross = vec1.Cross(vec2);

            EXPECT_NEAR(cross.x, float(0.0f), 1e-5);
            EXPECT_NEAR(cross.y, float(0.0f), 1e-5);
            EXPECT_NEAR(cross.z, float(0.0f), 1e-5);
        }
        {
            Vector3f32 vec1(0.0f, 100.0f, 0.0f);
            Vector3f32 vec2(0.0f, 0.0f, 200.0f);
            Vector3f32 cross = vec1.Cross(vec2);

            EXPECT_NEAR(cross.x, float(20000.0f), 1e-5);
            EXPECT_NEAR(cross.y, float(0.0f), 1e-5);
            EXPECT_NEAR(cross.z, float(0.0f), 1e-5);
        }
    }
}
