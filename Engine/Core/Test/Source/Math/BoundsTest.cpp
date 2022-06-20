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

#include "Test.hpp"
#include "Molten/Math/Bounds.hpp"

namespace Molten
{

    TEST(Math, Bounds_Constructor)
    {
        // Empty construction.
        {
            Bounds<2, int32_t> bounds1;
            EXPECT_TRUE(bounds1.IsEmpty());
            EXPECT_EQ(bounds1.low, Vector2i32(0, 0));
            EXPECT_EQ(bounds1.high, Vector2i32(0, 0));

            Bounds<3, int32_t> bounds2;
            EXPECT_TRUE(bounds2.IsEmpty());
            EXPECT_EQ(bounds2.low, Vector3i32(0, 0, 0));
            EXPECT_EQ(bounds2.high, Vector3i32(0, 0, 0));

            Bounds<6, int32_t> bounds3;
            EXPECT_EQ(bounds3.low, (Vector<6, int32_t>(0, 0, 0, 0, 0, 0)));
            EXPECT_EQ(bounds3.high, (Vector<6, int32_t>(0, 0, 0, 0, 0, 0)));
        }
        // Bounds 6
        {
            const Vector<6, int32_t> low = { 1, 2, 3, 4, 5, 6 };
            const Vector<6, int32_t> high = Vector<6, int32_t>{ -1, -2, -3, -4, -5, -6 };
            const Bounds<6, int32_t> bounds = Bounds<6, int32_t>{ low, high };

            EXPECT_EQ(bounds.low, low);
            EXPECT_EQ(bounds.high, high);

            Bounds<6, int32_t> boundsCopy = bounds;

            EXPECT_EQ(boundsCopy.low, low);
            EXPECT_EQ(boundsCopy.high, high);

            Bounds<6, int32_t> boundsCopy2;

            EXPECT_NE(boundsCopy2.low, low);
            EXPECT_NE(boundsCopy2.high, high);

            boundsCopy2 = bounds;

            EXPECT_EQ(boundsCopy2.low, low);
            EXPECT_EQ(boundsCopy2.high, high);
        }
        // Bounds 2
        {
            {
                const Vector2i32 low = { 43, 76 };
                const Vector2i32 high = { 13, -143 };
                const Bounds2i32 bounds = { low, high };

                EXPECT_EQ(bounds.low, low);
                EXPECT_EQ(bounds.left, low.x);
                EXPECT_EQ(bounds.top, low.y);

                EXPECT_EQ(bounds.high, high);
                EXPECT_EQ(bounds.right, high.x);
                EXPECT_EQ(bounds.bottom, high.y);

                Bounds2i32 boundsCopy = bounds;

                EXPECT_EQ(boundsCopy.low, low);
                EXPECT_EQ(boundsCopy.high, high);

                Bounds2i32 boundsCopy2;

                EXPECT_NE(boundsCopy2.low, low);
                EXPECT_NE(boundsCopy2.high, high);

                boundsCopy2 = bounds;

                EXPECT_EQ(boundsCopy2.low, low);
                EXPECT_EQ(boundsCopy2.high, high);
            }
            {
                {
                    const Bounds2i32 bounds = { 2, 34, 61, 23 };

                    EXPECT_EQ(bounds.left, 2);
                    EXPECT_EQ(bounds.top, 34);
                    EXPECT_EQ(bounds.right, 61);
                    EXPECT_EQ(bounds.bottom, 23);
                }
            }
        }
        // Bounds 3
        {
            {
                const Vector3i32 low = { 74, -123, 543 };
                const Vector3i32 high = { 1111, 2, 3 };
                const Bounds3i32 bounds = { low, high };

                EXPECT_EQ(bounds.low, low);
                EXPECT_EQ(bounds.left, low.x);
                EXPECT_EQ(bounds.top, low.y);
                EXPECT_EQ(bounds.near, low.z);

                EXPECT_EQ(bounds.high, high);
                EXPECT_EQ(bounds.right, high.x);
                EXPECT_EQ(bounds.bottom, high.y);
                EXPECT_EQ(bounds.far, high.z);

                Bounds3i32 boundsCopy = bounds;

                EXPECT_EQ(boundsCopy.low, low);
                EXPECT_EQ(boundsCopy.high, high);

                Bounds3i32 boundsCopy2;

                EXPECT_NE(boundsCopy2.low, low);
                EXPECT_NE(boundsCopy2.high, high);

                boundsCopy2 = bounds;

                EXPECT_EQ(boundsCopy2.low, low);
                EXPECT_EQ(boundsCopy2.high, high);
            }
            {
                const Bounds3i32 bounds = { 4, 7, 2, 3, 5, 8 };

                EXPECT_EQ(bounds.left, 4);
                EXPECT_EQ(bounds.top, 7);
                EXPECT_EQ(bounds.near, 2);
                EXPECT_EQ(bounds.right, 3);
                EXPECT_EQ(bounds.bottom, 5);
                EXPECT_EQ(bounds.far, 8);
            }
        }
    }

    TEST(Math, Bounds_Union)
    {
        // Bounds 2
        {
            const Bounds2i32 boundsA{ { 10, 15}, { 20, 21} };
            const Bounds2i32 boundsB{ { -11, -14}, { 13, 14} };
            const Bounds2i32 boundsUnion = Bounds2i32::Union(boundsA, boundsB);

            EXPECT_EQ(boundsUnion.low, Vector2i32(-11, -14));
            EXPECT_EQ(boundsUnion.high, Vector2i32(20, 21));

            const Bounds2i32 boundsUnionReverse = Bounds2i32::Union(boundsB, boundsA);

            EXPECT_EQ(boundsUnion, boundsUnionReverse);
        }

        // Bounds 3
        {
            const Bounds3i32 boundsA{ { 10, 15, 30}, { 20, 21, 35} };
            const Bounds3i32 boundsB{ { -11, -14, 2}, { 13, 14, 12} };
            const Bounds3i32 boundsUnion = Bounds3i32::Union(boundsA, boundsB);

            EXPECT_EQ(boundsUnion.low, Vector3i32(-11, -14, 2));
            EXPECT_EQ(boundsUnion.high, Vector3i32(20, 21, 35));

            const Bounds3i32 boundsUnionReverse = Bounds3i32::Union(boundsB, boundsA);

            EXPECT_EQ(boundsUnion, boundsUnionReverse);
        }
    }

    TEST(Math, Bounds_GetSize)
    {
        // Bounds 2
        {
            EXPECT_EQ(Bounds2i32({ 0, 0 }, { 0, 0 }).GetSize(), Vector2i32(0, 0));
            EXPECT_EQ(Bounds2i32({ 20, 21 }, { 20, 21 }).GetSize(), Vector2i32(0, 0));
            EXPECT_EQ(Bounds2i32({ -20, -21 }, { -20, -21 }).GetSize(), Vector2i32(0, 0));
            EXPECT_EQ(Bounds2i32({ 9, 15 }, { 20, 21 }).GetSize(), Vector2i32(11, 6));
            EXPECT_EQ(Bounds2i32({ 20, 21 }, { 9, 15 }).GetSize(), Vector2i32(-11, -6));
        }
        // Bounds 3
        {
            EXPECT_EQ(Bounds3i32({ 0, 0, 0 }, { 0, 0, 0 }).GetSize(), Vector3i32(0, 0, 0));
            EXPECT_EQ(Bounds3i32({ 20, 21, 22 }, { 20, 21, 22 }).GetSize(), Vector3i32(0, 0, 0));
            EXPECT_EQ(Bounds3i32({ -20, -21, -22 }, { -20, -21, -22 }).GetSize(), Vector3i32(0, 0, 0));
            EXPECT_EQ(Bounds3i32({ 9, 15, 4 }, { 20, 21, 17 }).GetSize(), Vector3i32(11, 6, 13));
            EXPECT_EQ(Bounds3i32({ 20, 21, 17 }, { 9, 15, 4 }).GetSize(), Vector3i32(-11, -6, -13));
        }
    }

    TEST(Math, Bounds_Intersects)
    {
        // Bounds 2
        {
            EXPECT_TRUE(Bounds2i32({ 0, 0 }, { 0, 0 }).Intersects(Vector2i32(0, 0)));

            {
                Bounds2i32 bounds({ 11, 12 }, { 13, 14 });
                EXPECT_TRUE(bounds.Intersects(Vector2i32(12, 13)));

                EXPECT_TRUE(bounds.Intersects(Vector2i32(11, 12)));
                EXPECT_TRUE(bounds.Intersects(Vector2i32(11, 14)));
                EXPECT_TRUE(bounds.Intersects(Vector2i32(13, 12)));
                EXPECT_TRUE(bounds.Intersects(Vector2i32(13, 14)));

                EXPECT_FALSE(bounds.Intersects(Vector2i32(10, 11)));
                EXPECT_FALSE(bounds.Intersects(Vector2i32(10, 15)));
                EXPECT_FALSE(bounds.Intersects(Vector2i32(14, 11)));
                EXPECT_FALSE(bounds.Intersects(Vector2i32(14, 15)));
            }
            
        }
        // Bounds 3
        {
            EXPECT_TRUE(Bounds3i32({ 0, 0, 0 }, { 0, 0, 0 }).Intersects(Vector3i32(0, 0, 0)));

            {
                Bounds3i32 bounds({ 11, 12, 13 }, { 13, 14, 15 });
                EXPECT_TRUE(bounds.Intersects(Vector3i32(12, 13, 14)));

                EXPECT_TRUE(bounds.Intersects(Vector3i32(11, 12, 13)));
                EXPECT_TRUE(bounds.Intersects(Vector3i32(11, 12, 15)));
                EXPECT_TRUE(bounds.Intersects(Vector3i32(11, 14, 13)));
                EXPECT_TRUE(bounds.Intersects(Vector3i32(11, 14, 15)));
                EXPECT_TRUE(bounds.Intersects(Vector3i32(13, 12, 13)));
                EXPECT_TRUE(bounds.Intersects(Vector3i32(13, 12, 15)));
                EXPECT_TRUE(bounds.Intersects(Vector3i32(13, 14, 13)));
                EXPECT_TRUE(bounds.Intersects(Vector3i32(13, 14, 15)));

                EXPECT_FALSE(bounds.Intersects(Vector3i32(10, 11, 12)));
                EXPECT_FALSE(bounds.Intersects(Vector3i32(10, 11, 16)));
                EXPECT_FALSE(bounds.Intersects(Vector3i32(10, 15, 12)));
                EXPECT_FALSE(bounds.Intersects(Vector3i32(10, 15, 16)));
                EXPECT_FALSE(bounds.Intersects(Vector3i32(14, 11, 12)));
                EXPECT_FALSE(bounds.Intersects(Vector3i32(14, 11, 16)));
                EXPECT_FALSE(bounds.Intersects(Vector3i32(14, 15, 12)));
                EXPECT_FALSE(bounds.Intersects(Vector3i32(14, 15, 16)));
            }
        }
    }

    TEST(Math, Bounds_Move)
    {
        // Bounds 2
        {
            EXPECT_EQ(Bounds2i32({ 1, 2 }, { 3, 4 }).Move({ 1, 2 }), Bounds2i32({ 2, 4 }, { 4, 6 }));
            EXPECT_EQ(Bounds2i32({ 1, 2 }, { 3, 4 }).Move({ -1, -2 }), Bounds2i32({ 0, 0 }, { 2, 2 }));
        }
        // Bounds 3
        {
            EXPECT_EQ(Bounds3i32({ 1, 2, 3 }, { 4, 5, 6 }).Move({ 1, 2, 3 }), Bounds3i32({ 2, 4, 6 }, { 5, 7, 9 }));
            EXPECT_EQ(Bounds3i32({ 1, 2, 3 }, { 4, 5, 6 }).Move({ -1, -2, -3 }), Bounds3i32({ 0, 0, 0 }, { 3, 3, 3 }));
        }
    }

    TEST(Math, Bounds_WithMargins)
    {
        // Bounds 2
        {
            const Bounds2i32 bounds({ 10, 20 }, { 30, 40 });
            EXPECT_EQ(bounds.WithMargins(Bounds2i32({ 1, 2 }, { 3, 4 })), Bounds2i32({ 9, 18 }, { 33, 44 }));
        }
        // Bounds 3
        {
            const Bounds3i32 bounds({ 10, 20, 30 }, { 40, 50, 60 });
            EXPECT_EQ(bounds.WithMargins(Bounds3i32({ 1, 2, 3 }, { 4, 5, 6 })), Bounds3i32({ 9, 18, 27 }, { 44, 55, 66 }));
        }
    }

    TEST(Math, Bounds_WithoutMargins)
    {
        // Bounds 2
        {
            const Bounds2i32 bounds({ 10, 20 }, { 30, 40 });
            EXPECT_EQ(bounds.WithoutMargins(Bounds2i32({ 1, 2 }, { 3, 4 })), Bounds2i32({ 11, 22 }, { 27, 36 }));
        }
        // Bounds 3
        {
            const Bounds3i32 bounds({ 10, 20, 30 }, { 40, 50, 60 });
            EXPECT_EQ(bounds.WithoutMargins(Bounds3i32({ 1, 2, 3 }, { 4, 5, 6 })), Bounds3i32({ 11, 22, 33 }, { 36, 45, 54 }));
        }
    }

    TEST(Math, Bounds_AddMargins)
    {
        // Bounds 2
        {
            Bounds2i32 bounds({ 10, 20 }, { 30, 40 });
            bounds.AddMargins(Bounds2i32({ 1, 2 }, { 3, 4 }));
            EXPECT_EQ(bounds, Bounds2i32({ 9, 18 }, { 33, 44 }));
        }
        // Bounds 3
        {
            Bounds3i32 bounds({ 10, 20, 30 }, { 40, 50, 60 });
            bounds.AddMargins(Bounds3i32({ 1, 2, 3 }, { 4, 5, 6 }));
            EXPECT_EQ(bounds, Bounds3i32({ 9, 18, 27 }, { 44, 55, 66 }));
        }
    }

    TEST(Math, Bounds_RemoveMargins)
    {
        // Bounds 2
        {
            Bounds2i32 bounds({ 10, 20 }, { 30, 40 });
            bounds.RemoveMargins(Bounds2i32({ 1, 2 }, { 3, 4 }));
            EXPECT_EQ(bounds, Bounds2i32({ 11, 22 }, { 27, 36 }));
        }
        // Bounds 3
        {
            Bounds3i32 bounds({ 10, 20, 30 }, { 40, 50, 60 });
            bounds.RemoveMargins(Bounds3i32({ 1, 2, 3 }, { 4, 5, 6 }));
            EXPECT_EQ(bounds, Bounds3i32({ 11, 22, 33 }, { 36, 45, 54 }));
        }
    }

    TEST(Math, Bounds_ClampHighToLow)
    {
        // Bounds 2
        {
            Bounds2i32 bounds({ 30, 20 }, { -10, 40 });
            bounds.ClampHighToLow();
            EXPECT_EQ(bounds, Bounds2i32({ 30, 20 }, { 30, 40 }));
        }
        // Bounds 3
        {
            Bounds3i32 bounds({ 30, 20, 30 }, { -10, 40, 50 });
            bounds.ClampHighToLow();
            EXPECT_EQ(bounds, Bounds3i32({ 30, 20, 30 }, { 30, 40, 50 }));
        }
    }

    TEST(Math, Bounds_IsEmpty)
    {
        // Bounds 2
        {
            EXPECT_TRUE(Bounds2i32({ 0, 0 }, { 0, 0 }).IsEmpty());
            EXPECT_TRUE(Bounds2i32({ 1, 2 }, { 1, 2 }).IsEmpty());
            EXPECT_TRUE(Bounds2i32({ -1, -2 }, { -1, -2 }).IsEmpty());

            EXPECT_TRUE(Bounds2i32({ 1, 2 }, { 0, 2 }).IsEmpty());
            EXPECT_TRUE(Bounds2i32({ 1, 2 }, { 1, 1 }).IsEmpty());
            EXPECT_TRUE(Bounds2i32({ 1, 2 }, { 1, 2 }).IsEmpty());
        }
        // Bounds 3
        {
            EXPECT_TRUE(Bounds3i32({ 0, 0, 0 }, { 0, 0, 0 }).IsEmpty());
            EXPECT_TRUE(Bounds3i32({ 1, 2, 3 }, { 1, 2, 3 }).IsEmpty());
            EXPECT_TRUE(Bounds3i32({ -1, -2, -3 }, { -1, -2, -3 }).IsEmpty());

            EXPECT_TRUE(Bounds3i32({ 1, 2, 3 }, { 0, 2, 3 }).IsEmpty());
            EXPECT_TRUE(Bounds3i32({ 1, 2, 3 }, { 1, 1, 3 }).IsEmpty());
            EXPECT_TRUE(Bounds3i32({ 1, 2, 3 }, { 1, 2, 2 }).IsEmpty());
        }
    }

}