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

#ifndef MOLTEN_CORE_MATH_ANGLE_HPP
#define MOLTEN_CORE_MATH_ANGLE_HPP

namespace Molten
{

    /**
    * @brief Angle class.
    */
    class Angle
    {

    public:

        Angle();

        template<typename T>
        T AsRadians() const;

        template<typename T>
        T AsDegrees() const;

        /**
        * @brief Get normalized angle by range [0, 2pi](0, 360 degrees)
        */
        Angle Normal() const;

        /**
        * @brief Normalize this angle by range [0, 2pi](0, 360 degrees)
        */
        Angle& Normalize();

        bool operator ==(const Angle& angle) const;
        bool operator !=(const Angle& angle) const;

        Angle operator +(const Angle& angle) const;
        Angle operator -(const Angle& angle) const;
        Angle operator *(const Angle& angle) const;
        Angle operator /(const Angle& angle) const;

        Angle& operator +=(const Angle& angle);
        Angle& operator -=(const Angle& angle);
        Angle& operator *=(const Angle& angle);
        Angle& operator /=(const Angle& angle);

    private:

        explicit Angle(double radians);

        double m_radians;

        template<typename T> friend Angle Radians(const T radians);
        template<typename T> friend Angle Degrees(const T degrees);

    };

    template<typename T>
    Angle Radians(const T radians);

    template<typename T>
    Angle Degrees(const T degrees);

}

#include "Molten/Math/Angle.inl"

#endif