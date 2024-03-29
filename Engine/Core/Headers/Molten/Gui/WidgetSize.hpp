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

#ifndef MOLTEN_CORE_GUI_WIDGETSIZE_HPP
#define MOLTEN_CORE_GUI_WIDGETSIZE_HPP

#include "Molten/Math/Vector.hpp"
#include <variant>

namespace Molten::Gui::Size
{

    /** Fixed size in pixels. DPI affects scale of pixel value. */
    struct Pixels
    {
        float value;

        bool operator == (const Pixels& rhs) const;
        bool operator != (const Pixels& rhs) const;
    };

    /** Fixed size in percent of available space from parent. */
    struct Percent
    {
        float value;

        bool operator == (const Percent& rhs) const;
        bool operator != (const Percent& rhs) const;
    };

    /** Fit size to child elements. */
    enum class Fit
    {
        Parent,
        Content,
        ContentThenParent
    };

}

namespace Molten::Gui
{

    /** Variant of single size element of XY coordinate system. */
    using WidgetElementSize = std::variant<Size::Pixels, Size::Percent, Size::Fit>;

    /** Vector of size variants. It is possible to mix size variants for X and Y coordinates. */
    using WidgetSize = Vector2<WidgetElementSize>;

}

#include "Molten/Gui/WidgetSize.inl"

#endif