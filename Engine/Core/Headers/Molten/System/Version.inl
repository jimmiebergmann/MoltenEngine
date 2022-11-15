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

namespace Molten
{

    template<>
    inline std::string ToString(const Version& value)
    {
        return ToString(value, true);
    }

    inline std::string ToString(const Version& value, const bool ignoreTrail)
    {
        std::string str = std::to_string(value.Major);

        if (!ignoreTrail || value.Minor != 0 || value.Patch != 0)
        {
            str += "." + std::to_string(value.Minor);
        }
        if (!ignoreTrail || value.Patch != 0)
        {
            str += "." + std::to_string(value.Patch);
        }

        return str;
    }

    template<>
    inline Expected<Version, size_t> FromString(const std::string_view input)
    {
        auto readNextNumber = [&input](std::string_view& string) -> Expected<uint32_t, size_t>
        {
            const auto* first = string.data();
            const auto* last = first + string.size();

            uint32_t value = 0;
            const auto result = std::from_chars(first, last, value);
            if (result.ec != std::errc())
            {
                return Unexpected(static_cast<size_t>(result.ptr - input.data()));
            }

            size_t ignoreDot = 0;
            if (result.ptr < last)
            {
                if (*result.ptr != '.')
                {
                    return Unexpected(static_cast<size_t>(result.ptr - input.data()));
                }

                ignoreDot = 1;
            }

            const auto* newFirst = result.ptr + ignoreDot;
            const auto newSize = static_cast<size_t>(last - newFirst);
            string = { newFirst, newSize };

            return value;
        };

        Version version = {};

        auto values = std::array{ &version.Major, &version.Minor, &version.Patch };
        std::string_view remainingInput = input;

        for (size_t i = 0; i < values.size(); i++)
        {
            auto numberResult = readNextNumber(remainingInput);
            if (!numberResult.HasValue())
            {
                return Unexpected(numberResult.Error());
            }

            *values[i] = numberResult.Value();

            if (remainingInput.empty())
            {
                break;
            }
        }

        return version;
    }

}