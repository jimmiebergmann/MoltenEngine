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

    // Matrix X * Y
    template<size_t _Rows, size_t _Columns, typename T>
    constexpr size_t Matrix<_Rows, _Columns, T>::Rows;

    template<size_t _Rows, size_t _Columns, typename T>
    constexpr size_t Matrix<_Rows, _Columns, T>::Columns;

    template<size_t _Rows, size_t _Columns, typename T>
    constexpr size_t Matrix<_Rows, _Columns, T>::Components;

    template<size_t _Rows, size_t _Columns, typename T>
    Matrix<_Rows, _Columns, T>::Matrix()
    {
    }

    // Matrix 4 * 4
    template<typename T>
    constexpr size_t Matrix<4, 4, T>::Rows;

    template<typename T>
    constexpr size_t Matrix<4, 4, T>::Columns;

    template<typename T>
    constexpr size_t Matrix<4, 4, T>::Components;

    template<typename T>
    inline Matrix<4, 4, T> Matrix<4, 4, T>::Perspective(const T fov, const T aspect, const T zNear, const T zFar)
    {
        static constexpr T radiansExpr = static_cast<T>(1) / (static_cast<T>(2) * Constants::Pi<T>() / static_cast<T>(180));
        const T fovRadians = fov * radiansExpr;
        const T sine = std::sin(fovRadians);
        const T zRange = zFar - zNear;    

        if (zRange == static_cast<T>(0) || sine == static_cast<T>(0) || aspect == static_cast<T>(0))
        {
            return {};
        }

        const T cotan = std::cos(fovRadians) / sine;

        return { cotan / aspect,    static_cast<T>(0), static_cast<T>(0),        static_cast<T>(0),
                 static_cast<T>(0), cotan,             static_cast<T>(0),        static_cast<T>(0),
                 static_cast<T>(0), static_cast<T>(0), -(zFar + zNear) / zRange, static_cast<T>(-1),
                 static_cast<T>(0), static_cast<T>(0) , static_cast<T>(-2) * zNear * zFar / zRange, static_cast<T>(0) };
    }

    template<typename T>
    inline Matrix<4, 4, T> Matrix<4, 4, T>::Orthographic(const T left, const T right, const T bottom, const T top,
                                                         const T zNear, const T zFar)
    {
        return {};
    }

    template<typename T>
    inline Matrix<4, 4, T>::Matrix()
    {
    }

    template<typename T>
    inline Matrix<4, 4, T>::Matrix(
            const T e1,  const T e2,  const T e3,  const T e4,
            const T e5,  const T e6,  const T e7,  const T e8,
            const T e9,  const T e10, const T e11, const T e12,
            const T e13, const T e14, const T e15, const T e16) :

        e{ e1,  e2,  e3,  e4,
           e5,  e6,  e7,  e8,
           e9,  e10, e11, e12,
           e13, e14, e15, e16 }
    {}

    template<typename T>
    inline Matrix<4, 4, T>::Matrix(const Vector4<T>& row1, const Vector4<T>& row2,
                                   const Vector4<T>& row3, const Vector4<T>& row4) :
        row{ row1, row2, row3, row4 }
    {}

}