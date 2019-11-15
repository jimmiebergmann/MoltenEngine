/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#ifndef CURSE_CORE_SYSTEM_VERSION_HPP
#define CURSE_CORE_SYSTEM_VERSION_HPP

#include "Curse/Types.hpp"
#include <string>

namespace Curse
{

    /**
    * @brief Version class.
    */
    class CURSE_API Version
    {

    public:

        /**
        * @brief Version object representing no version.
        */
        static const Version None;

        /**
        * @brief Constructor.
        */
        Version(const uint32_t major = 0, const uint32_t minor = 0, const uint32_t patch = 0);

        /**
        * @brief Get version as string.
        *
        * @param ignoreTrail[in] Ignoring trailing zeros of version.
        *                        Major version is always returned, even if being 0.
        */
        std::string AsString(const bool ignoreTrail = true) const;

        /**
        * @brief Equal to compare operator.
        */
        bool operator == (const Version& version) const;

        /**
        * @brief Not equal to compare operator.
        */
        bool operator != (const Version& version) const;

        /**
        * @brief Less than compare operator.
        */
        bool operator < (const Version& version) const;

        /**
        * @brief Less than or equal to compare operator.
        */
        bool operator <= (const Version& version) const;

        /**
        * @brief Greater than compare operator.
        */
        bool operator > (const Version& version) const;

        /**
        * @brief Greater than or equal to compare operator.
        */
        bool operator >= (const Version& version) const;

        uint32_t Major;
        uint32_t Minor;
        uint32_t Patch;

    };

}

#endif
