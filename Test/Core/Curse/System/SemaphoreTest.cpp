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
#include "Curse/System/Semaphore.hpp"
#include "Curse/System/Clock.hpp"
#include <thread>

namespace Curse
{

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

    TEST(System, Semaphore_WaitFor)
    {
        {
            Semaphore sem;
            Clock clock;
            sem.WaitFor(Seconds(1));
            auto time = clock.GetTime();
            EXPECT_GE(time, Seconds(0.99f));
            EXPECT_LE(time, Seconds(5.0f));
        }
    }

 }
