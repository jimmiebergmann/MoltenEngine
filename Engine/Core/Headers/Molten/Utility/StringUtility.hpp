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

#ifndef MOLTEN_CORE_UTILITY_STRINGUTILITY_HPP
#define MOLTEN_CORE_UTILITY_STRINGUTILITY_HPP

#include <string>
#include <string_view>
#include <array>

namespace Molten::StringUtility
{
    namespace Hex
    {
        static inline const std::array<char, 16> Lowercase = {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
        };

        static inline const std::array<char, 16> Uppercase = {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
        };
    }

    template<size_t VSize>
    std::array<char, VSize * 2> ToLowercaseHex(const std::array<uint8_t, VSize>& input);

    template<size_t VSize>
    std::array<char, VSize * 2> ToUppercaseHex(const std::array<uint8_t, VSize>& input);


    /** String view trim functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    void Trim(std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    [[nodiscard]] std::basic_string_view<T> Trim(const std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/

    /** String trim functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    void Trim(std::basic_string<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    [[nodiscard]] std::basic_string<T> Trim(const std::basic_string<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/


    /** String view trim front functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    void TrimFront(std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    [[nodiscard]] std::basic_string_view<T> TrimFront(const std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/

    /** String trim front functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    void TrimFront(std::basic_string<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    [[nodiscard]] std::basic_string<T> TrimFront(const std::basic_string<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/


    /** String view trim back functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    void TrimBack(std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    [[nodiscard]] std::basic_string_view<T> TrimBack(const std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/

    /** String trim back functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    void TrimBack(std::basic_string<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string_view<T>>
    [[nodiscard]] std::basic_string<T> TrimBack(const std::basic_string<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/

}

#include "Molten/Utility/StringUtility.inl"

#endif