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
#include "Molten/Utility/Template.hpp"


namespace Molten
{
    TEST(Utility, Template_VariantIndexByType)
    {
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
    }

}