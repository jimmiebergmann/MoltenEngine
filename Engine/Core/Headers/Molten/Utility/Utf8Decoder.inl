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

namespace Molten
{

    // UTF-8 decoder implementations.
    inline Utf8DecoderIterator::Utf8DecoderIterator() :
        m_data(nullptr),
        m_begin(nullptr),
        m_end(nullptr),
        m_codePoint(0) 
    {}

    inline Utf8DecoderIterator& Utf8DecoderIterator::operator ++()
    {
        ReadNextCodePoint();
        return *this;
    }

    //inline Utf8DecoderIterator& Utf8DecoderIterator::operator --()
    //{
    //    return *this;
    //}

    inline Utf8DecoderIterator Utf8DecoderIterator::operator ++(int)
    {
        Utf8DecoderIterator copy = *this;
        ReadNextCodePoint();
        return copy;
    }

    //inline Utf8DecoderIterator Utf8DecoderIterator::operator --(int)
    //{
    //    return {};
    //}

    inline Utf8DecoderIterator::CodePointType Utf8DecoderIterator::operator *() const
    {
        return m_codePoint;
    }

    inline bool Utf8DecoderIterator::operator ==(const Utf8DecoderIterator& iterator) const
    {
        return m_data == iterator.m_data;
    }

    inline bool Utf8DecoderIterator::operator !=(const Utf8DecoderIterator& iterator) const
    {
        return m_data != iterator.m_data;
    }

    inline Utf8DecoderIterator::Utf8DecoderIterator(const uint8_t* begin, const uint8_t* end) :
        m_data(begin),
        m_begin(begin),
        m_end(end),
        m_codePoint(0)
    {
        ReadNextCodePoint();
    }

    inline Utf8DecoderIterator::Utf8DecoderIterator(const uint8_t* data, const uint8_t* begin, const uint8_t* end) :
        m_data(data),
        m_begin(begin),
        m_end(end),
        m_codePoint(0)
    {}

    inline size_t Utf8DecoderIterator::GetCharSize(uint8_t byte)
    {
        if ((byte & 0xE0) == 0xC0)
        {
            return 2;
        }
        else if ((byte & 0xF0) == 0xE0)
        {
            return 3;
        }
        else if ((byte & 0xF8) == 0xF0)
        {
            return 4;
        }

        return 1;
    }

    inline void Utf8DecoderIterator::ReadNextCodePoint()
    {
        static const uint8_t firstBitMask[4] = { 0xFF, 0x1F, 0x0F, 0x07 };

        const auto size = GetCharSize(*m_data);
        if(m_data + size >= m_end)
        {
            m_data = m_end;
            m_codePoint = 0;
            return;
        }

        const auto sizeMinusOne = size - 1;
        m_codePoint = (firstBitMask[sizeMinusOne] & m_data[0]) << (sizeMinusOne * 6);
        
        for (size_t i = 1; i < size; ++i)
        {
            m_codePoint |= static_cast<CodePointType>(m_data[i] & 0x3F) << ((sizeMinusOne - i) * 6);
        }

        m_data += size;
    }


    // UTF-8 decoder implementations.
    inline Utf8Decoder::Utf8Decoder() :
        m_data(nullptr),
        m_dataSize(0)
    {}

    inline Utf8Decoder::Utf8Decoder(const char* cString) :
        m_data(reinterpret_cast<const uint8_t*>(cString)),
        m_dataSize(strlen(cString))
    {}

    template<typename TValueType>
    Utf8Decoder::Utf8Decoder(const TValueType* data, const size_t size) :
        m_data(reinterpret_cast<const uint8_t*>(data)),
        m_dataSize(size)
    {
        static_assert(sizeof(TValueType) == 1, "TValueType must be of size 1.");
    }

    template<typename TValueType>  
    Utf8Decoder::Utf8Decoder(const std::basic_string<TValueType>& string) :
        m_data(reinterpret_cast<const uint8_t*>(string.c_str())),
        m_dataSize(string.size())
    {
        static_assert(sizeof(TValueType) == 1, "TValueType must be of size 1.");
    }

    template<typename TValueType>
    Utf8Decoder::Utf8Decoder(const std::basic_string_view<TValueType>& stringView) :
        m_data(reinterpret_cast<const uint8_t*>(stringView.data())),
        m_dataSize(stringView.size())
    {
        static_assert(sizeof(TValueType) == 1, "TStringValueType must be of size 1.");
    }

    inline bool Utf8Decoder::operator ==(const Utf8Decoder& rhs) const
    {
        return m_data == rhs.m_data && m_dataSize == rhs.m_dataSize;
    }

    inline bool Utf8Decoder::operator !=(const Utf8Decoder& rhs) const
    {
        return m_data != rhs.m_data || m_dataSize != rhs.m_dataSize;
    }

    inline Utf8Decoder::Iterator Utf8Decoder::begin() const
    {
        return Iterator{ m_data, m_data + m_dataSize + 1 };
    }
    
    inline Utf8Decoder::Iterator Utf8Decoder::end() const
    {
        auto* endPointer = m_data + m_dataSize + 1;
        return Iterator{ endPointer, m_data, endPointer };
    }

    inline bool Utf8Decoder::IsEmpty() const
    {
        return m_data == nullptr;
    }

}