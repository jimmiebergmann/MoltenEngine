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

namespace Molten::StringUtility
{

    // To hex implementations.
    template<size_t VSize>
    std::array<char, VSize * 2> ToLowercaseHex(const std::array<uint8_t, VSize>& input)
    {
        auto result = std::array<char, VSize * 2>{};

        for (size_t i = 0; i < VSize; i++)
        {
            result[i * 2] = Hex::Lowercase[static_cast<size_t>((input[i] >> 4) & 0x0F)];
            result[(i * 2) + 1] = Hex::Lowercase[static_cast<size_t>(input[i] & 0x0F)];
        }

        return result;
    }

    template<size_t VSize>
    std::array<char, VSize * 2> ToUppercaseHex(const std::array<uint8_t, VSize>& input)
    {
        auto result = std::array<char, VSize * 2>{};

        for (size_t i = 0; i < VSize; i++)
        {
            result[i * 2] = Hex::Uppercase[static_cast<size_t>((input[i] >> 4) & 0x0F)];
            result[(i * 2) + 1] = Hex::Uppercase[static_cast<size_t>(input[i] & 0x0F)];
        }

        return result;
    }

    // Trim implementations.
    template<typename T, typename TTrimChars>
    void Trim(std::basic_string_view<T>& string, const TTrimChars& trimChars)
    {
        const auto start = string.find_first_not_of(trimChars);
        if (start == std::string::npos)
        {
            string = { string.data(), 0 };
            return;
        }

        const auto end = string.find_last_not_of(trimChars);
        string = { string.data() + start, end - start + 1 };
    }

    template<typename T, typename TTrimChars>
    [[nodiscard]] std::basic_string_view<T> Trim(const std::basic_string_view<T>& string, const TTrimChars& trimChars)
    {
        auto copy = string;
        Trim(copy, trimChars);
        return copy;
    }

    template<typename T, typename TTrimChars>
    void Trim(std::basic_string<T>& string, const TTrimChars& trimChars)
    {
        const auto start = string.find_first_not_of(trimChars);
        if (start == std::string::npos)
        {
            string.clear();
            return;
        }

        const auto end = string.find_last_not_of(trimChars);
        const auto range = end - start;
        if (range == string.size())
        {
            return;
        }

        std::copy(string.begin() + start, string.begin() + end + 1, string.begin());
        string.resize(range + 1);
    }

    template<typename T, typename TTrimChars>
    [[nodiscard]] std::basic_string<T> Trim(const std::basic_string<T>& string, const TTrimChars& trimChars)
    {
        auto copy = string;
        Trim(copy, trimChars);
        return copy;
    }


    // Trim front implementations.
    template<typename T, typename TTrimChars>
    void TrimFront(std::basic_string_view<T>& string, const TTrimChars& trimChars)
    {
        const auto start = string.find_first_not_of(trimChars);
        if (start == std::string::npos)
        {
            string = { string.data(), 0 };
            return;
        }

        if (start == 0)
        {
            return;
        }

        string = { string.data() + start, string.size() - start };
    }

    template<typename T, typename TTrimChars>
    [[nodiscard]] std::basic_string_view<T> TrimFront(const std::basic_string_view<T>& string, const TTrimChars& trimChars)
    {
        auto copy = string;
        TrimFront(copy, trimChars);
        return copy;
    }

    template<typename T, typename TTrimChars>
    void TrimFront(std::basic_string<T>& string, const TTrimChars& trimChars)
    {
        const auto start = string.find_first_not_of(trimChars);
        if (start == std::string::npos)
        {
            string.clear();
            return;
        }

        if (start == 0)
        {
            return;
        }

        std::copy(string.begin() + start, string.end(), string.begin());
        string.resize(string.size() - start);
    }

    template<typename T, typename TTrimChars>
    [[nodiscard]] std::basic_string<T> TrimFront(const std::basic_string<T>& string, const TTrimChars& trimChars)
    {
        auto copy = string;
        TrimFront(copy, trimChars);
        return copy;
    }


    // Trim back implementations.
    template<typename T, typename TTrimChars>
    void TrimBack(std::basic_string_view<T>& string, const TTrimChars& trimChars)
    {
        const auto end = string.find_last_not_of(trimChars);
        if (end == std::string::npos)
        {
            string = { string.data(), 0 };
            return;
        }

        const size_t newSize = end + 1;
        string = { string.data(), newSize };
    }

    template<typename T, typename TTrimChars>
    [[nodiscard]] std::basic_string_view<T> TrimBack(const std::basic_string_view<T>& string, const TTrimChars& trimChars)
    {
        auto copy = string;
        TrimBack(copy, trimChars);
        return copy;
    }

    template<typename T, typename TTrimChars>
    void TrimBack(std::basic_string<T>& string, const TTrimChars& trimChars)
    {
        const auto end = string.find_last_not_of(trimChars);
        if (end == std::string::npos)
        {
            string.clear();
            return;
        }

        const size_t newSize = end + 1;
        if (string.size() == newSize)
        {
            return;
        }

        string.resize(newSize);
    }

    template<typename T, typename TTrimChars>
    [[nodiscard]] std::basic_string<T> TrimBack(const std::basic_string<T>& string, const TTrimChars& trimChars)
    {
        auto copy = string;
        TrimBack(copy, trimChars);
        return copy;
    }
}