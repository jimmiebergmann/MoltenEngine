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

#ifndef CURSE_CORE_GUI_TEMPLATES_PADDING_HPP
#define CURSE_CORE_GUI_TEMPLATES_PADDING_HPP

#include "Curse/Gui/WidgetTemplate.hpp"

namespace Curse
{

    namespace Gui
    {

        struct Padding
        {
            float left;
            float right;
            float top;
            float bottom;
        };

        template<>
        static inline const WidgetDescriptor WidgetDescriptorTemplate<Padding> =
        {
            1, // maxChildCount
            Vector2f32(), // maxSize
            Vector2f32(), // minSize
            (WidgetFlagsType)WidgetFlags::FitChildren |
            (WidgetFlagsType)WidgetFlags::FitParent // flags
        };

        template<>
        static inline const auto RenderObjectTemplate<Padding> = [](RenderObject& renderObject)
        {

        };

    }

}

#endif