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

#ifndef CURSE_CORE_MATH_AXISALIGNEDBOUNDINGBOX_HPP
#define CURSE_CORE_MATH_AXISALIGNEDBOUNDINGBOX_HPP

#include "Curse/Math/Vector.hpp"
#include <utility>

namespace Curse
{

    /**
    * @brief Axis aligned bounding box class.
    *        Helper typedefs are available for a shorter names, see Aabb, Aabb2, etc..
    */
    template<size_t D, typename T>
    class AxisAlignedBoundingBox
    {

    public:

        static constexpr size_t Dimensions = D; ///< Number of dimensions of this bounding box.
        using Type = T;                         ///< Data type used for the coordinate system.
        using VectorType = Vector<D, T>;        ///< Dimensional vector, used for storing min and max values.

        /**
        * @brief Default constructing an AABB with min and max values of 0.
        */
        AxisAlignedBoundingBox();

        /**
        * @brief Construct an AABB with given size.
        *        min is set to size/-2 and max is set to size/2.
        */
        explicit AxisAlignedBoundingBox(const VectorType& size);

        /**
        * @brief Constructor.
        */
        AxisAlignedBoundingBox(const VectorType& min, const VectorType& max);

        /**
        * @brief Safe setter function of min value, making sure that min is <= max.
        */
        void SetMin(const VectorType& max);

        /**
        * @brief Safe setter function of max value, making sure that max is >= min.
        */
        void SetMax(const VectorType& min);

        /**
        * @brief Swap components if min is > max.
        */
        void FixMinMax();

        /**
        * @brief Checks if this AABB intersects with another AABB.
        */
        bool Intersects(const AxisAlignedBoundingBox& aabb) const;

        /**
        * @brief Add vector to AABB's bounds. Resulting AABB is being moved by vector.
        */
        AxisAlignedBoundingBox operator + (const VectorType& vector) const;

        /**
        * @brief Add vector to this AABB's bounds. This AABB is being moved by vector.
        */
        AxisAlignedBoundingBox& operator += (const VectorType& vector);

        /**
        * @brief Subtract vector from AABB's bounds. Resulting AABB is being moved by vector.
        */
        AxisAlignedBoundingBox operator - (const VectorType& vector) const;

        /**
        * @brief Subtract vector from this AABB's bounds. This AABB is being moved by vector.
        */
        AxisAlignedBoundingBox& operator -= (const VectorType& vector);


        VectorType min; ///< Min value of AABB, also know as "lower corner";
        VectorType max; ///< Max value of AABB, also know as "higher corner";

    private:

        /**
        * @brief Helper function, for checking if an AABB intersects with this AABB.
        *        This function uses an index sequence for checking D dimensions.
        */
        template<size_t... Index>
        bool Intersects(std::index_sequence<Index...> && indices, const AxisAlignedBoundingBox& aabb) const;

    };

    template<size_t D, typename T>
    using Aabb = AxisAlignedBoundingBox<D, T>;

    template<typename T>
    using Aabb2 = AxisAlignedBoundingBox<2, T>;

    template<typename T>
    using Aabb3 = AxisAlignedBoundingBox<3, T>;

}

#include "Curse/Math/AxisAlignedBoundingBox.inl"

#endif