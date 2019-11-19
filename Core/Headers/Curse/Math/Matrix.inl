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
    Matrix<4, 4, T> Matrix<4, 4, T>::Identity()
    {
        return { static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0),
                 static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
                 static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
                 static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
    }

    template<typename T>
    inline Matrix<4, 4, T> Matrix<4, 4, T>::Perspective(const T fov, const T aspect, const T near, const T far)
    {
        static constexpr T radiansExpr = static_cast<T>(1) / (static_cast<T>(2) * Constants::Pi<T>() / static_cast<T>(180));
        const T fovRadians = fov * radiansExpr;
        const T sine = std::sin(fovRadians);
        const T zRange = far - near;    

        if (zRange == static_cast<T>(0) )
        {
            throw Exception("Matrix::Perspective: Difference between near and far is 0.");
        }
        if (sine == static_cast<T>(0) )
        {
            throw Exception("Matrix::Perspective: FOV is 0 or invalid.");
        }
        if (aspect == static_cast<T>(0))
        {
            throw Exception("Matrix::Perspective: Aspect ratio is 0.");
        }

        const T cotan = std::cos(fovRadians) / sine;

        return { cotan / aspect,    static_cast<T>(0), static_cast<T>(0),                       static_cast<T>(0),
                 static_cast<T>(0), cotan,             static_cast<T>(0),                       static_cast<T>(0),
                 static_cast<T>(0), static_cast<T>(0), -(far + near) / zRange,                  static_cast<T>(-1),
                 static_cast<T>(0), static_cast<T>(0), static_cast<T>(-2) * near* far / zRange, static_cast<T>(0) };
    }

    template<typename T>
    inline Matrix<4, 4, T> Matrix<4, 4, T>::Orthographic(const T left, const T right, const T bottom, const T top,
                                                         const T near, const T far)
    {     
        const T rangeX = right - left;
        const T rangeY = top - bottom;
        const T rangeZ = near - far;

        if (rangeX == static_cast<T>(0))
        {
            throw Exception("Matrix::Perspective: Difference between left and right is 0.");
        }
        if (rangeY == static_cast<T>(0))
        {
            throw Exception("Matrix::Perspective: Difference between top and bottom is 0.");
        }
        if (rangeZ == static_cast<T>(0))
        {
            throw Exception("Matrix::Perspective: Difference between near and far is 0.");
        }

        return { static_cast<T>(2) / rangeX, static_cast<T>(0), static_cast<T>(0),  -(right + left) / rangeX,
                 static_cast<T>(0), static_cast<T>(2) / rangeY, static_cast<T>(0),  -(top + bottom) / rangeY,
                 static_cast<T>(0), static_cast<T>(0), static_cast<T>(-2) / rangeZ, -(far + near) / rangeZ,
                 static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1) };
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

    template<typename T>
    inline void Matrix<4, 4, T>::Translate(const Vector3<T>& translation)
    {
        Matrix<4, 4, T> trans =
        {
            Vector4<T>(1, 0, 0, translation.x),
            Vector4<T>(0, 1, 0, translation.y),
            Vector4<T>(0, 0, 1, translation.z),
            Vector4<T>(0, 0, 0, 1)
        };
        (*this) *= trans;
    }

    template<typename T>
    inline void Matrix<4, 4, T>::Scale(const Vector3<T>& scale)
    {
        Matrix<4, 4, T> trans =
        {
            Vector4<T>(scale.x, 0, 0, 0),
            Vector4<T>(0, scale.y, 0, 0),
            Vector4<T>(0, 0, scale.z, 0),
            Vector4<T>(0, 0, 0, 1)
        };
        (*this) *= trans;
    }

    template<typename T>
    inline Matrix<4, 4, T> Matrix<4, 4, T>::operator *(const Matrix<4, 4, T>& matrix) const
    {
        return
        {
            (e[0] * matrix.e[0]) + (e[1] * matrix.e[4]) + (e[2] * matrix.e[8])  + (e[3] * matrix.e[12]),
            (e[0] * matrix.e[1]) + (e[1] * matrix.e[5]) + (e[2] * matrix.e[9])  + (e[3] * matrix.e[13]),
            (e[0] * matrix.e[2]) + (e[1] * matrix.e[6]) + (e[2] * matrix.e[10]) + (e[3] * matrix.e[14]),
            (e[0] * matrix.e[3]) + (e[1] * matrix.e[7]) + (e[2] * matrix.e[11]) + (e[3] * matrix.e[15]),

            (e[4] * matrix.e[0]) + (e[5] * matrix.e[4]) + (e[6] * matrix.e[8])  + (e[7] * matrix.e[12]),
            (e[4] * matrix.e[1]) + (e[5] * matrix.e[5]) + (e[6] * matrix.e[9])  + (e[7] * matrix.e[13]),
            (e[4] * matrix.e[2]) + (e[5] * matrix.e[6]) + (e[6] * matrix.e[10]) + (e[7] * matrix.e[14]),
            (e[4] * matrix.e[3]) + (e[5] * matrix.e[7]) + (e[6] * matrix.e[11]) + (e[7] * matrix.e[15]),

            (e[8] * matrix.e[0]) + (e[9] * matrix.e[4]) + (e[10] * matrix.e[8])  + (e[11] * matrix.e[12]),
            (e[8] * matrix.e[1]) + (e[9] * matrix.e[5]) + (e[10] * matrix.e[9])  + (e[11] * matrix.e[13]),
            (e[8] * matrix.e[2]) + (e[9] * matrix.e[6]) + (e[10] * matrix.e[10]) + (e[11] * matrix.e[14]),
            (e[8] * matrix.e[3]) + (e[9] * matrix.e[7]) + (e[10] * matrix.e[11]) + (e[11] * matrix.e[15]),

            (e[12] * matrix.e[0]) + (e[13] * matrix.e[4]) + (e[14] * matrix.e[8])  + (e[15] * matrix.e[12]),
            (e[12] * matrix.e[1]) + (e[13] * matrix.e[5]) + (e[14] * matrix.e[9])  + (e[15] * matrix.e[13]),
            (e[12] * matrix.e[2]) + (e[13] * matrix.e[6]) + (e[14] * matrix.e[10]) + (e[15] * matrix.e[14]),
            (e[12] * matrix.e[3]) + (e[13] * matrix.e[7]) + (e[14] * matrix.e[11]) + (e[15] * matrix.e[15])
        };
    }

    template<typename T>
    inline Matrix<4, 4, T>& Matrix<4, 4, T>::operator *=(const Matrix<4, 4, T>& matrix)
    {
        (*this) = (*this) * matrix;
        return *this;
    }


    template<typename T>
    inline Vector<4, T> Matrix<4, 4, T>::operator *(const Vector<4, T>& vector) const
    {
        return
        {
            (e[0]  * vector.c[0]) + (e[1]  * vector.c[1]) + (e[2]  * vector.c[2]) + (e[3]  * vector.c[3]),
            (e[4]  * vector.c[0]) + (e[5]  * vector.c[1]) + (e[6]  * vector.c[2]) + (e[7]  * vector.c[3]),
            (e[8]  * vector.c[0]) + (e[9]  * vector.c[1]) + (e[10] * vector.c[2]) + (e[11] * vector.c[3]),
            (e[12] * vector.c[0]) + (e[13] * vector.c[1]) + (e[14] * vector.c[2]) + (e[15] * vector.c[3])
        };
    }

}