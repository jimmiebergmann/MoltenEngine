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
    // Multi layer repository implementations.
    template<typename TTheme>
    MultiLayerRepository<TTheme>::PressedWidget::PressedWidget(
        WidgetData<TTheme>* widgetData,
        Mouse::Button button
    ) :
        widgetData(widgetData),
        button(button)
    {}

    template<typename TTheme>
    MultiLayerRepository<TTheme>::MultiLayerRepository() :
        hoveredWidgetData(nullptr)
    {}


    template<typename TTheme>
    bool MultiLayerRepository<TTheme>::HandleMouseMove(
        WidgetData<TTheme>* widgetData,
        const Vector2f32& position)
    {
        
        auto& mouseEventFunction = widgetData->GetMouseEventFunction();
        if (!mouseEventFunction)
        {
            return false;
        }

        if (widgetData->GetGrantedBounds().Intersects(position))
        {
            if (hoveredWidgetData != widgetData)
            {
                const bool handledMouseEnter = mouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseEnter, position }) != nullptr;
                const bool handledMouseMove = mouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseMove, position }) != nullptr;

                if (handledMouseEnter || handledMouseMove)
                {
                    if (hoveredWidgetData != nullptr)
                    {
                        auto& hoveredWidgetMouseEventFunction = hoveredWidgetData->GetMouseEventFunction();
                        hoveredWidgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseLeave, position });
                    }

                    hoveredWidgetData = widgetData;
                    return true;
                }
            }
            else if (hoveredWidgetData != nullptr)
            {
                auto& hoveredWidgetMouseEventFunction = hoveredWidgetData->GetMouseEventFunction();
                const bool handledMouseMove = hoveredWidgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseMove, position });

                if (!handledMouseMove)
                {
                    hoveredWidgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseLeave, position });
                    hoveredWidgetData = nullptr;
                    return false;
                }
                return true;
            }
        }

        return false;
    }

    template<typename TTheme>
    bool MultiLayerRepository<TTheme>::HandleMouseButtonPress(
        WidgetData<TTheme>* widgetData,
        const Vector2f32& position,
        const Mouse::Button button)
    {
       auto& mouseEventFunction = widgetData->GetMouseEventFunction();
       if (!mouseEventFunction)
       {
           return false;
       }

       if (widgetData->GetGrantedBounds().Intersects(position))
       {
           auto foundIt = std::find_if(pressedWidgets.begin(), pressedWidgets.end(),
               [&](const auto& pressedWidget)
           {
               return pressedWidget.button == button;
           });

           if(foundIt == pressedWidgets.end())
           {
               const bool handledMouseButtonPress = mouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseButtonPressed, position, button }) != nullptr;

               if(handledMouseButtonPress)
               {
                   pressedWidgets.push_back({ widgetData, button });
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
    bool MultiLayerRepository<TTheme>::HandleMouseButtonRelease(
        const Vector2f32& position,
        const Mouse::Button button)
    {
        for(auto it = pressedWidgets.begin(); it != pressedWidgets.end();)
        {
            auto& pressedWidget = *it;
            if(pressedWidget.button == button)
            {
                auto& widgetMouseEventFunction = pressedWidget.widgetData->GetMouseEventFunction();

                if (pressedWidget.widgetData->GetGrantedBounds().Intersects(position))
                {
                    widgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseButtonReleasedIn, position, button });
                }
                else
                {
                    widgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseButtonReleasedOut, position, button });
                }

                it = pressedWidgets.erase(it);
            }
            else
            {
                ++it;
            }
        }

        return false;
    }

    template<typename TTheme>
    void MultiLayerRepository<TTheme>::ResetHoveredWidget(const Vector2f32& position)
    {
        if (hoveredWidgetData)
        {
            auto& hoveredWidgetMouseEventFunction = hoveredWidgetData->GetMouseEventFunction();
            hoveredWidgetMouseEventFunction(WidgetMouseEvent{ WidgetMouseEventType::MouseLeave, position });
        }
        hoveredWidgetData = nullptr;
    }


    // Layer implementations.
    template<typename TTheme>
    Layer<TTheme>::Layer(
        TTheme& theme,
        LayerData<TTheme>& data
    ) :
        m_theme(theme),
        m_data(data),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f),
        m_currentVisibleWidgetsContainer(&m_visibleWidgets[0])
    {}

    template<typename TTheme>
    bool Layer<TTheme>::HandleUserInput(
        const UserInput::Event& userInputEvent,
        MultiLayerRepository<TTheme>& multiLayerRepository)
    {
        if (userInputEvent.type == UserInput::EventType::Mouse)
        {
            switch (userInputEvent.subType)
            {
                case UserInput::EventSubType::MouseMove: return HandleMouseMoveEvent(userInputEvent.mouseMoveEvent, multiLayerRepository);
                case UserInput::EventSubType::MouseButtonPressed: return HandleMouseButtonPressedEvent(userInputEvent.mouseButtonEvent, multiLayerRepository);
                case UserInput::EventSubType::MouseButtonReleased: return HandleMouseButtonReleasedEvent(userInputEvent.mouseButtonEvent, multiLayerRepository);
                default: break;
            }
        }

        return false;
    }

    template<typename TTheme>
    void Layer<TTheme>::Update(const Time&)
    {
        auto rootLane = m_widgetTree.template GetLane<typename WidgetData<TTheme>::TreePartialLaneType>();
        for (auto& rootWidget : rootLane)
        {
            rootWidget.GetValue()->SetGrantedBounds({ { 0.0f, 0.0f }, m_size });
        }

        m_widgetTree.template ForEachPreorder<typename WidgetData<TTheme>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            widgetData->GetWidget()->Update();
        });

        VisibilityUpdate();
    }

    template<typename TTheme>
    void Layer<TTheme>::Draw()
    {
        m_widgetTree.template ForEachPreorder<typename WidgetData<TTheme>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            widgetData->GetWidgetSkin()->Draw();
        });
    }

    template<typename TTheme>
    void Layer<TTheme>::SetSize(const Vector2f32& size)
    {
        m_size = size;
    }

    template<typename TTheme>
    void Layer<TTheme>::SetScale(const Vector2f32& scale)
    {
        m_scale = scale;
    }

    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    TWidget<TTheme>* Layer<TTheme>::CreateChild(TArgs ... args)
    {
        auto normalLane = m_widgetTree.template GetLane<typename WidgetData<TTheme>::TreeNormalLaneType>();
        if (!AllowsMultipleRoots() && !normalLane.IsEmpty())
        {
            return nullptr;
        }

        auto partialLane = m_widgetTree.template GetLane<typename WidgetData<TTheme>::TreePartialLaneType>();

        return InternalCreateChild<TWidget, TArgs...>(partialLane, normalLane.end(), nullptr, args...);
    }

    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    TWidget<TTheme>* Layer<TTheme>::CreateChild(
        Widget<TTheme>& parent,
        TArgs ... args)
    {
        auto& parentWidgetData = parent.GetData();
        auto parentNormalLane = parentWidgetData.GetChildrenNormalLane();
        auto parentPartialLane = parentWidgetData.GetChildrenPartialLane();

        return InternalCreateChild<TWidget, TArgs...>(parentPartialLane, parentNormalLane.end(), &parent, args...);
    }

    template<typename TTheme>
    TTheme& Layer<TTheme>::GetTheme()
    {
        return m_theme;
    }
    template<typename TTheme>
    const TTheme& Layer<TTheme>::GetTheme() const
    {
        return m_theme;
    }

    template<typename TTheme>
    LayerData<TTheme>& Layer<TTheme>::GetData()
    {
        return m_data;
    }
    template<typename TTheme>
    const LayerData<TTheme>& Layer<TTheme>::GetData() const
    {
        return m_data;
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::Tree& Layer<TTheme>::GetWidgetTree()
    {
        return m_widgetTree;
    }
    template<typename TTheme>
    const typename WidgetData<TTheme>::Tree& Layer<TTheme>::GetWidgetTree() const
    {
        return m_widgetTree;
    }

    template<typename TTheme>
    template<template<typename> typename TWidget>
    WidgetData<TTheme>& Layer<TTheme>::GetWidgetData(TWidget<TTheme>& widget)
    {
        return widget.GetData();
    }
    template<typename TTheme>
    template<template<typename> typename TWidget>
    const WidgetData<TTheme>& Layer<TTheme>::GetWidgetData(const Widget<TTheme>& widget)
    {
        return widget.GetData();
    }

    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs, typename TLane, typename TTreeIterator>
    TWidget<TTheme>* Layer<TTheme>::InternalCreateChild(
        TLane& lane,
        TTreeIterator iterator,
        Widget<TTheme>* parent,
        TArgs ... args)
    {
        static_assert(std::is_base_of_v<WidgetMixin<TTheme, TWidget>, TWidget<TTheme>>,
            "WidgetMixin<TTheme, TWidget> is not a base of TWidget<TTheme>.");

        auto widgetDataMixin = std::make_unique<WidgetDataMixin<TTheme, TWidget>>(m_data.GetCanvas(), this);
        auto* widgetDataMixinPointer = widgetDataMixin.get();
        
        auto widgetDataIt = m_widgetTree.Insert(lane, iterator, std::move(widgetDataMixin));

        auto widget = std::make_unique<TWidget<TTheme>>(*widgetDataMixinPointer, args...);
        auto* widgetPointer = widget.get();

        auto widgetSkin = GetTheme().template Create<TWidget>(*widgetPointer, *widgetDataMixinPointer);

        widgetDataMixinPointer->InitializeMixin(
            &m_widgetTree,
            widgetDataIt,
            parent,
            std::move(widget),
            std::move(widgetSkin),
            CreateChildMouseEventFunction(widgetPointer, parent));

        static_cast<Widget<TTheme>*>(widgetPointer)->OnCreate();

        if(parent)
        {
            auto& parentWidgetData = parent->GetData();
            parentWidgetData.GetWidget()->OnAddChild(*widgetDataMixinPointer);
        }

        if constexpr(std::is_base_of_v<VisibilityWidget, TWidget<TTheme>>)
        {
            m_visibilityWidgets.push_back(widgetPointer);
        }

        return widgetPointer;
    }

    template<typename TTheme>
    template<template<typename> typename TWidget>
    typename WidgetData<TTheme>::MouseEventFunction Layer<TTheme>::CreateChildMouseEventFunction(
        [[maybe_unused]] TWidget<TTheme>* child,
        Widget<TTheme>*)
    {
        //if (parentEventHandler && parentWidget->GetOverrideChildrenMouseEvents())
        //{
        //    if constexpr (std::is_base_of_v<WidgetEventHandler, TWidget<TTheme>>)
        //    {
        //        /*auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
        //        widgetData->mouseEventFunction =
        //            [handler = eventHandler, widget = widget.get(), parentHandler = parentEventHandler, parent = parentWidget]
        //        (const WidgetEvent& widgetEvent)->Widget<TTheme>*
        //        {
        //            if (parentHandler->HandleEvent(widgetEvent))
        //            {
        //                return parent;
        //            }
        //            handler->HandleEvent(widgetEvent);
        //            return widget;
        //        };*/
        //    }
        //    else
        //    {
        //       /* widgetData->mouseEventFunction = [parentHandler = parentEventHandler, parent = parentWidget](const WidgetEvent& widgetEvent)  -> Widget<TTheme>*
        //        {
        //            parentHandler->HandleEvent(widgetEvent);
        //            return parent;
        //        };*/
        //    }
        //}
        //else
        //{
        //    if constexpr (std::is_base_of_v<WidgetEventHandler, TWidget<TTheme>>)
        //    {
        //       /* auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
        //        widgetData->mouseEventFunction = [handler = eventHandler, widget = widget.get()](const WidgetEvent& widgetEvent)->Widget<TTheme>*
        //        {
        //            handler->HandleEvent(widgetEvent);
        //            return widget;
        //        };*/
        //    }
        //}

        /*return [](const WidgetMouseEvent&) -> Widget<TTheme>*
        {
            return nullptr;
        };*/

        if constexpr (std::is_base_of_v<WidgetMouseEventHandler, TWidget<TTheme>>)
        {
            auto* eventHandler = static_cast<WidgetMouseEventHandler*>(child);
            return [childHandler = eventHandler, childWidget = child](const WidgetMouseEvent& widgetEvent) -> Widget<TTheme>*
            {
                return childHandler->OnMouseEvent(widgetEvent) ? childWidget : nullptr;
            };
        }
        else
        {
            return nullptr;
        }
    }

    template<typename TTheme>
    void Layer<TTheme>::VisibilityUpdate()
    {
        auto* nextVisibilityWidgetsContainer = m_currentVisibleWidgetsContainer == &m_visibleWidgets[0] ?
            &m_visibleWidgets[1] : &m_visibleWidgets[0];

        for (auto* visibilityWidget : m_visibilityWidgets)
        {
            if (visibilityWidget->m_isVisible)
            {
                nextVisibilityWidgetsContainer->push_back(visibilityWidget);

                const auto it = std::find(m_currentVisibleWidgetsContainer->begin(), m_currentVisibleWidgetsContainer->end(), visibilityWidget);
                if (it == m_currentVisibleWidgetsContainer->end())
                {
                    visibilityWidget->onShow();
                }

                visibilityWidget->onIsVisible();
            }
            else
            {
                const auto it = std::find(m_currentVisibleWidgetsContainer->begin(), m_currentVisibleWidgetsContainer->end(), visibilityWidget);
                if (it != m_currentVisibleWidgetsContainer->end())
                {
                    visibilityWidget->onHide();
                }
            }
        }

        m_currentVisibleWidgetsContainer->clear();
        m_currentVisibleWidgetsContainer = m_currentVisibleWidgetsContainer == &m_visibleWidgets[0] ?
            &m_visibleWidgets[1] : &m_visibleWidgets[0];
    }

    template<typename TTheme>
    bool Layer<TTheme>::HandleMouseMoveEvent(
        const UserInput::MouseMoveEvent& mouseMoveEvent,
        MultiLayerRepository<TTheme>& multiLayerRepository)
    {
        bool handledEvent = false;

        m_widgetTree.template ForEachReversePreorder<typename WidgetData<TTheme>::TreePartialLaneType>(
        [&](auto& widgetData) -> bool
        {
            if (multiLayerRepository.HandleMouseMove(widgetData.get(), mouseMoveEvent.position))
            {
                handledEvent = true;
                return false;
            }
            return true;
        });

        return handledEvent;
    }

    template<typename TTheme>
    bool Layer<TTheme>::HandleMouseButtonPressedEvent(
        const UserInput::MouseButtonEvent& mouseButtonEvent,
        MultiLayerRepository<TTheme>& multiLayerRepository)
    {
        bool handledEvent = false;

        m_widgetTree.template ForEachReversePreorder<typename WidgetData<TTheme>::TreePartialLaneType>(
            [&](auto& widgetData) -> bool
        {
            if(multiLayerRepository.HandleMouseButtonPress(widgetData.get(), mouseButtonEvent.position, mouseButtonEvent.button))
            {
                handledEvent = true;
                return false;
            }
            return true;
        });

        return handledEvent;
    }

    template<typename TTheme>
    bool Layer<TTheme>::HandleMouseButtonReleasedEvent(
        const UserInput::MouseButtonEvent& mouseButtonEvent,
        MultiLayerRepository<TTheme>& multiLayerRepository)
    {
        multiLayerRepository.HandleMouseButtonRelease(mouseButtonEvent.position, mouseButtonEvent.button);
        return false;
    }
   
}
