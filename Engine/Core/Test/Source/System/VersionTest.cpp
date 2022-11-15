/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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
#include "Molten/System/Version.hpp"
#include <limits>

namespace Molten
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

            EXPECT_FALSE(Version(2, 1, 0) < Version(1, 2, 3));
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

            EXPECT_FALSE(Version(1, 2, 3) > Version(2, 1, 0));
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

    TEST(System, Version_ToString)
    {
        {
            EXPECT_STREQ(ToString(Version{}).c_str(), "0");
            EXPECT_STREQ(ToString(Version{}, false).c_str(), "0.0.0");

            EXPECT_STREQ(ToString(Version{ 1 }).c_str(), "1");
            EXPECT_STREQ(ToString(Version{ 1 }, false).c_str(), "1.0.0");

            EXPECT_STREQ(ToString(Version{ 1, 1 }).c_str(), "1.1");
            EXPECT_STREQ(ToString(Version{ 1, 1 }, false).c_str(), "1.1.0");

            EXPECT_STREQ(ToString(Version{ 1, 1, 1 }).c_str(), "1.1.1");
            EXPECT_STREQ(ToString(Version{ 1, 1, 1 }, false).c_str(), "1.1.1");
            
            EXPECT_STREQ(ToString(Version{0, 12 }, true).c_str(), "0.12");
            EXPECT_STREQ(ToString(Version{ 0, 12 }, false).c_str(), "0.12.0");

            EXPECT_STREQ(ToString(Version{ 0, 0, 12 }, true).c_str(), "0.0.12");
            EXPECT_STREQ(ToString(Version{ 0, 0, 12 }, false).c_str(), "0.0.12");

            EXPECT_STREQ(ToString(Version{ 0, 45, 34 }, true).c_str(), "0.45.34");
            EXPECT_STREQ(ToString(Version{ 0, 45, 34 }, false).c_str(), "0.45.34");
            EXPECT_STREQ(ToString(Version{ 123, 456, 789 }, false).c_str(), "123.456.789");
            EXPECT_STREQ(ToString(Version{ 123, 456, 789 }, true).c_str(), "123.456.789");
        }
    }
    TEST(System, Version_FromString)
    {
        {
            EXPECT_FALSE(FromString<Version>("").HasValue());
            EXPECT_FALSE(FromString<Version>(" ").HasValue());
            EXPECT_FALSE(FromString<Version>("a").HasValue());
            EXPECT_FALSE(FromString<Version>("1.a").HasValue());
            EXPECT_FALSE(FromString<Version>("1.1.a").HasValue());
        }
        {
            const auto version1 = FromString<Version>("1");
            ASSERT_TRUE(version1.HasValue());
            EXPECT_EQ(version1.Value(), Version(1, 0, 0));

            const auto version2 = FromString<Version>("2.3");
            ASSERT_TRUE(version2.HasValue());
            EXPECT_EQ(version2.Value(), Version(2, 3, 0));

            const auto version3 = FromString<Version>("4.5.6");
            ASSERT_TRUE(version3.HasValue());
            EXPECT_EQ(version3.Value(), Version(4, 5, 6));
        }
        {     
            {
                Version expectedVersion{
                    std::numeric_limits<uint32_t>::max() - 1
                };

                const auto str = std::to_string(expectedVersion.Major);

                const auto version = FromString<Version>(str);
                ASSERT_TRUE(version.HasValue());
                EXPECT_EQ(version.Value(), expectedVersion);
            }
            { 
                Version expectedVersion{
                    std::numeric_limits<uint32_t>::max() - 1,
                    std::numeric_limits<uint32_t>::max() - 2
                };

                const auto str =
                    std::to_string(expectedVersion.Major)
                    + "." +
                    std::to_string(expectedVersion.Minor);

                const auto version = FromString<Version>(str);
                ASSERT_TRUE(version.HasValue());
                EXPECT_EQ(version.Value(), expectedVersion);
            }
            {
                Version expectedVersion{
                    std::numeric_limits<uint32_t>::max() - 1,
                    std::numeric_limits<uint32_t>::max() - 2,
                    std::numeric_limits<uint32_t>::max() - 3
                };

                const auto str =
                    std::to_string(expectedVersion.Major)
                    + "." +
                    std::to_string(expectedVersion.Minor)
                    + "." +
                    std::to_string(expectedVersion.Patch);

                const auto version = FromString<Version>(str);
                ASSERT_TRUE(version.HasValue());
                EXPECT_EQ(version.Value(), expectedVersion);
            }
        }
    }

}
