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

}