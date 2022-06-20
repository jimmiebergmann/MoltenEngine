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
#include "Molten/Utility/StringUtility.hpp"
#include <thread>

template<typename T>
void TestTrim()
{
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "" }, " ") }.c_str(), "");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "" }, "\t") }.c_str(), "");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "" }, " \t") }.c_str(), "");

    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ " \t \t \t\t \t \t \t" }, " \t") }.c_str(), "");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\t \t \t\t \t \t \t" }, " \t") }.c_str(), "");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ " \t \t \t\t \t \t \t " }, " \t") }.c_str(), "");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\t \t \t\t \t \t \t " }, " \t") }.c_str(), "");

    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "hello world" }, " ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "hello world " }, " ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "hello world  " }, " ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ " hello world" }, " ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ " hello world " }, " ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ " hello world  " }, " ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "  hello world" }, " ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "  hello world " }, " ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "  hello world  " }, " ") }.c_str(), "hello world");

    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "hello world" }, "\t") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "hello world\t" }, "\t") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "hello world\t\t" }, "\t") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\thello world" }, "\t") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\thello world\t" }, "\t") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\thello world\t\t" }, "\t") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\t\thello world" }, "\t") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\t\thello world\t" }, "\t") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\t\thello world\t\t" }, "\t") }.c_str(), "hello world");

    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "hello world" }, "\t ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "hello world\t\t  \t " }, "\t ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\t\t  \t hello world\t\t  \t " }, "\t ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "\t\t  \t hello world\t\t  \t " }, "\t ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ "hello world \t\t  \t " }, "\t ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ " \t\t  \t hello world \t\t  \t " }, "\t ") }.c_str(), "hello world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::Trim(T{ " \t\t  \t hello world \t\t  \t " }, "\t ") }.c_str(), "hello world");
}

template<typename T>
void TestTrimFront()
{
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "" }, " ") }.c_str(), "");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "" }, "\t") }.c_str(), "");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "" }, " \t") }.c_str(), "");

    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "hello \t world" }, " \t") }.c_str(), "hello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "hello \t world \t" }, " \t") }.c_str(), "hello \t world \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "hello \t world \t " }, " \t ") }.c_str(), "hello \t world \t ");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "hello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");

    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "hello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ " hello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "  hello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "\thello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "\t\thello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ " \thello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ " \t hello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ " \t\thello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "\t hello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimFront(T{ "\t \thello \t world \t \t" }, " \t") }.c_str(), "hello \t world \t \t");
}

template<typename T>
void TestTrimBack()
{
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ "" }, " ") }.c_str(), "");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ "" }, "\t") }.c_str(), "");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ "" }, " \t") }.c_str(), "");

    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ "hello \t world" }, " \t") }.c_str(), "hello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \thello \t world" }, " \t") }.c_str(), " \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t hello \t world" }, " \t ") }.c_str(), " \t hello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world" }, " \t") }.c_str(), " \t \thello \t world");

    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world" }, " \t") }.c_str(), " \t \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world " }, " \t") }.c_str(), " \t \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world  " }, " \t") }.c_str(), " \t \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world\t" }, " \t") }.c_str(), " \t \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world\t\t" }, " \t") }.c_str(), " \t \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world \t" }, " \t") }.c_str(), " \t \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world \t " }, " \t") }.c_str(), " \t \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world \t\t" }, " \t") }.c_str(), " \t \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world\t " }, " \t") }.c_str(), " \t \thello \t world");
    EXPECT_STREQ(std::basic_string<typename T::value_type>{ Molten::StringUtility::TrimBack(T{ " \t \thello \t world\t \t" }, " \t") }.c_str(), " \t \thello \t world");
}

namespace Molten
{
    TEST(Utility, StringUtility_Trim)
    {
        NESTED_TEST_FUNCTION(TestTrim<std::string>())
        NESTED_TEST_FUNCTION(TestTrim<std::string_view>())
    }

    TEST(Utility, StringUtility_TrimNoReturn)
    {
        std::string input = "\t \t  \thello world\t  \t\t ";
        EXPECT_NO_THROW(StringUtility::Trim(input, " \t"));
        EXPECT_STREQ(input.c_str(), "hello world");

        auto inputView = std::string_view{ input };
        EXPECT_NO_THROW(StringUtility::Trim(inputView, " \t"));
        EXPECT_STREQ(std::string{ inputView }.c_str(), "hello world");
    }

    TEST(Utility, StringUtility_TrimFront)
    {
        NESTED_TEST_FUNCTION(TestTrimFront<std::string>())
        NESTED_TEST_FUNCTION(TestTrimFront<std::string_view>())
    }

    TEST(Utility, StringUtility_TrimFrontNoReturn)
    {
        std::string input = "\t \t  \thello world\t  \t\t ";
        EXPECT_NO_THROW(StringUtility::TrimFront(input, " \t"));
        EXPECT_STREQ(input.c_str(), "hello world\t  \t\t ");

        auto inputView = std::string_view{ input };
        EXPECT_NO_THROW(StringUtility::TrimFront(inputView, " \t"));
        EXPECT_STREQ(std::string{ inputView }.c_str(), "hello world\t  \t\t ");
    }

    TEST(Utility, StringUtility_TrimBack)
    {
        NESTED_TEST_FUNCTION(TestTrimBack<std::string>())
        NESTED_TEST_FUNCTION(TestTrimBack<std::string_view>())
    }

    TEST(Utility, StringUtility_TrimBackNoReturn)
    {
        std::string input = "\t \t  \thello world\t  \t\t ";
        EXPECT_NO_THROW(StringUtility::TrimBack(input, " \t"));
        EXPECT_STREQ(input.c_str(), "\t \t  \thello world");

        auto inputView = std::string_view{ input };
        EXPECT_NO_THROW(StringUtility::TrimBack(inputView, " \t"));
        EXPECT_STREQ(std::string{ inputView }.c_str(), "\t \t  \thello world");
    }

    TEST(Utility, StringUtility_TrimBenchmark)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        {
            std::string value = "\t\t  \t \t\t  \t Hello world Hello world Hello world Hello world\t\t  \t \t\t  \t ";
            {
                Molten::Test::Benchmarker bm("Trim_String");

                for(size_t i = 0; i < 10000; i++)
                {
                    value = "\t\t  \t \t\t  \t Hello world Hello world Hello world Hello world\t\t  \t \t\t  \t ";
                    StringUtility::Trim(value, "\t ");
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        {
            std::string value = "\t\t  \t \t\t  \t Hello world Hello world Hello world Hello world\t\t  \t \t\t  \t ";
            {
                Molten::Test::Benchmarker bm("Trim_StringView");

                for (size_t i = 0; i < 10000; i++)
                {
                    value = "\t\t  \t \t\t  \t Hello world Hello world Hello world Hello world\t\t  \t \t\t  \t ";
                    auto stringView = std::string_view{ value };
                    StringUtility::Trim(stringView, "\t ");
                }
            }
        }
    }

}