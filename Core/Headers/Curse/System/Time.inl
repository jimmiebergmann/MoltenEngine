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
        auto time = std::chrono::duration_cast<std::chrono::duration<T> >(m_duration);
        return static_cast<T>(time.count());
    }

    template<typename T>
    inline T Time::AsMilliseconds() const
    {
        auto time = std::chrono::duration_cast<std::chrono::duration<T, std::milli> >(m_duration);
        return static_cast<T>(time.count());
    }

    template<typename T>
    inline T Time::AsMicroseconds() const
    {
        auto time = std::chrono::duration_cast<std::chrono::duration<T, std::micro> >(m_duration);
        return static_cast<T>(time.count());
    }

    template<typename T>
    inline T Time::AsNanoseconds() const
    {
        auto time = std::chrono::duration_cast<std::chrono::duration<T, std::nano> >(m_duration);
        return static_cast<T>(time.count());
    }
    template<>
    inline std::chrono::nanoseconds::rep Time::AsNanoseconds() const
    {
        return m_duration.count();
    }

    template<typename T>
    inline Time Time::operator * (const T scalar) const
    {
        auto count = m_duration.count() * static_cast<std::chrono::nanoseconds::rep>(scalar);
        return { std::chrono::nanoseconds(count) };
    }
    template <>
    inline Time Time::operator * (const float scalar) const
    {
        std::chrono::duration<double, std::nano> dur(m_duration * static_cast<double>(scalar));
        return { std::chrono::duration_cast<std::chrono::nanoseconds>(dur) };
    }
    template <>
    inline Time Time::operator * (const double scalar) const
    {
        std::chrono::duration<double, std::nano> dur(m_duration * scalar);
        return { std::chrono::duration_cast<std::chrono::nanoseconds>(dur) };
    }
    template <>
    inline Time Time::operator * (const long double scalar) const
    {
        std::chrono::duration<double, std::nano> dur(m_duration * static_cast<double>(scalar));
        return { std::chrono::duration_cast<std::chrono::nanoseconds>(dur) };
    }

    template<typename T>
    inline Time& Time::operator *= (const T scalar)
    {
        m_duration *= static_cast<std::chrono::nanoseconds::rep>(scalar);
        return *this;
    }
    template <>
    inline Time& Time::operator *= (const float scalar)
    {
        std::chrono::duration<double, std::nano> dur(m_duration * static_cast<double>(scalar));
        m_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
        return *this;
    }
    template <>
    inline Time& Time::operator *= (const double scalar)
    {
        std::chrono::duration<double, std::nano> dur(m_duration * scalar);
        m_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
        return *this;
    }
    template <>
    inline Time& Time::operator *= (const long double scalar)
    {
        std::chrono::duration<double, std::nano> dur(m_duration * static_cast<double>(scalar));
        m_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
        return *this;
    }

    template<typename T>
    inline Time Time::operator / (const T scalar) const
    {
        auto count = m_duration.count() / static_cast<std::chrono::nanoseconds::rep>(scalar);
        return { std::chrono::nanoseconds(count) };
    }
    template <>
    inline Time Time::operator / (const float scalar) const
    {
        std::chrono::duration<double, std::nano> dur(m_duration / static_cast<double>(scalar));
        return { std::chrono::duration_cast<std::chrono::nanoseconds>(dur) };
    }
    template <>
    inline Time Time::operator / (const double scalar) const
    {
        std::chrono::duration<double, std::nano> dur(m_duration / scalar);
        return { std::chrono::duration_cast<std::chrono::nanoseconds>(dur) };
    }
    template <>
    inline Time Time::operator / (const long double scalar) const
    {
        std::chrono::duration<double, std::nano> dur(m_duration / static_cast<double>(scalar));
        return { std::chrono::duration_cast<std::chrono::nanoseconds>(dur) };
    }

    template<typename T>
    inline Time& Time::operator /= (const T scalar)
    {
        m_duration /= static_cast<std::chrono::nanoseconds::rep>(scalar);
        return *this;
    }
    template <>
    inline Time& Time::operator /= (const float scalar)
    {
        std::chrono::duration<double, std::nano> dur(m_duration / static_cast<double>(scalar));
        m_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
        return *this;
    }
    template <>
    inline Time& Time::operator /= (const double scalar)
    {
        std::chrono::duration<double, std::nano> dur(m_duration / scalar);
        m_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
        return *this;
    }
    template <>
    inline Time& Time::operator /= (const long double scalar)
    {
        std::chrono::duration<double, std::nano> dur(m_duration / static_cast<double>(scalar));
        m_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
        return *this;
    }

    template<typename T>
    inline Time Seconds(const T seconds)
    {   
        std::chrono::duration<T> durBase(seconds);
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(durBase);
        return { std::chrono::nanoseconds(dur) };
    }

    template<typename T>
    inline Time Milliseconds(const T milliseconds)
    {
        std::chrono::duration<T, std::milli> durBase(milliseconds);
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(durBase);
        return { std::chrono::nanoseconds(dur) };
    }

    template<typename T>
    inline Time Microseconds(const T microseconds)
    {
        std::chrono::duration<T, std::micro> durBase(microseconds);
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(durBase);
        return { std::chrono::nanoseconds(dur) };
    }

    template<typename T>
    inline Time Nanoseconds(const T nanoseconds)
    {
        std::chrono::duration<T, std::nano> durBase(nanoseconds);
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(durBase);
        return { std::chrono::nanoseconds(dur) };
    }

}