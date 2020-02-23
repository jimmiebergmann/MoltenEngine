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

#ifndef CURSE_CORE_MATH_MATRIX_HPP
#define CURSE_CORE_MATH_MATRIX_HPP

#include "Curse/Math.hpp"
#include "Curse/Math/Vector.hpp"
#include "Curse/System/Exception.hpp"

namespace Curse
{

    /**
    * @brief Linear algebra class for matrix.
    */
    template<size_t _Rows, size_t _Columns, typename T>
    class Matrix
    {

    public:

        constexpr static size_t Rows = _Rows;
        constexpr static size_t Columns = _Columns;
        constexpr static size_t Components = Rows * Columns;
        using Type = T;

        /**
        * @brief Constructor.
        *        Elements are uninitialized.
        */
        Matrix();

        union
        {
            T e[Components]; //< Elements of matrix.
            Vector<Rows, T> column[Columns];  //< Columns of matrix.
        };

    };

    /**
    * @brief Linear algebra class for matrix.
    */
    template<typename T>
    class Matrix<4, 4, T>
    {

    public:

        constexpr static size_t Rows = 4;
        constexpr static size_t Columns = 4;
        constexpr static size_t Components = Rows * Columns;
        using Type = T;

        /**
        * @brief Creates an identity matrix.
        */
        static Matrix<4, 4, T> Identity();

        /**
        * @brief Creates a "Look at point" view matrix.
        *
        * @param position Position of the viewer.
        * @param point Point in space to view.
        * @param up Orientation of the viewer.
        */
        static Matrix<4, 4, T> LookAtPoint(const Vector3<T>& position, const Vector3<T>& point, const Vector3<T>& up);

        /**
        * @brief Creates a perspective projection matrix.
        */
        static Matrix<4, 4, T> Perspective(const T fov, const T aspect, const T near, const T far);


        /**
        * @brief Creates an orthographic projection matrix.
        */
        static Matrix<4, 4, T> Orthographic(const T left, const T right, const T bottom, const T top,
                                            const T near, const T far);

        /**
        * @brief Constructor.
        *        Elements are initialized as 0.
        */
        Matrix();

        /**
        * @brief Constructor.
        *        Constructing and initializing all elements by a single value.
        */
        Matrix(const T value);

        /**
        * @brief Constructing and initializing all elements individually.
        */
        Matrix(const T e1,  const T e2,  const T e3,  const T e4,
               const T e5,  const T e6,  const T e7,  const T e8,
               const T e9,  const T e10, const T e11, const T e12,
               const T e13, const T e14, const T e15, const T e16);

        /**
        * @brief Constructing and initializing all elements by rows.
        */
        Matrix(const Vector4<T>& column1, const Vector4<T>& column2,
               const Vector4<T>& column3, const Vector4<T>& column4);

        /**
        * @brief Translate matrix.
        */
        void Translate(const Vector3<T> & translation);

        /**
        * @brief Scale matrix.
        */
        void Scale(const Vector3<T>& scale);

        /**
        * @brief Multiplication by matrix operator.
        */
        Matrix<4, 4, T> operator *(const Matrix<4, 4, T>& matrix) const;

        /**
        * @brief Multiplication by matrix assignment operator.
        */
        Matrix<4, 4, T> & operator *=(const Matrix<4, 4, T>& matrix);

        /**
        * @brief Multiplication by vector operator.
        */
        Vector<4, T> operator *(const Vector<4, T>& vector) const;

        union
        {        
            T e[Components]; //< Elements of matrix.
            Vector<Rows, T> column[Columns];  //< Columns of matrix.
        };

    };

    template<typename T>
    using Matrix4x4 = Matrix<4, 4, T>;
    using Matrix4x4i32 = Matrix4x4<int32_t>;
    using Matrix4x4i64 = Matrix4x4<int64_t>;
    using Matrix4x4f32 = Matrix4x4<float>;
    using Matrix4x4f64 = Matrix4x4<double>;

}

#include "Matrix.inl"

#endif