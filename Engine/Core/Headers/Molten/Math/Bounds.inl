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

namespace Molten
{

    // Bounds D.
    template<size_t VDimensions, typename T>
    constexpr Bounds<VDimensions, T>::Bounds()
    {}

    template<size_t VDimensions, typename T>
    constexpr Bounds<VDimensions, T>::Bounds(
        const Vector<VDimensions, T>& low,
        const Vector<VDimensions, T>& high
    ) :
        low(low),
        high(high)
    {}


    // Bounds 2 
    template<typename T>
    constexpr Bounds<2, T>::Bounds() :
        low(static_cast<T>(0), static_cast<T>(0)),
        high(static_cast<T>(0), static_cast<T>(0))
    {}

    template<typename T>
    constexpr Bounds<2, T>::Bounds(
        const Vector<2, T>& low,
        const Vector<2, T>& high
    ) :
        low(low),
        high(high)
    {}

    template<typename T>
    constexpr Bounds<2, T>::Bounds(
        const T left,
        const T top,
        const T right,
        const T bottom
    ) :
        low(left, top),
        high(right, bottom)
    {}

    template<typename T>
    template<typename U>
    constexpr Bounds<2, T>::Bounds(const Bounds<2, U>& bounds) :
        low(bounds.low),
        high(bounds.high)
    {}

    template<typename T>
    template<typename U1, typename U2>
    constexpr Bounds<2, T> Bounds<2, T>::Union(const Bounds<2, U1>& first, const Bounds<2, U2>& second)
    {
        return {
            std::min(static_cast<T>(first.left), static_cast<T>(second.left)),
            std::min(static_cast<T>(first.top), static_cast<T>(second.top)),
            std::max(static_cast<T>(first.right), static_cast<T>(second.right)),
            std::max(static_cast<T>(first.bottom), static_cast<T>(second.bottom))
        };
    }

    template<typename T>
    constexpr Vector<2, T> Bounds<2, T>::GetSize() const
    {
        return high - low;
    }

    template<typename T>
    constexpr bool Bounds<2, T>::Intersects(const Vector2<T>& point) const
    {
        return 
            point.x >= low.x && point.x <= high.x &&
            point.y >= low.y && point.y <= high.y;
    }

    template<typename T>
    constexpr Bounds<2, T>& Bounds<2, T>::Move(const Vector2<T>& distance)
    {
        low += distance;
        high += distance;
        return *this;
    }

    template<typename T>
    constexpr Bounds<2, T> Bounds<2, T>::WithMargins(const Bounds<2, T>& margins) const
    {
        return { low - margins.low, high + margins.high };
    }

    template<typename T>
    constexpr Bounds<2, T> Bounds<2, T>::WithoutMargins(const Bounds<2, T>& margins) const
    {
        return { low + margins.low, high - margins.high };
    }

    template<typename T>
    constexpr Bounds<2, T>& Bounds<2, T>::AddMargins(const Bounds<2, T>& margins)
    {
        low -= margins.low;
        high += margins.high;
        return *this;
    }

    template<typename T>
    constexpr Bounds<2, T>& Bounds<2, T>::RemoveMargins(const Bounds<2, T>& margins)
    {
        low += margins.low;
        high -= margins.high;
        return *this;
    }

    template<typename T>
    constexpr Bounds<2, T>& Bounds<2, T>::ClampHighToLow()
    {
        high.x = std::max(high.x, low.x);
        high.y = std::max(high.y, low.y);
        return *this;
    }

    template<typename T>
    constexpr bool Bounds<2, T>::IsEmpty() const
    {
        return high.x <= low.x || high.y <= low.y;
    }

    template<typename T>
    constexpr bool Bounds<2, T>::operator == (const Bounds<2, T>& rhs) const
    {
        return high == rhs.high && low == rhs.low;
    }

    template<typename T>
    constexpr bool Bounds<2, T>::operator != (const Bounds<2, T>& rhs) const
    {
        return high != rhs.high || low != rhs.low;
    }


    // Bounds 3
    template<typename T>
    constexpr Bounds<3, T>::Bounds() :
        low(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)),
        high(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0))
    {}

    template<typename T>
    constexpr Bounds<3, T>::Bounds(
        const Vector<3, T>& low,
        const Vector<3, T>& high
    ) :
        low(low),
        high(high)
    {}

    template<typename T>
    constexpr Bounds<3, T>::Bounds(
        const T left,
        const T top,
        const T near,
        const T right,
        const T bottom,
        const T far
    ) :
        low(left, top, near),
        high(right, bottom, far)
    {}

    template<typename T>
    template<typename U>
    constexpr Bounds<3, T>::Bounds(const Bounds<3, U>& bounds) :
        low(bounds.low),
        high(bounds.high)
    {}

    template<typename T>
    template<typename U1, typename U2>
    constexpr Bounds<3, T> Bounds<3, T>::Union(const Bounds<3, U1>& first, const Bounds<3, U2>& second)
    {
        return {
            std::min(static_cast<T>(first.left), static_cast<T>(second.left)),
            std::min(static_cast<T>(first.top), static_cast<T>(second.top)),
            std::min(static_cast<T>(first.near), static_cast<T>(second.near)),
            std::max(static_cast<T>(first.right), static_cast<T>(second.right)),
            std::max(static_cast<T>(first.bottom), static_cast<T>(second.bottom)),
            std::max(static_cast<T>(first.far), static_cast<T>(second.far))
        };
    }

    template<typename T>
    constexpr Vector<3, T> Bounds<3, T>::GetSize() const
    {
        return high - low;
    }

    template<typename T>
    constexpr bool Bounds<3, T>::Intersects(const Vector3<T>& point) const
    {
        return 
            point.x >= low.x && point.x <= high.x &&
            point.y >= low.y && point.y <= high.y &&
            point.z >= low.z && point.z <= high.z;
    }

    template<typename T>
    constexpr Bounds<3, T>& Bounds<3, T>::Move(const Vector3<T>& distance)
    {
        low += distance;
        high += distance;
        return *this;
    }

    template<typename T>
    constexpr Bounds<3, T> Bounds<3, T>::WithMargins(const Bounds<3, T>& margins) const
    {
        return { low - margins.low, high + margins.high };
    }

    template<typename T>
    constexpr Bounds<3, T> Bounds<3, T>::WithoutMargins(const Bounds<3, T>& margins) const
    {
        return { low + margins.low, high - margins.high };
    }

    template<typename T>
    constexpr Bounds<3, T>& Bounds<3, T>::AddMargins(const Bounds<3, T>& margins)
    {
        low -= margins.low;
        high += margins.high;
        return *this;
    }

    template<typename T>
    constexpr Bounds<3, T>& Bounds<3, T>::RemoveMargins(const Bounds<3, T>& margins)
    {
        low += margins.low;
        high -= margins.high;
        return *this;
    }

    template<typename T>
    constexpr Bounds<3, T>& Bounds<3, T>::ClampHighToLow()
    {
        high.x = std::max(high.x, low.x);
        high.y = std::max(high.y, low.y);
        high.z = std::max(high.z, low.z);
        return *this;
    }

    template<typename T>
    constexpr bool Bounds<3, T>::IsEmpty() const
    {
        return high.x <= low.x || high.y <= low.y || high.z <= low.z;
    }

    template<typename T>
    constexpr bool Bounds<3, T>::operator == (const Bounds<3, T>& rhs) const
    {
        return high == rhs.high && low == rhs.low;
    }

    template<typename T>
    constexpr bool Bounds<3, T>::operator != (const Bounds<3, T>& rhs) const
    {
        return high != rhs.high || low != rhs.low;
    }

}