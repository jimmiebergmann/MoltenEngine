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

#ifndef MOLTEN_CORE_SYSTEM_TIME_HPP
#define MOLTEN_CORE_SYSTEM_TIME_HPP

#include "Molten/Types.hpp"
#include <chrono>

namespace Molten
{

    /** Time class. */
    class MOLTEN_API Time
    {

    public:

        /** Constant representation of zero time. */
        static const Time Zero;


        /** Constructor. Time is initialized as 0. */
        Time();


        /** Get time as seconds, as type T. */
        template<typename T>
        T AsSeconds() const;

        /** Get time as milliseconds, as type T. */
        template<typename T>
        T AsMilliseconds() const;

        /** Get time as microseconds, as type T. */
        template<typename T>
        T AsMicroseconds() const;

        /** Get time as nanoseconds, as type T. */
        template<typename T>
        T AsNanoseconds() const;


        /** Operators. */
        /**@{*/
        Time operator + (const Time& time) const;
        Time operator - (const Time& time) const;
        template<typename T>
        Time operator * (const T scalar) const;
        template<typename T>
        Time operator / (const T scalar) const;
        Time operator % (const Time& time) const;
        /**@}*/


        /** Assigment operators. */
        /**@{*/
        Time& operator += (const Time& time);
        Time& operator -= (const Time& time);
        template<typename T>
        Time& operator *= (const T scalar);
        template<typename T>
        Time& operator /= (const T scalar);
        Time& operator %= (const Time& time);
        /**@}*/


        /** Compare operators. */
        /**@{*/
        bool operator == (const Time& time) const;
        bool operator != (const Time& time) const;
        bool operator < (const Time& time) const;
        bool operator <= (const Time& time) const;
        bool operator > (const Time& time) const;
        bool operator >= (const Time& time) const;
        /**@}*/


        /** Static function to retreive the current system time. */
        static Time GetSystemTime();

    private:

        Time(const std::chrono::nanoseconds nanoseconds);

        std::chrono::nanoseconds m_duration;

        template<typename T> friend Time Seconds(const T seconds);
        template<typename T> friend Time Milliseconds(const T milliseconds);
        template<typename T> friend Time Microseconds(const T microseconds);
        template<typename T> friend Time Nanoseconds(const T nanoseconds);

    };


    /** Function for constructing a time object in seconds. */
    template<typename T>
    Time Seconds(const T seconds);

    /** Function for constructing a time object in milliseconds. */
    template<typename T>
    Time Milliseconds(const T milliseconds);

    /** Function for constructing a time object in microseconds. */
    template<typename T>
    Time Microseconds(const T microseconds);

    /**  Function for constructing a time object in nanoseconds. */
    template<typename T>
    Time Nanoseconds(const T nanoseconds);


}

#include "Molten/System/Time.inl"

#endif