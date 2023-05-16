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

#include "Molten/Graphics/Gui2/Widgets/ButtonWidget.hpp"

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    Button::Button(WidgetDescriptor&& desc) :
        StyledWidget<ButtonStyle>(std::move(desc)),
        m_color{ m_widgetStyle->colors.normal },
        m_pressed{ false }
    {
    }

    Button::Button(WidgetDescriptor&& desc, std::shared_ptr<const ButtonStyle> style) :
        StyledWidget<ButtonStyle>(std::move(desc), style),
        m_color{ m_widgetStyle->colors.normal },
        m_pressed{ false }
    {}

    Vector2f32 Button::OnUpdate(WidgetUpdateContext& context)
    {
        auto bounds = WithoutMargins(context.grantedSize, m_widgetStyle->margins);
        if (!IsvalidBounds(bounds))
        {
            return {};
        }

        [[maybe_unused]] auto boundsGuard = context.RegisterBounds(bounds);
        context.RegisterDraw();

        UpdateChild(context, bounds);

        return bounds.size;
    }

    Vector2f32 Button::UpdateChild(WidgetUpdateContext& context, AABB2f32& parentBounds)
    {
        if (m_children.empty())
        {
            return {};
        }

        auto& child = *m_children.front();

        auto childBounds = WithoutMargins(parentBounds, m_widgetStyle->padding);
        if (!IsvalidBounds(childBounds))
        {
            return {};
        }

        childBounds.position -= parentBounds.position;

        [[maybe_unused]] auto boundsGuard = context.RegisterBounds(childBounds);
        const auto childUsedSize = Update(context, child, childBounds.size);
        if (!IsvalidSize(childUsedSize))
        {
            return {};
        }

        return childUsedSize;
    }

    void Button::OnDraw(WidgetDrawContext& context)
    {
        context.DrawQuad(context.registeredSize, m_color);
    }

    bool Button::OnMouseEvent(const WidgetMouseEvent& mouseEvent)
    {
        switch (mouseEvent.type)
        {
            case WidgetMouseEventType::MouseMove: break;
            case WidgetMouseEventType::MouseEnter: {
                m_color = m_pressed ? m_widgetStyle->colors.pressed : m_widgetStyle->colors.hovered;
            } break;
            case WidgetMouseEventType::MouseLeave: { 
                m_color = m_pressed ? m_widgetStyle->colors.pressed : m_widgetStyle->colors.normal; 
            } break;
            case WidgetMouseEventType::MouseButtonPressed: { 
                m_color = m_widgetStyle->colors.pressed; 
                m_pressed = true;
            } break;
            case WidgetMouseEventType::MouseButtonReleasedIn: { 
                m_color = m_widgetStyle->colors.hovered; 
                onPress(mouseEvent.button);
                m_pressed = false; 
            } break;
            case WidgetMouseEventType::MouseButtonReleasedOut: { 
                m_color = m_widgetStyle->colors.normal; m_pressed = false; 
            } break;
        }
        return true;
    }

}
