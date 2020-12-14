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

#ifndef MOLTEN_CORE_GUI_WIDGET_HPP
#define MOLTEN_CORE_GUI_WIDGET_HPP

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Gui/WidgetData.hpp"
#include "Molten/Gui/Layer.hpp"
#include "Molten/Gui/Data/OutlineData.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Time.hpp"
#include <algorithm>

namespace Molten::Gui
{

    template<typename TSkin>
    class Widget
    {

    public:

        Widget() = default;

        virtual ~Widget() = default;

        MarginData margin;

        virtual void Update(const Time& deltaTime);

        virtual void Draw(CanvasRenderer& renderer);

        virtual Vector2f32 CalculateSize(const Vector2f32& grantedSize);

        virtual void CalculateChildrenGrantedSize(typename WidgetTreeData<TSkin>::Tree::template ConstLane<WidgetTreeData<TSkin>::Tree::PartialLaneType> children);

        template<template<typename> typename TWidgetType, typename ... TArgs>
        WidgetTypePointer<TWidgetType<TSkin>> CreateChild(TArgs ... args);

    protected:

        template<typename TWidget>
        using WidgetSkin = typename TSkin::template WidgetSkin<TWidget>;

        virtual bool OnAddChild(WidgetPointer<TSkin> widget);

        const WidgetRenderData& GetRenderData() const;
        const Vector2f32& GetGrantedSize() const;

        void SetRenderData(
            typename WidgetTreeData<TSkin>::Tree::template ConstIterator<WidgetTreeData<TSkin>::Tree::PartialLaneType> it,
            const Vector2f32& position,
            const Vector2f32& grantedSize);

    private:

        WidgetTreeData<TSkin>& GetWidgetTreeData();
        WidgetRenderData& GetWidgetRenderData();
        
        Widget(const Widget&) = delete;
        Widget(Widget&&) = delete;
        Widget& operator= (const Widget&) = delete;
        Widget& operator= (Widget&&) = delete;

        WidgetTreeData<TSkin> m_treeData;
        WidgetRenderData m_renderData;

        template<typename TLayerSkin>
        friend class Layer;

    };


}

#include "Molten/Gui/Widget.inl"

#endif