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
    Vector<D, T>::Vector()
    {
    }

    // Vector 2.
    template<typename T>
    Vector<2, T>::Vector()
    {
    }

    template<typename T>
    Vector<2, T>::Vector(const Type xy) :
        x(xy), y(xy)
    {
    }

    template<typename T>
    Vector<2, T>::Vector(const Type x, const Type y) :
       x(x), y(y)
    {
    }

    template<typename T>
    Vector<2, T> Vector<2, T>::operator + (const Vector<2, T>& vector) const
    {
        return { x + vector.x, y + vector.y };
    }

    template<typename T>
    Vector<2, T>& Vector<2, T>::operator += (const Vector<2, T>& vector)
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
    Vector<2, T>& Vector<2, T>::operator -= (const Vector<2, T>& vector)
    {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }

    template<typename T>
    Vector<2, T> Vector<2, T>::operator * (const Vector<2, T>& vector) const
    {
        return { x * vector.x, y * vector.y };
    }
    template<typename T>
    Vector<2, T> Vector<2, T>::operator * (const Type scalar) const
    {
        return { x * scalar, y * scalar };
    }

    template<typename T>
    Vector<2, T>& Vector<2, T>::operator *= (const Vector<2, T>& vector)
    {
        x *= vector.x;
        y *= vector.y;
        return *this;
    }
    template<typename T>
    Vector<2, T>& Vector<2, T>::operator *= (const Type scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    template<typename T>
    Vector<2, T> Vector<2, T>::operator / (const Vector<2, T>& vector) const
    {
        return { x / vector.x, y / vector.y };
    }
    template<typename T>
    Vector<2, T> Vector<2, T>::operator / (const Type scalar) const
    {
        return { x / scalar, y / scalar };
    }

    template<typename T>
    Vector<2, T>& Vector<2, T>::operator /= (const Vector<2, T>& vector)
    {
        x /= vector.x;
        y /= vector.y;
        return *this;
    }
    template<typename T>
    Vector<2, T>& Vector<2, T>::operator /= (const Type scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    template<typename T>
    bool Vector<2, T>::operator == (const Vector<2, Type>& vector) const
    {
        return x == vector.x && y == vector.y;
    }

    template<typename T>
    bool Vector<2, T>::operator != (const Vector<2, Type>& vector) const
    {
        return x != vector.x || y != vector.y;
    }

}