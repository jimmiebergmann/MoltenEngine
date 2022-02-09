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

#ifndef MOLTEN_CORE_GUI_WIDGETS_DOCKERWIDGET_HPP
#define MOLTEN_CORE_GUI_WIDGETS_DOCKERWIDGET_HPP

#include "Molten/Gui/Widget.hpp"
#include "Molten/Gui/WidgetDescriptor.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Gui/DraggableWidget.hpp"
#include "Molten/Gui/Widgets/Overlays/DockerOverlayWidget.hpp"
#include "Molten/System/Signal.hpp"
#include "Molten/Math/Triangle.hpp"
#include "Molten/Utility/Template.hpp"
#include <memory>
#include <map>
#include <set>
#include <queue>
#include <variant>
#include <algorithm>

namespace Molten::Gui
{

    enum class DockingPosition
    {
        Left,
        Right,
        Top,
        Bottom
    };


    template<typename TTheme>
    class Docker : public WidgetMixin<TTheme, Docker>, public WidgetMouseEventHandler, public OverrideChildMouseEvents
    {

    public:

        Signal<Mouse::Cursor> onCursorChange;

        float edgeWidth;
        float cellSpacing;
        Vector2f32 minCellSize;
        Vector2f32 defaultCellSize;

        explicit Docker(WidgetMixinDescriptor<TTheme, Docker>& desc);

        template<template<typename> typename TWidget, typename ... TArgs>
        TWidget<TTheme>* CreateChild(
            const DockingPosition dockingPosition,
            TArgs ... args);

    private:

        void PreUpdate() override;
        void OnAddChild(Widget<TTheme>& child) override;
        bool OnMouseEvent(const WidgetMouseEvent& widgetMouseEvent) override;

        struct Impl;
        Impl m_impl;

    };

}

#include "Molten/Gui/Widgets/DockerWidget.inl"

#endif
