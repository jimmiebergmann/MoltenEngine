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

#ifndef MOLTEN_CORE_UTILITY_BUFFERCAPACITYPOLICY_HPP
#define MOLTEN_CORE_UTILITY_BUFFERCAPACITYPOLICY_HPP

#include "Molten/Core.hpp"
#include <functional>
#include <algorithm>

namespace Molten
{
    class BufferCapacityScalarPolicy
    {

    public:

        explicit BufferCapacityScalarPolicy(const size_t value);

        size_t operator()(size_t requestedSize, size_t oldCapacity) const;

    private:

        size_t m_value;

    };


    /** Utility class for controlling capacity and growth of buffers by pre-allocating/shrinking storage. */
    class BufferCapacityPolicy
    {

    public:

        using CapacityFunctor = std::function<size_t(size_t, size_t)>;

        BufferCapacityPolicy(CapacityFunctor&& capacityFunctor);
        ~BufferCapacityPolicy() = default;

        BufferCapacityPolicy(const BufferCapacityPolicy&) = default;
        BufferCapacityPolicy(BufferCapacityPolicy&&) = default;
        BufferCapacityPolicy& operator = (const BufferCapacityPolicy&) = default;
        BufferCapacityPolicy& operator = (BufferCapacityPolicy&&) = default;

        bool SetSize(const size_t size);
        [[nodiscard]] size_t GetSize() const;

        void SetCapacity(const size_t capacity);
        [[nodiscard]] size_t GetCapacity() const;

    private:

        size_t m_capacity;
        size_t m_size;
        CapacityFunctor m_capacityFunctor;

    };

}

#include "Molten/Utility/BufferCapacityPolicy.inl"

#endif