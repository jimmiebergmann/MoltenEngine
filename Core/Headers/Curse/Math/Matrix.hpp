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

#ifndef CURSE_CORE_MATH_MATRIX_HPP
#define CURSE_CORE_MATH_MATRIX_HPP

#include "Curse/Math/Vector.hpp"

namespace Curse
{

    /**
    * @brief Linear algebra class for matrix.
    */
    template<size_t Dx, size_t Dy, typename T>
    class Matrix
    {

    public:

        static constexpr size_t Width = Dx; //< Number of componenets in x.
        static constexpr size_t Height = Dy; //< Number of componenets in y.
        static constexpr size_t ComponentCount = Width * Height; //< Number of componenets in matrix.
        using Type = T; //< Data type of matrix components.

        /**
        * @brief Constructor.
        *        Vector's components are uninitialized.
        */
        Matrix();

        union
        {
            T v[Width][Height]; //< 2D array of matrix components.
            T c[ComponentCount]; //< Components of matrix.
        };

    };

    /**
    * @brief Linear algebra class for matrix.
    */
    template<typename T>
    class Matrix<4, 4, T>
    {

    public:

        static constexpr size_t Width = 4; //< Number of componenets in x.
        static constexpr size_t Height = 4; //< Number of componenets in y.
        static constexpr size_t ComponentCount = Width * Height; //< Number of componenets in matrix.
        using Type = T; //< Data type of matrix components.

        /**
        * @brief Constructor.
        *        Vector's components are uninitialized.
        */
        Matrix();

        /**
        * @brief Constructing and initializing all components.
        */
        Matrix(const T s00, const T s10, const T s20, const T s30,
               const T s01, const T s11, const T s21, const T s31,
               const T s02, const T s12, const T s22, const T s32,
               const T s03, const T s13, const T s23, const T s33);

        union
        {
            T v[Width][Height]; //< 2D array of matrix components.
            T c[ComponentCount]; //< Components of matrix.
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