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
#include "Molten/Utility/Template.hpp"

template<typename T>
struct Foo
{

};

template<typename T>
struct Bar
{

};

namespace Molten
{
    TEST(Utility, Template_VariantEqualsType)
    { 
        constexpr std::variant<int, float> var1(int{ 3 });
        EXPECT_TRUE(VariantEqualsType<int>(var1));
        EXPECT_FALSE(VariantEqualsType<float>(var1));

        constexpr std::variant<int, float> var2(float{ 2.0f });
        EXPECT_FALSE(VariantEqualsType<int>(var2));
        EXPECT_TRUE(VariantEqualsType<float>(var2));
            
        constexpr std::variant<float, int> var3(int{ 3 });
        EXPECT_TRUE(VariantEqualsType<int>(var3));
        EXPECT_FALSE(VariantEqualsType<float>(var3));

        constexpr std::variant<float, int> var4(float{ 2.0f });
        EXPECT_FALSE(VariantEqualsType<int>(var4));
        EXPECT_TRUE(VariantEqualsType<float>(var4));   
    }

    TEST(Utility, Template_VariantEqualsValue)
    {
        constexpr std::variant<int, float> var1(int{ 3 });
        EXPECT_TRUE(VariantEqualsValue(var1, int{ 3 }));
        EXPECT_FALSE(VariantEqualsValue(var1, float{ 3.0f }));

        constexpr std::variant<int, float> var2(float{ 2.0f });
        EXPECT_FALSE(VariantEqualsValue(var2, int{ 2 }));
        EXPECT_TRUE(VariantEqualsValue(var2, float{ 2.0f }));

        constexpr std::variant<float, int> var3(int{ 3 });
        EXPECT_TRUE(VariantEqualsValue(var3, int{ 3 }));
        EXPECT_FALSE(VariantEqualsValue(var3, float{ 3.0f }));

        constexpr std::variant<float, int> var4(float{ 2.0f });
        EXPECT_FALSE(VariantEqualsValue(var4, int{ 2 }));
        EXPECT_TRUE(VariantEqualsValue(var4, float{ 2.0f }));
    }

    TEST(Utility, Template_IsInstance)
    {
        const auto value = Foo<int>{};

        EXPECT_TRUE((IsTemplateInstance<Foo<int>, Foo>::value));
        EXPECT_TRUE((IsTemplateInstance<Foo<float>, Foo>::value));
        EXPECT_FALSE((IsTemplateInstance<Foo<int>, Bar>::value));
        EXPECT_FALSE((IsTemplateInstance<Foo<float>, Bar>::value));

        EXPECT_FALSE((IsTemplateInstance<Bar<int>, Foo>::value));
        EXPECT_FALSE((IsTemplateInstance<Bar<float>, Foo>::value));
        EXPECT_TRUE((IsTemplateInstance<Bar<int>, Bar>::value));
        EXPECT_TRUE((IsTemplateInstance<Bar<float>, Bar>::value));

        EXPECT_FALSE((IsTemplateInstance<float, Bar>::value));
    }  

}