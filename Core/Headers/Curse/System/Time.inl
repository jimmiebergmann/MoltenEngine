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

namespace Curse
{

    template<typename T>
    inline T Time::AsSeconds() const
    {
        return static_cast<T>(m_duration.count() / std::nano::den);
    }

    template<typename T>
    inline T Time::AsMilliseconds() const
    {
        return static_cast<T>(m_duration.count() / std::micro::den);
    }

    template<typename T>
    inline T Time::AsMicroseconds() const
    {
        return static_cast<T>(m_duration.count() / std::milli::den);
    }

    template<typename T>
    inline T Time::AsNanoseconds() const
    {
        return static_cast<T>(m_duration.count());
    }

    template<typename T>
    inline Time Time::operator * (const T multiplier) const
    {
        Time newTime;
        std::chrono::duration<T, std::nano> newDur(m_duration * multiplier);
        newTime.m_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(newDur);
        return newTime;
    }

    template<typename T>
    inline Time Seconds(const T seconds)
    {   
        std::chrono::duration<T> durSecs(seconds);
        auto durNano = std::chrono::duration_cast<std::chrono::nanoseconds>(durSecs);

        Time time;
        time.m_duration = std::chrono::nanoseconds(durNano);
        return time;
    }

    template<typename T>
    inline Time Milliseconds(const T milliseconds)
    {
        std::chrono::duration<T> durSecs(milliseconds);
        auto durNano = std::chrono::duration_cast<std::chrono::nanoseconds>(durSecs);

        Time time;
        time.m_duration = std::chrono::nanoseconds(durNano);
        return time;
    }

    template<typename T>
    inline Time Microseconds(const T microseconds)
    {
        std::chrono::duration<T> durSecs(milliseconds);
        auto durNano = std::chrono::duration_cast<std::chrono::nanoseconds>(durSecs);

        Time time;
        time.m_duration = std::chrono::nanoseconds(durNano);
        return time;
    }

    template<typename T>
    inline Time Nanoseconds(const T nanoseconds)
    {
        std::chrono::duration<T> durSecs(milliseconds);
        auto durNano = std::chrono::duration_cast<std::chrono::nanoseconds>(durSecs);

        Time time;
        time.m_duration = std::chrono::nanoseconds(durNano);
        return time;
    }

}