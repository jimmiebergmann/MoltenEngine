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

#ifndef MOLTEN_CORE_GUI_WIDGETS_PANEWIDGET_HPP
#define MOLTEN_CORE_GUI_WIDGETS_PANEWIDGET_HPP

#include "Molten/Gui/Widget.hpp"
#include "Molten/Gui/DraggableWidget.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Gui/Widgets/LabelWidget.hpp"
#include <string>

namespace Molten::Gui
{

    template<typename TTheme>
    class Pane : public WidgetMixin<TTheme, Pane>, public WidgetMouseEventHandler, public DraggableWidget
    {

    public:
    
        using Mixin = WidgetMixin<TTheme, Pane>;

        static constexpr bool handleKeyboardEvents = false;
        static constexpr bool handleMouseEvents = true;

        Pane(
            WidgetDataMixin<TTheme, Pane>& data,
            const std::string& label,
            const WidgetSize& size);

        void Update() override;

        bool OnMouseEvent(const WidgetMouseEvent& widgetMouseEvent) override;

        [[nodiscard]] const Bounds2f32& GetDragBounds() const override;

    private:

        void OnCreate() override;

        std::string m_label;
        Widget<TTheme>* m_labelWidget;
        Bounds2f32 m_dragBounds;

    };

}

#include "Molten/Gui/Widgets/PaneWidget.inl"

#endif
