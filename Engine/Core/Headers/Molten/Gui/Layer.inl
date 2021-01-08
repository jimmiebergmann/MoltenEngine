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
    inline void Layer<TSkin>::PushUserInputEvents(std::vector<UserInput::Event>& inputEvents)
    {
    }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    inline WidgetTypePointer<TWidgetType<TSkin>> Layer<TSkin>::CreateChild(TArgs ... args)
    {
        static_assert(std::is_base_of<Gui::Widget<TSkin>, TWidgetType<TSkin>>::value, "TWidgetType is not base of Widget.");

        auto widgetData = std::make_shared<WidgetData<TSkin>>(this);
        auto& widgetDataRef = *widgetData.get();

        auto widget = std::make_shared<TWidgetType<TSkin>>(*widgetData.get(), args...);
        widgetData->widget = widget;

        if (!OnAddChild(nullptr, widgetData))
        {
            return nullptr;
        }

        widgetData->widgetSkin = m_skin.template Create<TWidgetType<TSkin>>(*widget.get(), widgetDataRef);

        return widget;
    }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    inline WidgetTypePointer<TWidgetType<TSkin>> Layer<TSkin>::CreateChild(Widget<TSkin>& parent, TArgs ... args)
    {
        static_assert(std::is_base_of<Gui::Widget<TSkin>, TWidgetType<TSkin>>::value, "TWidgetType is not base of Widget.");

        auto widgetData = std::make_shared<WidgetData<TSkin>>(this);
        auto& widgetDataRef = *widgetData.get();

        auto widget = std::make_shared<TWidgetType<TSkin>>(widgetDataRef, args...);
        widgetData->widget = widget;

        if constexpr (std::is_base_of_v<WidgetEventHandler, TWidgetType<TSkin>>)
        {
            if constexpr (TWidgetType<TSkin>::handleMouseEvents)
            {
                widgetData->mouseEventHandler = widget.get();
            }
            if constexpr (TWidgetType<TSkin>::handleKeyboardEvents)
            {
                widgetData->keyboardEventHandler = widget.get();
            }
        }

        if (!OnAddChild(&parent, widgetData))
        {
            return nullptr;
        }

        widgetData->widgetSkin = m_skin.template Create<TWidgetType<TSkin>>(*widget.get(), widgetDataRef);

        return widget;
    }

    /*template<typename TSkin>
    inline WidgetTreeData<TSkin>& Layer<TSkin>::GetWidgetTreeData(Widget<TSkin>& widget)
    {
        return widget.m_treeData;
    }

    template<typename TSkin>
    inline WidgetRenderData& Layer<TSkin>::GetWidgetRenderData(Widget<TSkin>& widget)
    {
        return widget.m_renderData;
    }*/

    template<typename TSkin>
    inline TSkin& Layer<TSkin>::GetSkin()
    {
        return m_skin;
    }
    template<typename TSkin>
    inline const TSkin& Layer<TSkin>::GetSkin() const
    {
        return m_skin;
    }

    template<typename TSkin>
    inline bool Layer<TSkin>::CallWidgetOnAddChild(Widget<TSkin>* parent, WidgetPointer<TSkin> child)
    {
        return parent->OnAddChild(child);
    }

    template<typename TSkin>
    inline WidgetData<TSkin>& Layer<TSkin>::GetWidgetData(Widget<TSkin>& widget)
    {
        return widget.GetData();
    }
   
}
