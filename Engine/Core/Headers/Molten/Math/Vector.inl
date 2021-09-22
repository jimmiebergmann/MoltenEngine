/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

namespace Molten
{

    // Vector implementations.
    template<size_t D, typename T>
    constexpr Vector<D, T>::Vector() :
        c{}
    {}

    template<size_t D, typename T>
    template<typename U, typename ... Us>
    constexpr Vector<D, T>::Vector(const U element, const Us ... elements) :
        c{ static_cast<T>(element), static_cast<T>(elements)... }
    {}

    template<size_t D, typename T>
    bool Vector<D, T>::operator == (const Vector<D, T>& vector) const
    {
        return std::memcmp(c, vector.c, D) == 0;
    }

    template<size_t D, typename T>
    bool Vector<D, T>::operator != (const Vector<D, T>& vector) const
    {
        return std::memcmp(c, vector.c, D) != 0;
    }

    template<size_t D, typename T>
    T Vector<D, T>::operator[](const size_t index) const
    {
        return c[index];
    }


    // Vector 2 implementations.
    template<typename T>
    constexpr Vector<2, T>::Vector() :
        x{}, y{}
    {}

    template<typename T>
    template<typename U>
    constexpr Vector<2, T>::Vector(const U xy) :
        x(static_cast<T>(xy)), y(static_cast<T>(xy))
    {}

    template<typename T>
    template<typename U1, typename U2>
    constexpr Vector<2, T>::Vector(const U1 x, const U2 y) :
       x(static_cast<T>(x)), y(static_cast<T>(y))
    {}

    template<typename T>
    template<typename U>
    constexpr Vector<2, T>::Vector(const Vector<2, U>& vector) :
        x(static_cast<T>(vector.x)), y(static_cast<T>(vector.y))
    {}

    template<typename T>
    Vector<2, T> Vector<2, T>::Absolute() const
    {
        return Vector2<T>(std::abs(x), std::abs(y));
    }

    template<typename T>
    template<typename R, typename U>
    R Vector<2, T>::Dot(const Vector<2, U>& vector) const
    {
        return static_cast<R>(
            (x * static_cast<T>(vector.x)) + 
            (y * static_cast<T>(vector.y))
        );
    }

    template<typename T>
    template<typename R>
    R Vector<2, T>::Length() const
    {
        return static_cast<R>(std::sqrt((x * x) + (y * y)));
    }

    template<typename T>
    Vector<2, T> Vector<2, T>::Normal() const
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
        if (len == 0.0)
        {
            return { 0.0, 0.0 };
        }
        len = 1.0 / len;
        return { x * len, y * len };
    }
    template<>
    inline Vector<2, long double> Vector<2, long double>::Normal() const
    {
        auto len = static_cast<long double>(std::sqrt((x * x) + (y * y)));
        if (len == 0.0)
        {
            return { 0.0, 0.0 };
        }
        len = 1.0 / len;
        return { x * len, y * len };
    }

    template<typename T>
    Vector<2, T>& Vector<2, T>::Normalize()
    {
        return *this = Normal();
    }

    template<typename T>
    template<typename U>
    Vector<2, T> Vector<2, T>::operator + (const Vector<2, U>& vector) const
    {
        return { x + static_cast<T>(vector.x), y + static_cast<T>(vector.y) };
    }

    template<typename T>
    template<typename U>
    Vector<2, T>& Vector<2, T>::operator += (const Vector<2, U>& vector)
    {
        x += static_cast<T>(vector.x);
        y += static_cast<T>(vector.y);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vector<2, T> Vector<2, T>::operator - (const Vector<2, U>& vector) const
    {
        return { x - static_cast<T>(vector.x), y - static_cast<T>(vector.y) };
    }

    template<typename T>
    template<typename U>
    Vector<2, T>& Vector<2, T>::operator -= (const Vector<2, U>& vector)
    {
        x -= static_cast<T>(vector.x);
        y -= static_cast<T>(vector.y);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vector<2, T> Vector<2, T>::operator * (const Vector<2, U>& vector) const
    {
        return { x * static_cast<T>(vector.x), y * static_cast<T>(vector.y) };
    }
    template<typename T>
    template<typename U>
    Vector<2, T> Vector<2, T>::operator * (const U scalar) const
    {
        return { x * static_cast<T>(scalar), y * static_cast<T>(scalar) };
    }

    template<typename T>
    template<typename U>
    Vector<2, T>& Vector<2, T>::operator *= (const Vector<2, U>& vector)
    {
        x *= static_cast<T>(vector.x);
        y *= static_cast<T>(vector.y);
        return *this;
    }
    template<typename T>
    template<typename U>
    Vector<2, T>& Vector<2, T>::operator *= (const U scalar)
    {
        x *= static_cast<T>(scalar);
        y *= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vector<2, T> Vector<2, T>::operator / (const Vector<2, U>& vector) const
    {
        return { x / static_cast<T>(vector.x), y / static_cast<T>(vector.y) };
    }

    template<typename T>
    template<typename U>
    Vector<2, T> Vector<2, T>::operator / (const U scalar) const
    {
        return { x / static_cast<T>(scalar), y / static_cast<T>(scalar) };
    }

    template<typename T>
    template<typename U>
    Vector<2, T>& Vector<2, T>::operator /= (const Vector<2, U>& vector)
    {
        x /= static_cast<T>(vector.x);
        y /= static_cast<T>(vector.y);
        return *this;
    }
    template<typename T>
    template<typename U>
    Vector<2, T>& Vector<2, T>::operator /= (const U scalar)
    {
        x /= static_cast<T>(scalar);
        y /= static_cast<T>(scalar);
        return *this;
    }
  
    template<typename T>
    template<typename U>
    bool Vector<2, T>::operator == (const Vector<2, U>& vector) const
    {
        return x == static_cast<T>(vector.x) && y == static_cast<T>(vector.y);
    }

    template<typename T>
    template<typename U>
    bool Vector<2, T>::operator != (const Vector<2, U>& vector) const
    {
        return x != static_cast<T>(vector.x) || y != static_cast<T>(vector.y);
    }

    template<typename T>
    T Vector<2, T>::operator[](const size_t index) const
    {
        return c[index];
    }


    // Vector 3 implementations.
    template<typename T>
    constexpr Vector<3, T>::Vector() :
        x{}, y{}, z{}
    {}

    template<typename T>
    template<typename U>
    constexpr Vector<3, T>::Vector(const U xyz) :
        x(static_cast<T>(xyz)), y(static_cast<T>(xyz)), z(static_cast<T>(xyz))
    {}

    template<typename T>
    template<typename U1, typename U2, typename U3>
    constexpr Vector<3, T>::Vector(const U1 x, const U2 y, const U3 z) :
        x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z))
    {}

    template<typename T>
    template<typename U>
    constexpr Vector<3, T>::Vector(const Vector<3, U>& vector) :
        x(static_cast<T>(vector.x)), y(static_cast<T>(vector.y)), z(static_cast<T>(vector.z))
    {}

    template<typename T>
    Vector<3, T> Vector<3, T>::Absolute() const
    {
        return Vector2<T>(std::abs(x), std::abs(y), std::abs(z));
    }

    template<typename T>
    template<typename R, typename U>
    R Vector<3, T>::Dot(const Vector<3, U>& vector) const
    {
        return static_cast<R>(
            (x * static_cast<T>(vector.x)) +
            (y * static_cast<T>(vector.y)) +
            (z * static_cast<T>(vector.z))
        );
    }

    template<typename T>
    template<typename U>
    Vector<3, T> Vector<3, T>::Cross(const Vector<3, U>& vector) const
    {
        return {
            (y * static_cast<T>(vector.z)) - (z * static_cast<T>(vector.y)),
            (z * static_cast<T>(vector.x)) - (x * static_cast<T>(vector.z)),
            (x * static_cast<T>(vector.y)) - (y * static_cast<T>(vector.x))
        };
    }

    template<typename T>
    template<typename R>
    R Vector<3, T>::Length() const
    {
        return static_cast<R>(std::sqrt((x * x) + (y * y) + (z * z)));
    }

    template<typename T>
    Vector<3, T> Vector<3, T>::Normal() const
    {
        T len = static_cast<T>(std::sqrt((x * x) + (y * y) + (z * z)));
        if (len == static_cast<T>(0))
        {
            return { static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        }
        return { x / len, y / len, z / len };
    }
    template<>
    inline Vector<3, float> Vector<3, float>::Normal() const
    {
        auto len = static_cast<float>(std::sqrt((x * x) + (y * y) + (z * z)));
        if (len == 0.0f)
        {
            return { 0.0f, 0.0f, 0.0f };
        }
        len = 1.0f / len;
        return { x * len, y * len, z * len };
    }
    template<>
    inline Vector<3, double> Vector<3, double>::Normal() const
    {
        auto len = static_cast<double>(std::sqrt((x * x) + (y * y) + (z * z)));
        if (len == 0.0)
        {
            return { 0.0, 0.0, 0.0 };
        }
        len = 1.0 / len;
        return { x * len, y * len, z * len };
    }
    template<>
    inline Vector<3, long double> Vector<3, long double>::Normal() const
    {
        auto len = static_cast<long double>(std::sqrt((x * x) + (y * y) + (z * z)));
        if (len == 0.0f)
        {
            return { 0.0, 0.0, 0.0 };
        }
        len = 1.0 / len;
        return { x * len, y * len, z * len };
    }

    template<typename T>
    Vector<3, T>& Vector<3, T>::Normalize()
    {
        return *this = Normal();
    }

    template<typename T>
    template<typename U>
    Vector<3, T> Vector<3, T>::operator + (const Vector<3, U>& vector) const
    {
        return {
            x + static_cast<T>(vector.x),
            y + static_cast<T>(vector.y),
            z + static_cast<T>(vector.z)
        };
    }

    template<typename T>
    template<typename U>
    Vector<3, T>& Vector<3, T>::operator += (const Vector<3, U>& vector)
    {
        x += static_cast<T>(vector.x);
        y += static_cast<T>(vector.y);
        z += static_cast<T>(vector.z);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vector<3, T> Vector<3, T>::operator - (const Vector<3, U>& vector) const
    {
        return {
            x - static_cast<T>(vector.x),
            y - static_cast<T>(vector.y),
            z - static_cast<T>(vector.z)
        };
    }

    template<typename T>
    template<typename U>
    Vector<3, T>& Vector<3, T>::operator -= (const Vector<3, U>& vector)
    {
        x -= static_cast<T>(vector.x);
        y -= static_cast<T>(vector.y);
        z -= static_cast<T>(vector.z);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vector<3, T> Vector<3, T>::operator * (const Vector<3, U>& vector) const
    {
        return {
            x * static_cast<T>(vector.x),
            y * static_cast<T>(vector.y),
            z * static_cast<T>(vector.z)
        };
    }
    template<typename T>
    template<typename U>
    Vector<3, T> Vector<3, T>::operator * (const U scalar) const
    {
        return {
            x * static_cast<T>(scalar),
            y * static_cast<T>(scalar),
            z * static_cast<T>(scalar)
        };
    }

    template<typename T>
    template<typename U>
    Vector<3, T>& Vector<3, T>::operator *= (const Vector<3, U>& vector)
    {
        x *= static_cast<T>(vector.x);
        y *= static_cast<T>(vector.y);
        z *= static_cast<T>(vector.z);
        return *this;
    }
    template<typename T>
    template<typename U>
    Vector<3, T>& Vector<3, T>::operator *= (const U scalar)
    {
        x *= static_cast<T>(scalar);
        y *= static_cast<T>(scalar);
        z *= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vector<3, T> Vector<3, T>::operator / (const Vector<3, U>& vector) const
    {
        return {
            x / static_cast<T>(vector.x),
            y / static_cast<T>(vector.y),
            z / static_cast<T>(vector.z)
        };
    }
    template<typename T>
    template<typename U>
    Vector<3, T> Vector<3, T>::operator / (const U scalar) const
    {
        return {
            x / static_cast<T>(scalar),
            y / static_cast<T>(scalar),
            z / static_cast<T>(scalar)
        };
    }

    template<typename T>
    template<typename U>
    Vector<3, T>& Vector<3, T>::operator /= (const Vector<3, U>& vector)
    {
        x /= static_cast<T>(vector.x);
        y /= static_cast<T>(vector.y);
        z /= static_cast<T>(vector.z);
        return *this;
    }
    template<typename T>
    template<typename U>
    Vector<3, T>& Vector<3, T>::operator /= (const U scalar)
    {
        x /= static_cast<T>(scalar);
        y /= static_cast<T>(scalar);
        y /= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    bool Vector<3, T>::operator == (const Vector<3, U>& vector) const
    {
        return
            x == static_cast<T>(vector.x) &&
            y == static_cast<T>(vector.y) &&
            z == static_cast<T>(vector.z);
    }

    template<typename T>
    template<typename U>
    bool Vector<3, T>::operator != (const Vector<3, U>& vector) const
    {
        return
            x != static_cast<T>(vector.x) ||
            y != static_cast<T>(vector.y) ||
            z != static_cast<T>(vector.z);
    }

    template<typename T>
    T Vector<3, T>::operator[](const size_t index) const
    {
        return c[index];
    }


    // Vector 4 implementations.
    template<typename T>
    constexpr Vector<4, T>::Vector() :
        x{}, y{}, z{}, w{}
    {}

    template<typename T>
    template<typename U>
    constexpr Vector<4, T>::Vector(const U xyzw) :
        x(static_cast<T>(xyzw)), y(static_cast<T>(xyzw)), z(static_cast<T>(xyzw)), w(static_cast<T>(xyzw))
    {}

    template<typename T>
    template<typename U1, typename U2, typename U3, typename U4>
    constexpr Vector<4, T>::Vector(const U1 x, const U2 y, const U3 z, const U4 w) :
        x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z)), w(static_cast<T>(w))
    {}

    template<typename T>
    template<typename U>
    constexpr Vector<4, T>::Vector(const Vector<4, U>& vector) :
        x(static_cast<T>(vector.x)), y(static_cast<T>(vector.y)), z(static_cast<T>(vector.z)), w(static_cast<T>(vector.w))
    {}

    template<typename T>
    Vector<4, T> Vector<4, T>::Absolute() const
    {
        return Vector2<T>(std::abs(x), std::abs(y), std::abs(z), std::abs(w));
    }

    template<typename T>
    template<typename R, typename U>
    R Vector<4, T>::Dot(const Vector<4, U>& vector) const
    {
        return static_cast<R>(
            (x * static_cast<T>(vector.x)) +
            (y * static_cast<T>(vector.y)) +
            (z * static_cast<T>(vector.z)) +
            (w * static_cast<T>(vector.w))
        );
    }

    template<typename T>
    template<typename R>
    R Vector<4, T>::Length() const
    {
        return static_cast<R>(std::sqrt((x * x) + (y * y) + (z * z) + (w * w)));
    }

    template<typename T>
    Vector<4, T> Vector<4, T>::Normal() const
    {
        T len = static_cast<T>(std::sqrt((x * x) + (y * y) + (z * z) + (w * w)));
        if (len == static_cast<T>(0))
        {
            return { static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) };
        }
        return { x / len, y / len, z / len, w / len };
    }
    template<>
    inline Vector<4, float> Vector<4, float>::Normal() const
    {
        auto len = static_cast<float>(std::sqrt((x * x) + (y * y) + (z * z) + (w * w)));
        if (len == 0.0f)
        {
            return { 0.0f, 0.0f, 0.0f, 0.0f };
        }
        len = 1.0f / len;
        return { x * len, y * len, z * len, w * len };
    }
    template<>
    inline Vector<4, double> Vector<4, double>::Normal() const
    {
        auto len = static_cast<double>(std::sqrt((x * x) + (y * y) + (z * z) + (w * w)));
        if (len == 0.0)
        {
            return { 0.0, 0.0, 0.0, 0.0 };
        }
        len = 1.0 / len;
        return { x * len, y * len, z * len, w * len };
    }
    template<>
    inline Vector<4, long double> Vector<4, long double>::Normal() const
    {
        auto len = static_cast<long double>(std::sqrt((x * x) + (y * y) + (z * z) + (w * w)));
        if (len == 0.0)
        {
            return { 0.0, 0.0, 0.0, 0.0 };
        }
        len = 1.0 / len;
        return { x * len, y * len, z * len, w * len };
    }

    template<typename T>
    Vector<4, T>& Vector<4, T>::Normalize()
    {
        return *this = Normal();
    }

    template<typename T>
    template<typename U>
    Vector<4, T> Vector<4, T>::operator + (const Vector<4, U>& vector) const
    {
        return {
            x + static_cast<T>(vector.x),
            y + static_cast<T>(vector.y),
            z + static_cast<T>(vector.z),
            w + static_cast<T>(vector.w)
        };
    }

    template<typename T>
    template<typename U>
    Vector<4, T>& Vector<4, T>::operator += (const Vector<4, U>& vector)
    {
        x += static_cast<T>(vector.x);
        y += static_cast<T>(vector.y);
        z += static_cast<T>(vector.z);
        w += static_cast<T>(vector.w);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vector<4, T> Vector<4, T>::operator - (const Vector<4, U>& vector) const
    {
        return {
            x - static_cast<T>(vector.x),
            y - static_cast<T>(vector.y),
            z - static_cast<T>(vector.z),
            w - static_cast<T>(vector.w)
        };
    }

    template<typename T>
    template<typename U>
    Vector<4, T>& Vector<4, T>::operator -= (const Vector<4, U>& vector)
    {
        x -= static_cast<T>(vector.x);
        y -= static_cast<T>(vector.y);
        z -= static_cast<T>(vector.z);
        w -= static_cast<T>(vector.w);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vector<4, T> Vector<4, T>::operator * (const Vector<4, U>& vector) const
    {
        return {
            x * static_cast<T>(vector.x),
            y * static_cast<T>(vector.y),
            z * static_cast<T>(vector.z),
            w * static_cast<T>(vector.w)
        };
    }
    template<typename T>
    template<typename U>
    Vector<4, T> Vector<4, T>::operator * (const U scalar) const
    {
        return {
            x * static_cast<T>(scalar),
            y* static_cast<T>(scalar),
            z * static_cast<T>(scalar),
            w * static_cast<T>(scalar)
        };
    }

    template<typename T>
    template<typename U>
    Vector<4, T>& Vector<4, T>::operator *= (const Vector<4, U>& vector)
    {
        x *= static_cast<T>(vector.x);
        y *= static_cast<T>(vector.y);
        z *= static_cast<T>(vector.z);
        w *= static_cast<T>(vector.w);
        return *this;
    }
    template<typename T>
    template<typename U>
    Vector<4, T>& Vector<4, T>::operator *= (const U scalar)
    {
        x *= static_cast<T>(scalar);
        y *= static_cast<T>(scalar);
        z *= static_cast<T>(scalar);
        w *= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    Vector<4, T> Vector<4, T>::operator / (const Vector<4, U>& vector) const
    {
        return {
            x / static_cast<T>(vector.x),
            y / static_cast<T>(vector.y),
            z / static_cast<T>(vector.z),
            w / static_cast<T>(vector.w)
        };
    }
    template<typename T>
    template<typename U>
    Vector<4, T> Vector<4, T>::operator / (const U scalar) const
    {
        return {
            x / static_cast<T>(scalar),
            y / static_cast<T>(scalar),
            z / static_cast<T>(scalar),
            w / static_cast<T>(scalar)
        };
    }

    template<typename T>
    template<typename U>
    Vector<4, T>& Vector<4, T>::operator /= (const Vector<4, U>& vector)
    {
        x /= static_cast<T>(vector.x);
        y /= static_cast<T>(vector.y);
        z /= static_cast<T>(vector.z);
        w /= static_cast<T>(vector.w);
        return *this;
    }
    template<typename T>
    template<typename U>
    Vector<4, T>& Vector<4, T>::operator /= (const U scalar)
    {
        x /= static_cast<T>(scalar);
        y /= static_cast<T>(scalar);
        y /= static_cast<T>(scalar);
        w /= static_cast<T>(scalar);
        return *this;
    }

    template<typename T>
    template<typename U>
    bool Vector<4, T>::operator == (const Vector<4, U>& vector) const
    {
        return
            x == static_cast<T>(vector.x) &&
            y == static_cast<T>(vector.y) &&
            z == static_cast<T>(vector.z) &&
            w == static_cast<T>(vector.w);
    }

    template<typename T>
    template<typename U>
    bool Vector<4, T>::operator != (const Vector<4, U>& vector) const
    {
        return
            x != static_cast<T>(vector.x) ||
            y != static_cast<T>(vector.y) ||
            z != static_cast<T>(vector.z) ||
            w != static_cast<T>(vector.w);
    }

    template<typename T>
    T Vector<4, T>::operator[](const size_t index) const
    {
        return c[index];
    }
}
