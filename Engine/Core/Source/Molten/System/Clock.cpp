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

#include "Molten/System/Clock.hpp"

#if MOLTEN_PLATFORM
#include "Molten/Platform/Win32Headers.hpp"
#else
#include <thread>
#endif

#include <iostream>

namespace Molten
{

    // Clock implementations.
    Clock::Clock(const Time startTime) :
        m_startTime(Time::GetSystemTime() - startTime)
    {
    }

    Time Clock::GetTime() const
    {
        return { Time::GetSystemTime() - m_startTime };
    }

    void Clock::Reset(const Time startTime)
    {
        m_startTime = Time::GetSystemTime() - startTime;
    }


    // Sleep clock implementations.
    SleepClock::SleepClock(const Time sleepTime) :
        m_sleepTime(sleepTime),
        m_startTime(Time::GetSystemTime())
    {}

    void SleepClock::SetSleepTime(const Time sleepTime)
    {
        m_sleepTime = sleepTime;
    }

    Time SleepClock::GetSleepTime() const
    {
        return m_sleepTime;
    }

    void SleepClock::Reset()
    {
        m_startTime = Time::GetSystemTime();
    }

    void SleepClock::Sleep() const
    {
        auto delta = Time::GetSystemTime() - m_startTime;
        auto sleepTime = std::max(m_sleepTime - delta, Nanoseconds(0));
        SleepThreadFor(sleepTime);  
    }

    void SleepClock::PrecisionSleep() const
    {
        if (m_sleepTime == Time::Zero)
        {
            return;
        }

        auto now = Time::GetSystemTime();

        auto frameTime = now - m_startTime;

        auto busyLoopTo = m_sleepTime - frameTime;
        auto sleepTo = busyLoopTo - Microseconds(100);

        SleepThreadFor(sleepTo);

        while (Time::GetSystemTime() < (m_startTime + m_sleepTime))
        {

        }
    }


    // Global functions implementations.
    void SleepThreadFor(const Time& time)
    {
#if MOLTEN_PLATFORM

        struct WaitableTimer
        {
            WaitableTimer() :
                handle(::CreateWaitableTimer(NULL, TRUE, NULL))
            {}

            ~WaitableTimer()
            {
                ::CloseHandle(handle);
            }

            HANDLE handle;
        };

        thread_local WaitableTimer timer = {};

        LARGE_INTEGER dueTime;
        dueTime.QuadPart = -(static_cast<LONGLONG>(time.AsNanoseconds<int64_t>() / 100LL));

        ::SetWaitableTimer(timer.handle, &dueTime, 0, NULL, NULL, FALSE);
        ::WaitForSingleObject(timer.handle, INFINITE);

#else

        std::this_thread::sleep_for(std::chrono::nanoseconds(time.AsNanoseconds<int64_t>()));

#endif
    }

}