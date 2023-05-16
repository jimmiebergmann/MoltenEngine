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

#include "Molten/Graphics/Gui2/Canvas2.hpp"
#include "Molten/Graphics/Gui2/CanvasRenderer2.hpp"

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    Canvas::Canvas(
        std::shared_ptr<CanvasRenderer> canvasRenderer,
        std::shared_ptr<Style> style
    ) :
        m_canvasRenderer(canvasRenderer),
        m_style(style ? style : std::make_shared<Style>()),
        m_widgetStyle(m_style->Get<CanvasStyle>()),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f)
    {}


    void Canvas::AddChild(std::shared_ptr<Widget> )
    {
    }

    void Canvas::PushUserInputEvent(const UserInput::Event& inputEvent)
    {
        m_userInputEvents.push_back(inputEvent);
    }

    void Canvas::Update()
    {
        UpdateUserInputs();
        UpdateChildren();
    }

    void Canvas::Draw()
    {
        m_canvasRenderer->DrawQuad({ { 0.0f, 0.0f }, m_size }, m_widgetStyle->colors.background);

        if (m_childBounds.size.x <= 0.0f || m_childBounds.size.y <= 0.0f)
        {
            return;
        }

        auto context = WidgetDrawContext{ *m_canvasRenderer, m_scale };

        for (auto& widgetDrawEntry : m_widgetDrawBuffer)
        {
            context.SetCurrentWidgetDrawBufferEntry(&widgetDrawEntry);
            widgetDrawEntry.widget->OnDraw(context);
            //Widget::Draw(context, *widgetDrawEntry.widget);
        }

        /*[[maybe_unused]] auto pushedPosition = context.PushPosition(m_childBounds.position);

        for (auto& child : m_children)
        {
            Widget::Draw(context, *child);
        }*/
    }

    void Canvas::SetSize(const Vector2f32& size)
    {
       
        if (size != m_size && size.x != 0.0f && size.y != 0.0f)
        {
            m_canvasRenderer->Resize(size);
        }

        m_size = size;
    }

    void Canvas::SetScale(const Vector2f32& scale)
    {
        m_scale = scale;
    }

    void Canvas::UpdateUserInputs()
    {
        for (auto& inputEvent : m_userInputEvents)
        {
            switch (inputEvent.type)
            {
                case UserInput::EventType::Mouse: HandleMouseEvent(inputEvent); break;
                default: break;
            }
        }

        m_userInputEvents.clear();
    }

    void Canvas::HandleMouseEvent(const UserInput::Event& /*mouseEvent*/) 
    {
        /*switch (mouseEvent.subType)
        {
            case UserInput::EventSubType::MouseMove: return HandleMouseMoveEvent(userInputEvent.mouseMoveEvent);
            case UserInput::EventSubType::MouseButtonPressed: return HandleMouseButtonPressedEvent(userInputEvent.mouseButtonEvent);
            case UserInput::EventSubType::MouseButtonReleased: return HandleMouseButtonReleasedEvent(userInputEvent.mouseButtonEvent);
            default: break;
        }*/
    }

    bool Canvas::HandleMouseMoveEvent(const UserInput::MouseMoveEvent& mouseMoveEvent)
    {
        return false;
    }

    bool Canvas::HandleMouseButtonPressedEvent(const UserInput::MouseButtonEvent& mouseButtonEvent)
    {
        return false;
    }

    bool Canvas::HandleMouseButtonReleasedEvent(const UserInput::MouseButtonEvent& mouseButtonEvent)
    {
        return false;
    }


    void Canvas::UpdateChildren()
    {
        m_widgetDrawBuffer.clear();
        m_widgetDrawBoundsStack.clear();

        m_childBounds = Widget::WithoutMarginsAndPadding(
            m_size / m_scale,
            m_widgetStyle->padding,
            m_widgetStyle->margins);

        m_widgetDrawBoundsStack.push_back(m_childBounds);

        auto context = WidgetUpdateContext{
            m_widgetDrawBuffer,
            m_widgetDrawBoundsStack
        };

        for (auto& child : m_children)
        {
            Widget::Update(context, *child, m_childBounds.size);
        }
    }

}