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

    template<size_t BitCount>
    inline Bitfield<BitCount>::Bitfield() :
        m_fragments(CreateEmptyFragmentArray())
    { }

    template<size_t BitCount>
    inline Bitfield<BitCount>::Bitfield(const Bitfield& bitfield) :
        m_fragments(bitfield.m_fragments)
    { }

    template<size_t BitCount>
    inline Bitfield<BitCount>::Bitfield(Bitfield&& bitfield) noexcept :
        m_fragments(std::move(bitfield.m_fragments))
    { }

    template<size_t BitCount>
    template<typename BitType, typename ... RestBitTypes>
    inline Bitfield<BitCount>::Bitfield(const BitType bit, const RestBitTypes ... rest) :
        m_fragments(CreateEmptyFragmentArray())
    {
        Set(bit, rest...);
    }

    template<size_t BitCount>
    template<typename BitType>
    inline void Bitfield<BitCount>::Set(const BitType bit)
    {
        static_assert(std::is_integral<BitType>::value, "Bit type is not an integral.");
        if (ActualBitCount <= static_cast<size_t>(bit))
        {
            throw Exception("Bit is out of range.");
        }
        const size_t fragmentIndex = static_cast<size_t>(bit) / FragmentBitCount;
        const size_t index = static_cast<size_t>(bit) % FragmentBitCount;
        m_fragments[fragmentIndex] |= FragmentType(1) << index;
    }

    template<size_t BitCount>
    template<typename BitType, typename ... RestBitTypes>
    inline void Bitfield<BitCount>::Set(const BitType bit, const RestBitTypes ... rest)
    {
        static_assert(std::is_integral<BitType>::value, "Bit type is not an integral.");
        Set(bit);
        Set(rest...);
    }

    template<size_t BitCount>
    inline bool Bitfield<BitCount>::IsSet(const size_t bit) const
    {
        const size_t fragmentIndex = bit / FragmentBitCount;
        const size_t index = bit % FragmentBitCount;
        return m_fragments[fragmentIndex] & FragmentType(1) << index;
    }

    template<size_t BitCount>
    inline bool Bitfield<BitCount>::IsUnset(const size_t bit) const
    {
        const size_t fragmentIndex = bit / FragmentBitCount;
        const size_t index = bit % FragmentBitCount;
        return !(m_fragments[fragmentIndex] & FragmentType(1) << index);
    }

    template<size_t BitCount>
    inline bool Bitfield<BitCount>::IsAnySet() const
    {
        for (size_t i = 0; i < FragmentCount; i++)
        {
            if (m_fragments[i])
            {
                return true;
            }
        }
        return false;
    }

    template<size_t BitCount>
    template<typename BitType>
    inline void Bitfield<BitCount>::Unset(const BitType bit)
    {
        static_assert(std::is_integral<BitType>::value, "Bit type is not an integral.");
        if (ActualBitCount <= static_cast<size_t>(bit))
        {
            throw Exception("Bit is out of range.");
        }
        const size_t fragmentIndex = static_cast<size_t>(bit) / FragmentBitCount;
        const size_t index = static_cast<size_t>(bit) % FragmentBitCount;
        m_fragments[fragmentIndex] &= ~(FragmentType(1) << index);
    }

    template<size_t BitCount>
    template<typename BitType, typename ... RestBitTypes>
    inline void Bitfield<BitCount>::Unset(const BitType bit, const RestBitTypes ... rest)
    {
        static_assert(std::is_integral<BitType>::value, "Bit type is not an integral.");
        Unset(bit);
        Unset(rest...);
    }

    template<size_t BitCount>
    void Bitfield<BitCount>::UnsetAll()
    {
        for (size_t i = 0; i < FragmentCount; i++)
        {
            m_fragments[i] = size_t(0);
        }
    }

    template<size_t BitCount>
    inline Bitfield<BitCount>& Bitfield<BitCount>::operator =(const Bitfield& bitfield)
    {
        m_fragments = bitfield.m_fragments;
        return *this;
    }

    template<size_t BitCount>
    inline Bitfield<BitCount>& Bitfield<BitCount>::operator =(Bitfield&& bitfield) noexcept
    {
        m_fragments = std::move(bitfield.m_fragments);
        return *this;
    }

    template<size_t BitCount>
    inline Bitfield<BitCount> Bitfield<BitCount>::operator &(const Bitfield& bitfield) const
    {
        Bitfield output;
        for (size_t i = 0; i < FragmentCount; i++)
        {
            output.m_fragments[i] = m_fragments[i] & bitfield.m_fragments[i];
        }
        return output;
    }

    template<size_t BitCount>
    inline Bitfield<BitCount>& Bitfield<BitCount>::operator &=(const Bitfield& bitfield)
    {
        for (size_t i = 0; i < FragmentCount; i++)
        {
            m_fragments[i] = m_fragments[i] & bitfield.m_fragments[i];
        }

        return *this;
    }

    template<size_t BitCount>
    inline Bitfield<BitCount> Bitfield<BitCount>::operator ~() const
    {
        Bitfield output;
        for (size_t i = 0; i < FragmentCount; i++)
        {
            output.m_fragments[i] = ~m_fragments[i];
        }
        return output;
    }

    template<size_t BitCount>
    inline Bitfield<BitCount> Bitfield<BitCount>::operator |(const Bitfield& bitfield) const
    {
        Bitfield output;
        for (size_t i = 0; i < FragmentCount; i++)
        {
            output.m_fragments[i] = m_fragments[i] | bitfield.m_fragments[i];
        }
        return output;
    }

    template<size_t BitCount>
    inline Bitfield<BitCount>& Bitfield<BitCount>::operator |=(const Bitfield& bitfield)
    {
        for (size_t i = 0; i < FragmentCount; i++)
        {
            m_fragments[i] = m_fragments[i] | bitfield.m_fragments[i];
        }
        return *this;
    }

    template<size_t BitCount>
    inline bool Bitfield<BitCount>::operator ==(const Bitfield& bitfield) const
    {
        for (size_t i = 0; i < FragmentCount; i++)
        {
            if (m_fragments[i] != bitfield.m_fragments[i])
            {
                return false;
            }
        }
        return true;
    }

    template<size_t BitCount>
    inline bool Bitfield<BitCount>::operator !=(const Bitfield& bitfield) const
    {
        for (size_t i = 0; i < FragmentCount; i++)
        {
            if (m_fragments[i] != bitfield.m_fragments[i])
            {
                return true;
            }
        }
        return false;
    }

    template<size_t BitCount>
    inline bool Bitfield<BitCount>::operator <(const Bitfield& bitfield) const
    {
        for (size_t i = 0; i < FragmentCount; i++)
        {
            if (m_fragments[i] < bitfield.m_fragments[i])
            {
                return true;
            }
        }
        return false;
    }

    template<size_t BitCount>
    inline bool Bitfield<BitCount>::operator >(const Bitfield& bitfield) const
    {
        for (auto i = static_cast<int32_t>(FragmentCount - 1); i >= 0; i--)
        {
            if (m_fragments[i] > bitfield.m_fragments[i])
            {
                return true;
            }
        }
        return false;
    }

    template<size_t BitCount>
    inline std::string Bitfield<BitCount>::ToString() const
    {
        std::string output;
        output.reserve(ActualBitCount);

        for (auto i = static_cast<int32_t>(FragmentCount) - 1; i >= 0; i--)
        {
            for (auto j = static_cast<int32_t>(FragmentBitCount) - 1; j >= 0; j--)
            {
                output += m_fragments[i] & (FragmentType(1) << j) ? '1' : '0';
            }
        }

        return std::move(output);
    }

    template<size_t BitCount>
    inline constexpr typename Bitfield<BitCount>::FragmentArray Bitfield<BitCount>::CreateEmptyFragmentArray()
    {
        FragmentArray fragments = {};
        for (size_t i = 0; i < FragmentCount; i++)
        {
            fragments[i] = size_t(0);
        }

        return std::move(fragments);
    }

}