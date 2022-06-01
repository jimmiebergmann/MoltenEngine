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

#include "Molten/Utility/Uuid.hpp"
#include <sstream>
#include <iomanip>
#include "Molten/Utility/StringUtility.hpp"

namespace Molten
{

    // Uuid implementations.
    Uuid::Uuid(const std::array<uint8_t, 16>& data) :
        Data{ data }
    { }

    Uuid::Uuid(
        const uint64_t low,
        const uint64_t high
    ) :
        Low(low),
        High(high)
    {}

    Uuid::Uuid(
        const uint32_t low1,
        const uint16_t low2,
        const uint16_t low3,
        const uint64_t high
    ) :
        Low1(low1),
        Low2(low2),
        Low3(low3),
        High(high)
    {}

    uint8_t Uuid::GetVersion() const
    {
        return static_cast<uint8_t>((Data[6] >> 4) & 0x0F);
    }

    uint8_t Uuid::GetVariant() const
    {
        return static_cast<uint8_t>((Data[8] >> 4));
    }

    bool Uuid::operator == (const Uuid& rhs) const
    {
        return Low == rhs.Low && High == rhs.High;
    }

    bool Uuid::operator != (const Uuid& rhs) const
    {
        return Low != rhs.Low || High != rhs.High;
    }

    /*bool Uuid::operator < (const Uuid& rhs) const
    {
        return Low;
    }

    bool Uuid::operator <= (const Uuid& rhs) const
    {
        return false;
    }

    bool Uuid::operator > (const Uuid& rhs) const
    {
        return false;
    }

    bool Uuid::operator >= (const Uuid& rhs) const
    {
        return false;
    }*/

}