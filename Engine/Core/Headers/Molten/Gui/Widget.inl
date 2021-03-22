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
    template<typename TTheme>
    Widget<TTheme>::Widget(WidgetData<TTheme>& data) :
        m_data(data)
    {}

    template<typename TTheme>
    Widget<TTheme>::Widget(
        WidgetData<TTheme>& data,
        const Vector2f32& size
    ) :
        size(size),
        m_data(data)
    {}

    template<typename TTheme>
    bool Widget<TTheme>::GetOverrideChildrenMouseEvents() const
    {
        return overrideChildrenMouseEvents;
    }

    template<typename TTheme>
    void Widget<TTheme>::Update()
    {}

    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    TWidget<TTheme>* Widget<TTheme>::CreateChild(TArgs ... args)
    {
        return m_data.GetLayer()->template CreateChild<TWidget>(*this, std::forward<TArgs>(args)...);
    }

    template<typename TTheme>
    void Widget<TTheme>::OnAddChild(WidgetData<TTheme>& childData)
    {
    }

    template<typename TTheme>
    void Widget<TTheme>::OnRemoveChild(WidgetData<TTheme>& childData)
    {
    }

    template<typename TTheme>
    WidgetData<TTheme>& Widget<TTheme>::GetData()
    {
        return m_data;
    }
    template<typename TTheme>
    const WidgetData<TTheme>& Widget<TTheme>::GetData() const
    {
        return m_data;
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::TreeNormalLane Widget<TTheme>::GetChildrenNormalLane()
    {
        return m_data.GetChildrenNormalLane();
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::TreePartialLane Widget<TTheme>::GetChildrenPartialLane()
    {
        return m_data.GetChildrenPartialLane();
    }

    template<typename TTheme>
    const Bounds2f32& Widget<TTheme>::GetGrantedBounds() const
    {
        return m_data.GetGrantedBounds();
    }

    template<typename TTheme>
    void Widget<TTheme>::SetGrantedBounds(const Bounds2f32& grantedBounds)
    {
        m_data.SetGrantedBounds(grantedBounds);
    }

    template<typename TTheme>
    void Widget<TTheme>::ApplyMarginsToGrantedBounds()
    {
        auto grantedBounds = m_data.GetGrantedBounds().WithoutMargins(margin).ClampHighToLow();
        m_data.SetGrantedBounds(grantedBounds);
    }


    // Widget mixin implementations.
    template<typename TTheme, template<typename> typename TWidget>
    bool WidgetMixin<TTheme, TWidget>::GetOverrideChildrenMouseEvents() const
    {
        return TWidget<TTheme>::overrideChildrenMouseEvents;
    }

    template<typename TTheme, template<typename> typename TWidget>
    WidgetMixin<TTheme, TWidget>::WidgetMixin(WidgetDataMixin<TTheme, TWidget>& data) :
        Widget<TTheme>(data),
        m_dataMixin(data)
    {}

    template<typename TTheme, template<typename> typename TWidget>
    WidgetMixin<TTheme, TWidget>::WidgetMixin(
        WidgetDataMixin<TTheme, TWidget>& data,
        const Vector2f32& size
    ) :
        Widget<TTheme>(data, size),
        m_dataMixin(data)
    {}

    template<typename TTheme, template<typename> typename TWidget>
    WidgetDataMixin<TTheme, TWidget>& WidgetMixin<TTheme, TWidget>::GetDataMixin()
    {
        return m_dataMixin;
    }
    template<typename TTheme, template<typename> typename TWidget>
    const WidgetDataMixin<TTheme, TWidget>& WidgetMixin<TTheme, TWidget>::GetDataMixin() const
    {
        return m_dataMixin;
    }

    template<typename TTheme, template<typename> typename TWidget>
    template<typename TState>
    const TState& WidgetMixin<TTheme, TWidget>::GetSkinState() const
    {
        static_assert(std::is_same_v<TState, typename TWidget<TTheme>::State>,
            "Passed invalid widget state type to WidgetMixin<TTheme, TWidget>::GetSkinState.");

        return m_dataMixin.GetWidgetSkinMixin->GetState();
    }
    template<typename TTheme, template<typename> typename TWidget>
    template<typename TState>
    void WidgetMixin<TTheme, TWidget>::SetSkinState(const TState& state)
    {
        static_assert(std::is_same_v<TState, typename TWidget<TTheme>::State>,
            "Passed invalid widget state type to WidgetMixin<TTheme, TWidget>::SetSkinState.");

        m_dataMixin.GetWidgetSkinMixin()->SetState(state);
    }

}
