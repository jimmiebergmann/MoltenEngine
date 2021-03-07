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

#include <type_traits>

namespace Molten::Gui
{

    template<typename TSkin>
    inline Canvas<TSkin>::Canvas(Renderer& backendRenderer, CanvasRendererPointer renderer) :
        m_backendRenderer(backendRenderer),
        m_renderer(renderer),
        m_skin(backendRenderer, *m_renderer.get()),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f),
        m_pressedWidget(nullptr),
        m_widgetOverrideMouseEvents(nullptr),
        m_mouseInputUpdate(&Canvas<TSkin>::UpdateTreeMouseInputs)
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

        for (auto& drawCommand : m_topDrawCommands)
        {
            drawCommand();
        }
        m_topDrawCommands.clear();

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
        static_assert(std::is_base_of_v<Widget<TSkin>, TWidgetType<TSkin>>, "TWidgetType is not base of Molten::Gui::Widget.");

        auto normalLane = m_widgetTree.template GetLane<typename WidgetData<TSkin>::TreeNormalLaneType>();
        auto partialLane = m_widgetTree.template GetLane<typename WidgetData<TSkin>::TreePartialLaneType>();

        if (!normalLane.IsEmpty())
        {
            return nullptr;
        }

        constexpr bool usingWidgetMixin = std::is_base_of_v<WidgetMixin<TSkin, TWidgetType>, TWidgetType<TSkin>>;
        using WidgetDataType = std::conditional_t<usingWidgetMixin, WidgetDataMixin<TSkin, TWidgetType>, WidgetData<TSkin>>;

        auto widgetData = std::make_shared<WidgetDataType>();
        auto widget = std::make_shared<TWidgetType<TSkin>>(*widgetData.get(), args...);

        widgetData->canvas = this;
        widgetData->layer = nullptr;
        widgetData->tree = &m_widgetTree;
        widgetData->iterator = m_widgetTree.Insert(partialLane, normalLane.end(), widgetData);
        widgetData->widget = widget;
        
        auto widgetSkin = m_skin.template Create<TWidgetType<TSkin>>(*widget.get(), *widgetData.get());
        if constexpr (usingWidgetMixin == true)
        {
            widgetData->widgetSkinMixin = widgetSkin.get();
        }
        widgetData->widgetSkin = std::move(widgetSkin);

        if constexpr (std::is_base_of_v<WidgetEventHandler, TWidgetType<TSkin>>)
        {
            auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
            widgetData->mouseEventFunction = [handler = eventHandler, widget = widget.get()](const WidgetEvent& widgetEvent) -> Widget<TSkin>*
            {
                handler->HandleEvent(widgetEvent);
                return widget;
            };
        }

        return widget;
    }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    WidgetTypePointer<TWidgetType<TSkin>> Canvas<TSkin>::CreateChild(Widget<TSkin>& parent, TArgs ... args)
    {
        static_assert(std::is_base_of_v<Widget<TSkin>, TWidgetType<TSkin>>, "TWidgetType is not base of Molten::Gui::Widget.");

        auto& parentData = parent.GetData();
        auto* parentTreeItem = std::addressof(*parentData.iterator);
        auto normalLane = parentTreeItem->GetChildren().template GetLane<typename WidgetData<TSkin>::TreeNormalLaneType>();
        auto partialLane = parentTreeItem->GetChildren().template GetLane<typename WidgetData<TSkin>::TreePartialLaneType>();
        
        constexpr bool usingWidgetMixin = std::is_base_of_v<WidgetMixin<TSkin, TWidgetType>, TWidgetType<TSkin>>;
        using WidgetDataType = std::conditional_t<usingWidgetMixin, WidgetDataMixin<TSkin, TWidgetType>, WidgetData<TSkin>>;
     
        auto widgetData = std::make_shared<WidgetDataType>();
        auto widget = std::make_shared<TWidgetType<TSkin>>(*widgetData.get(), args...);
        
        widgetData->canvas = this;
        widgetData->layer = nullptr;
        widgetData->tree = &m_widgetTree;
        widgetData->iterator = m_widgetTree.Insert(partialLane, normalLane.end(), widgetData);
        widgetData->widget = widget;  

        auto widgetSkin = m_skin.template Create<TWidgetType<TSkin>>(*widget.get(), *widgetData.get());
        if constexpr (usingWidgetMixin == true)
        {
            widgetData->widgetSkinMixin = widgetSkin.get();
        }
        widgetData->widgetSkin = std::move(widgetSkin);

        parentData.widget->OnAddChild(*widgetData.get());

        auto* parentWidget = parentData.widget.get();
        auto* parentEventHandler = dynamic_cast<WidgetEventHandler*>(parentWidget);
        if (parentEventHandler && parentWidget->GetOverrideChildrenMouseEvents())
        {
            if constexpr (std::is_base_of_v<WidgetEventHandler, TWidgetType<TSkin>>)
            {
                auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
                widgetData->mouseEventFunction = 
                    [handler = eventHandler, widget = widget.get(), parentHandler = parentEventHandler, parent = parentWidget]
                    (const WidgetEvent& widgetEvent)  -> Widget<TSkin>*
                    {
                        if (parentHandler->HandleEvent(widgetEvent))
                        {
                            return parent;
                        }
                        handler->HandleEvent(widgetEvent);
                        return widget;
                    };
            }
            else
            {
                widgetData->mouseEventFunction = [parentHandler = parentEventHandler, parent = parentWidget](const WidgetEvent& widgetEvent)  -> Widget<TSkin>*
                {
                    parentHandler->HandleEvent(widgetEvent);
                    return parent;
                };
            }
        }  
        else 
        {
            if constexpr (std::is_base_of_v<WidgetEventHandler, TWidgetType<TSkin>>)
            {
                auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
                widgetData->mouseEventFunction = [handler = eventHandler, widget = widget.get()](const WidgetEvent& widgetEvent)->Widget<TSkin>*
                {
                    handler->HandleEvent(widgetEvent);
                    return widget;
                };
            }
        }

        return widget;
    }

    template<typename TSkin>
    void Canvas<TSkin>::OverrideMouseEventsUntilMouseRelease(Widget<TSkin>& widget)
    {
        m_mouseInputUpdate = &Canvas<TSkin>::UpdateModalMouseInputs;
        m_widgetOverrideMouseEvents = &widget;
    }

    template<typename TSkin>
    void Canvas<TSkin>::OverrideMouseEventsReset()
    {
        m_mouseInputUpdate = &Canvas<TSkin>::UpdateTreeMouseInputs;
        m_widgetOverrideMouseEvents = nullptr;
    }

    template<typename TSkin>
    void Canvas<TSkin>::PushTopDrawCommand(std::function<void()>&& drawCommand)
    {
        m_topDrawCommands.push_back(drawCommand);
    }

    template<typename TSkin>
    void Canvas<TSkin>::UpdateUserInputs()
    {
        for (auto& inputEvent : m_userInputEvents)
        {
            switch (inputEvent.type)
            {
                case UserInput::EventType::Mouse: (*this.*m_mouseInputUpdate)(inputEvent); break;
                case UserInput::EventType::Keyboard: break;
                default: break;
            }
        }

        m_userInputEvents.clear();
    }

    template<typename TSkin>
    void Canvas<TSkin>::UpdateTreeMouseInputs(const UserInput::Event& mouseEvent)
    {
        switch (mouseEvent.subType)
        {
            case UserInput::EventSubType::MouseMove: HandleTreeMouseMove(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonPressed: HandleTreeMouseButtonPressed(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonReleased: HandleTreeMouseButtonReleased(mouseEvent); break;
            default: break;
        }      
    }

    template<typename TSkin>
    void Canvas<TSkin>::HandleTreeMouseMove(const UserInput::Event& mouseEvent)
    {
        bool hitWidget = false;

        m_widgetTree.template ForEachReversePreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            if (!widgetData->mouseEventFunction)
            {
                return true;
            }

            hitWidget = true;

            if (widgetData->GetGrantedBounds().Intersects(mouseEvent.mouseMoveEvent.position))
            {
                HandleMouseMoveTriggers(*widgetData, mouseEvent.mouseMoveEvent.position);
                return false;
            }

            return true;
        });

        if (!hitWidget && m_hoveredWidget)
        {
            TriggerMouseMoveEvent(m_hoveredWidget->GetData(), mouseEvent.mouseMoveEvent.position, WidgetEventSubType::MouseLeave);
            m_hoveredWidget = nullptr;
        }
    }

    template<typename TSkin>
    void Canvas<TSkin>::HandleTreeMouseButtonPressed(const UserInput::Event& mouseEvent)
    {
        bool hitWidget = false;

        m_widgetTree.template ForEachReversePreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            if (widgetData->GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
            {
                if (!widgetData->mouseEventFunction)
                {
                    return true;
                }

                hitWidget = true;

                HandleMouseMoveTriggers(*widgetData, mouseEvent.mouseButtonEvent.position);

                WidgetEvent widgetEvent;
                widgetEvent.type = WidgetEventType::Mouse;
                widgetEvent.subType = WidgetEventSubType::MouseButtonPressed;
                widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;
                widgetEvent.mouseEvent.button = mouseEvent.mouseButtonEvent.button;
                m_pressedWidget = widgetData->mouseEventFunction(widgetEvent);

                return false;
            }

            if (!hitWidget && m_hoveredWidget)
            {
                TriggerMouseMoveEvent(m_hoveredWidget->GetData(), mouseEvent.mouseMoveEvent.position, WidgetEventSubType::MouseLeave);
                m_hoveredWidget = nullptr;
            }

            return true;
        });
    }

    template<typename TSkin>
    void Canvas<TSkin>::HandleTreeMouseButtonReleased(const UserInput::Event& mouseEvent)
    {
        if (!m_pressedWidget)
        {
            return;
        }

        WidgetEvent widgetEvent;
        widgetEvent.type = WidgetEventType::Mouse;
        widgetEvent.mouseEvent.button = mouseEvent.mouseButtonEvent.button;
        widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;

        auto& widgetData = m_pressedWidget->GetData();

        if (widgetData.GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
        {
            widgetEvent.subType = WidgetEventSubType::MouseButtonReleasedIn;
        }
        else
        {
            widgetEvent.subType = WidgetEventSubType::MouseButtonReleasedOut;
        }

        widgetData.mouseEventFunction(widgetEvent);

        m_pressedWidget = nullptr;
    }

    template<typename TSkin>
    void Canvas<TSkin>::UpdateModalMouseInputs(const UserInput::Event& mouseEvent)
    {
        if (m_widgetOverrideMouseEvents == nullptr)
        {
            m_mouseInputUpdate = &Canvas<TSkin>::UpdateModalMouseInputs;
            UpdateTreeMouseInputs(mouseEvent);
        }

        switch (mouseEvent.subType)
        {
            case UserInput::EventSubType::MouseMove: HandleModalMouseMove(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonPressed: HandleModalMouseButtonPressed(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonReleased: HandleModalMouseButtonReleased(mouseEvent); break;
            default: break;
        }
    }

    template<typename TSkin>
    void Canvas<TSkin>::HandleModalMouseMove(const UserInput::Event& mouseEvent)
    {
        const auto& position = mouseEvent.mouseMoveEvent.position;
        TriggerMouseMoveEvent(m_pressedWidget->GetData(), position, WidgetEventSubType::MouseMove);
    }
    
    template<typename TSkin>
    void Canvas<TSkin>::HandleModalMouseButtonPressed(const UserInput::Event& mouseEvent)
    {
        // Nothing here.
    }
    
    template<typename TSkin>
    void Canvas<TSkin>::HandleModalMouseButtonReleased(const UserInput::Event& mouseEvent)
    {
        OverrideMouseEventsReset();

        if (!m_pressedWidget)
        {
            return;
        }

        WidgetEvent widgetEvent;
        widgetEvent.type = WidgetEventType::Mouse;
        widgetEvent.mouseEvent.button = mouseEvent.mouseButtonEvent.button;
        widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;

        auto& widgetData = m_pressedWidget->GetData();

        if (widgetData.GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
        {
            widgetEvent.subType = WidgetEventSubType::MouseButtonReleasedIn;
        }
        else
        {
            widgetEvent.subType = WidgetEventSubType::MouseButtonReleasedOut;
        }

        widgetData.mouseEventFunction(widgetEvent);

        m_pressedWidget = nullptr;
    }

    template<typename TSkin>
    void Canvas<TSkin>::HandleMouseMoveTriggers(WidgetData<TSkin>& widgetData, const Vector2i32& position)
    {
        if (m_hoveredWidget != widgetData.widget)
        {
            if (m_hoveredWidget)
            {
                TriggerMouseMoveEvent(m_hoveredWidget->GetData(), position, WidgetEventSubType::MouseLeave);
            }

            TriggerMouseMoveEvent(widgetData, position, WidgetEventSubType::MouseEnter);
        }

        TriggerMouseMoveEvent(widgetData, position, WidgetEventSubType::MouseMove);
        m_hoveredWidget = widgetData.widget;
    }

    template<typename TSkin>
    void Canvas<TSkin>::TriggerMouseMoveEvent(WidgetData<TSkin>& widgetData, const Vector2f32& position, const WidgetEventSubType subType)
    {
        WidgetEvent widgetEvent;
        widgetEvent.type = WidgetEventType::Mouse;
        widgetEvent.subType = subType;
        widgetEvent.mouseEvent.position = position;
        widgetData.mouseEventFunction(widgetEvent);
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

}