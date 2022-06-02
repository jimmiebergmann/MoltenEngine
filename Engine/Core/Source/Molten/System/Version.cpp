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

#include "Molten/System/Version.hpp"

namespace Molten
{

    // Version implementations.
    const Version Version::None = Version(0, 0, 0);

    Version::Version() :
        Major(0),
        Minor(0),
        Patch(0)
    { }

    Version::Version(const uint32_t major, const uint32_t minor, const uint32_t patch) :
        Major(major),
        Minor(minor),
        Patch(patch)
    { }

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
        return
            Major < version.Major ||
            (Major == version.Major && Minor < version.Minor) ||
            (Major == version.Major && Minor == version.Minor && Patch < version.Patch);
    }

    bool Version::operator <= (const Version& version) const
    {
        return *this == version || *this < version;
    }

    bool Version::operator > (const Version& version) const
    {
        return
            Major > version.Major ||
            (Major == version.Major && Minor > version.Minor) ||
            (Major == version.Major && Minor == version.Minor && Patch > version.Patch);
    }

    bool Version::operator >= (const Version& version) const
    {
        return *this == version || *this > version;
    }

}