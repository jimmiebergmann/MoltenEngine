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
#include "Curse/Math.hpp"
#include <type_traits>

namespace Curse
{

    TEST(Math, Constants)
    {
        {
            constexpr auto pi_1 = Constants::Pi<float>();
            float pi_2 = pi_1;
            EXPECT_TRUE((std::is_same<decltype(pi_2), float>::value));
            EXPECT_NEAR(pi_2, 3.1415927410125732421875f, 1e-16);
        }
        {
            constexpr auto pi_1 = Constants::Pi<double>();
            double pi_2 = pi_1;
            EXPECT_TRUE((std::is_same<decltype(pi_2), double>::value));
            EXPECT_NEAR(pi_2, 3.141592653589793115997963468544185161590576171875, 1e-61);
        }
        {
            constexpr auto pi_1 = Constants::Pi<long double>();
            long double pi_2 = pi_1;
            EXPECT_TRUE((std::is_same<decltype(pi_2), long double>::value));
            EXPECT_NEAR(static_cast<double>(pi_2), 3.141592653589793115997963468544185161590576171875, 1e-61);
        }

    }

    TEST(Math, Power2)
    {
        static_assert(Power2<int32_t>(0) == int32_t(1), "Incorrect power of.");
        static_assert(Power2<int32_t>(1) == int32_t(2), "Incorrect power of.");
        static_assert(Power2<int32_t>(2) == int32_t(4), "Incorrect power of.");
        static_assert(Power2<int32_t>(3) == int32_t(8), "Incorrect power of.");
        static_assert(Power2<int32_t>(4) == int32_t(16), "Incorrect power of.");
        static_assert(Power2<int32_t>(5) == int32_t(32), "Incorrect power of.");
        static_assert(Power2<int32_t>(6) == int32_t(64), "Incorrect power of.");
        static_assert(Power2<int32_t>(7) == int32_t(128), "Incorrect power of.");
        static_assert(Power2<int32_t>(20) == int32_t(1048576), "Incorrect power of.");

        EXPECT_EQ(Power2<int64_t>(0), int32_t(1));
        EXPECT_EQ(Power2<int64_t>(1), int32_t(2));
        EXPECT_EQ(Power2<int64_t>(2), int32_t(4));
        EXPECT_EQ(Power2<int64_t>(20), int32_t(1048576));
    }
}
