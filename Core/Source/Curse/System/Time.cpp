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

#include "Curse/System/Time.hpp"
#include <iostream>

namespace Curse
{

    const Time Time::Zero;

    Time::Time() :
        m_duration(0)
    {
    }

    Time Time::operator - (const Time& time) const
    {
        Time newTime;
        newTime.m_duration = m_duration - time.m_duration;
        return newTime;
    }

    Time Time::operator + (const Time& time) const
    {
        Time newTime;
        newTime.m_duration = m_duration + time.m_duration;
        return newTime;
    }

    Time Time::operator / (const Time& time) const
    {
        Time newTime;
        newTime.m_duration = std::chrono::nanoseconds(m_duration / time.m_duration);
        return newTime;
    }

    Time Time::GetSystemTime()
    {      
        Time time;        
        time.m_duration = std::chrono::steady_clock::now().time_since_epoch();
        return time;
    }

}