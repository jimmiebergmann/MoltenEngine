/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#include "Molten/Graphics/Gui2/Widget.hpp"
#include "Molten/Graphics/Gui2/CanvasRenderer2.hpp"
#include <Molten/Graphics/Gui2/WidgetEvent.hpp>

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    // Widget bounds guard implementations.
    WidgetBoundsGuard::WidgetBoundsGuard(WidgetDrawBoundsStack& drawBoundsStack) :
        m_drawBoundsStack(drawBoundsStack)
    {}

    WidgetBoundsGuard::~WidgetBoundsGuard()
    {
        m_drawBoundsStack.pop_back();
    }


    // Widget update context implementations.
    WidgetUpdateContext::WidgetUpdateContext(
        WidgetDrawBuffer& drawBuffer,
        WidgetDrawBoundsStack& drawBoundsStack) :
        m_drawBuffer(drawBuffer),
        m_drawBoundsStack(drawBoundsStack)
    {}

    WidgetBoundsGuard WidgetUpdateContext::RegisterBounds(const AABB2f32& bounds)
    {
        const auto lastBounds = m_drawBoundsStack.back();
        const auto newBounds = AABB2f32{ 
            .position = lastBounds.position + bounds.position,
            .size = bounds.size
        }; 
        //lastPosition + bounds.position;
        m_drawBoundsStack.push_back(newBounds);
        return WidgetBoundsGuard{ m_drawBoundsStack };
    }

    void WidgetUpdateContext::RegisterDraw()
    {
        const auto lastBounds = m_drawBoundsStack.back();
        m_drawBuffer.emplace_back(lastBounds, m_currentWidget);
    }

    void WidgetUpdateContext::SetCurrentWidget(Widget* widget)
    {
        m_currentWidget = widget;
    }

    /*void WidgetUpdateContext::ListenForMouseEvents(WidgetMouseEventHandler* eventHandler, AABB2f32 bounds)
    {
        m_widgetMouseEventQueue.push_back({ bounds, eventHandler });
    }*/

    /*void WidgetUpdateContext::RegisterForDraw(const AABB2f32& bounds)
    {
        if (m_registeredWidgetBounds)
        {
            return;
        }

        m_drawWidgets.emplace_back(bounds, m_currentWidget);
        m_registeredWidgetBounds = true;
    }

    Widget* WidgetUpdateContext::GetCurrentWidget()
    {
        return m_currentWidget;
    }

    WidgetUpdateContext::DrawWidgetsType& WidgetUpdateContext::GetDrawWidgets()
    {
        return m_drawWidgets;
    }

    void WidgetUpdateContext::SetCurrentWidget(Widget* widget)
    {
        m_currentWidget = widget;
        m_registeredWidgetBounds = false;
    }*/


    // Widget position guard implementations.
    //WidgetPositionGuard::~WidgetPositionGuard()
    //{
    //    m_positionList.pop_back();
    //}

    //WidgetPositionGuard::WidgetPositionGuard(std::vector<Vector2f32>& positionList) :
    //    m_positionList(positionList)
    //{}


    // Widget draw context implementations.
    WidgetDrawContext::WidgetDrawContext(CanvasRenderer& renderer, Vector2f32 scale) :
        m_renderer(renderer),
        //m_currentWidget(nullptr),
        m_scale(scale)//,
        //m_positionList{ Vector2f32{ 0.0f, 0.0f } }
    {}

    /*void WidgetDrawContext::SetCurrentWidget(Widget& widget)
    {
        m_currentWidget = &widget;
    }*/

   /* void WidgetDrawContext::RegisterBounds(AABB2f32 bounds)
    {
        bounds.position += m_positionList.back();
        bounds.position *= m_scale;
        bounds.size *= m_scale;
        m_widgetBounds.push_back({ bounds, m_currentWidget });
    }*/

    /*WidgetPositionGuard WidgetDrawContext::PushPosition(Vector2f32 position)
    {
        m_positionList.push_back(m_positionList.back() + position);
        return WidgetPositionGuard{ m_positionList };
    }*/


    // Widget implementations.
    Widget::Widget(WidgetDescriptor&& desc) :
        m_children{},
        m_descriptor{ std::move(desc) },
        m_canvas{ nullptr }
    {
    }

    void WidgetDrawContext::DrawQuad(AABB2f32 bounds, const Vector4f32& color)
    {
        bounds.position += m_currentDrawBufferEntry->bounds.position;
        bounds.position *= m_scale;
        bounds.size *= m_scale;
        m_renderer.DrawQuad(bounds, color);
    }

    void WidgetDrawContext::DrawQuad(Vector2f32 size, const Vector4f32& color)
    {
        const auto bounds = AABB2f32{
            .position = m_currentDrawBufferEntry->bounds.position * m_scale,
            .size = size * m_scale
        };

        m_renderer.DrawQuad(bounds, color);
    }

    void WidgetDrawContext::SetCurrentWidgetDrawBufferEntry(WidgetDrawBufferEntry* currentDrawBufferEntry)
    {
        m_currentDrawBufferEntry = currentDrawBufferEntry;
        registeredSize = m_currentDrawBufferEntry->bounds.size;
    }

    void Widget::AddChild(std::shared_ptr<Widget> widget)
    {
        if (widget->m_canvas != m_canvas) {
            widget->m_canvas = m_canvas;
            widget->OnChangeCanvas(m_canvas);

            // TODO: Nested loop here for all children...
        }

        //widget->m_canvas = m_canvas;
        m_children.push_back(std::move(widget));
    }

    Canvas* Widget::GetCanvas()
    {
        return m_canvas;
    }
    const Canvas* Widget::GetCanvas() const
    {
        return m_canvas;
    }

    Vector2f32 Widget::Update(WidgetUpdateContext& context, Widget& widget, Vector2f32 grantedSize)
    {
        MOLTEN_DEBUG_ASSERT(context.m_currentWidget != &widget, "Cannot call Update with widget == m_currentWidget.");
        context.SetCurrentWidget(&widget);
        context.grantedSize = grantedSize;
        return widget.OnUpdate(context);
    }

    /*void Widget::Draw(WidgetDrawContext& context, Widget& widget)
    {
        //context.SetCurrentWidget(widget);
        auto usedSize = widget.OnDraw(context);
        usedSize = { std::max(usedSize.x, 0.0f), std::max(usedSize.y, 0.0f) };
        return usedSize;
    }*/

    bool Widget::IsvalidSize(const Vector2f32& size)
    {
        return size.x > 0.0f && size.y > 0.0f;
    }

    bool Widget::IsvalidBounds(const AABB2f32& bounds)
    {
        return bounds.size.x > 0.0f && bounds.size.y > 0.0f;
    }

    AABB2f32 Widget::WithoutMargins(const AABB2f32& bounds, Bounds2f32 margins)
    {
        return  AABB2f32{
            .position = bounds.position + margins.low,
            .size = bounds.size - margins.low - margins.high
        };
    }

    AABB2f32 Widget::WithoutMargins(const Vector2f32& size, Bounds2f32 margins)
    {
        return {
            .position = margins.low,
            .size = size - margins.low - margins.high
        };
    }

    AABB2f32 Widget::WithoutMarginsAndPadding(const AABB2f32& bounds, Bounds2f32 margins, Bounds2f32 padding)
    {
        return  AABB2f32{
            .position = bounds.position + margins.low + padding.low,
            .size = bounds.size - margins.low - margins.high - padding.low - padding.high
        };
    }

    AABB2f32 Widget::WithoutMarginsAndPadding(const Vector2f32& size, Bounds2f32 margins, Bounds2f32 padding)
    {
        return {
            .position = margins.low + padding.low,
            .size = size - margins.low - margins.high - padding.low - padding.high
        };
    }

    void Widget::OnAddWidget(std::shared_ptr<Widget>&)
    {
    }

    void Widget::OnRemoveWidget(std::shared_ptr<Widget>&)
    {
    }

    void Widget::OnChangeCanvas(Canvas*)
    {
    }

}