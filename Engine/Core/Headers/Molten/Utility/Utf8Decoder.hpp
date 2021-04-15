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

#ifndef MOLTEN_CORE_UTILITY_UTF8DECODER_HPP
#define MOLTEN_CORE_UTILITY_UTF8DECODER_HPP

#include "Molten/Types.hpp"
#include <string>
#include <string_view>
#include <type_traits>

namespace Molten
{

    class Utf8Decoder;

  
    class Utf8DecoderIterator
    {

    public:

        using CodePointType = uint32_t;

        Utf8DecoderIterator();

        Utf8DecoderIterator(const Utf8DecoderIterator&) = default;
        Utf8DecoderIterator(Utf8DecoderIterator&&) = default;
        Utf8DecoderIterator& operator =(const Utf8DecoderIterator&) = default;
        Utf8DecoderIterator& operator =(Utf8DecoderIterator&&) = default;

        ~Utf8DecoderIterator() = default;

        Utf8DecoderIterator& operator ++();
        //Utf8DecoderIterator<TValueType>& operator --();
        Utf8DecoderIterator operator ++(int);
        //Utf8DecoderIterator<TValueType> operator --(int);

        [[nodiscard]] CodePointType operator *() const;

        [[nodiscard]] bool operator ==(const Utf8DecoderIterator& iterator) const;
        [[nodiscard]] bool operator !=(const Utf8DecoderIterator& iterator) const;

    private:

        Utf8DecoderIterator(const uint8_t* begin, const uint8_t* end); ///< Used for begin.
        Utf8DecoderIterator(const uint8_t* data, const uint8_t* begin, const uint8_t* end); ///< Used for end.

        friend class Utf8Decoder;

        static size_t GetCharSize(uint8_t byte);
        void ReadNextCodePoint();  

        const uint8_t* m_data;
        const uint8_t* m_begin;
        const uint8_t* m_end;
        CodePointType m_codePoint;

    };


 
    class Utf8Decoder
    {

    public:

        using Iterator = Utf8DecoderIterator;

        Utf8Decoder();
     
        explicit Utf8Decoder(const char* cString);

        template<typename TValueType>
        Utf8Decoder(const TValueType* data, const size_t size);

        template<typename TValueType>
        explicit Utf8Decoder(const std::basic_string<TValueType>& string);

        template<typename TValueType>
        explicit Utf8Decoder(const std::basic_string_view<TValueType>& stringView);

        ~Utf8Decoder() = default;

        Utf8Decoder(const Utf8Decoder&) = default;
        Utf8Decoder(Utf8Decoder&&) = default;
        Utf8Decoder& operator =(const Utf8Decoder&) = default;
        Utf8Decoder& operator =(Utf8Decoder&&) = default;

        [[nodiscard]] bool operator ==(const Utf8Decoder& rhs) const;
        [[nodiscard]] bool operator !=(const Utf8Decoder& rhs) const;

        [[nodiscard]] bool IsEmpty() const;

        [[nodiscard]] Iterator begin() const;
        [[nodiscard]] Iterator end() const;

    private:

        const uint8_t* m_data;
        size_t m_dataSize;

    };

}

#include "Molten/Utility/Utf8Decoder.inl"

#endif