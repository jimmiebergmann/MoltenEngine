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

#ifndef CURSE_CORE_MATH_VECTOR_HPP
#define CURSE_CORE_MATH_VECTOR_HPP

#include "Curse/Types.hpp"
#include <cmath>

namespace Curse
{

    /**
    * @brief Linear algebra class for vectors.
    */
    template<size_t D, typename T>
    class Vector
    {

    public:

        static const size_t Dimensions = D; //< Number of componenets.
        using Type = T; //< Data type of vector components.

        /**
        * @brief Constructor.
        *        Vector's components are uninitialized.
        */
        Vector();

        T c[Dimensions]; //< Components of vector.

    };

    /**
   * @brief Linear algebra class for vectors.
   */
    template<typename T>
    class Vector<2, T>
    {

    public:

        static const size_t Dimensions = 2; //< Number of componenets.
        using Type = T; //< Data type of vector components.   

        /**
        * @brief Constructor.
        *        Vector's components are uninitialized.
        */
        Vector();

        /**
        * @brief Constructor.
        *        Initializes all components by input parameter.
        */
        template<typename U>
        Vector(const U xy);

        /**
        * @brief Constructor.
        *        Initializes all components separately by input parameters.
        */
        template<typename U1, typename U2>
        Vector(const U1 x, const U2 y);

        /**
        * @brief Get the absolute valute vector.
        */
        Vector<2, T> Absolute() const;

        /**
        * @brief Get dot product of vector.
        */
        template<typename U = T>
        U Dot(const Vector<2, T>& vector) const;

        /**
        * @brief Get length of vector.
        */
        template<typename U = T>
        U Length() const;

        /**
        * @brief Get normal out of vector.
        */
        Vector<2, T> Normal() const;

        /**
        * @brief Normalize vector.
        */
        Vector<2, T>& Normalize();

        /**
        * @brief Arithmetic addition operator.
        */
        Vector<2, T> operator + (const Vector<2, T>& vector) const;

        /**
        * @brief Arithmetic addition assignment operator.
        */
        Vector<2, T>& operator += (const Vector<2, T>& vector);

        /**
        * @brief Arithmetic subtraction operator.
        */
        Vector<2, T> operator - (const Vector<2, T>& vector) const;

        /**
        * @brief Arithmetic subtraction assignment operator.
        */
        Vector<2, T>& operator -= (const Vector<2, T>& vector);

        /**
        * @brief Arithmetic multiplication operator.
        */
        Vector<2, T> operator * (const Vector<2, T>& vector) const;

        /**
        * @brief Arithmetic multiplication operator.
        */
        Vector<2, T> operator * (const T scalar) const;

        /**
        * @brief Arithmetic multiplication assignment operator.
        */
        Vector<2, T>& operator *= (const Vector<2, T>& vector);

        /**
        * @brief Arithmetic multiplication operator.
        */
        Vector<2, T>& operator *= (const T scalar);

        /**
        * @brief Arithmetic division operator.
        */
        Vector<2, T> operator / (const Vector<2, T>& vector) const;

        /**
        * @brief Arithmetic division operator.
        */
        Vector<2, T> operator / (const T scalar) const;

        /**
        * @brief Arithmetic division assignment operator.
        */
        Vector<2, T>& operator /= (const Vector<2, T>& vector);

        /**
        * @brief Arithmetic division assignment operator.
        */
        Vector<2, T>& operator /= (const T scalar);

        /**
        * @brief Equals to time comparation operator.
        */
        bool operator == (const Vector<2, T>& vector) const;
        /**
        * @brief Not equals to time comparation operator.
        */
        bool operator != (const Vector<2, T>& vector) const;

        union
        {
            struct
            {
                T x; //< X component of vector.
                T y; //< Y component of vector.
            } ;
            T c[Dimensions]; //< Components of vector.
        };

    };

    template<typename T>
    using Vector2 = Vector<2, T>;
    using Vector2b = Vector2<bool>;
    using Vector2i32 = Vector2<int32_t>;
    using Vector2ui32 = Vector2<uint32_t>;
    using Vector2i64 = Vector2<int64_t>;
    using Vector2ui64 = Vector2<uint64_t>;
    using Vector2f32 = Vector2<float>;
    using Vector2f64 = Vector2<double>;

    template<typename T>
    using Vector3 = Vector<3, T>;
    using Vector3b = Vector3<bool>;
    using Vector3i32 = Vector3<int32_t>;
    using Vector3ui32 = Vector3<uint32_t>;
    using Vector3i64 = Vector3<int64_t>;
    using Vector3ui64 = Vector3<uint64_t>;
    using Vector3f32 = Vector3<float>;
    using Vector3f64 = Vector3<double>;

}

#include "Vector.inl"

#endif
