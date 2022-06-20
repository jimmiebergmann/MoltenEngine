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
#include "Molten/System/Result.hpp"

struct TestData
{
    int32_t a;
    float b;
};

static Molten::Result<TestData, int32_t> CreateSuccess(TestData&& value)
{
    return Molten::Result<TestData, int32_t>::CreateSuccess(std::move(value));
}

static Molten::Result<TestData, int32_t> CreateError(int32_t error)
{
    return Molten::Result<TestData, int32_t>::CreateError(std::move(error));
}

namespace Molten
{
    TEST(System, Result)
    {
        {
            Result<int32_t, int32_t> result = Result<int32_t, int32_t>::CreateSuccess(123);
            EXPECT_TRUE(result);
            EXPECT_EQ(result.Value(), int32_t(123));

            Result<int32_t, int32_t> result2 = std::move(result);
            EXPECT_TRUE(result2);
            EXPECT_EQ(result2.Value(), int32_t(123));

            result2 = std::move(result);
            EXPECT_TRUE(result2);
            EXPECT_EQ(result2.Value(), int32_t(123));
        }
        {
            {
                Result<int32_t, int32_t> result = Result<int32_t, int32_t>::CreateError(404);
                EXPECT_FALSE(result);
                EXPECT_FALSE(result.IsValid());
                EXPECT_EQ(result.Error(), int32_t(404));
            }
            {
                Result<int32_t, int32_t> result(Result<int32_t, int32_t>::CreateError(405));
                EXPECT_FALSE(result);
                EXPECT_FALSE(result.IsValid());
                EXPECT_EQ(result.Error(), int32_t(405));
            }
        }
        {
            {
                auto result = CreateError(-2);
                EXPECT_FALSE(result);
                EXPECT_FALSE(result.IsValid());
                EXPECT_EQ(result.Error(), int32_t(-2));
            }
            {
                auto result = CreateError(65);
                EXPECT_FALSE(result);
                EXPECT_FALSE(result.IsValid());
                EXPECT_EQ(result.Error(), int32_t(65));
            }
        }
        {
            {
                auto result = CreateSuccess({ 2, 3.0f });
                EXPECT_TRUE(result);
                EXPECT_TRUE(result.IsValid());
                EXPECT_EQ(result.Value().a, int32_t(2));
                EXPECT_EQ(result.Value().b, 3.0f);
            }
            {
                auto result = CreateSuccess({ -2, -3.0f });
                EXPECT_TRUE(result);
                EXPECT_TRUE(result.IsValid());
                EXPECT_EQ(result.Value().a, int32_t(-2));
                EXPECT_EQ(result.Value().b, -3.0f);
            }
            {
                auto result = CreateSuccess({ -2, -3.0f });
                EXPECT_TRUE(result);
                EXPECT_TRUE(result.IsValid());
                EXPECT_EQ((*result).a, int32_t(-2));
                EXPECT_EQ(result->a, int32_t(-2));
                EXPECT_EQ((*result).b, -3.0f);
                EXPECT_EQ(result->b, -3.0f);
            }
        }
    }

    TEST(System, Result_Const)
    {
        {
            auto result = CreateSuccess({ 2, 3.0f });
            const auto constResult = std::move(result);
            {
                auto& value = constResult.Value();
                static_assert(std::is_const_v<std::remove_reference_t<decltype(value)>>, "Value is not const.");
            }
            {
                auto& value = *constResult;
                static_assert(std::is_const_v<std::remove_reference_t<decltype(value)>>, "Value is not const.");
            }
            {
                auto& value = *constResult;
                static_assert(std::is_const_v<std::remove_reference_t<decltype(value)>>, "Value is not const.");
            }
        }
        {
            {
                auto result = CreateError(2);
                const auto constResult = std::move(result);
                {
                    auto& value = constResult.Error();
                    static_assert(std::is_const_v<std::remove_reference_t<decltype(value)>>, "Error is not const.");
                }
            }
        }
    }

    TEST(System, Result_NoCopy)
    {

        struct Foo
        {

            Foo() = default;

            Foo(int32_t integer, const std::string& string) : 
                integer(integer),
                string(string)
            {}

            explicit Foo(Foo&& foo) noexcept :
                integer(std::move(foo.integer)),
                string(std::move(foo.string))
            {}

            Foo& operator =(Foo&& foo) noexcept
            {
                integer = std::move(foo.integer);
                string = std::move(foo.string);
                return *this;
            }

            explicit Foo(const Foo& foo) :
                integer(foo.integer),
                string(foo.string)
            {
                ADD_FAILURE();
            }

            Foo& operator =(const Foo&) noexcept
            {
                ADD_FAILURE();
                return *this;
            }

            int32_t integer;
            std::string string;
        };

        {
            {
                auto result = Result<Foo, bool>::CreateSuccess({ 100, "test" });
                EXPECT_TRUE(result);
            }
            {
                Foo foo{ 100, "test" };
                auto result = Result<Foo, bool>::CreateSuccess(std::move(foo));
                EXPECT_TRUE(result);
            }
            {
                auto result = Result<bool, Foo>::CreateError({ 100, "test" });
                EXPECT_FALSE(result);
            }
            {
                Foo foo{ 100, "test" };
                auto result = Result<bool, Foo>::CreateError(std::move(foo));
                EXPECT_FALSE(result);
            }
        }


    }
}