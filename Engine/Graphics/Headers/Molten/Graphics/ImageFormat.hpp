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

#ifndef MOLTEN_GRAPHICS_IMAGEFORMAT_HPP
#define MOLTEN_GRAPHICS_IMAGEFORMAT_HPP

#include "Molten/Core.hpp"

namespace Molten
{

    enum class ImageFormat : uint8_t
    {
        URed8,                      ///< Linear { uint8_t red; }
        URed8Green8,                ///< Linear { uint8_t red; uint8_t green; }
        URed8Green8Blue8,           ///< Linear { uint8_t red; uint8_t green; uint8_t blue; }
        URed8Green8Blue8Alpha8,     ///< Linear { uint8_t red; uint8_t green; uint8_t blue; uint8_t alpha; }

        SRed8,                      ///< Linear { uint8_t red; }
        SRed8Green8,                ///< Linear { uint8_t red; uint8_t green; }
        SRed8Green8Blue8,           ///< Linear { uint8_t red; uint8_t green; uint8_t blue; }
        SRed8Green8Blue8Alpha8,     ///< Linear { uint8_t red; uint8_t green; uint8_t blue; uint8_t alpha; }
        SDepthFloat24StencilUint8,  ///< Linear { float(24 bit) depth; uint8_t stencil; }

        SrgbRed8Green8Blue8,        ///< Non-linear sRGB { int8_t red; int8_t green; int8_t blue; }
        SrgbRed8Green8Blue8Alpha8,  ///< Non-linear sRGB { int8_t red; int8_t green; int8_t blue; int8_t alpha; }

        UBlue8Green8Red8,           ///< Linear { uint8_t blue; uint8_t green; uint8_t red; }
        UBlue8Green8Red8Alpha8,     ///< Linear { uint8_t blue; uint8_t green; uint8_t red; uint8_t alpha; }
    };

}

#endif