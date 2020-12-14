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
    inline RootLayer<TSkin>::RootLayer(TSkin& skin) :
        Layer<TSkin>(skin),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f)
    {}

    template<typename TSkin>
    inline void RootLayer<TSkin>::Update(const Time&)
    {
        auto& rootWidget = m_widgetTree.GetItem().GetValue();
        auto& rootWidgetRenderData = Layer<TSkin>::GetWidgetRenderData(*rootWidget);
        rootWidgetRenderData.grantedSize = m_size;

        m_widgetTree.template ForEachPreorder<TreePartialLaneType>(
            [&](auto& widget)
        {
            auto& widgetRenderData = Layer<TSkin>::GetWidgetRenderData(*widget);

            const Vector2f32 margin = widget->margin.low + widget->margin.high;
            const Vector2f32 grantedChildSize = {
                std::max(widgetRenderData.grantedSize.x - margin.x, 0.0f),
                std::max(widgetRenderData.grantedSize.y - margin.y, 0.0f)
            };

            Vector2f32 size = widget->CalculateSize(grantedChildSize);
            widgetRenderData.size = size;

            auto& widgetTreeData = Layer<TSkin>::GetWidgetTreeData(*widget);
            auto childLane = widgetTreeData.treeItem->template GetLane<TreePartialLaneType>();
            if (childLane.begin() != childLane.end())
            {
                widget->CalculateChildrenGrantedSize(childLane);
            }
        });
    }

    template<typename TSkin>
    inline void RootLayer<TSkin>::Draw(CanvasRenderer& renderer)
    {
        m_widgetTree.template ForEachPreorder<TreePartialLaneType>(
            [&](auto& widget)
        {
            auto& renderData = Layer<TSkin>::GetWidgetRenderData(*widget);

            //renderer.MaskArea(renderData.position, renderData.size);
            const Vector2f32 position = renderData.position + widget->margin.low;
            renderer.PushPosition(position);
            widget->Draw(renderer);
        },
            [&](auto&)
        {
            renderer.PopPosition();
        });
    }

    template<typename TSkin>
    inline void RootLayer<TSkin>::Resize(const Vector2f32& size)
    {
        m_size = size;
    }

    template<typename TSkin>
    inline void RootLayer<TSkin>::SetScale(const Vector2f32& scale)
    {
        m_scale = scale;
    }

    template<typename TSkin>
    inline bool RootLayer<TSkin>::OnAddChild(Widget<TSkin>* parent, WidgetPointer<TSkin> child)
    {
        if (!parent)
        {
            auto& rootWidget = m_widgetTree.GetItem().GetValue();
            if (rootWidget)
            {
                return false;
            }

            rootWidget = child;

            auto& childData = Layer<TSkin>::GetWidgetTreeData(*child);
            childData.treeItem = std::addressof(m_widgetTree.GetItem());
        }
        else
        {
            auto& parentData = Layer<TSkin>::GetWidgetTreeData(*parent);

            auto& childData = Layer<TSkin>::GetWidgetTreeData(*child);

            if (Layer<TSkin>::CallWidgetOnAddChild(parent, child) == true)
            {
                auto partialLane = parentData.treeItem->template GetLane<TreePartialLaneType>();
                auto it = partialLane.Insert(partialLane.end(), child);
                childData.treeIterator = it;
                childData.treeItem = std::addressof(*it);
            }
            else
            {
                auto normalLane = parentData.treeItem->template GetLane<TreeNormalLaneType>();
                auto it = normalLane.Insert(normalLane.end(), child);
                childData.treeIterator = it;
                childData.treeItem = std::addressof(*it);
            }
        }

        return true;
    }


}