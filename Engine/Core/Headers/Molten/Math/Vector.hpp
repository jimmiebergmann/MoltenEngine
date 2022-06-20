/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_MATH_VECTOR_HPP
#define MOLTEN_CORE_MATH_VECTOR_HPP

#include "Molten/Core.hpp"
#include <cmath>
#include <cstring>

namespace Molten
{

    /** Linear algebra class for vectors. */
    template<size_t D, typename T>
    class Vector
    {

    public:

        static constexpr inline size_t Dimensions = D; ///< Number of components.
        using Type = T; ///< Data type of vector components.

        /** Constructor. Vector's components are uninitialized. */
        constexpr Vector();

        /**Constructor. Vector's components are initialized by initializer list. */
        template<typename U, typename ... Us>
        constexpr Vector(const U element, const Us ... elements);

        constexpr Vector(const Vector&) = default;
        constexpr Vector(Vector&&) = default;
        constexpr Vector& operator = (const Vector&) = default;
        constexpr Vector& operator = (Vector&&) = default;

        /** Comparison operators. */
        /**@{*/
        bool operator == (const Vector<D, T>& vector) const;

        bool operator != (const Vector<D, T>& vector) const;
        /**@}*/

        /** Access vector component by index. */
        T operator[](const size_t index) const;

        T c[Dimensions]; ///< Components of vector.

    };

    /**
    * @brief Linear algebra class for 2D vector.
    */
    template<typename T>
    class Vector<2, T>
    {

    public:

        static constexpr inline size_t Dimensions = 2; ///< Number of components.
        using Type = T; ///< Data type of vector components.   

        /** Constructor. Vector's components are uninitialized. */
        constexpr Vector();

        /** Constructor. Initializes all components by input parameter. */
        template<typename U>
        constexpr explicit Vector(const U xy);

        /** Constructor. Initializes all components separately by input parameters. */
        template<typename U1, typename U2>
        constexpr Vector(const U1 x, const U2 y);

        /** Constructor. Initializes vector by any other 2 dimensional vector class. */
        template<typename U>
        constexpr Vector(const Vector<2, U>& vector);

        /**Get absolute value of this vector. */
        [[nodiscard]] Vector<2, T> Absolute() const;

        /** Get dot product of this vector.*/
        template<typename R = T, typename U>
        [[nodiscard]] R Dot(const Vector<2, U>& vector) const;

        /** Get length of this vector. */
        template<typename R = T>
        [[nodiscard]] R Length() const;

        /** Get normal out of this vector. */
        [[nodiscard]] Vector<2, T> Normal() const;

        /** Normalize this vector. */
        Vector<2, T>& Normalize();

        /** Arithmetic operators. */
        /**@{*/
        template<typename U>
        Vector<2, T> operator + (const Vector<2, U>& vector) const;

        template<typename U>
        Vector<2, T>& operator += (const Vector<2, U>& vector);

        template<typename U>
        Vector<2, T> operator - (const Vector<2, U>& vector) const;

        template<typename U>
        Vector<2, T>& operator -= (const Vector<2, U>& vector);

        template<typename U>
        Vector<2, T> operator * (const Vector<2, U>& vector) const;

        template<typename U>
        Vector<2, T> operator * (const U scalar) const;

        template<typename U>
        Vector<2, T>& operator *= (const Vector<2, U>& vector);

        template<typename U>
        Vector<2, T>& operator *= (const U scalar);

        template<typename U>
        Vector<2, T> operator / (const Vector<2, U>& vector) const;

        template<typename U>
        Vector<2, T> operator / (const U scalar) const;

        template<typename U>
        Vector<2, T>& operator /= (const Vector<2, U>& vector);

        template<typename U>
        Vector<2, T>& operator /= (const U scalar);
        /**@}*/

        /** Comparison operators. */
        /**@{*/
        template<typename U>
        bool operator == (const Vector<2, U>& vector) const;

        template<typename U>
        bool operator != (const Vector<2, U>& vector) const;
        /**@}*/

        /** Access vector component by index. */
        T operator[](const size_t index) const;

        MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
        union
        {
            struct
            {
                T x; ///< X component of vector.
                T y; ///< Y component of vector.
            } ;
            T c[Dimensions]; ///< Components of vector.
        };
        MOLTEN_ANONYMOUS_STRUCTURE_END

    };

    /** Linear algebra class for 3D vector. */
    template<typename T>
    class Vector<3, T>
    {

    public:

        static constexpr inline size_t Dimensions = 3; ///< Number of components.
        using Type = T; ///< Data type of vector components.   

        /** Constructor. Vector's components are uninitialized. */
        constexpr Vector();

        /** Constructor. Initializes all components by input parameter. */
        template<typename U>
        constexpr explicit Vector(const U xyz);

        /** Constructor. Initializes all components separately by input parameters. */
        template<typename U1, typename U2, typename U3>
        constexpr Vector(const U1 x, const U2 y, const U3 z);

        /** Constructor. Initializes vector by any other 3 dimensional vector class. */
        template<typename U>
        constexpr explicit Vector(const Vector<3, U>& vector);

        /** Get absolute value of this vector. */
        [[nodiscard]] Vector<3, T> Absolute() const;

        /** Get dot product of this vector. */
        template<typename R = T, typename U>
        [[nodiscard]] R Dot(const Vector<3, U>& vector) const;

        /** Get cross product of this vector. */
        template<typename U>
        [[nodiscard]] Vector<3, T> Cross(const Vector<3, U>& vector) const;

        /** Get length of this vector. */
        template<typename R = T>
        [[nodiscard]] R Length() const;

        /** Get normal from this vector. */
        [[nodiscard]] Vector<3, T> Normal() const;

        /** Normalize this vector. */
        Vector<3, T>& Normalize();

        /** Arithmetic operators. */
        /**@{*/
        template<typename U>
        Vector<3, T> operator + (const Vector<3, U>& vector) const;

        template<typename U>
        Vector<3, T>& operator += (const Vector<3, U>& vector);

        template<typename U>
        Vector<3, T> operator - (const Vector<3, U>& vector) const;

        template<typename U>
        Vector<3, T>& operator -= (const Vector<3, U>& vector);

        template<typename U>
        Vector<3, T> operator * (const Vector<3, U>& vector) const;

        template<typename U>
        Vector<3, T> operator * (const U scalar) const;

        template<typename U>
        Vector<3, T>& operator *= (const Vector<3, U>& vector);

        template<typename U>
        Vector<3, T>& operator *= (const U scalar);

        template<typename U>
        Vector<3, T> operator / (const Vector<3, U>& vector) const;

        template<typename U>
        Vector<3, T> operator / (const U scalar) const;

        template<typename U>
        Vector<3, T>& operator /= (const Vector<3, U>& vector);

        template<typename U>
        Vector<3, T>& operator /= (const U scalar);
        /**@}*/

        /** Comparison operators. */
        /**@{*/
        template<typename U>
        bool operator == (const Vector<3, U>& vector) const;
 
        template<typename U>
        bool operator != (const Vector<3, U>& vector) const;
        /**@}*/

        /** Access vector component by index. */
        T operator[](const size_t index) const;

        MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
        union
        {
            struct
            {
                T x; ///< X component of vector.
                T y; ///< Y component of vector.
                T z; ///< Z component of vector.
            };
            T c[Dimensions]; ///< Components of vector.
        };
        MOLTEN_ANONYMOUS_STRUCTURE_END

    };

    /** Linear algebra class for 4D vector. */
    template<typename T>
    class Vector<4, T>
    {

    public:

        static constexpr inline size_t Dimensions = 4; ///< Number of components.
        using Type = T; ///< Data type of vector components.   

        /** Constructor. Vector's components are uninitialized. */
        constexpr Vector();

        /** Constructor. Initializes all components by input parameter. */
        template<typename U>
        constexpr explicit Vector(const U xyzw);

        /** Constructor. Initializes all components separately by input parameters. */
        template<typename U1, typename U2, typename U3, typename U4>
        constexpr Vector(const U1 x, const U2 y, const U3 z, const U4 w);

        /** Constructor. Initializes vector by any other 4 dimensional vector class. */
        template<typename U>
        constexpr explicit Vector(const Vector<4, U>& vector);

        /** Get absolute value of this vector. */
        [[nodiscard]] Vector<4, T> Absolute() const;

        /** Get dot product of this vector. */
        template<typename R = T, typename U>
        [[nodiscard]] R Dot(const Vector<4, U>& vector) const;

        /** Get length of this vector. */
        template<typename R = T>
        [[nodiscard]] R Length() const;

        /** Get normal from this vector. */
        [[nodiscard]] Vector<4, T> Normal() const;

        /** Normalize this vector. */
        Vector<4, T>& Normalize();

        /** Arithmetic operators. */
        /**@{*/
        template<typename U>
        Vector<4, T> operator + (const Vector<4, U>& vector) const;

        template<typename U>
        Vector<4, T>& operator += (const Vector<4, U>& vector);

        template<typename U>
        Vector<4, T> operator - (const Vector<4, U>& vector) const;

        template<typename U>
        Vector<4, T>& operator -= (const Vector<4, U>& vector);

        template<typename U>
        Vector<4, T> operator * (const Vector<4, U>& vector) const;

        template<typename U>
        Vector<4, T> operator * (const U scalar) const;

        template<typename U>
        Vector<4, T>& operator *= (const Vector<4, U>& vector);

        template<typename U>
        Vector<4, T>& operator *= (const U scalar);

        template<typename U>
        Vector<4, T> operator / (const Vector<4, U>& vector) const;

        template<typename U>
        Vector<4, T> operator / (const U scalar) const;

        template<typename U>
        Vector<4, T>& operator /= (const Vector<4, U>& vector);

        template<typename U>
        Vector<4, T>& operator /= (const U scalar);
        /**@}*/

        /** Comparison operators. */
        /**@{*/
        template<typename U>
        bool operator == (const Vector<4, U>& vector) const;

        template<typename U>
        bool operator != (const Vector<4, U>& vector) const;
        /**@}*/

        /** Access vector component by index. */
        T operator[](const size_t index) const;

        MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
        union
        {
            struct
            {
                T x; ///< X component of vector.
                T y; ///< Y component of vector.
                T z; ///< Z component of vector.
                T w; ///< W component of vector.
            };
            T c[Dimensions]; ///< Components of vector.
        };
        MOLTEN_ANONYMOUS_STRUCTURE_END

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
    using Vector2size = Vector2<size_t>;

    template<typename T>
    using Vector3 = Vector<3, T>;
    using Vector3b = Vector3<bool>;
    using Vector3i32 = Vector3<int32_t>;
    using Vector3ui32 = Vector3<uint32_t>;
    using Vector3i64 = Vector3<int64_t>;
    using Vector3ui64 = Vector3<uint64_t>;
    using Vector3f32 = Vector3<float>;
    using Vector3f64 = Vector3<double>;
    using Vector3size = Vector3<size_t>;

    template<typename T>
    using Vector4 = Vector<4, T>;
    using Vector4b = Vector4<bool>;
    using Vector4i32 = Vector4<int32_t>;
    using Vector4ui32 = Vector4<uint32_t>;
    using Vector4i64 = Vector4<int64_t>;
    using Vector4ui64 = Vector4<uint64_t>;
    using Vector4f32 = Vector4<float>;
    using Vector4f64 = Vector4<double>;
    using Vector4size = Vector4<size_t>;

}

#include "Molten/Math/Vector.inl"

#endif
