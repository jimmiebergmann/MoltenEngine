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

#ifndef CURSE_CORE_UTILITY_BITFIELD_HPP
#define CURSE_CORE_UTILITY_BITFIELD_HPP

#include "Curse/Types.hpp"
#include "Curse/System/Exception.hpp"
#include <array>
#include <type_traits>

namespace Curse
{

    /**
    * @brief Bitfield class.
    */
    template<size_t BitCount>
    class Bitfield
    {

    public:

        using FragmentType = uint64_t;
        static constexpr size_t FragmentBitCount = sizeof(FragmentType) * 8;
        static constexpr size_t ActualBitCount = (BitCount + FragmentBitCount - 1) & ~(FragmentBitCount - 1);
        static constexpr size_t FragmentCount = ActualBitCount / FragmentBitCount;

        static_assert(FragmentCount != 0, "Bitfield<0> is not allowed.");

        /**
        * @brief Constructing bitfield, with all bits set to 0.
        */
        Bitfield();

        /**
        * @brief Copy bitfield from another bitfield of the same length.
        */
        Bitfield(const Bitfield& bitfield);

        /**
        * @brief Move bitfield from another bitfield.
        */
        Bitfield(Bitfield&& bitfield) noexcept;

        /**
        * @brief Constructing bitfield by setting all bits to zero, except the passed bits.
        */
        template<typename BitType, typename ... RestBitTypes>
        Bitfield(const BitType bit, const RestBitTypes ... rest);

        /**
        * @brief Set bit to 1. Passed bit is the bit index.
        */
        template<typename BitType>
        void Set(const BitType bit);

        /**
        * @brief Set multiple bits to 1.
        */
        template<typename BitType, typename ... RestBitTypes>
        void Set(const BitType bit, const RestBitTypes ... rest);

        /**
        * @return True if passed bit index is 1, else false.
        */
        bool IsSet(const size_t bit) const;

        /**
        * @return True if passed bit index is 0, else false.
        */
        bool IsUnset(const size_t bit) const;

        /**
        * @return True if any bit in the bitfield is set to 1.
        */
        bool IsAnySet() const;

        /**
        * @brief Unset passed bit index.
        */
        template<typename BitType>
        void Unset(const BitType bit);

        /**
        * @brief Unset multiple bits.
        */
        template<typename BitType, typename ... RestBitTypes>
        void Unset(const BitType bit, const RestBitTypes ... rest);

        /**
        * @brief Unset all bits.
        */
        void UnsetAll();

        /**
        * @brief Assign bitfield from another bitfield of the same length.
        */
        Bitfield& operator =(const Bitfield& bitfield);

        /**
        * @brief Move assign bitfield from another bitfield.
        */
        Bitfield& operator =(Bitfield&& bitfield) noexcept;

        /**
        * @return AND operator.
        */
        Bitfield operator &(const Bitfield& bitmask) const;

        /**
        * @return Self AND operator.
        */
        Bitfield& operator &=(const Bitfield& bitmask);
        
        /**
        * @return NOT operator.
        */
        Bitfield operator ~() const;

        /**
        * @return OR operator.
        */
        Bitfield operator |(const Bitfield& bitmask) const;

        /**
        * @return Self OR operator.
        */
        Bitfield& operator |=(const Bitfield& bitmask);

        /**
        * @return @return True if passed bitfield is same as this bitfield, else false.
        */
        bool operator ==(const Bitfield& bitmask) const;

        /**
        * @return @return True if passed bitfield is different from this bitfield, else false.
        */
        bool operator !=(const Bitfield& bitmask) const;

        /**
        * @return @return True if this bitfield is smaller than passed bitfield.
        */
        bool operator <(const Bitfield& bitmask) const;

        /**
        * @return @return True if this bitfield is bigger than passed bitfield.
        */
        bool operator >(const Bitfield& bitmask) const;

        /**
        * @return @return String representation.
        */
        std::string ToString() const;

    private:

        using FragmentArray = std::array<FragmentType, FragmentCount>;

        FragmentArray m_fragments;

        /**
        * @brief Helper function for initializing the bitfield.
        */
        constexpr FragmentArray CreateEmptyFragmentArray();

    };

}

#include "Curse/Utility/Bitfield.inl"

#endif