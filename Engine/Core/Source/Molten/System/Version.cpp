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

#include "Molten/System/Version.hpp"
#include <array>
#include <charconv>

namespace Molten
{

    static size_t FindNextNumberEnd(const std::string_view& string)
    {
        size_t i = 0;
        for(; i < string.size(); i++)
        {
            if(string[i] == '.')
            {
                return i;
            }
        }
        return i;
    }

    // Static implementations.
    static bool ReadNextNumber(std::string_view& string, uint32_t& value)
    {
        const size_t numberEnd = FindNextNumberEnd(string);
        if(!numberEnd)
        {
            return false;
        }
        const auto result = std::from_chars(string.data(), string.data() + numberEnd, value);
        if(result.ec != std::errc())
        {
            return false;
        }
        string = { result.ptr, string.size() - static_cast<size_t>(result.ptr - string.data()) };
        return true;
    }


    // Version implementations.
    const Version Version::None = Version(0, 0, 0);

    Version::Version(const uint32_t major, const uint32_t minor, const uint32_t patch) :
        Major(major),
        Minor(minor),
        Patch(patch)
    {
    }

    std::string Version::AsString(const bool ignoreTrail) const
    {
        std::string str = std::to_string(Major);

        if (!ignoreTrail || Minor != 0 || Patch != 0)
        {
            str += "." + std::to_string(Minor);
        }
        if (!ignoreTrail|| Patch != 0)
        {
            str += "." + std::to_string(Patch);
        }

        return str;
    }

    [[nodiscard]] bool Version::FromString(const std::string_view& version)
    {
        Major = 0;
        Minor = 0;
        Patch = 0;

        std::array<uint32_t*, 2> values{ &Minor, &Patch };
        std::string_view remaining = version;

        if (!ReadNextNumber(remaining, Major))
        {
            return false;
        }

        for(size_t i = 0; i < values.size(); i++)
        {
            if(remaining.empty())
            {
                return true;
            }

            if(remaining[0] != '.')
            {
                return false;
            }

            remaining = { remaining.data() + 1, remaining.size() - 1 };
            if(!ReadNextNumber(remaining, *values[i]))
            {
                return false;
            }
        }

        return true;
    }

    bool Version::operator == (const Version& version) const
    {
        return Major == version.Major && Minor == version.Minor && Patch == version.Patch;
    }

    bool Version::operator != (const Version& version) const
    {
        return Major != version.Major || Minor != version.Minor || Patch != version.Patch;
    }

    bool Version::operator < (const Version& version) const
    {
        if (Major < version.Major)
        {
            return true;
        }
        if (Minor < version.Minor)
        {
            return true;
        }
        return Patch < version.Patch;
    }

    bool Version::operator <= (const Version& version) const
    {
        return *this == version || *this < version;
    }

    bool Version::operator > (const Version& version) const
    {
        if (Major > version.Major)
        {
            return true;
        }
        if (Minor > version.Minor)
        {
            return true;
        }
        return Patch > version.Patch;
    }

    bool Version::operator >= (const Version& version) const
    {
        return *this == version || *this > version;
    }

}