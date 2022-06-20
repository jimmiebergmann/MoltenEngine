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

#ifndef MOLTEN_GRAPHICS_GUI_WIDGETS_PROGRESSBARWIDGET_HPP
#define MOLTEN_GRAPHICS_GUI_WIDGETS_PROGRESSBARWIDGET_HPP

#include "Molten/Graphics/Gui/Widget.hpp"

namespace Molten::Gui
{

    enum class ProgressBarDirection
    {
        Right,
        Left,
        Up,
        Down
    };

    template<typename TTheme>
    class ProgressBar : public WidgetMixin<TTheme, ProgressBar>
    {

    public:

        double value; ///< Value between 0.0 and 100.0
        ProgressBarDirection direction;

        explicit ProgressBar(
            WidgetMixinDescriptor<TTheme, ProgressBar>& desc,
            const ProgressBarDirection direction = ProgressBarDirection::Right);

    };

}

#include "Molten/Graphics/Gui/Widgets/ProgressBarWidget.inl"

#endif
