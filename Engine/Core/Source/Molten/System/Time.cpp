/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#include "Molten/System/Time.hpp"

namespace Molten
{

    const Time Time::Zero;
    const Time Time::Infinite = Nanoseconds(std::chrono::nanoseconds::max().count());

    Time::Time() :
        m_duration(0)
    {}

    Time Time::operator + (const Time& time) const
    {
        return { m_duration + time.m_duration };
    }

    Time Time::operator - (const Time& time) const
    {
        return { m_duration - time.m_duration };
    }

    Time Time::operator % (const Time& time) const
    {
        return { m_duration % time.m_duration };
    }

    Time& Time::operator += (const Time& time)
    {
        m_duration += time.m_duration;
        return *this;
    }

    Time& Time::operator -= (const Time& time)
    {
        m_duration -= time.m_duration;
        return *this;
    }

    Time& Time::operator %= (const Time& time)
    {
        m_duration %= time.m_duration;
        return *this;
    }

    bool Time::operator == (const Time& time) const
    {
        return m_duration == time.m_duration;
    }

    bool Time::operator != (const Time& time) const
    {
        return m_duration != time.m_duration;
    }

    bool Time::operator < (const Time& time) const
    {
        return m_duration < time.m_duration;
    }

    bool Time::operator <= (const Time& time) const
    {
        return m_duration <= time.m_duration;
    }

    bool Time::operator > (const Time& time) const
    {
        return m_duration > time.m_duration;
    }

    bool Time::operator >= (const Time& time) const
    {
        return m_duration >= time.m_duration;
    }

    Time Time::GetSystemTime()
    {      
        return { std::chrono::steady_clock::now().time_since_epoch() };
    }

    Time::Time(const std::chrono::nanoseconds nanoseconds) :
        m_duration(nanoseconds)
    {}
  
}