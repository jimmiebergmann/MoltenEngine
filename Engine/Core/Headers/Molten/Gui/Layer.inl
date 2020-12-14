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

#include <type_traits>

namespace Molten::Gui
{

    template<typename TSkin>
    inline Layer<TSkin>::Layer(TSkin& skin) :
        m_skin(skin)
    {}

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    inline WidgetTypePointer<TWidgetType<TSkin>> Layer<TSkin>::CreateChild(TArgs ... args)
    {
        static_assert(std::is_base_of<Gui::Widget<TSkin>, TWidgetType<TSkin>>::value, "TWidgetType is not base of Widget.");

        auto widget = std::make_shared<TWidgetType<TSkin>>(m_skin, args...);

        auto& childData = GetWidgetTreeData(*widget);
        childData.layer = this;
        childData.widget = widget.get();

        if (!OnAddChild(nullptr, widget))
        {
            return nullptr;
        }

        return widget;
    }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    inline WidgetTypePointer<TWidgetType<TSkin>> Layer<TSkin>::CreateChild(Widget<TSkin>& parent, TArgs ... args)
    {
        static_assert(std::is_base_of<Gui::Widget<TSkin>, TWidgetType<TSkin>>::value, "TWidgetType is not base of Widget.");

        auto& parentData = GetWidgetTreeData(parent);
        auto* parentLayer = parentData.layer;
        
        if(parentLayer != this)
        {
            return nullptr;
        }

        auto widget = std::make_shared<TWidgetType<TSkin>>(m_skin, args...);

        auto& childData = GetWidgetTreeData(*widget);
        childData.layer = this;
        childData.widget = widget.get();

        if (!OnAddChild(&parent, widget))
        {
            return nullptr;
        }

        

        return widget;
    }

    template<typename TSkin>
    inline WidgetTreeData<TSkin>& Layer<TSkin>::GetWidgetTreeData(Widget<TSkin>& widget)
    {
        return widget.m_treeData;
    }

    template<typename TSkin>
    inline WidgetRenderData& Layer<TSkin>::GetWidgetRenderData(Widget<TSkin>& widget)
    {
        return widget.m_renderData;
    }

    template<typename TSkin>
    inline bool Layer<TSkin>::CallWidgetOnAddChild(Widget<TSkin>* parent, WidgetPointer<TSkin> child)
    {
        return parent->OnAddChild(child);
    }
   
}
