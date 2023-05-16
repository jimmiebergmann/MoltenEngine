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

#ifndef MOLTEN_GRAPHICS_GUI2_WIDGETSIZE_HPP
#define MOLTEN_GRAPHICS_GUI2_WIDGETSIZE_HPP

#include "Molten/Graphics/Gui2/Gui2Namespace.hpp"
#include "Molten/Math/Vector.hpp"
#include <variant>

namespace Molten::MOLTEN_GUI2NAMESPACE::Size
{

    /** Fixed size in pixels. DPI affects scale of pixel value. */
    struct Pixels
    {
        float value;
    };

    /** Fixed size in percent of available space from parent. */
    struct Percent
    {
        float value;
    };

    /** Fit size to child elements. */
    enum class Fit
    {
        Parent,
        Content,
        ContentThenParent
    };

    bool operator == (const Pixels& lhs, const Pixels& rhs);
    bool operator == (const Percent& lhs, const Percent& rhs);

}

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    /** Variant of single size element of XY coordinate system. */
    using WidgetElementSize = std::variant<Size::Pixels, Size::Percent, Size::Fit>;

    /** Vector of size variants. It is possible to mix size variants for X and Y coordinates. */
    using WidgetSize = Vector2<WidgetElementSize>;

}

#include "Molten/Graphics/Gui2/WidgetSize.inl"

#endif