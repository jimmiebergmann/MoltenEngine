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
            T v[Dx][Dy]; //< Components of vector.
            T c[ComponentCount]; //< Components of vector.
        };
        
    };

}

#include "Matrix.inl"

#endif