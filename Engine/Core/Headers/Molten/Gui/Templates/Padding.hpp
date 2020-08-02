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

#ifndef MOLTEN_CORE_GUI_TEMPLATES_PADDING_HPP
#define MOLTEN_CORE_GUI_TEMPLATES_PADDING_HPP

#include "Molten/Gui/WidgetTemplate.hpp"

namespace Molten::Gui
{

    struct Padding
    {
        Padding(
            const float left = 0.0f,
            const float right = 0.0f,
            const float top = 0.0f,
            const float bottom = 0.0f)
            :
            left(left), right(right), top(top), bottom(bottom)
        { }

        float left;
        float right;
        float top;
        float bottom;
    };

}

namespace Molten::Gui::Template
{

    template<>
    inline const WidgetDescriptor Descriptor<Padding> =
    {
        1, // maxChildCount
        Vector2f32(), // maxSize
        Vector2f32(), // minSize
        (uint32_t)WidgetFlags::FitParent // flags
    };

    template<>
    inline const auto CalculateWidgetSize<Padding> =
        [](TemplatedWidget<Padding>& /*padding*/, const Vector2f32& /*grantedSize*/)
    {

    };

    template<>
    inline const auto LoadRenderObject<Padding> =
        [](RenderObject& renderObject)
    {
        renderObject.AddRect({ 300.0f, 300.0f }, { 100.0f, 100.0f });
    };

}

#endif