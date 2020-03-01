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

namespace Curse
{

    // Angle implementations.
    inline Angle::Angle() :
        m_radians(0.0f)
    { }

    template<typename T>
    inline T Angle::AsRadians() const
    {
        return static_cast<T>(m_radians);
    }

    template<typename T>
    inline T Angle::AsDegrees() const
    {
        return static_cast<T>(m_radians * 180.0f / Constants::Pi<double>());
    }

    inline Angle Angle::Normal() const
    {
        auto radians = std::fmod(m_radians, Constants::Pi<double>() * 2.0f);
        if (radians < 0.0f)
        {
            radians += Constants::Pi<double>() * 2.0f;
        }
        return { radians };
    }

    inline Angle& Angle::Normalize()
    {
        return *this = Normal();
    }

    inline bool Angle::operator ==(const Angle& angle) const
    {
        return m_radians == angle.m_radians;
    }

    inline bool Angle::operator !=(const Angle& angle) const
    {
        return m_radians != angle.m_radians;
    }

    inline Angle Angle::operator +(const Angle& angle) const
    {
        return Angle(m_radians + angle.m_radians);
    }

    inline Angle Angle::operator -(const Angle& angle) const
    {
        return Angle(m_radians - angle.m_radians);
    }

    inline Angle Angle::operator *(const Angle& angle) const
    {
        return Angle(m_radians * angle.m_radians);
    }

    inline Angle Angle::operator /(const Angle& angle) const
    {
        return Angle(m_radians / angle.m_radians);
    }

    inline Angle& Angle::operator +=(const Angle& angle)
    {
        m_radians += angle.m_radians;
        return *this;
    }

    inline Angle& Angle::operator -=(const Angle& angle)
    {
        m_radians -= angle.m_radians;
        return *this;
    }

    inline Angle& Angle::operator *=(const Angle& angle)
    {
        m_radians *= angle.m_radians;
        return *this;
    }

    inline Angle& Angle::operator /=(const Angle& angle)
    {
        m_radians /= angle.m_radians;
        return *this;
    }

    inline Angle::Angle(double radians) :
        m_radians(radians)
    { }


    // Angle creation implementations.
    template<typename T>
    inline Angle Radians(const T radians)
    {
        return { static_cast<double>(radians) };
    }

    template<typename T>
    inline Angle Degrees(const T degrees)
    {
        return { static_cast<double>(degrees) * Constants::Pi<double>() / 180.0f };
    }

}