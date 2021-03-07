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

namespace Molten::Gui
{

    // Widget base class implementations.
    template<typename TSkin>
    Widget<TSkin>::Widget(WidgetData<TSkin>& data) :
        m_data(data)
    {}

    template<typename TSkin>
    Widget<TSkin>::Widget(
        WidgetData<TSkin>& data,
        const Vector2f32& size
    ) :
        size(size),
        m_data(data)
    {}

    template<typename TSkin>
    bool Widget<TSkin>::GetOverrideChildrenMouseEvents() const
    {
        return overrideChildrenMouseEvents;
    }

    template<typename TSkin>
    void Widget<TSkin>::Update()
    {}

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    WidgetTypePointer<TWidgetType<TSkin>> Widget<TSkin>::CreateChild(TArgs ... args)
    {
        return m_data.canvas->template CreateChild<TWidgetType>(*this, std::forward<TArgs>(args)...);
    }

    template<typename TSkin>
    void Widget<TSkin>::OnAddChild(WidgetData<TSkin>& childData)
    {
    }

    template<typename TSkin>
    void Widget<TSkin>::OnRemoveChild(WidgetData<TSkin>& childData)
    {
    }

    template<typename TSkin>
    WidgetData<TSkin>& Widget<TSkin>::GetData()
    {
        return m_data;
    }
    template<typename TSkin>
    const WidgetData<TSkin>& Widget<TSkin>::GetData() const
    {
        return m_data;
    }

    template<typename TSkin>
    typename WidgetData<TSkin>::TreeNormalLane Widget<TSkin>::GetChildrenNormalLane()
    {
        return (*m_data.iterator).GetChildren().template GetLane<typename WidgetData<TSkin>::TreeNormalLaneType>();
    }

    template<typename TSkin>
    typename WidgetData<TSkin>::TreePartialLane Widget<TSkin>::GetChildrenPartialLane()
    {
        return (*m_data.iterator).GetChildren().template GetLane<typename WidgetData<TSkin>::TreePartialLaneType>();
    }

    template<typename TSkin>
    const Bounds2f32& Widget<TSkin>::GetGrantedBounds() const
    {
        return m_data.GetGrantedBounds();
    }

    template<typename TSkin>
    void Widget<TSkin>::SetGrantedBounds(const Bounds2f32& grantedBounds)
    {
        m_data.SetGrantedBounds(grantedBounds);
    }

    template<typename TSkin>
    void Widget<TSkin>::ApplyMarginsToGrantedBounds()
    {
        auto grantedBounds = m_data.GetGrantedBounds().WithoutMargins(margin).ClampHighToLow();
        m_data.SetGrantedBounds(grantedBounds);
    }


    // Widget mixin implementations.
    template<typename TSkin, template<typename> typename TWidget>
    bool WidgetMixin<TSkin, TWidget>::GetOverrideChildrenMouseEvents() const
    {
        return TWidget<TSkin>::overrideChildrenMouseEvents;
    }

    template<typename TSkin, template<typename> typename TWidget>
    WidgetMixin<TSkin, TWidget>::WidgetMixin(WidgetDataMixin<TSkin, TWidget>& data) :
        Widget<TSkin>(data),
        m_dataMixin(data)
    {}

    template<typename TSkin, template<typename> typename TWidget>
    WidgetMixin<TSkin, TWidget>::WidgetMixin(
        WidgetDataMixin<TSkin, TWidget>& data,
        const Vector2f32& size
    ) :
        Widget<TSkin>(data, size),
        m_dataMixin(data)
    {}


    template<typename TSkin, template<typename> typename TWidget>
    template<typename TState>
    const TState& WidgetMixin<TSkin, TWidget>::GetSkinState() const
    {
        static_assert(std::is_same_v<TState, typename TWidget<TSkin>::State>,
            "Passed invalid widget state type to WidgetMixin<...>::GetSkinState.");

        return m_dataMixin.widgetSkinMixin->GetState();
    }
    template<typename TSkin, template<typename> typename TWidget>
    template<typename TState>
    void WidgetMixin<TSkin, TWidget>::SetSkinState(const TState& state)
    {
        static_assert(std::is_same_v<TState, typename TWidget<TSkin>::State>, 
            "Passed invalid widget state type to WidgetMixin<...>::SetSkinState.");

        m_dataMixin.widgetSkinMixin->SetState(state);
    }

    template<typename TSkin, template<typename> typename TWidget>
    WidgetDataMixin<TSkin, TWidget>& WidgetMixin<TSkin, TWidget>::GetDataMixin()
    {
        return m_dataMixin;
    }

    template<typename TSkin, template<typename> typename TWidget>
    const WidgetDataMixin<TSkin, TWidget>& WidgetMixin<TSkin, TWidget>::GetDataMixin() const
    {
        return m_dataMixin;
    }

}
