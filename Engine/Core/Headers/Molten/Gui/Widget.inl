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
    template<typename TSkin>
    inline Widget<TSkin>::Widget(WidgetData<TSkin>& data) :
        m_data(data)
    {}

    template<typename TSkin>
    inline Layer<TSkin>& Widget<TSkin>::GetLayer()
    {
        return *m_data.layer;
    }
    template<typename TSkin>
    inline const Layer<TSkin>& Widget<TSkin>::GetLayer() const
    {
        return *m_data.layer;
    }

    template<typename TSkin>
    inline WidgetSkinBase& Widget<TSkin>::GetSkin()
    {
        return *m_data.skin;
    }
    template<typename TSkin>
    inline const WidgetSkinBase& Widget<TSkin>::GetSkin() const
    {
        return *m_data.skin;
    }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    inline WidgetTypePointer<TWidgetType<TSkin>> Widget<TSkin>::CreateChild(TArgs ... args)
    {
        return GetLayer().template CreateChild<TWidgetType>(*this, std::forward<TArgs>(args)...);
    }

    template<typename TSkin>
    inline bool Widget<TSkin>::OnAddChild(WidgetPointer<TSkin>)
    {
        return false;
    }

    template<typename TSkin>
    inline void Widget<TSkin>::SetSkinState(WidgetSkinStateType state)
    {
        m_data.widgetSkin->SetState(state);
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

}
