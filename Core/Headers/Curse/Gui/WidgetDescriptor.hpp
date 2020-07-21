/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#ifndef CURSE_CORE_GUI_WIDGETDESCRIPTOR_HPP
#define CURSE_CORE_GUI_WIDGETDESCRIPTOR_HPP

#include "Curse/Math/Vector.hpp"

namespace Curse::Gui
{

    using WidgetFlagsType = uint32_t;

    enum class WidgetFlags : WidgetFlagsType
    {
        None            = 0 << 0,
        FitChildren     = 0 << 1,
        FitParent       = 0 << 2
    };

    inline WidgetFlagsType operator| (const WidgetFlags lhs, const WidgetFlags rhs)
    {
        return static_cast<WidgetFlagsType>(lhs) | static_cast<WidgetFlagsType>(rhs);
    }
    inline WidgetFlagsType operator& (const WidgetFlags lhs, const WidgetFlags rhs)
    {
        return static_cast<WidgetFlagsType>(lhs) & static_cast<WidgetFlagsType>(rhs);
    }

    struct CURSE_API WidgetDescriptor
    {
        size_t maxChildrenCount;
        Vector2f32 maxSize;
        Vector2f32 minSize;
        WidgetFlagsType flags;
    };

}

#endif