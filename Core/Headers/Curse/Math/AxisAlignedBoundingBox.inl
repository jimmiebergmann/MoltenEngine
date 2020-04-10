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


#include <algorithm>

namespace Curse
{

    template<size_t D, typename T>
    inline AxisAlignedBoundingBox<D, T>::AxisAlignedBoundingBox() :
        min{ 0 },
        max{ 0 }
    { }

    template<size_t D, typename T>
    inline AxisAlignedBoundingBox<D, T>::AxisAlignedBoundingBox(const VectorType& size) :
        min(size / static_cast<T>(-2)),
        max(size / static_cast<T>(2))
    { }

    template<size_t D, typename T>
    inline AxisAlignedBoundingBox<D, T>::AxisAlignedBoundingBox(const VectorType& min, const VectorType& max) :
        min(min),
        max(max)
    { }

    template<size_t D, typename T>
    inline void AxisAlignedBoundingBox<D, T>::SetMin(const VectorType& max)
    {

    }

    template<size_t D, typename T>
    inline void AxisAlignedBoundingBox<D, T>::SetMax(const VectorType& min)
    {
    }

    template<size_t D, typename T>
    inline void AxisAlignedBoundingBox<D, T>::FixMinMax()
    {
        for (size_t i = 0; i < D; i++)
        {
            if (min.c[i] > max.c[i])
            {
                std::swap(min.c[i], max.c[i]);
            }
        }
    }

    template<size_t D, typename T>
    inline bool AxisAlignedBoundingBox<D, T>::Intersects(const AxisAlignedBoundingBox& aabb) const
    {
        return Intersects(std::make_index_sequence<D>{}, aabb);
    }

    template<size_t D, typename T>
    inline AxisAlignedBoundingBox<D, T> AxisAlignedBoundingBox<D, T>::operator + (const VectorType& vector) const
    {
        return { min + vector, max + vector };
    }

    template<size_t D, typename T>
    inline AxisAlignedBoundingBox<D, T>& AxisAlignedBoundingBox<D, T>::operator += (const VectorType& vector)
    {
        min += vector;
        max += vector;
        return *this;
    }

    template<size_t D, typename T>
    inline AxisAlignedBoundingBox<D, T> AxisAlignedBoundingBox<D, T>::operator - (const VectorType& vector) const
    {
        return { min - vector, max - vector };
    }

    template<size_t D, typename T>
    inline AxisAlignedBoundingBox<D, T>& AxisAlignedBoundingBox<D, T>::operator -= (const VectorType& vector)
    {
        min -= vector;
        max -= vector;
        return *this;
    }

    template<size_t D, typename T>
    template<size_t... Index>
    inline bool AxisAlignedBoundingBox<D, T>::Intersects(std::index_sequence<Index...> && indices, const AxisAlignedBoundingBox& aabb) const
    {
        return (((max.c[Index] > aabb.min.c[Index]) && (min.c[Index] < aabb.max.c[Index])) && ...);
    }

}