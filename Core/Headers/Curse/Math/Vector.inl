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

    // Vector D.
    template<size_t D, typename T>
    inline Vector<D, T>::Vector()
    {
    }

    // Vector 2.
    template<typename T>
    inline Vector<2, T>::Vector()
    {
    }

    template<typename T>
    template<typename U>
    inline Vector<2, T>::Vector(const U xy) :
        x(static_cast<T>(xy)), y(static_cast<T>(xy))
    {
    }

    template<typename T>
    template<typename U1, typename U2>
    inline Vector<2, T>::Vector(const U1 x, const U2 y) :
       x(static_cast<T>(x)), y(static_cast<T>(y))
    {
    }

    template<typename T>
    inline Vector<2, T> Vector<2, T>::Absolute() const
    {
        return Vector2<T>(std::abs(x), std::abs(y));
    }

    template<typename T>
    template<typename U>
    inline U Vector<2, T>::Dot(const Vector<2, T>& vector) const
    {
        return static_cast<U>((x * vector.x) + (y * vector.y));
    }

    template<typename T>
    template<typename U>
    inline U Vector<2, T>::Length() const
    {
        return static_cast<U>(std::sqrt((x * x) + (y * y)));
    }

    template<typename T>
    inline Vector<2, T> Vector<2, T>::Normal() const
    {
        T len = static_cast<T>(std::sqrt((x * x) + (y * y)));
        if (len == static_cast<T>(0))
        {
            return { static_cast<T>(0), static_cast<T>(0) };
        }
        return { x / len, y / len };
    }
    template<>
    inline Vector<2, float> Vector<2, float>::Normal() const
    {
        auto len = static_cast<float>(std::sqrt((x * x) + (y * y)));
        if (len == 0.0f)
        {
            return { 0.0f, 0.0f };
        }
        len = 1.0f / len;
        return { x * len, y * len };
    }
    template<>
    inline Vector<2, double> Vector<2, double>::Normal() const
    {
        auto len = static_cast<double>(std::sqrt((x * x) + (y * y)));
        if (len == 0.0f)
        {
            return { 0.0f, 0.0f };
        }
        len = 1.0f / len;
        return { x * len, y * len };
    }
    template<>
    inline Vector<2, long double> Vector<2, long double>::Normal() const
    {
        auto len = static_cast<long double>(std::sqrt((x * x) + (y * y)));
        if (len == 0.0f)
        {
            return { 0.0f, 0.0f };
        }
        len = 1.0f / len;
        return { x * len, y * len };
    }

    template<typename T>
    inline Vector<2, T>& Vector<2, T>::Normalize()
    {
        return *this = Normal();
    }

    template<typename T>
    inline Vector<2, T> Vector<2, T>::operator + (const Vector<2, T>& vector) const
    {
        return { x + vector.x, y + vector.y };
    }

    template<typename T>
    inline Vector<2, T>& Vector<2, T>::operator += (const Vector<2, T>& vector)
    {
        x += vector.x;
        y += vector.y;
        return *this;
    }

    template<typename T>
    Vector<2, T> Vector<2, T>::operator - (const Vector<2, T>& vector) const
    {
        return { x - vector.x, y - vector.y };
    }

    template<typename T>
    inline Vector<2, T>& Vector<2, T>::operator -= (const Vector<2, T>& vector)
    {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }

    template<typename T>
    inline Vector<2, T> Vector<2, T>::operator * (const Vector<2, T>& vector) const
    {
        return { x * vector.x, y * vector.y };
    }
    template<typename T>
    inline Vector<2, T> Vector<2, T>::operator * (const T scalar) const
    {
        return { x * scalar, y * scalar };
    }

    template<typename T>
    inline Vector<2, T>& Vector<2, T>::operator *= (const Vector<2, T>& vector)
    {
        x *= vector.x;
        y *= vector.y;
        return *this;
    }
    template<typename T>
    inline Vector<2, T>& Vector<2, T>::operator *= (const T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    template<typename T>
    inline Vector<2, T> Vector<2, T>::operator / (const Vector<2, T>& vector) const
    {
        return { x / vector.x, y / vector.y };
    }
    template<typename T>
    inline Vector<2, T> Vector<2, T>::operator / (const T scalar) const
    {
        return { x / scalar, y / scalar };
    }

    template<typename T>
    inline Vector<2, T>& Vector<2, T>::operator /= (const Vector<2, T>& vector)
    {
        x /= vector.x;
        y /= vector.y;
        return *this;
    }
    template<typename T>
    inline Vector<2, T>& Vector<2, T>::operator /= (const T scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    template<typename T>
    inline bool Vector<2, T>::operator == (const Vector<2, T>& vector) const
    {
        return x == vector.x && y == vector.y;
    }

    template<typename T>
    inline bool Vector<2, T>::operator != (const Vector<2, T>& vector) const
    {
        return x != vector.x || y != vector.y;
    }

}