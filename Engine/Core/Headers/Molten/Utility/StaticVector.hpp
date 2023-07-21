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

#ifndef MOLTEN_CORE_UTILITY_STATICVECTOR_HPP
#define MOLTEN_CORE_UTILITY_STATICVECTOR_HPP

#include "Molten/Core.hpp"
#include <stdexcept>
#include <memory>

namespace Molten
{

    template<typename T, size_t VCapacity>
    class StaticVector
    {

        static_assert(VCapacity > 0, "StaticVector cannot be of size 0.");

    public:

        using size_type = size_t;
        using value_type = T;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        static constexpr auto capacity = VCapacity;

        constexpr StaticVector() = default;

        template<typename ... VValues>
        constexpr StaticVector(VValues ... values) :
            m_values{ values... },
            m_size{ sizeof...(values)}
        {}

        constexpr pointer begin()
        {
            return m_values;
        }
        constexpr const_pointer begin() const
        {
            return m_values;
        }

        constexpr pointer end()
        {
            return m_values + m_size;
        }
        constexpr const_pointer end() const
        {
            return m_values + m_size;
        }

        constexpr size_type size() const
        {
            return m_size;
        }

        constexpr reference at(size_type pos)
        {
            if (pos >= m_size)
            {
                throw std::out_of_range("");
            }
            return m_values[pos];
        }

        constexpr const_reference at(size_type pos) const
        {
            if (pos >= m_size)
            {
                throw std::out_of_range("");
            }
            return m_values[pos];
        }

        constexpr reference operator[](size_type pos)
        {
            return m_values[pos];
        }

        constexpr const_reference operator[](size_type pos) const
        {
            return m_values[pos];
        }

    private:

        T m_values[VCapacity];
        size_t m_size = 0;

    };

}

#endif
