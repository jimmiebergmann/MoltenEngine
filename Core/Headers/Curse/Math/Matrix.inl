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
    template<size_t Dx, size_t Dy, typename T>
    constexpr size_t Matrix<Dx, Dy, T>::Width;

    template<size_t Dx, size_t Dy, typename T>
    constexpr size_t Matrix<Dx, Dy, T>::Height;

    template<size_t Dx, size_t Dy, typename T>
    constexpr size_t Matrix<Dx, Dy, T>::ComponentCount;

    template<size_t Dx, size_t Dy, typename T>
    Matrix<Dx, Dy, T>::Matrix()
    {
    }

    // Matrix 4 * 4
    template<typename T>
    constexpr size_t Matrix<4, 4, T>::Width;

    template<typename T>
    constexpr size_t Matrix<4, 4, T>::Height;

    template<typename T>
    constexpr size_t Matrix<4, 4, T>::ComponentCount;

    template<typename T>
    Matrix<4, 4, T>::Matrix()
    {
    }

    template<typename T>
    Matrix<4, 4, T>::Matrix(const T s00, const T s10, const T s20, const T s30,
                            const T s01, const T s11, const T s21, const T s31,
                            const T s02, const T s12, const T s22, const T s32,
                            const T s03, const T s13, const T s23, const T s33) :
        c{ s00, s01, s02, s03,
           s10, s11, s12, s13,
           s20, s21, s22, s23,
           s30, s31, s32, s33 }
    {
    }

}