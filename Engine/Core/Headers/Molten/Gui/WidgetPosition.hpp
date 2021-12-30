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

#ifndef MOLTEN_CORE_GUI_WIDGETPOSITION_HPP
#define MOLTEN_CORE_GUI_WIDGETPOSITION_HPP

#include "Molten/Math/Vector.hpp"
#include <variant>

namespace Molten::Gui::Position
{

    /** Fixed position in pixels. DPI affects scale of pixel value. */
    struct Pixels
    {
        float value;
    };

    /** Fixed position in percent of available space from parent. */
    struct Percent
    {
        float value;
    };

    /** Enumerator of different types of fixed positions. */
    enum class Fixed
    {
        Left,
        Right,
        Top,
        Bottom,
        Center
    };

}

namespace Molten::Gui
{

    /** Variant of single position element of XY coordinate system. */
    using WidgetElementPosition = std::variant<Position::Pixels, Position::Percent, Position::Fixed>;

    /** Vector of position variants. It is possible to mix position variants for X and Y coordinates. */
    using WidgetPosition = Vector2<WidgetElementPosition>;

}


#endif