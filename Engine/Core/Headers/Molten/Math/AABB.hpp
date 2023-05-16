/*
* MIT License
*
* Copyright (c) 2023 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_MATH_AABB_HPP
#define MOLTEN_CORE_MATH_AABB_HPP

#include "Molten/Math/Vector.hpp"
#include <algorithm>

namespace Molten
{

    /** Axis aligned bounding box. */
    template<size_t VDimensions, typename T>
    class AABB
    {

    public:

        static constexpr size_t Dimensions = VDimensions;
        using Type = T;

        Vector<VDimensions, T> position;
        Vector<VDimensions, T> size;

    };

    template<typename T>
    class AABB<2, T>
    {

    public:

        static constexpr size_t Dimensions = 2;
        using Type = T;

        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] Vector<2, T> GetHigh() const;
        [[nodiscard]] bool Intersects(const Vector<2, T>& point) const;

        Vector<2, T> position;
        Vector<2, T> size;

    };

    template<typename T>
    class AABB<3, T>
    {

    public:

        static constexpr size_t Dimensions = 3;
        using Type = T;

        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] Vector<3, T> GetHigh() const;
        [[nodiscard]] bool Intersects(const Vector<3, T>& point) const;

        Vector<3, T> position;
        Vector<3, T> size;

    };

    template<typename T>
    using AABB2 = AABB<2, T>;
    using AABB2f32 = AABB2<float>;
    using AABB2i32 = AABB2<int32_t>;

    template<typename T>
    using AABB3 = AABB<3, T>;
    using AABB3f32 = AABB3<float>;
    using AABB3i32 = AABB3<int32_t>;

}

#include "Molten/Math/AABB.inl"

#endif