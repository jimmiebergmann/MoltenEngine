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
#include "Molten/Utility/Bitfield.hpp"

namespace Molten
{
    TEST(Utility, Bitfield)
    {
        {
            Bitfield<1> a;
            EXPECT_EQ(a.FragmentBitCount, size_t(64));
            EXPECT_EQ(a.ActualBitCount, size_t(64));
            EXPECT_EQ(a.FragmentCount, size_t(1));
        }
        {
            Bitfield<64> a;
            EXPECT_EQ(a.FragmentBitCount, size_t(64));
            EXPECT_EQ(a.ActualBitCount, size_t(64));
            EXPECT_EQ(a.FragmentCount, size_t(1));
        }
        {
            Bitfield<65> a;
            EXPECT_EQ(a.FragmentBitCount, size_t(64));
            EXPECT_EQ(a.ActualBitCount, size_t(128));
            EXPECT_EQ(a.FragmentCount, size_t(2));
        }
        {
            Bitfield<123456> a;
            EXPECT_EQ(a.FragmentBitCount, size_t(64));
            EXPECT_EQ(a.ActualBitCount, size_t(123456));
            EXPECT_EQ(a.FragmentCount, size_t(1929));
        }
        {
            Bitfield<100> a;
            EXPECT_EQ(a.IsAnySet(), false);
            a.Set(1);
            EXPECT_EQ(a.IsAnySet(), true);
            EXPECT_EQ(a.IsSet(0), false);
            EXPECT_EQ(a.IsSet(1), true);
            EXPECT_EQ(a.IsSet(2), false);

            a.Set(3, 4, 5);
            EXPECT_EQ(a.IsSet(0), false);
            EXPECT_EQ(a.IsSet(1), true);
            EXPECT_EQ(a.IsSet(2), false);
            EXPECT_EQ(a.IsSet(3), true);
            EXPECT_EQ(a.IsSet(4), true);
            EXPECT_EQ(a.IsSet(5), true);
            EXPECT_EQ(a.IsSet(6), false);

            a.Set(70);
            EXPECT_EQ(a.IsSet(70), true);

            a.Unset(4);
            a.Unset(70, 3);
            EXPECT_EQ(a.IsSet(0), false);
            EXPECT_EQ(a.IsSet(1), true);
            EXPECT_EQ(a.IsSet(2), false);
            EXPECT_EQ(a.IsSet(3), false);
            EXPECT_EQ(a.IsSet(4), false);
            EXPECT_EQ(a.IsSet(5), true);
            EXPECT_EQ(a.IsSet(6), false);
            EXPECT_EQ(a.IsSet(70), false);

            EXPECT_EQ(a.IsUnset(0), true);
            EXPECT_EQ(a.IsUnset(1), false);
            EXPECT_EQ(a.IsUnset(2), true);
            EXPECT_EQ(a.IsUnset(3), true);
            EXPECT_EQ(a.IsUnset(4), true);
            EXPECT_EQ(a.IsUnset(5), false);
            EXPECT_EQ(a.IsUnset(6), true);
            EXPECT_EQ(a.IsUnset(70), true);


            a.Set(127);
            a.Set(125);

            std::string str = "10100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100010";
            EXPECT_STREQ(a.ToString().c_str(), str.c_str());
        }
        {
            Bitfield<100> a;
            Bitfield<100> b;
            EXPECT_TRUE(a == b);
            EXPECT_FALSE(a != b);

            a.Set(3);
            EXPECT_FALSE(a == b);
            EXPECT_TRUE(a != b);

            Bitfield<100> c = a;
            EXPECT_TRUE(a == c);

            EXPECT_TRUE(a > b);
            EXPECT_FALSE(b > c);

            EXPECT_TRUE(b < a);
            EXPECT_FALSE(a < b);
        }
        {
            Bitfield<100> a(1, 4, 5, 64);
            Bitfield<100> b(5, 64, 65, 66);
        
            {
                std::string strOr = "00000000000000000000000000000000000000000000000000000000000001110000000000000000000000000000000000000000000000000000000000110010";

                auto c1 = a | b;
                EXPECT_STREQ(c1.ToString().c_str(), strOr.c_str());

                auto c2 = a;
                c2 |= b;
                EXPECT_TRUE(c1 == c2);
                EXPECT_STREQ(c2.ToString().c_str(), strOr.c_str());
            }
            {
                std::string strAnd = "00000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000100000";
            
                auto c1 = a & b;
                EXPECT_STREQ(c1.ToString().c_str(), strAnd.c_str());

                auto c2 = a;
                c2 &= b;
                EXPECT_TRUE(c1 == c2);
                EXPECT_STREQ(c2.ToString().c_str(), strAnd.c_str());
            }
            {
                std::string strNot = "11111111111111111111111111111111111111111111111111111111111110001111111111111111111111111111111111111111111111111111111111001101";

                auto c1 = a | b;
                auto inverse = ~c1;
                EXPECT_STREQ(inverse.ToString().c_str(), strNot.c_str());
            }
        }

    }

}