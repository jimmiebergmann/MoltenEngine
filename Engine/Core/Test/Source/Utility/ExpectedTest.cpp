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
#include "Molten/Utility/Expected.hpp"
#include <variant>

namespace Molten
{
    TEST(Utility, Expected)
    {
        {
            Expected<int32_t, bool> expected = { 10 };
            EXPECT_TRUE(expected);
            EXPECT_TRUE(expected.HasValue());
            EXPECT_EQ(expected.Value(), int32_t{ 10 });

            expected = { 11 };
            EXPECT_TRUE(expected);
            EXPECT_TRUE(expected.HasValue());
            EXPECT_EQ(expected.Value(), int32_t{ 11 });
        }
        {
            Expected<int32_t, bool> expected = Unexpected(false);
            EXPECT_FALSE(expected);
            EXPECT_FALSE(expected.HasValue());
            EXPECT_EQ(expected.Error(), false);

            expected = Unexpected(true);
            EXPECT_FALSE(expected);
            EXPECT_FALSE(expected.HasValue());
            EXPECT_EQ(expected.Error(), true);
        }
        {
            auto call = [](bool error) -> Expected<int32_t, int32_t>
            {
                if (error)
                {
                    return Unexpected(20);
                }
                return 30;
            };

            {
                auto result = call(true);
                EXPECT_FALSE(result);
                EXPECT_FALSE(result.HasValue());
                EXPECT_EQ(result.Error(), int32_t{ 20 });
            }
            {
                auto result = call(false);
                EXPECT_TRUE(result);
                EXPECT_TRUE(result.HasValue());
                EXPECT_EQ(result.Value(), int32_t{ 30 });
            }
        }
        {
            enum class call_type
            {
                value_1,
                value_2,
                error_1,
                error_2
            };

            auto call = [](const call_type type) -> Expected<std::variant<int32_t, std::string>, std::variant<float, std::string>>
            {
                switch (type)
                {
                    case call_type::value_1: return 10;
                    case call_type::value_2: return "Value";
                    case call_type::error_1: return Unexpected(1.0f);
                    case call_type::error_2: return Unexpected("Error");
                    default: break;
                }

                return "Error";
            };

            {
                auto result = call(call_type::value_1);
                EXPECT_TRUE(result);
                EXPECT_TRUE(result.HasValue());
            }
            {
                auto result = call(call_type::value_2);
                EXPECT_TRUE(result);
                EXPECT_TRUE(result.HasValue());
            }
            {
                auto result = call(call_type::error_1);
                EXPECT_FALSE(result);
                EXPECT_FALSE(result.HasValue());
            }
            {
                auto result = call(call_type::error_2);
                EXPECT_FALSE(result);
                EXPECT_FALSE(result.HasValue());
            }
        }


    }

}