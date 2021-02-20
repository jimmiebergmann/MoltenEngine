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

#include <type_traits>

namespace Molten::Gui
{

    template<typename TSkin>
    inline Canvas<TSkin>::Canvas(Renderer& backendRenderer, CanvasRendererPointer renderer) :
        m_backendRenderer(backendRenderer),
        m_renderer(renderer),
        m_skin(backendRenderer, *m_renderer.get()),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f)
    {}

    template<typename TSkin>
    inline Canvas<TSkin>::~Canvas()
    {}

    template<typename TSkin>
    inline void Canvas<TSkin>::SetRenderer(CanvasRendererPointer renderer)
    {
        m_renderer = renderer;
    }

    template<typename TSkin>
    inline CanvasRendererPointer Canvas<TSkin>::GetRenderer()
    {
        return m_renderer;
    }
    template<typename TSkin>
    inline const CanvasRendererPointer Canvas<TSkin>::GetRenderer() const
    {
        return m_renderer;
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::PushUserInputEvent(const UserInput::Event& inputEvent)
    {
        m_userInputEvents.push_back(inputEvent);
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::Update(const Time& /*deltaTime*/)
    {
        UpdateUserInputs();
        UpdateWidgetSkins();
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::Draw()
    {
        m_renderer->BeginDraw();

        m_widgetTree.template ForEachPreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            //renderer.MaskArea(renderData.position, renderData.size);
            widgetData->widgetSkin->Draw();
        });

        m_renderer->EndDraw();
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::SetSize(const Vector2f32& size)
    {
        if (size != m_size && size.x != 0.0f && size.y != 0.0f)
        {
            m_renderer->Resize(size);
        }

        m_size = size;
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::SetScale(const Vector2f32& scale)
    {
        m_scale = scale;
    }

    template<typename TSkin>
    inline const Vector2f32& Canvas<TSkin>::GetSize() const
    {
        return m_size;
    }

    template<typename TSkin>
    inline const Vector2f32& Canvas<TSkin>::GetScale() const
    {
        return m_scale;
    }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    WidgetTypePointer<TWidgetType<TSkin>> Canvas<TSkin>::CreateChild(TArgs ... args)
    {
        static_assert(std::is_base_of<Gui::Widget<TSkin>, TWidgetType<TSkin>>::value, "TWidgetType is not base of Molten::Gui::Widget.");

        auto normalLane = m_widgetTree.template GetLane<typename WidgetData<TSkin>::TreeNormalLaneType>();
        auto partialLane = m_widgetTree.template GetLane<typename WidgetData<TSkin>::TreePartialLaneType>();

        if (!normalLane.IsEmpty())
        {
            return nullptr;
        }

        auto widgetData = std::make_shared<WidgetData<TSkin>>();
        auto widget = std::make_shared<TWidgetType<TSkin>>(*widgetData.get(), args...);

        widgetData->canvas = this;
        widgetData->layer = nullptr;
        widgetData->tree = &m_widgetTree;
        widgetData->iterator = m_widgetTree.Insert(partialLane, normalLane.end(), widgetData);
        widgetData->widget = widget;
        widgetData->widgetSkin = m_skin.template Create<TWidgetType<TSkin>>(*widget.get(), *widgetData.get());

        if constexpr (std::is_base_of_v<WidgetEventHandler, TWidgetType<TSkin>>)
        {
            auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
            widgetData->mouseEventFunction = [handler = eventHandler](const WidgetEvent& widgetEvent)
            {
                return handler->HandleEvent(widgetEvent);
            };
        }

        return widget;
    }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    WidgetTypePointer<TWidgetType<TSkin>> Canvas<TSkin>::CreateChild(Widget<TSkin>& parent, TArgs ... args)
    {
        static_assert(std::is_base_of<Gui::Widget<TSkin>, TWidgetType<TSkin>>::value, "TWidgetType is not base of Molten::Gui::Widget.");

        auto& parentData = parent.GetData();
        auto* parentTreeItem = std::addressof(*parentData.iterator);
        auto normalLane = parentTreeItem->GetChildren().template GetLane<typename WidgetData<TSkin>::TreeNormalLaneType>();
        auto partialLane = parentTreeItem->GetChildren().template GetLane<typename WidgetData<TSkin>::TreePartialLaneType>();

        auto widgetData = std::make_shared<WidgetData<TSkin>>();
        auto widget = std::make_shared<TWidgetType<TSkin>>(*widgetData.get(), args...);

        widgetData->canvas = this;
        widgetData->layer = nullptr;
        widgetData->tree = &m_widgetTree;
        widgetData->iterator = m_widgetTree.Insert(partialLane, normalLane.end(), widgetData);
        widgetData->widget = widget;
        widgetData->widgetSkin = m_skin.template Create<TWidgetType<TSkin>>(*widget.get(), *widgetData.get());       

        parentData.widget->OnAddChild(*widgetData.get());

        auto* parentWidget = parentData.widget.get();
        auto* parentEventHandler = dynamic_cast<WidgetEventHandler*>(parentWidget);
        if (parentEventHandler && parentWidget->GetOverrideChildrenMouseEvents())
        {
            if constexpr (std::is_base_of_v<WidgetEventHandler, TWidgetType<TSkin>>)
            {
                auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
                widgetData->mouseEventFunction = [handler = eventHandler, parentHandler = parentEventHandler](const WidgetEvent& widgetEvent)
                {
                    if (parentHandler->HandleEvent(widgetEvent))
                    {
                        return true;
                    }
                    return handler->HandleEvent(widgetEvent);
                };
            }
            else
            {
                widgetData->mouseEventFunction = [parentHandler = parentEventHandler](const WidgetEvent& widgetEvent)
                {
                    return parentHandler->HandleEvent(widgetEvent);
                };
            }
        }  
        else if(std::is_base_of_v<WidgetEventHandler, TWidgetType<TSkin>>)
        {
            auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
            widgetData->mouseEventFunction = [handler = eventHandler](const WidgetEvent& widgetEvent)
            {
                return handler->HandleEvent(widgetEvent);
            };
        }

        return widget;
    }

    template<typename TSkin>
    void Canvas<TSkin>::UpdateUserInputs()
    {
        std::vector<UserInput::Event> mouseEvents;
        std::vector<UserInput::Event> keyboardEvents;

        for (auto& inputEvent : m_userInputEvents)
        {
            switch (inputEvent.type)
            {
                case UserInput::EventType::Mouse: mouseEvents.push_back(inputEvent);
                case UserInput::EventType::Keyboard: keyboardEvents.push_back(inputEvent);
                default: break;
            }
        }
        m_userInputEvents.clear();

        for (auto& mouseEvent : mouseEvents)
        {
            switch (mouseEvent.subType)
            {
                case UserInput::EventSubType::MouseMove: HandleMouseMove(mouseEvent); break;
                case UserInput::EventSubType::MouseButtonPressed: HandleMousePressed(mouseEvent); break;
                case UserInput::EventSubType::MouseButtonReleased: HandleMouseReleased(mouseEvent); break;
                default: break;
            }
        }
    }

    template<typename TSkin>
    void Canvas<TSkin>::UpdateWidgetSkins()
    {
        auto rootLane = m_widgetTree.GetLane<typename WidgetData<TSkin>::TreePartialLaneType>();
        for (auto& rootWidget : rootLane)
        {
            rootWidget.GetValue()->SetGrantedBounds({ { 0.0f, 0.0f }, m_size });
        }

        m_widgetTree.template ForEachPreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            widgetData->widget->Update();
        });
    }

    template<typename TSkin>
    void Canvas<TSkin>::HandleMouseMove(UserInput::Event& mouseEvent)
    {
        bool hitWidget = false;

        m_widgetTree.template ForEachReversePreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            if (!widgetData->mouseEventFunction)
            {
                return true;
            }

            if (widgetData->GetGrantedBounds().Intersects(mouseEvent.mouseMoveEvent.position))
            {
                WidgetEvent widgetEvent;
                widgetEvent.type = WidgetEventType::Mouse;

                if (m_hoveredWidget && m_hoveredWidget != widgetData->widget)
                {
                    widgetEvent.subType = WidgetEventSubType::MouseLeave;
                    widgetEvent.mouseEvent.position = mouseEvent.mouseMoveEvent.position;
                    auto& mouseEventFunction = m_hoveredWidget->GetData().mouseEventFunction;
                    mouseEventFunction(widgetEvent);
                }

                m_hoveredWidget = widgetData->widget;

                widgetEvent.subType = WidgetEventSubType::MouseEnter; // FIX!!!
                widgetEvent.mouseEvent.position = mouseEvent.mouseMoveEvent.position;
                auto& mouseEventFunction = m_hoveredWidget->GetData().mouseEventFunction;
                mouseEventFunction(widgetEvent);

                hitWidget = true;

                return false;
            }

            return true;
        });

        if (!hitWidget && m_hoveredWidget)
        {
            WidgetEvent widgetEvent;
            widgetEvent.type = WidgetEventType::Mouse;
            widgetEvent.subType = WidgetEventSubType::MouseLeave;
            widgetEvent.mouseEvent.position = mouseEvent.mouseMoveEvent.position;
            auto& mouseEventFunction = m_hoveredWidget->GetData().mouseEventFunction;
            mouseEventFunction(widgetEvent);
        }
    }

    template<typename TSkin>
    void Canvas<TSkin>::HandleMousePressed(UserInput::Event& mouseEvent)
    {
        m_widgetTree.template ForEachReversePreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            if (widgetData->GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
            {
                if (!widgetData->mouseEventFunction)
                {
                    return true;
                }

                WidgetEvent widgetEvent;
                widgetEvent.type = WidgetEventType::Mouse;
                
                if (m_hoveredWidget != widgetData->widget)
                {
                    if (m_hoveredWidget)
                    {
                        widgetEvent.subType = WidgetEventSubType::MouseLeave;
                        widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;
                        auto& mouseEventFunction = m_hoveredWidget->GetData().mouseEventFunction;
                        mouseEventFunction(widgetEvent);
                    }
                    m_hoveredWidget = widgetData->widget;

                    widgetEvent.subType = WidgetEventSubType::MouseEnter;
                    widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;
                    auto& mouseEventFunction = m_hoveredWidget->GetData().mouseEventFunction;
                    mouseEventFunction(widgetEvent);
                }
                
                m_pressedWidget = widgetData->widget;

                widgetEvent.subType = WidgetEventSubType::MousePress;
                widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;

                auto& mouseEventFunction = m_pressedWidget->GetData().mouseEventFunction;
                mouseEventFunction(widgetEvent);
                
                return false;
            }
            return true;
        });
    }

    template<typename TSkin>
    void Canvas<TSkin>::HandleMouseReleased(UserInput::Event& mouseEvent)
    {
        if (!m_pressedWidget)
        {
            return;
        }

        WidgetEvent widgetEvent;
        widgetEvent.type = WidgetEventType::Mouse;
        widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;

        auto& widgetData = m_pressedWidget->GetData();

        if (widgetData.GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
        {
            widgetEvent.subType = WidgetEventSubType::MouseReleaseIn;
        }
        else
        {
            widgetEvent.subType = WidgetEventSubType::MouseReleaseOut;
        }

        auto& mouseEventFunction = widgetData.mouseEventFunction;
        mouseEventFunction(widgetEvent);

        m_pressedWidget = nullptr;
    }

}