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

#include "gtest/gtest.h"
#include "Curse/System/Time.hpp"

namespace Curse
{
   
    TEST(System, Time_Construction)
    {
        {
            EXPECT_EQ(Seconds<float>(1.0f).AsSeconds<float>(), 1.0f);
            EXPECT_EQ(Seconds<float>(2.0f).AsSeconds<float>(), 2.0f);
            EXPECT_EQ(Seconds<float>(-1.0f).AsSeconds<float>(), -1.0f);
            EXPECT_EQ(Seconds<float>(-2.0f).AsSeconds<float>(), -2.0f);
            EXPECT_EQ(Seconds<int32_t>(12345).AsSeconds<int32_t>(), int32_t(12345));
            EXPECT_EQ(Seconds<int32_t>(-12345).AsSeconds<int32_t>(), int32_t(-12345));
            EXPECT_EQ(Seconds<float>(1.5f).AsSeconds<int32_t>(), int32_t(1));
  
            EXPECT_EQ(Seconds<int64_t>(2).AsSeconds<int64_t>(), 2LL);
            EXPECT_EQ(Seconds<int64_t>(2).AsMilliseconds<int64_t>(), 2000LL);
            EXPECT_EQ(Seconds<int64_t>(2).AsMicroseconds<int64_t>(), 2000000LL);
            EXPECT_EQ(Seconds<int64_t>(2).AsNanoseconds<int64_t>(), 2000000000LL);
        }
        {
            EXPECT_EQ(Milliseconds<float>(1.0f).AsSeconds<float>(), 0.001f);
            EXPECT_EQ(Milliseconds<float>(2.0f).AsSeconds<float>(), 0.002f);
            EXPECT_EQ(Milliseconds<float>(-1.0f).AsSeconds<float>(), -0.001f);
            EXPECT_EQ(Milliseconds<float>(-2.0f).AsSeconds<float>(), -0.002f);
            EXPECT_EQ(Milliseconds<int32_t>(12345).AsSeconds<int32_t>(), int32_t(12));
            EXPECT_EQ(Milliseconds<int32_t>(-12345).AsSeconds<int32_t>(), int32_t(-12));
            EXPECT_EQ(Milliseconds<float>(1000.5f).AsSeconds<int32_t>(), int32_t(1));
        }
        {
            EXPECT_EQ(Microseconds<float>(1.0f).AsMilliseconds<float>(), 0.001f);
            EXPECT_EQ(Microseconds<float>(2.0f).AsMilliseconds<float>(), 0.002f);
            EXPECT_EQ(Microseconds<float>(-1.0f).AsMilliseconds<float>(), -0.001f);
            EXPECT_EQ(Microseconds<float>(-2.0f).AsMilliseconds<float>(), -0.002f);
            EXPECT_EQ(Microseconds<int32_t>(12345).AsMilliseconds<int32_t>(), int32_t(12));
            EXPECT_EQ(Microseconds<int32_t>(-12345).AsMilliseconds<int32_t>(), int32_t(-12));
            EXPECT_EQ(Microseconds<float>(1000.5f).AsMilliseconds<int32_t>(), int32_t(1));
        }
        {
            EXPECT_EQ(Nanoseconds<float>(1.0f).AsMicroseconds<float>(), 0.001f);
            EXPECT_EQ(Nanoseconds<float>(2.0f).AsMicroseconds<float>(), 0.002f);
            EXPECT_EQ(Nanoseconds<float>(-1.0f).AsMicroseconds<float>(), -0.001f);
            EXPECT_EQ(Nanoseconds<float>(-2.0f).AsMicroseconds<float>(), -0.002f);
            EXPECT_EQ(Nanoseconds<int32_t>(12345).AsMicroseconds<int32_t>(), int32_t(12));
            EXPECT_EQ(Nanoseconds<int32_t>(-12345).AsMicroseconds<int32_t>(), int32_t(-12));
            EXPECT_EQ(Nanoseconds<float>(1000.5f).AsMicroseconds<int32_t>(), int32_t(1));
        }
    }
    TEST(System, Time_Operators)
    {
        
        {
            {
                Time time = Seconds<int32_t>(3000) - Seconds<int32_t>(1000);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(2000));
            }
            {
                Time time = Seconds<int32_t>(5000) - Seconds<int32_t>(6000);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(-1000));
            }
            {
                Time time = Seconds<int32_t>(5000) - Seconds<int32_t>(-6000);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(11000));
            }
        }
        {
            {
                Time time = Seconds<int32_t>(3000) + Seconds<int32_t>(1000);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(4000));
            }
            {
                Time time = Seconds<int32_t>(5000) + Seconds<int32_t>(6000);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(11000));
            }
            {
                Time time = Seconds<int32_t>(5000) + Seconds<int32_t>(-6000);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(-1000));
            }
        }
        {
            {
                Time time = Seconds<int32_t>(5) * int32_t(7);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(35));
            }
            {
                Time time = Seconds<int32_t>(5) * float(6.0f);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(30));
            }
            {
                Time time = Seconds<int32_t>(5) * double(-6.5f);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(-32));
            }
            {
                Time time = Seconds<int32_t>(5) * long double(-6.5f);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(-32));
            }
        }
        {
            {
                Time time = Seconds<int32_t>(5) ;
                time *= int32_t(7);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(35));
            }
            {
                Time time = Seconds<int32_t>(5);
                time *= float(6.0f);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(30));
            }
            {
                Time time = Seconds<int32_t>(5);
                time *= double(-6.5f);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(-32));
            }
            {
                Time time = Seconds<int32_t>(5);
                time *= long double(-6.5f);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(-32));
            }
        }
        {
            {
                Time time = Seconds<int32_t>(300) / int32_t(10);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(30));
            }
            {
                Time time = Seconds<int32_t>(300) / float(1.5f);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(200));
            }
            {
                Time time = Seconds<float>(20.0f) / double(-40.0f);
                EXPECT_EQ(time.AsSeconds<float>(), float(-0.5f));
            }
            {
                Time time = Seconds<float>(20.0f) / long double(-40.0f);
                EXPECT_EQ(time.AsSeconds<float>(), float(-0.5f));
            }
        }
        {
            {
                Time time = Seconds<int32_t>(300);
                time /= int32_t(10);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(30));
            }
            {
                Time time = Seconds<int32_t>(300);
                time /= float(1.5f);
                EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(200));
            }
            {
                Time time = Seconds<float>(20.0f);
                time /= double(-40.0f);
                EXPECT_EQ(time.AsSeconds<float>(), float(-0.5f));
            }
            {
                Time time = Seconds<float>(20.0f);
                time /= long double(-40.0f);
                EXPECT_EQ(time.AsSeconds<float>(), float(-0.5f));
            }
        }
        {
            {
                Time time = Seconds<int32_t>(300) % Seconds<int32_t>(10);
                EXPECT_EQ(time, Seconds<int32_t>(0));
            }
            {
                Time time = Seconds<int32_t>(300) % Seconds<int32_t>(14);
                EXPECT_EQ(time, Seconds<int32_t>(6));
            }
        }
        {
            EXPECT_FALSE(Seconds<int32_t>(301) == Seconds<int32_t>(300));
            EXPECT_TRUE(Seconds<int32_t>(300) == Seconds<int32_t>(300));
            EXPECT_FALSE(Seconds<int32_t>(299) == Seconds<int32_t>(300));

            EXPECT_TRUE(Seconds<int32_t>(301) != Seconds<int32_t>(300));
            EXPECT_FALSE(Seconds<int32_t>(300) != Seconds<int32_t>(300));
            EXPECT_TRUE(Seconds<int32_t>(299) != Seconds<int32_t>(300));

            EXPECT_FALSE(Seconds<int32_t>(301) < Seconds<int32_t>(300));
            EXPECT_FALSE(Seconds<int32_t>(300) < Seconds<int32_t>(300));
            EXPECT_TRUE(Seconds<int32_t>(299) < Seconds<int32_t>(300));

            EXPECT_FALSE(Seconds<int32_t>(301) <= Seconds<int32_t>(300));
            EXPECT_TRUE(Seconds<int32_t>(300) <= Seconds<int32_t>(300));
            EXPECT_TRUE(Seconds<int32_t>(299) <= Seconds<int32_t>(300));

            EXPECT_TRUE(Seconds<int32_t>(301) > Seconds<int32_t>(300));
            EXPECT_FALSE(Seconds<int32_t>(300) > Seconds<int32_t>(300));
            EXPECT_FALSE(Seconds<int32_t>(299) > Seconds<int32_t>(300));

            EXPECT_TRUE(Seconds<int32_t>(301) >= Seconds<int32_t>(300));
            EXPECT_TRUE(Seconds<int32_t>(300) >= Seconds<int32_t>(300));
            EXPECT_FALSE(Seconds<int32_t>(299) >= Seconds<int32_t>(300));
        }
       
    }

}