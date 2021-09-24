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
#include "Molten/Math/Angle.hpp"

namespace Molten
{

    TEST(Math, Angle_Convert)
    {
        {
            Angle angle;
            EXPECT_EQ(angle.AsRadians<int32_t>(), int32_t{ 0 });
            EXPECT_NEAR(angle.AsRadians<double>(), 0.0, 1e-61);

            EXPECT_EQ(angle.AsDegrees<int32_t>(), int32_t{ 0 });
            EXPECT_NEAR(angle.AsDegrees<double>(), 0.0, 1e-61);
        }
        {
            {
                Angle angle = Degrees(180.0f);

                EXPECT_EQ(angle.AsDegrees<int32_t>(), int32_t{180});
                EXPECT_NEAR(angle.AsDegrees<float>(), 180.0f, 1e-16f);

                EXPECT_NEAR(angle.AsRadians<float>(), Constants::Pi<float>, 1e-16f);
                EXPECT_NEAR(angle.AsRadians<double>(), Constants::Pi<double>, 1e-61);
                EXPECT_EQ(angle.AsRadians<int32_t>(), int32_t{ 3 });
            }
            {
                Angle angle = Degrees(270);

                EXPECT_EQ(angle.AsDegrees<int32_t>(), int32_t(270));
                EXPECT_NEAR(angle.AsDegrees<float>(), 270.0f, 1e-16);

                EXPECT_NEAR(angle.AsRadians<float>(), 3.0f * Constants::Pi<float> / 2.0f, 1e-16f);
                EXPECT_NEAR(angle.AsRadians<double>(), 3.0 * Constants::Pi<double> / 2.0, 1e-61);
                EXPECT_EQ(angle.AsRadians<int32_t>(), int32_t{ 4 });
            }
        }
        {
            {
                Angle angle1 = Radians(Constants::Pi<float>);
                EXPECT_NEAR(angle1.AsRadians<float>(), Constants::Pi<float>, 1e-16f);

                Angle angle2 = Radians(Constants::Pi<double>);
                EXPECT_NEAR(angle2.AsRadians<double>(), Constants::Pi<double>, 1e-61);
            }
            {
                Angle angle = Radians(Constants::Pi<double> / 2.0);

                EXPECT_NEAR(angle.AsDegrees<double>(), 90.0, 1e-61);
            }
        }

    }

    TEST(Math, Angle_Normalize)
    {
        {
            {
                const Angle angle = Degrees(-1000).Normal();
                EXPECT_NEAR(angle.AsDegrees<double>(), 80.0, 1e-08);
            }
            {
                const Angle angle = Degrees(-360).Normal();
                EXPECT_NEAR(angle.AsDegrees<double>(), 0.0, 1e-08);
            }
            {
                const Angle angle = Degrees(0).Normal();
                EXPECT_NEAR(angle.AsDegrees<double>(), 0.0, 1e-08);
            }
            {
                const Angle angle = Degrees(250).Normal();
                EXPECT_NEAR(angle.AsDegrees<double>(), 250.0, 1e-08);
            }
            {
                const Angle angle = Degrees(360).Normal();
                EXPECT_NEAR(angle.AsDegrees<double>(), 0.0, 1e-08);
            }
            {
                const Angle angle = Degrees(1000).Normal();
                EXPECT_NEAR(angle.AsDegrees<double>(), 280.0, 1e-08);
            }
            {
                const Angle angle = Degrees(2000).Normal();
                EXPECT_NEAR(angle.AsDegrees<double>(), 200.0, 1e-08);
            }
        }
        {
            {
                Angle angle = Degrees(-1000);
                angle.Normalize();
                EXPECT_NEAR(angle.AsDegrees<double>(), 80.0, 1e-08);
            }
            {
                Angle angle = Degrees(-360);
                angle.Normalize();
                EXPECT_NEAR(angle.AsDegrees<double>(), 0.0, 1e-08);
            }
            {
                Angle angle = Degrees(0);
                angle.Normalize();
                EXPECT_NEAR(angle.AsDegrees<double>(), 0.0, 1e-08);
            }
            {
                Angle angle = Degrees(250);
                angle.Normalize();
                EXPECT_NEAR(angle.AsDegrees<double>(), 250.0, 1e-08);
            }
            {
                Angle angle = Degrees(360);
                angle.Normalize();
                EXPECT_NEAR(angle.AsDegrees<double>(), 0.0, 1e-08);
            }
            {
                Angle angle = Degrees(1000);
                angle.Normalize();
                EXPECT_NEAR(angle.AsDegrees<double>(), 280.0, 1e-08);
            }
            {
                Angle angle = Degrees(2000);
                angle.Normalize();
                EXPECT_NEAR(angle.AsDegrees<double>(), 200.0, 1e-08);
            }
        }
    }

    TEST(Math, Angle_Operators)
    {
        {
            EXPECT_TRUE(Degrees(0) == Degrees(0));
            EXPECT_TRUE(Degrees(250) == Degrees(250));
            EXPECT_TRUE(Degrees(360) == Degrees(360));
            EXPECT_TRUE(Degrees(1000) == Degrees(1000));
            EXPECT_TRUE(Degrees(2000) == Degrees(2000));

            EXPECT_FALSE(Degrees(0) == Degrees(1));
            EXPECT_FALSE(Degrees(250) == Degrees(249));
            EXPECT_FALSE(Degrees(360) == Degrees(361));
            EXPECT_FALSE(Degrees(1000) == Degrees(1001));
            EXPECT_FALSE(Degrees(2000) == Degrees(1999));
        }
        {
            EXPECT_FALSE(Degrees(0) != Degrees(0));
            EXPECT_FALSE(Degrees(250) != Degrees(250));
            EXPECT_FALSE(Degrees(360) != Degrees(360));
            EXPECT_FALSE(Degrees(1000) != Degrees(1000));
            EXPECT_FALSE(Degrees(2000) != Degrees(2000));

            EXPECT_TRUE(Degrees(0) != Degrees(1));
            EXPECT_TRUE(Degrees(250) != Degrees(249));
            EXPECT_TRUE(Degrees(360) != Degrees(361));
            EXPECT_TRUE(Degrees(1000) != Degrees(1001));
            EXPECT_TRUE(Degrees(2000) != Degrees(1999));
        }
        {
            EXPECT_EQ(Degrees(0) + Degrees(0), Degrees(0));
            EXPECT_EQ(Degrees(0) + Degrees(1), Degrees(1));
            EXPECT_EQ(Degrees(1) + Degrees(0), Degrees(1));
            EXPECT_EQ(Degrees(1) + Degrees(1), Degrees(2));

            EXPECT_EQ(Degrees(0) + Degrees(-1), Degrees(-1));
            EXPECT_EQ(Degrees(-1) + Degrees(0), Degrees(-1));
            EXPECT_EQ(Degrees(-1) + Degrees(-1), Degrees(-2));

            EXPECT_EQ(Degrees(1000) + Degrees(2000), Degrees(3000));
        }
        {

            EXPECT_EQ(Degrees(0) += Degrees(0), Degrees(0));
            EXPECT_EQ(Degrees(0) += Degrees(1), Degrees(1));
            EXPECT_EQ(Degrees(1) += Degrees(0), Degrees(1));
            EXPECT_EQ(Degrees(1) += Degrees(1), Degrees(2));

            EXPECT_EQ(Degrees(0) += Degrees(-1), Degrees(-1));
            EXPECT_EQ(Degrees(-1) += Degrees(0), Degrees(-1));
            EXPECT_EQ(Degrees(-1) += Degrees(-1), Degrees(-2));

            EXPECT_EQ(Degrees(1000) += Degrees(2000), Degrees(3000));
        }
        {
            EXPECT_EQ(Degrees(0) - Degrees(0), Degrees(0));
            EXPECT_EQ(Degrees(0) - Degrees(1), Degrees(-1));
            EXPECT_EQ(Degrees(1) - Degrees(0), Degrees(1));
            EXPECT_EQ(Degrees(1) - Degrees(1), Degrees(0));

            EXPECT_EQ(Degrees(0) - Degrees(-1), Degrees(1));
            EXPECT_EQ(Degrees(-1) - Degrees(0), Degrees(-1));
            EXPECT_EQ(Degrees(-1) - Degrees(-1), Degrees(0));

            EXPECT_EQ(Degrees(1000) - Degrees(2000), Degrees(-1000));
        }
        {
            EXPECT_EQ(Degrees(0) -= Degrees(0), Degrees(0));
            EXPECT_EQ(Degrees(0) -= Degrees(1), Degrees(-1));
            EXPECT_EQ(Degrees(1) -= Degrees(0), Degrees(1));
            EXPECT_EQ(Degrees(1) -= Degrees(1), Degrees(0));

            EXPECT_EQ(Degrees(0) -= Degrees(-1), Degrees(1));
            EXPECT_EQ(Degrees(-1) -= Degrees(0), Degrees(-1));
            EXPECT_EQ(Degrees(-1) -= Degrees(-1), Degrees(0));

            EXPECT_EQ(Degrees(1000) -= Degrees(2000), Degrees(-1000));
        }
        {
            EXPECT_NEAR((Radians(0.0f) * Radians(0.0f)).AsRadians<double>(), 0.0, 1e-08);
            EXPECT_NEAR((Radians(0.0f) * Radians(1.0f)).AsRadians<double>(), 0.0, 1e-08);
            EXPECT_NEAR((Radians(1.0f) * Radians(0.0f)).AsRadians<double>(), 0.0, 1e-08);
            EXPECT_NEAR((Radians(2.0f) * Radians(60.0f)).AsRadians<double>(), 120.0, 1e-08);
        }
        {
            EXPECT_NEAR((Radians(0.0f) *= Radians(0.0f)).AsRadians<double>(), 0.0, 1e-08);
            EXPECT_NEAR((Radians(0.0f) *= Radians(1.0f)).AsRadians<double>(), 0.0, 1e-08);
            EXPECT_NEAR((Radians(1.0f) *= Radians(0.0f)).AsRadians<double>(), 0.0, 1e-08);
            EXPECT_NEAR((Radians(2.0f) *= Radians(60.0f)).AsRadians<double>(), 120.0, 1e-08);
        }
        {
            EXPECT_NEAR((Radians(0.0f) / Radians(1.0f)).AsRadians<double>(), 0.0, 1e-08);
            EXPECT_NEAR((Radians(1.0f) / Radians(3.0f)).AsRadians<double>(), 1.0 / 3.0, 1e-08);
            EXPECT_NEAR((Radians(4.0f) / Radians(30.0f)).AsRadians<double>(), 4.0 / 30.0, 1e-08);
        }
        {
            EXPECT_NEAR((Radians(0.0f) /= Radians(1.0f)).AsRadians<double>(), 0.0, 1e-08);
            EXPECT_NEAR((Radians(1.0f) /= Radians(3.0f)).AsRadians<double>(), 1.0 / 3.0, 1e-08);
            EXPECT_NEAR((Radians(4.0f) /= Radians(30.0f)).AsRadians<double>(), 4.0 / 30.0, 1e-08);
        }
    }

}
