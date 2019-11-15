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
#include "Curse/System/Version.hpp"

namespace Curse
{
    TEST(System, Version)
    {
        {
            EXPECT_EQ(Version(), Version());
            
            EXPECT_EQ(Version(1), Version(1));
            EXPECT_EQ(Version(5), Version(5));

            EXPECT_EQ(Version(1, 0), Version(1));
            EXPECT_EQ(Version(1, 0), Version(1, 0));
            EXPECT_EQ(Version(1, 1), Version(1, 1));
            EXPECT_EQ(Version(5, 0), Version(5, 0));
            EXPECT_EQ(Version(5, 5), Version(5, 5));

            EXPECT_EQ(Version(1, 0, 0), Version(1));
            EXPECT_EQ(Version(1, 0, 0), Version(1, 0));
            EXPECT_EQ(Version(1, 0, 0), Version(1, 0, 0));
            EXPECT_EQ(Version(1, 1, 0), Version(1, 1, 0));
            EXPECT_EQ(Version(5, 0, 0), Version(5, 0, 0));
            EXPECT_EQ(Version(5, 0, 2), Version(5, 0, 2));
            EXPECT_EQ(Version(5, 5, 0), Version(5, 5, 0));
            EXPECT_EQ(Version(5, 5, 2), Version(5, 5, 2));
            EXPECT_EQ(Version(5, 5, 5), Version(5, 5, 5));
        }
        {
            EXPECT_NE(Version(), Version(1));
            EXPECT_NE(Version(), Version(1, 1));
            EXPECT_NE(Version(), Version(1, 1, 1));
            EXPECT_NE(Version(1), Version());
            EXPECT_NE(Version(1, 1), Version());
            EXPECT_NE(Version(1, 1, 1), Version());

            EXPECT_NE(Version(1), Version(2));
            EXPECT_NE(Version(2), Version(2, 1));
            EXPECT_NE(Version(2), Version(2, 1, 1));
        }
        {
            EXPECT_LT(Version(1), Version(2));
            EXPECT_LT(Version(1), Version(1, 1));
            EXPECT_LT(Version(1), Version(1, 0, 1));
        }
        {
            EXPECT_LE(Version(1), Version(1));
            EXPECT_LE(Version(1), Version(2));
            EXPECT_LE(Version(1), Version(1, 0));
            EXPECT_LE(Version(1), Version(1, 1));
            EXPECT_LE(Version(1), Version(1, 0, 0));
            EXPECT_LE(Version(1), Version(1, 0, 1));
        }
        {
            EXPECT_GT(Version(2), Version(1));
            EXPECT_GT(Version(1, 1), Version(1));
            EXPECT_GT(Version(1, 0, 1), Version(1));
        }
        {
            EXPECT_GE(Version(1), Version(1));
            EXPECT_GE(Version(2), Version(1));
            EXPECT_GE(Version(1, 0), Version(1));
            EXPECT_GE(Version(1, 1), Version(1));
            EXPECT_GE(Version(1, 0, 0), Version(1));
            EXPECT_GE(Version(1, 0, 1), Version());
        }

    }

    TEST(System, Version_AsString)
    {
        {
            EXPECT_STREQ(Version().AsString().c_str(), "0");
            EXPECT_STREQ(Version().AsString(false).c_str(), "0.0.0");          

            EXPECT_STREQ(Version(1).AsString().c_str(), "1");
            EXPECT_STREQ(Version(1).AsString(false).c_str(), "1.0.0");         

            EXPECT_STREQ(Version(1, 1).AsString().c_str(), "1.1");
            EXPECT_STREQ(Version(1, 1).AsString(false).c_str(), "1.1.0");           

            EXPECT_STREQ(Version(1, 1, 1).AsString().c_str(), "1.1.1");
            EXPECT_STREQ(Version(1, 1, 1).AsString(false).c_str(), "1.1.1");       
            
            EXPECT_STREQ(Version(0, 12 ).AsString(true).c_str(), "0.12");
            EXPECT_STREQ(Version(0, 12).AsString(false).c_str(), "0.12.0");

            EXPECT_STREQ(Version(0, 0, 12).AsString(true).c_str(), "0.0.12");
            EXPECT_STREQ(Version(0, 0, 12).AsString(false).c_str(), "0.0.12");
            EXPECT_STREQ(Version(0, 45, 34).AsString(true).c_str(), "0.45.34");
            EXPECT_STREQ(Version(0, 45, 34).AsString(false).c_str(), "0.45.34");
            EXPECT_STREQ(Version(123, 456, 789).AsString(false).c_str(), "123.456.789");
            EXPECT_STREQ(Version(123, 456, 789).AsString(true).c_str(), "123.456.789");
        }
    }

}
