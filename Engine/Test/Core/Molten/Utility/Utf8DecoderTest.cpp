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
#include "Molten/Utility/Utf8Decoder.hpp"
#include <array>

namespace Molten
{
    TEST(Utility, Utf8Decoder_CompareOperator)
    {
        {
            Utf8Decoder decoder;
            EXPECT_TRUE(decoder.IsEmpty());

            EXPECT_TRUE(decoder == decoder);
            EXPECT_FALSE(decoder != decoder);
        }
        {
            const char* string = "Hello world";
            Utf8Decoder decoder1(string);

            EXPECT_TRUE(decoder1 == decoder1);
            EXPECT_FALSE(decoder1 != decoder1);

            Utf8Decoder decoder2(string);

            EXPECT_TRUE(decoder2 == decoder2);
            EXPECT_FALSE(decoder2 != decoder2);
        }
        {
            auto decoder1 = Utf8Decoder{ "Car" };
            auto decoder2 = Utf8Decoder{ "Dog" };

            EXPECT_FALSE(decoder1 == decoder2);
            EXPECT_TRUE(decoder1 != decoder2);
        }
    }

    TEST(Utility, Utf8Decoder_1ByteCodePage)
    {
        const std::string string = "The quick brown fox jumps over the lazy dog!";
        auto decoder = Utf8Decoder{ string.c_str() };

        EXPECT_FALSE(decoder.IsEmpty());

        {
            size_t index = 0;
            for (auto it = decoder.begin(); it != decoder.end(); ++it)
            {
                ASSERT_LT(index, string.size());
                auto codePoint = *it;
                EXPECT_EQ(static_cast<uint32_t>(string.at(index)), codePoint);
                ++index;
            }
            EXPECT_EQ(index, string.size());
        }
        {
            size_t index = 0;
            for (auto codePoint : decoder)
            {
                ASSERT_LT(index, string.size());
                EXPECT_EQ(static_cast<uint32_t>(string.at(index)), codePoint);
                ++index;
            }
            EXPECT_EQ(index, string.size());
        }
    }

    TEST(Utility, Utf8Decoder_2ByteCodePage)
    {
        const std::string string = u8"διαφυλάξτε γενικά τη ζωή σας από βαθειά ψυχικά";
        const std::array<uint32_t, 46> stringCodePoints =
        {
            948, 953, 945, 966, 965, 955, 940, 958, 964, 949, 32, 947, 949, 957, 953,
            954, 940, 32, 964, 951, 32, 950, 969, 942, 32, 963, 945, 962, 32, 945, 960,
            972, 32, 946, 945, 952, 949, 953, 940, 32, 968, 965, 967, 953, 954, 940
        };

        auto decoder = Utf8Decoder{ string.c_str() };

        EXPECT_FALSE(decoder.IsEmpty());

        {
            size_t index = 0;
            for (auto it = decoder.begin(); it != decoder.end(); ++it)
            {
                ASSERT_LT(index, stringCodePoints.size());
                auto codePoint = *it;
                EXPECT_EQ(stringCodePoints.at(index), codePoint);
                ++index;
            }
            EXPECT_EQ(index, stringCodePoints.size());
        }
        {
            size_t index = 0;
            for (auto codePoint : decoder)
            {
                ASSERT_LT(index, stringCodePoints.size());
                EXPECT_EQ(stringCodePoints.at(index), codePoint);
                ++index;
            }
            EXPECT_EQ(index, stringCodePoints.size());
        }
    }

    TEST(Utility, Utf8Decoder_3ByteCodePage)
    {
        const std::string string = u8"色は匂へど散りぬるを我が世誰ぞ常ならん有為の奥山今日越えて浅き夢見じ酔ひもせず";
        const std::array<uint32_t, 39> stringCodePoints =
        {
            33394, 12399, 21250, 12408, 12393, 25955, 12426, 12396, 12427, 12434, 25105, 12364, 19990,
            35504, 12382, 24120, 12394, 12425, 12435, 26377, 28858, 12398, 22885, 23665, 20170, 26085,
            36234, 12360, 12390, 27973, 12365, 22818, 35211, 12376, 37204, 12402, 12418, 12379, 12378
        };

        auto decoder = Utf8Decoder{ string.c_str() };

        EXPECT_FALSE(decoder.IsEmpty());

        {
            size_t index = 0;
            for (auto it = decoder.begin(); it != decoder.end(); ++it)
            {
                ASSERT_LT(index, stringCodePoints.size());
                auto codePoint = *it;
                EXPECT_EQ(stringCodePoints.at(index), codePoint);
                ++index;
            }
            EXPECT_EQ(index, stringCodePoints.size());
        }
        {
            size_t index = 0;
            for (auto codePoint : decoder)
            {
                ASSERT_LT(index, stringCodePoints.size());
                EXPECT_EQ(stringCodePoints.at(index), codePoint);
                ++index;
            }
            EXPECT_EQ(index, stringCodePoints.size());
        }
    }

    TEST(Utility, Utf8Decoder_4ByteCodePage)
    {
        const std::string string = u8"𝄀𝄁𝄂𝄃𝄄𝄅𝄆𝄇";
        const std::array<uint32_t, 8> stringCodePoints =
        {
            119040, 119041, 119042, 119043, 119044, 119045, 119046, 119047
        };

        auto decoder = Utf8Decoder{ string.c_str() };

        EXPECT_FALSE(decoder.IsEmpty());

        {
            size_t index = 0;
            for (auto it = decoder.begin(); it != decoder.end(); ++it)
            {
                ASSERT_LT(index, stringCodePoints.size());
                auto codePoint = *it;
                EXPECT_EQ(stringCodePoints.at(index), codePoint);
                ++index;
            }
            EXPECT_EQ(index, stringCodePoints.size());
        }
        {
            size_t index = 0;
            for (auto codePoint : decoder)
            {
                ASSERT_LT(index, stringCodePoints.size());
                EXPECT_EQ(stringCodePoints.at(index), codePoint);
                ++index;
            }
            EXPECT_EQ(index, stringCodePoints.size());
        }
    }
    
}