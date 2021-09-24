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
#include "Molten/Utility/FpsTracker.hpp"

/*
explicit FpsTracker(const size_t averageSampleCount);

void ResetFrameSamples();
void RegisterSampleFrame(const Time frameTime);

[[nodiscard]] Time GetMinFrameTime() const;
[[nodiscard]] Time GetMaxFrameTime() const;
[[nodiscard]] Time GetAverageFrameTime() const;
*/


namespace Molten
{
    TEST(Utility, FpsTracker)
    {
        FpsTracker tracker(3);

        for(size_t i = 0; i < 2; i++)
        {
            EXPECT_EQ(tracker.GetMinFrameTime(), Time::Infinite);
            EXPECT_EQ(tracker.GetMaxFrameTime(), Time::Zero);
            EXPECT_EQ(tracker.GetAverageFrameTime(), Time::Zero);

            //// Milliseconds: 300 600 100 500 200 700 400 200
            // 300
            tracker.RegisterSampleFrame(Milliseconds(300));
            EXPECT_EQ(tracker.GetMinFrameTime(), Milliseconds(300));
            EXPECT_EQ(tracker.GetMaxFrameTime(), Milliseconds(300));
            EXPECT_EQ(tracker.GetAverageFrameTime().AsMilliseconds<double>(), 300.0);

            // 300 600
            tracker.RegisterSampleFrame(Milliseconds(600));
            EXPECT_EQ(tracker.GetMinFrameTime(), Milliseconds(300));
            EXPECT_EQ(tracker.GetMaxFrameTime(), Milliseconds(600));
            EXPECT_NEAR(tracker.GetAverageFrameTime().AsMilliseconds<double>(), static_cast<double>(300 + 600) / 2.0, 1e-05);

            // 300 600 100
            tracker.RegisterSampleFrame(Milliseconds(100));
            EXPECT_EQ(tracker.GetMinFrameTime(), Milliseconds(100));
            EXPECT_EQ(tracker.GetMaxFrameTime(), Milliseconds(600));
            EXPECT_NEAR(tracker.GetAverageFrameTime().AsMilliseconds<double>(), static_cast<double>(300 + 600 + 100) / 3.0, 1e-05);

            // 600 100 500
            tracker.RegisterSampleFrame(Milliseconds(500));
            EXPECT_EQ(tracker.GetMinFrameTime(), Milliseconds(100));
            EXPECT_EQ(tracker.GetMaxFrameTime(), Milliseconds(600));
            EXPECT_NEAR(tracker.GetAverageFrameTime().AsMilliseconds<double>(), static_cast<double>(600 + 100 + 500) / 3.0, 1e-05);

            // 100 500 200
            tracker.RegisterSampleFrame(Milliseconds(200));
            EXPECT_EQ(tracker.GetMinFrameTime(), Milliseconds(100));
            EXPECT_EQ(tracker.GetMaxFrameTime(), Milliseconds(600));
            EXPECT_NEAR(tracker.GetAverageFrameTime().AsMilliseconds<double>(), static_cast<double>(100 + 500 + 200) / 3.0, 1e-05);

            // 500 200 700
            tracker.RegisterSampleFrame(Milliseconds(700));
            EXPECT_EQ(tracker.GetMinFrameTime(), Milliseconds(100));
            EXPECT_EQ(tracker.GetMaxFrameTime(), Milliseconds(700));
            EXPECT_NEAR(tracker.GetAverageFrameTime().AsMilliseconds<double>(), static_cast<double>(500 + 200 + 700) / 3.0, 1e-05);

            // 200 700 400
            tracker.RegisterSampleFrame(Milliseconds(400));
            EXPECT_EQ(tracker.GetMinFrameTime(), Milliseconds(100));
            EXPECT_EQ(tracker.GetMaxFrameTime(), Milliseconds(700));
            EXPECT_NEAR(tracker.GetAverageFrameTime().AsMilliseconds<double>(), static_cast<double>(200 + 700 + 400) / 3.0, 1e-05);

            // 700 400 200
            tracker.RegisterSampleFrame(Milliseconds(200));
            EXPECT_EQ(tracker.GetMinFrameTime(), Milliseconds(100));
            EXPECT_EQ(tracker.GetMaxFrameTime(), Milliseconds(700));
            EXPECT_NEAR(tracker.GetAverageFrameTime().AsMilliseconds<double>(), static_cast<double>(700 + 400 + 200) / 3.0, 1e-05);

            tracker.ResetFrameSamples();
        }
    }

}