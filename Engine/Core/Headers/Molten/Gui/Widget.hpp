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

#ifndef MOLTEN_CORE_GUI_WIDGET_HPP
#define MOLTEN_CORE_GUI_WIDGET_HPP

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Gui/SpacingTypes.hpp"
#include "Molten/Gui/WidgetData.hpp"
#include "Molten/Gui/Layer.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Bounds.hpp"
#include "Molten/System/Time.hpp"
#include <algorithm>

namespace Molten::Gui
{

    template<typename TSkin>
    class Widget
    {

    public:

        MarginType margin;
        PaddingType padding;

        explicit Widget(WidgetData<TSkin>& data);

        virtual ~Widget() = default;

        Layer<TSkin>& GetLayer();
        const Layer<TSkin>& GetLayer() const;

        WidgetSkinBase& GetSkin();
        const WidgetSkinBase& GetSkin() const;

        template<template<typename> typename TWidgetType, typename ... TArgs>
        WidgetTypePointer<TWidgetType<TSkin>> CreateChild(TArgs ... args);

    protected:

        virtual bool OnAddChild(WidgetPointer<TSkin> widget);

        void SetSkinState(WidgetSkinStateType state);

    private:
        
        template<typename TLayerSkin>
        friend class Layer;

        friend TSkin;

        Widget(const Widget&) = delete;
        Widget(Widget&&) = delete;
        Widget& operator= (const Widget&) = delete;
        Widget& operator= (Widget&&) = delete;

        WidgetData<TSkin>& GetData();
        const WidgetData<TSkin>& GetData() const;

        WidgetData<TSkin>& m_data;

    };


}

#include "Molten/Gui/Widget.inl"

#endif