/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_MATH_TRIANGLE_HPP
#define MOLTEN_CORE_MATH_TRIANGLE_HPP

#include "Molten/Math/Vector.hpp"
#include <algorithm>

namespace Molten
{

    /** Axis aligned bounding box. */
    template<size_t VDimensions, typename T>
    class Triangle
    {

    public:

        static constexpr size_t Dimensions = VDimensions;
        using Type = T;

        MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
        union
        {
            struct
            {
                Vector<VDimensions, T> p0;
                Vector<VDimensions, T> p1;
                Vector<VDimensions, T> p2;
            };
            Vector<VDimensions, T> p[Dimensions];
        };
        MOLTEN_ANONYMOUS_STRUCTURE_END

    };

    template<typename T>
    class Triangle<2, T>
    {

    public:

        static constexpr size_t Dimensions = 2;
        using Type = T;

        Triangle();
        Triangle(
            Vector<2, T> p0,
            Vector<2, T> p1,
            Vector<2, T> p2);

        [[nodiscard]] bool Intersects(const Vector<2, T>& point) const;

        MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
        union
        {
            struct
            {
                Vector<2, T> p0;
                Vector<2, T> p1;
                Vector<2, T> p2;
            };
            Vector<2, T> p[Dimensions];
        };
        MOLTEN_ANONYMOUS_STRUCTURE_END

    };


    template<typename T>
    using Triangle2 = Triangle<2, T>;
    using Triangle2f32 = Triangle2<float>;

}

#include "Molten/Math/Triangle.inl"

#endif