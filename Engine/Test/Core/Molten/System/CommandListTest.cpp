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
#include "Molten/System/CommandLine.hpp"
#include <list>
#include <vector>

namespace Molten
{
    TEST(System, CommandLine)
    {
        int intValue = 0;
        std::optional<int> intValueOpt;
        std::string stringValue;
        std::optional<std::string> stringValueOpt;
        bool flag1 = false;
        bool flag2 = false;
        bool flag3 = false;
        bool flag4 = false;
        bool flag5 = false;
        bool flag6 = false;
        std::vector<int> list1;
        std::optional<std::vector<std::string>> listOpt1;

        CliParser parser{
            CliValue{ { "int_1", "int_12" }, intValue },
            CliValue{ { "int_2", "int_22" }, intValueOpt },
            CliValue{ { "string_1", "string_12" }, stringValue },
            CliValue{ { "string_2", "string_22" }, stringValueOpt },
            CliFlag{ { 'a' }, flag1 },
            CliFlag{ { 'b' }, flag2 },
            CliFlag{ { 'c' }, flag3 },
            CliFlag{ { 'd' }, flag4 },
            CliFlag{ { 'e' }, flag5 },
            CliFlag{ { 'f' }, flag6 },
            CliList{ { "list1" }, list1 },
            CliList{ { "list2" }, listOpt1 },
        };

        { // Values    
            {
                std::vector<char*> arguments = {
                    "--int_1", "1234", "--int_2", "5544"
                };

                EXPECT_TRUE(parser.Parse(static_cast<int>(arguments.size()), arguments.data(), false));
                EXPECT_EQ(intValue, int{ 1234 });
                ASSERT_TRUE(intValueOpt.has_value());
                EXPECT_EQ(*intValueOpt, int{ 5544 });
            }
            {
                std::vector<char*> arguments = {
                    "--int_12", "12345", "--int_22", "55445"
                };
                EXPECT_TRUE(parser.Parse(static_cast<int>(arguments.size()), arguments.data(), false));
                EXPECT_EQ(intValue, int{ 12345 });
                ASSERT_TRUE(intValueOpt.has_value());
                EXPECT_EQ(*intValueOpt, int{ 55445 });
            }
            /*{
                // NOT SUPPORTED YET!
                std::vector<const char*> arguments = {
                    "--string_1", "\"Hello, world.\"", "--string_2", "\"This is a test string.\""
                };

                EXPECT_TRUE(parser.Parse(static_cast<int>(arguments.size()), arguments.data(), false));
                EXPECT_STREQ(stringValue.c_str(), "Hello, world.");
                ASSERT_TRUE(stringValueOpt.has_value());
                EXPECT_EQ(stringValueOpt->c_str(), "This is a test string.");
            }
            {
                // NOT SUPPORTED YET!
                std::vector<const char*> arguments = {
                    "--string_12", "\"Hello, other world.\"", "--string_22", "\"This is a test string 2.\""
                };

                EXPECT_TRUE(parser.Parse(static_cast<int>(arguments.size()), arguments.data(), false));
                EXPECT_STREQ(stringValue.c_str(), "Hello, other world.");
                ASSERT_TRUE(stringValueOpt.has_value());
                EXPECT_EQ(stringValueOpt->c_str(), "This is a test string 2.");
            }*/

        }
        { // Flags
            {
                std::vector<char*> arguments = {
                    "-abcdef"
                };

                EXPECT_TRUE(parser.Parse(static_cast<int>(arguments.size()), arguments.data(), false));
                EXPECT_TRUE(flag1);
                EXPECT_TRUE(flag2);
                EXPECT_TRUE(flag3);
                EXPECT_TRUE(flag4);
                EXPECT_TRUE(flag5);
                EXPECT_TRUE(flag6);
            }
            {
                std::vector<char*> arguments = {
                    "-a" ,"-cd", "-f"
                };

                EXPECT_TRUE(parser.Parse(static_cast<int>(arguments.size()), arguments.data(), false));
                EXPECT_TRUE(flag1);
                EXPECT_FALSE(flag2);
                EXPECT_TRUE(flag3);
                EXPECT_TRUE(flag4);
                EXPECT_FALSE(flag5);
                EXPECT_TRUE(flag6);
            }
        }
        { // List
            {
                std::vector<char*> arguments = {
                    "--list1", "1", "2", "3", "10000", "--list2", "hello", "world"
                };

                EXPECT_TRUE(parser.Parse(static_cast<int>(arguments.size()), arguments.data(), false));
                ASSERT_EQ(list1.size(), size_t{ 4 });
                EXPECT_EQ(list1[0], int{ 1 });
                EXPECT_EQ(list1[1], int{ 2 });
                EXPECT_EQ(list1[2], int{ 3 });
                EXPECT_EQ(list1[3], int{ 10000 });

                ASSERT_TRUE(listOpt1.has_value());
                EXPECT_EQ(listOpt1->size(), size_t{ 2 });
                EXPECT_STREQ(listOpt1->at(0).c_str(), "hello");
                EXPECT_STREQ(listOpt1->at(1).c_str(), "world");
            }
        }
        
    }

}