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

#ifndef MOLTEN_CORE_MATH_BOUNDS_HPP
#define MOLTEN_CORE_MATH_BOUNDS_HPP

#include "Molten/Math/Vector.hpp"
#include <algorithm>

namespace Molten
{

    /** Boundings object with low and high bounds. */
    template<size_t VDimensions, typename T>
    class Bounds
    {

    public:

        static constexpr size_t Dimensions = VDimensions; //< Number of componenets.
        using Type = T; //< Data type of vector components.

        /** Constructor. lower and higher bounds components are initialized as 0. */
        constexpr Bounds();

        /** Constructor. */
        constexpr Bounds(
            const Vector<VDimensions, T>& low,
            const Vector<VDimensions, T>& high);

        Vector<VDimensions, T> low;
        Vector<VDimensions, T> high;

    };

    /** 2D boundings object. */
    template<typename T>
    class Bounds<2, T>
    {

    public:

        static constexpr size_t Dimensions = 2; //< Number of componenets.
        using Type = T; //< Data type of vector components.

        /** Constructor. lower and higher bounds components are initialized as 0. */
        constexpr Bounds();

        /** Constructor. */
        constexpr Bounds(
            const Vector<2, T>& low,
            const Vector<2, T>& high);

        /** Constructor. */
        constexpr Bounds(
            const T left,
            const T top,
            const T right,
            const T bottom);    

        /** Get size of bounds */
        constexpr Vector<2, T> GetSize() const;

        /** Checks if point is inside of bounds. */
        constexpr bool Intersects(const Vector2<T>& point) const;

        /** Returns a new bounds that represent the union of two(this and another) bounds. */
        constexpr Bounds<2, T> Union(const Bounds<2, T>& bounds) const;

        /** Move lower and highter value by a given distance. */
        constexpr Bounds<2, T>& Move(const Vector2f32& distance);

        /** Creates bounds by growing current bounds by margins. */
        constexpr Bounds<2, T> WithMargins(const Bounds<2, T>& margins) const;

        /** Creates bounds by shrinking current bounds by margins. */
        constexpr Bounds<2, T> WithoutMargins(const Bounds<2, T>& margins) const;

        /** Adds margins to bounds and causing the bounds to grow. */
        constexpr Bounds<2, T>& AddMargins(const Bounds<2, T>& margins);

        /** Removes margins from bounds and causing the bounds to shrink. */
        constexpr Bounds<2, T>& RemoveMargins(const Bounds<2, T>& margins);

        /** Clamps high values to low values if high < low. */
        constexpr Bounds<2, T>& ClampHighToLow();

        /** Checks if area of bounds are < 0. */
        constexpr bool IsEmpty() const;

        /** Operators. */
        /**@{*/
        constexpr bool operator == (const Bounds<2, T>& rhs) const;
        constexpr bool operator != (const Bounds<2, T>& rhs) const;
        /**@}*/

        MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
        union
        {
            struct
            {
                T left;
                T top;
            };
            Vector2<T> low;
        };
        union
        {
            struct
            {
                T right;
                T bottom;
            };
            Vector2<T> high;
        };
        MOLTEN_ANONYMOUS_STRUCTURE_END

    };

    /** 3D boundings object. */
    template<typename T>
    class Bounds<3, T>
    {

    public:

        static constexpr size_t Dimensions = 3; //< Number of componenets.
        using Type = T; //< Data type of vector components.

        /** Constructor. lower and higher bounds components are initialized as 0. */
        constexpr Bounds();

        /** Constructor. */
        constexpr Bounds(
            const Vector<3, T>& low,
            const Vector<3, T>& high);

        /** Constructor. */
        constexpr Bounds(
            const T left,
            const T top,
            const T near,
            const T right,
            const T bottom,
            const T far);

        /** Get size of bounds */
        constexpr Vector<3, T> GetSize() const;

        /** Checks if point is inside of bounds. */
        constexpr bool Intersects(const Vector3<T>& point) const;

        /** Returns a new bounds that represent the union of two(this and another) bounds. */
        constexpr Bounds<3, T> Union(const Bounds<3, T>& bounds) const;

        /** Move lower and highter value by a given distance. */
        constexpr Bounds<3, T>& Move(const Vector3f32& distance);

        /** Clamps high values to low values if high < low. */
        constexpr Bounds<3, T>& ClampHighToLow();

        /** Checks if area of bounds are < 0. */
        constexpr bool IsEmpty() const;

        /** Operators. */
        /**@{*/
        constexpr bool operator == (const Bounds<3, T>& rhs) const;
        constexpr bool operator != (const Bounds<3, T>& rhs) const;
        /**@}*/

        MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
        union
        {
            struct
            {
                T left;
                T top;
                T near;
            };
            Vector3<T> low;
        };
        union
        {
            struct
            {
                T right;
                T bottom;
                T far;
            };
            Vector3<T> high;
        };
        MOLTEN_ANONYMOUS_STRUCTURE_END

    };

    template<typename T>
    using Bounds2 = Bounds<2, T>;
    using Bounds2f32 = Bounds2<float>;
    using Bounds2i32 = Bounds2<int32_t>;
    using Bounds2ui32 = Bounds2<uint32_t>;

    template<typename T>
    using Bounds3 = Bounds<3, T>;
    using Bounds3f32 = Bounds3<float>;
    using Bounds3i32 = Bounds3<int32_t>;
    using Bounds3ui32 = Bounds3<uint32_t>;


}

#include "Molten/Math/Bounds.inl"

#endif