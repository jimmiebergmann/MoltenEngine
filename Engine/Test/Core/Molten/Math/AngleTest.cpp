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
#include "Molten/Math/Angle.hpp"
#include <type_traits>


namespace Molten
{

    TEST(Math, Angle)
    {
        {
            Angle angle;
            EXPECT_EQ(angle.AsRadians<int32_t>(), int32_t(0));
            EXPECT_NEAR(angle.AsRadians<double>(), 0.0f, 1e-61);

            EXPECT_EQ(angle.AsDegrees<int32_t>(), int32_t(0));
            EXPECT_NEAR(angle.AsDegrees<double>(), 0.0f, 1e-61);
        }
        {
            {
                Angle angle = Degrees(180.0f);

                EXPECT_EQ(angle.AsDegrees<int32_t>(), int32_t(180));
                EXPECT_NEAR(angle.AsDegrees<float>(), float(180.0f), 1e-16);

                EXPECT_NEAR(angle.AsRadians<float>(), Constants::Pi<float>(), 1e-16);
                EXPECT_NEAR(angle.AsRadians<double>(), Constants::Pi<double>(), 1e-61);
                EXPECT_EQ(angle.AsRadians<int32_t>(), int32_t(3));
            }
            {
                Angle angle = Degrees(270);

                EXPECT_EQ(angle.AsDegrees<int32_t>(), int32_t(270));
                EXPECT_NEAR(angle.AsDegrees<float>(), float(270.0f), 1e-16);

                EXPECT_NEAR(angle.AsRadians<float>(), 3.0f * Constants::Pi<float>() / 2.0f, 1e-16);
                EXPECT_NEAR(angle.AsRadians<double>(), 3.0f * Constants::Pi<double>() / 2.0f, 1e-61);
                EXPECT_EQ(angle.AsRadians<int32_t>(), int32_t(4));
            }
        }
        {
            {
                Angle angle1 = Radians(Constants::Pi<float>());
                EXPECT_NEAR(angle1.AsRadians<float>(), Constants::Pi<float>(), 1e-16);

                Angle angle2 = Radians(Constants::Pi<double>());
                EXPECT_NEAR(angle2.AsRadians<double>(), Constants::Pi<double>(), 1e-61);
            }
            {
                Angle angle = Radians(Constants::Pi<double>() / 2.0f);

                EXPECT_NEAR(angle.AsDegrees<double>(), double(90.0f), 1e-61);
            }
        }

    }
}
