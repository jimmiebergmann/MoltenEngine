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

#ifndef MOLTEN_CORE_UTILITY_STRINGUTILITY_HPP
#define MOLTEN_CORE_UTILITY_STRINGUTILITY_HPP

#include <string>
#include <string_view>

namespace Molten::StringUtility
{

    /** String view trim functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string<T>>
    void Trim(std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string<T>>
    [[nodiscard]] std::basic_string_view<T> Trim(const std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/

    /** String trim functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string<T>>
    void Trim(std::basic_string<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string<T>>
    [[nodiscard]] std::basic_string<T> Trim(const std::basic_string<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/


    /** String view trim front functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string<T>>
    void TrimFront(std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string<T>>
    [[nodiscard]] std::basic_string_view<T> TrimFront(const std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/

    /** String trim front functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string<T>>
    void TrimFront(std::basic_string<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string<T>>
    [[nodiscard]] std::basic_string<T> TrimFront(const std::basic_string<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/


    /** String view trim back functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string<T>>
    void TrimBack(std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string<T>>
    [[nodiscard]] std::basic_string_view<T> TrimBack(const std::basic_string_view<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/

    /** String trim back functions.*/
    /**@{*/
    template<typename T, typename TTrimChars = std::basic_string<T>>
    void TrimBack(std::basic_string<T>& string, const TTrimChars& trimChars = " \t");

    template<typename T, typename TTrimChars = std::basic_string<T>>
    [[nodiscard]] std::basic_string<T> TrimBack(const std::basic_string<T>& string, const TTrimChars& trimChars = " \t");
    /**@}*/

}

#include "Molten/Utility/StringUtility.inl"

#endif