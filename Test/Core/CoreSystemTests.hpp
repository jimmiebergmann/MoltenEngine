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

#ifndef CURSE_TEST_CORE_SYSTEM_HPP
#define CURSE_TEST_CORE_SYSTEM_HPP

#include "gtest/gtest.h"
#include "Curse/System/Time.hpp"
#include "Curse/System/Semaphore.hpp"
#include <thread>

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
        {
            Time time = Seconds<int32_t>(5) * 6.0f;
            EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(30));
        }
        {
            Time time = Seconds<int32_t>(5) * -6.5f;
            EXPECT_EQ(time.AsSeconds<int32_t>(), int32_t(-32));
        }
    }

    TEST(System, Semaphore)
    {
        {
            Semaphore sem;
            EXPECT_EQ(sem.GetWaitCount(), size_t(0));
            sem.NotifyOne();
            EXPECT_EQ(sem.GetWaitCount(), size_t(0));
            sem.Wait();
            EXPECT_EQ(sem.GetWaitCount(), size_t(0));
        }
    }
    TEST(System, Semaphore_NotifyOne)
    {
        {
            Semaphore sem;

            auto thread = std::thread(
                [&sem]()
                {
                    std::this_thread::sleep_for(std::chrono::duration<double>(0.5f));
                    EXPECT_EQ(sem.GetWaitCount(), size_t(1));
                    sem.NotifyOne();
                });
    
            sem.Wait();
            thread.join();

            EXPECT_EQ(sem.GetWaitCount(), size_t(0));
        }
    }
    TEST(System, Semaphore_NotifyAll)
    {
        {
            Semaphore sem;

            const size_t thread_count = 5;
            bool status[thread_count] = { 0 };
            std::thread threads[thread_count];

            auto func = [&sem](bool & status)
            {
                sem.Wait();
                status = true;
            };


            EXPECT_EQ(sem.GetWaitCount(), size_t(0));
            for (size_t i = 0; i < thread_count; i++)
            {
                threads[i] = std::thread(func, std::ref(status[i]));
            }

            std::this_thread::sleep_for(std::chrono::duration<double>(1.0f));
            EXPECT_EQ(sem.GetWaitCount(), thread_count);
            sem.NotifyAll();

            for (size_t i = 0; i < thread_count; i++)
            {
                threads[i].join();
                EXPECT_TRUE(status[i]);
            }

            EXPECT_EQ(sem.GetWaitCount(), size_t(0));
        }
    }

}

#endif