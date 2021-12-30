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

    // Mouse event tracker implementations.
    template<typename TTheme>
    WidgetMouseEventTracker<TTheme>::WidgetMouseEventTracker() :
        m_hoveredWidget(nullptr)
    {}

    template<typename TTheme>
    bool WidgetMouseEventTracker<TTheme>::IsHoveringWidget() const
    {
        return m_hoveredWidget != nullptr;
    }

    template<typename TTheme>
    bool WidgetMouseEventTracker<TTheme>::HandleMouseMove(
        Widget<TTheme>* widget,
        const Vector2f32& position)
    {
        auto& mouseEventFunction = widget->m_mouseEventFunction;
        if (!mouseEventFunction)
        {
            return false;
        }

        if (widget->GetBounds().Intersects(position))
        {
            if (m_hoveredWidget != widget)
            {
                const bool handledMouseEnter = mouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseEnter, position }) != nullptr;
                const bool handledMouseMove = mouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseMove, position }) != nullptr;

                if (handledMouseEnter || handledMouseMove)
                {
                    if (m_hoveredWidget != nullptr)
                    {
                        auto& hoveredWidgetMouseEventFunction = m_hoveredWidget->m_mouseEventFunction;
                        hoveredWidgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseLeave, position });
                    }

                    m_hoveredWidget = widget;
                    return true;
                }
            }
            else if (m_hoveredWidget != nullptr)
            {
                auto& hoveredWidgetMouseEventFunction = m_hoveredWidget->m_mouseEventFunction;
                const bool handledMouseMove = hoveredWidgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseMove, position });

                if (!handledMouseMove)
                {
                    hoveredWidgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseLeave, position });
                    m_hoveredWidget = nullptr;
                    return false;
                }
                return true;
            }
        }

        return false;
    }

    template<typename TTheme>
    bool WidgetMouseEventTracker<TTheme>::HandleMouseButtonPress(
        Widget<TTheme>* widget,
        const Vector2f32& position,
        const Mouse::Button button)
    {
        auto& mouseEventFunction = widget->m_mouseEventFunction;
        if (!mouseEventFunction)
        {
            return false;
        }

        if (widget->GetBounds().Intersects(position))
        {
            auto foundIt = std::find_if(m_pressedWidgets.begin(), m_pressedWidgets.end(),
                [&](const auto& pressedWidget)
                {
                    return pressedWidget.button == button;
                });

            if (foundIt == m_pressedWidgets.end())
            {
                const bool handledMouseButtonPress = mouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseButtonPressed, position, button }) != nullptr;
                if (handledMouseButtonPress)
                {
                    m_pressedWidgets.emplace_back(widget, button);
                    return true;
                }
            }
            else
            {
                return true;
            }
        }

        return false;
    }

    template<typename TTheme>
    void WidgetMouseEventTracker<TTheme>::HandleMouseButtonRelease(
        const Vector2f32& position,
        const Mouse::Button button)
    {
        for (auto it = m_pressedWidgets.begin(); it != m_pressedWidgets.end();)
        {
            auto& pressedWidget = *it;
            if (pressedWidget.button == button)
            {
                auto& widgetMouseEventFunction = pressedWidget.widget->m_mouseEventFunction;

                if (pressedWidget.widget->GetBounds().Intersects(position))
                {
                    widgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseButtonReleasedIn, position, button });
                }
                else
                {
                    widgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseButtonReleasedOut, position, button });
                }

                it = m_pressedWidgets.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    template<typename TTheme>
    void WidgetMouseEventTracker<TTheme>::ResetHoveredWidget(const Vector2f32& position)
    {
        if (m_hoveredWidget)
        {
            auto& hoveredWidgetMouseEventFunction = m_hoveredWidget->m_mouseEventFunction;
            hoveredWidgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseLeave, position });
            m_hoveredWidget = nullptr;
        }
    }

    template<typename TTheme>
    WidgetMouseEventTracker<TTheme>::PressedWidget::PressedWidget(
        Widget<TTheme>* widget,
        const Mouse::Button button
    ) :
        widget(widget),
        button(button)
    {}

}