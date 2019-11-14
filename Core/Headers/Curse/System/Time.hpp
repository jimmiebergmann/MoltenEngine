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

#ifndef CURSE_CORE_SYSTEM_TIME_HPP
#define CURSE_CORE_SYSTEM_TIME_HPP

#include "Curse/Types.hpp"
#include <chrono>

namespace Curse
{

    /**
    * @brief Time class.
    */
    class CURSE_API Time
    {

    public:

        /**
        * @brief Constructor.
        *        Time is initialized as 0.
        */
        Time();

        /**
        * @brief Get time as seconds.
        *
        * @tparam T data type of return type.
        */
        template<typename T>
        T AsSeconds() const;

        /**
        * @brief Get time as milliseconds.
        *
        * @tparam T data type of return type.
        */
        template<typename T>
        T AsMilliseconds() const;

        /**
        * @brief Get time as microseconds.
        *
        * @tparam T data type of return type.
        */
        template<typename T>
        T AsMicroseconds() const;

        /**
        * @brief Get time as nanoseconds.
        *
        * @tparam T data type of return type.
        */
        template<typename T>
        T AsNanoseconds() const;

        /**
        * @brief Arithmetic addition operator.
        */
        Time operator + (const Time& time) const;

        /**
        * @brief Assignment addition operator.
        */
        Time & operator += (const Time& time);

        /**
        * @brief Time subtraction operator.
        */
        Time operator - (const Time& time) const;

        /**
        * @brief Assignment subtraction operator.
        */
        Time& operator -= (const Time& time);

        /**
        * @brief Time multiplication operator.
        */
        template<typename T>
        Time operator * (const T scalar) const;

        /**
        * @brief Time multiplication assignment operator.
        */
        template<typename T>
        Time & operator *= (const T scalar);

        /**
        * @brief Time division operator.
        */
        template<typename T>
        Time operator / (const T scalar) const;

        /**
        * @brief Time division assignment operator.
        */
        template<typename T>
        Time & operator /= (const T scalar);

        /**
        * @brief Time modulus operator.
        */
        Time operator % (const Time& time) const;

        /**
        * @brief Time modulus assignment operator.
        */
        Time & operator %= (const Time& time);

        /**
        * @brief Equals to time comparation operator.
        */
        bool operator == (const Time& time) const;
        /**
        * @brief Not equals to time comparation operator.
        */
        bool operator != (const Time& time) const;

        /**
        * @brief Less than time comparation operator.
        */
        bool operator < (const Time& time) const;

        /**
        * @brief Less than or equal to time comparation operator.
        */
        bool operator <= (const Time& time) const;

        /**
        * @brief Greater than time comparation operator.
        */
        bool operator > (const Time& time) const;

        /**
        * @brief Greater than or equal to time comparation operator.
        */
        bool operator >= (const Time& time) const;

        /**
        * @brief Static function to retreive the current cpu time.
        */
        static Time GetSystemTime();

        /**
        * @brief Contant representation of zero time.
        */
        static const Time Zero;

    private:

        Time(const std::chrono::nanoseconds nanoseconds);

        std::chrono::nanoseconds m_duration;

        template<typename T> friend Time Seconds(const T seconds);
        template<typename T> friend Time Milliseconds(const T milliseconds);
        template<typename T> friend Time Microseconds(const T microseconds);
        template<typename T> friend Time Nanoseconds(const T nanoseconds);

    };

    /**
    * @brief Function for constructing a time object in seconds.
    */
    template<typename T>
    Time Seconds(const T seconds);

    /**
    * @brief Function for constructing a time object in milliseconds.
    */
    template<typename T>
    Time Milliseconds(const T milliseconds);

    /**
    * @brief Function for constructing a time object in microseconds.
    */
    template<typename T>
    Time Microseconds(const T microseconds);

    /**
    * @brief Function for constructing a time object in nanoseconds.
    */
    template<typename T>
    Time Nanoseconds(const T nanoseconds);

}

#include "Time.inl"

#endif