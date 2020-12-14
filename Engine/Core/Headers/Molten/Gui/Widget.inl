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

namespace Molten::Gui
{

    template<typename TSkin>
    inline void Widget<TSkin>::Update(const Time&)
    {}

    template<typename TSkin>
    inline void Widget<TSkin>::Draw(CanvasRenderer&)
    {}

    template<typename TSkin>
    inline Vector2f32 Widget<TSkin>::CalculateSize(const Vector2f32& grantedSize)
    {
        return grantedSize;
    }

    template<typename TSkin>
    inline void Widget<TSkin>::CalculateChildrenGrantedSize(typename WidgetTreeData<TSkin>::Tree::template ConstLane<WidgetTreeData<TSkin>::Tree::PartialLaneType>)
    {
    }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    inline WidgetTypePointer<TWidgetType<TSkin>> Widget<TSkin>::CreateChild(TArgs ... args)
    {
        auto& widgetData = GetWidgetTreeData();
        if(widgetData.layer == nullptr)
        {
            return nullptr;
        }

        return widgetData.layer->template CreateChild<TWidgetType>(*this, std::forward<TArgs>(args)...);
    }

    template<typename TSkin>
    inline bool Widget<TSkin>::OnAddChild(WidgetPointer<TSkin>)
    {
        return false;
    }

    template<typename TSkin>
    inline const WidgetRenderData& Widget<TSkin>::GetRenderData() const
    {
        return m_renderData;
    }

    template<typename TSkin>
    inline const Vector2f32& Widget<TSkin>::GetGrantedSize() const
    {
        return m_renderData.grantedSize;
    }

    template<typename TSkin>
    inline void Widget<TSkin>::SetRenderData(
        typename WidgetTreeData<TSkin>::Tree::template ConstIterator<WidgetTreeData<TSkin>::Tree::PartialLaneType> it,
        const Vector2f32& position,
        const Vector2f32& grantedSize)
    {
        const auto& parentGrantedSize = GetGrantedSize();
        auto& childRenderData = (*it).GetValue()->GetWidgetRenderData();

        childRenderData.position = {
            std::clamp(position.x, 0.0f, parentGrantedSize.x),
            std::clamp(position.y, 0.0f, parentGrantedSize.y)
        };

        childRenderData.grantedSize = {
            std::clamp(grantedSize.x, 0.0f, parentGrantedSize.x - childRenderData.position.x),
            std::clamp(grantedSize.y, 0.0f, parentGrantedSize.y - childRenderData.position.y)
        };
    }

    /*template<typename TSkin>
    inline bool Widget<TSkin>::AddChild(WidgetPointer<TSkin> parent, WidgetPointer<TSkin> child)
    {
        return Layer<TSkin>::AddChild(parent, child);
    }*/

    template<typename TSkin>
    inline WidgetTreeData<TSkin>& Widget<TSkin>::GetWidgetTreeData()
    {
        return m_treeData;
    }

    template<typename TSkin>
    inline WidgetRenderData& Widget<TSkin>::GetWidgetRenderData()
    {
        return m_renderData;
    }

}
