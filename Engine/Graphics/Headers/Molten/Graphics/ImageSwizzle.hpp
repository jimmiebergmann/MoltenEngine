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

#ifndef MOLTEN_GRAPHICS_IMAGESWIZZLE_HPP
#define MOLTEN_GRAPHICS_IMAGESWIZZLE_HPP

#include "Molten/Core.hpp"

namespace Molten
{

    enum class ImageComponentSwizzle : uint8_t
    {
        Identity,   ///< No swizzle of component.
        Red,        ///< Sets component to red channel's value.
        Green,      ///< Sets component to green channel's value.
        Blue,       ///< Sets component to blue channel's value.
        Alpha,      ///< Sets component to alpha channel's value.
        Zero,       ///< Sets component to 0.
        One         ///< Sets component to 1.   
    };

    struct ImageSwizzleMapping
    {
        ImageComponentSwizzle red;
        ImageComponentSwizzle green;
        ImageComponentSwizzle blue;
        ImageComponentSwizzle alpha;
    };

}

#endif