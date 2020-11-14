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

#include "Molten/Gui/Layers/RootLayer.hpp"
#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Gui/Widget.hpp"
#include <algorithm>

namespace Molten::Gui
{

    RootLayer::RootLayer(Canvas& canvas) :
        Layer(canvas),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f)
    {}

    void RootLayer::Update(const Time& )
    {
        auto& rootWidget = m_widgetTree.GetItem().GetValue();
        auto& rootWidgetRenderData = Layer::GetWidgetRenderData(*rootWidget);
        rootWidgetRenderData.grantedSize = m_size;

        m_widgetTree.ForEachPreorder<WidgetTreeData::Tree::PartialLaneType>(
            [&](auto& widget)
        {
            auto& widgetRenderData = Layer::GetWidgetRenderData(*widget);

            const Vector2f32 margin = widget->margin.low + widget->margin.high;
            const Vector2f32 grantedChildSize = {
                std::max(widgetRenderData.grantedSize.x - margin.x, 0.0f),
                std::max(widgetRenderData.grantedSize.y - margin.y, 0.0f)
            };

            Vector2f32 size = widget->CalculateSize(grantedChildSize);
            widgetRenderData.size = size;

            auto& widgetTreeData = Layer::GetWidgetTreeData(*widget);
            auto childLane = widgetTreeData.item->template GetLane<WidgetTreeData::Tree::PartialLaneType>();
            if (childLane.begin() != childLane.end())
            {
                widget->CalculateChildrenGrantedSize(childLane);
            }      
        });
    }

    void RootLayer::Draw(CanvasRenderer& renderer)
    {
        m_widgetTree.ForEachPreorder<WidgetTreeData::Tree::PartialLaneType>(
            [&](auto& widget)
        {
            auto& renderData = Layer::GetWidgetRenderData(*widget);

            //renderer.MaskArea(renderData.position, renderData.size);
            const Vector2f32 position = renderData.position + widget->margin.low;
            renderer.PushPosition(position);
            widget->Draw(renderer);
        },
            [&](auto& )
        {
            renderer.PopPosition();
        });
    }

    void RootLayer::Resize(const Vector2f32& size)
    {
        m_size = size;
    }

    void RootLayer::SetScale(const Vector2f32& scale)
    {
        m_scale = scale;
    }

    bool RootLayer::OnAddChild(WidgetPointer parent, WidgetPointer child)
    {
        if (!parent)
        {
            auto& rootWidget = m_widgetTree.GetItem().GetValue();
            if (rootWidget)
            {
                return false;
            }

            rootWidget = child;

            auto& childData = Layer::GetWidgetTreeData(*child);
            childData.widget = child;
            childData.item = std::addressof(m_widgetTree.GetItem());
        }
        else
        {
            auto& parentData = Layer::GetWidgetTreeData(*parent);

            auto& childData = Layer::GetWidgetTreeData(*child);
            childData.widget = child;

            if (CallWidgetOnAddChild(parent, child) == true)
            {
                auto partialLane = parentData.item->GetLane<WidgetTreeData::Tree::PartialLaneType>();
                auto it = partialLane.Insert(partialLane.end(), child);
                childData.iterator = it;
                childData.item = std::addressof(*it);
            }
            else
            {
                auto normalLane = parentData.item->GetLane<WidgetTreeData::Tree::NormalLaneType>();
                auto it = normalLane.Insert(normalLane.end(), child);
                childData.iterator = it;
                childData.item = std::addressof(*it);
            }
        }

        return true;
    }

}