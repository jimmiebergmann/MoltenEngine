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
#include "Molten/Math.hpp"
#include <type_traits>

namespace Molten
{

    TEST(Math, Constants)
    {
        {
            constexpr auto pi_1 = Constants::Pi<float>;
            float pi_2 = pi_1;
            EXPECT_TRUE((std::is_same_v<decltype(pi_2), float>));
            EXPECT_NEAR(pi_2, 3.1415927410125732421875f, 1e-16);
        }
        {
            constexpr auto pi_1 = Constants::Pi<double>;
            double pi_2 = pi_1;
            EXPECT_TRUE((std::is_same_v<decltype(pi_2), double>));
            EXPECT_NEAR(pi_2, 3.141592653589793115997963468544185161590576171875, 1e-61);
        }
        {
            constexpr auto pi_1 = Constants::Pi<long double>;
            long double pi_2 = pi_1;
            EXPECT_TRUE((std::is_same_v<decltype(pi_2), long double>));
            EXPECT_NEAR(static_cast<double>(pi_2), 3.141592653589793115997963468544185161590576171875, 1e-61);
        }

    }
}
