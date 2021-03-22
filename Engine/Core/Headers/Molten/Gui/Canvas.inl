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

    template<typename TTheme>
    Canvas<TTheme>::Canvas(CanvasRenderer& canvasRenderer) :
        m_canvasRenderer(canvasRenderer),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f),
        m_theme(m_canvasRenderer),
        m_overlayLayer(nullptr),
        m_widgetOverrideMouseEvents(nullptr),
        m_mouseInputUpdate(&Canvas<TTheme>::UpdateNormalMouseInputs)     
    {
        auto normalLane = m_layers.template GetLane<typename LayerData<TTheme>::ListNormalLaneType>();
        m_overlayLayer = InternalCreateLayer<MultiRootLayer>(normalLane.end());
    }

    template<typename TTheme>
    void Canvas<TTheme>::PushUserInputEvent(const UserInput::Event& inputEvent)
    {
        m_userInputEvents.push_back(inputEvent);
    }

    template<typename TTheme>
    void Canvas<TTheme>::Update(const Time& /*deltaTime*/)
    {
        UpdateUserInputs();

        auto layerPartialLane = m_layers.template GetLane<typename LayerData<TTheme>::ListPartialLaneType>();
        for (auto& layerData : layerPartialLane)
        {
            auto* layer = layerData->GetLayer();
            layer->SetSize(m_size);
            layer->Update({});
        }
    }

    template<typename TTheme>
    void Canvas<TTheme>::Draw()
    {
        m_canvasRenderer.BeginDraw();

        m_canvasRenderer.DrawRect({ { 0.0f, 0.0f }, m_size }, m_theme.backgroundColor);

        auto layerPartialLane = m_layers.template GetLane<typename LayerData<TTheme>::ListPartialLaneType>();
        for(auto& layer : layerPartialLane)
        {
            layer->GetLayer()->Draw();
        }

        m_canvasRenderer.EndDraw();
    }

    template<typename TTheme>
    void Canvas<TTheme>::SetSize(const Vector2f32& size)
    {
        if (size != m_size && size.x != 0.0f && size.y != 0.0f)
        {
            m_canvasRenderer.Resize(size);
        }

        m_size = size;
    }

    template<typename TTheme>
    void Canvas<TTheme>::SetScale(const Vector2f32& scale)
    {
        m_scale = scale;
    }

    template<typename TTheme>
    const Vector2f32& Canvas<TTheme>::GetSize() const
    {
        return m_size;
    }

    template<typename TTheme>
    const Vector2f32& Canvas<TTheme>::GetScale() const
    {
        return m_scale;
    }

    /*
    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    WidgetTypePointer<TWidget<TTheme>> Canvas<TTheme>::CreateChild(TArgs ... args)
    {
        constexpr bool usingWidgetMixin = std::is_base_of_v<WidgetMixin<TTheme, TWidget>, TWidget<TTheme>>;
        constexpr bool usingWidgetSkinMixin = std::is_base_of_v<WidgetSkinMixin<TTheme, TWidget>, WidgetSkin<TTheme, TWidget>>;

        static_assert(std::is_base_of_v<Widget<TTheme>, TWidget<TTheme>>, "TWidget is not base of Widget<TTheme>.");
        static_assert(!usingWidgetMixin || usingWidgetSkinMixin, "Using TWidgetMixin but WidgetSkinMixin<TTheme, TWidget> is not base of WidgetSkin<TTheme, TWidget>.");

        using WidgetDataType = std::conditional_t<usingWidgetMixin, WidgetDataMixin<TTheme, TWidget>, WidgetData<TTheme>>;

        auto normalLane = m_widgetTree.template GetLane<typename WidgetData<TTheme>::TreeNormalLaneType>();
        auto partialLane = m_widgetTree.template GetLane<typename WidgetData<TTheme>::TreePartialLaneType>();

        if (!normalLane.IsEmpty())
        {
            return nullptr;
        }

        auto widgetData = std::make_shared<WidgetDataType>();
        auto widget = std::make_shared<TWidget<TTheme>>(*widgetData, args...);

        widgetData->canvas = this;
        widgetData->layer = nullptr;
        widgetData->tree = &m_widgetTree;
        widgetData->iterator = m_widgetTree.Insert(partialLane, normalLane.end(), widgetData);
        widgetData->widget = widget;
        
        auto widgetSkin = m_theme.template Create<TWidget>(*widget, *widgetData);
        if constexpr (usingWidgetMixin == true)
        {
            widgetData->widgetSkinMixin = widgetSkin.get();
        }
        widgetData->widgetSkin = std::move(widgetSkin);

        if constexpr (std::is_base_of_v<WidgetEventHandler, TWidget<TTheme>>)
        {
            auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
            widgetData->mouseEventFunction = [handler = eventHandler, widget = widget.get()](const WidgetEvent& widgetEvent) -> Widget<TTheme>*
            {
                handler->HandleEvent(widgetEvent);
                return widget;
            };
        }

        return widget;
    }

    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    WidgetTypePointer<TWidget<TTheme>> Canvas<TTheme>::CreateChild(Widget<TTheme>& parent, TArgs ... args)
    {
        constexpr bool usingWidgetMixin = std::is_base_of_v<WidgetMixin<TTheme, TWidget>, TWidget<TTheme>>;
        constexpr bool usingWidgetSkinMixin = std::is_base_of_v<WidgetSkinMixin<TTheme, TWidget>, WidgetSkin<TTheme, TWidget>>;

        static_assert(std::is_base_of_v<Widget<TTheme>, TWidget<TTheme>>, "TWidget is not base of Widget<TTheme>.");
        static_assert(!usingWidgetMixin || usingWidgetSkinMixin, "Using TWidgetMixin but WidgetSkinMixin<TTheme, TWidget> is not base of WidgetSkin<TTheme, TWidget>.");

        using WidgetDataType = std::conditional_t<usingWidgetMixin, WidgetDataMixin<TTheme, TWidget>, WidgetData<TTheme>>;

        auto& parentData = parent.GetData();
        auto* parentTreeItem = std::addressof(*parentData.iterator);
        auto normalLane = parentTreeItem->GetChildren().template GetLane<typename WidgetData<TTheme>::TreeNormalLaneType>();
        auto partialLane = parentTreeItem->GetChildren().template GetLane<typename WidgetData<TTheme>::TreePartialLaneType>();

        auto widgetData = std::make_shared<WidgetDataType>();
        auto widget = std::make_shared<TWidget<TTheme>>(*widgetData, args...);
        
        widgetData->canvas = this;
        widgetData->layer = nullptr;
        widgetData->tree = &m_widgetTree;
        widgetData->iterator = m_widgetTree.Insert(partialLane, normalLane.end(), widgetData);
        widgetData->widget = widget;  

        auto widgetSkin = m_theme.template Create<TWidget>(*widget, *widgetData);
        if constexpr (usingWidgetMixin == true)
        {
            widgetData->widgetSkinMixin = widgetSkin.get();
        }
        widgetData->widgetSkin = std::move(widgetSkin);

        parentData.widget->OnAddChild(*widgetData);

        auto* parentWidget = parentData.widget.get();
        auto* parentEventHandler = dynamic_cast<WidgetEventHandler*>(parentWidget);
        if (parentEventHandler && parentWidget->GetOverrideChildrenMouseEvents())
        {
            if constexpr (std::is_base_of_v<WidgetEventHandler, TWidget<TTheme>>)
            {
                auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
                widgetData->mouseEventFunction = 
                    [handler = eventHandler, widget = widget.get(), parentHandler = parentEventHandler, parent = parentWidget]
                    (const WidgetEvent& widgetEvent)  -> Widget<TTheme>*
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
                widgetData->mouseEventFunction = [parentHandler = parentEventHandler, parent = parentWidget](const WidgetEvent& widgetEvent)  -> Widget<TTheme>*
                {
                    parentHandler->HandleEvent(widgetEvent);
                    return parent;
                };
            }
        }  
        else 
        {
            if constexpr (std::is_base_of_v<WidgetEventHandler, TWidget<TTheme>>)
            {
                auto* eventHandler = static_cast<WidgetEventHandler*>(widget.get());
                widgetData->mouseEventFunction = [handler = eventHandler, widget = widget.get()](const WidgetEvent& widgetEvent)->Widget<TTheme>*
                {
                    handler->HandleEvent(widgetEvent);
                    return widget;
                };
            }
        }

        return widget;
    }*/

    template<typename TTheme>
    template<template<typename> typename TLayer, typename ... TArgs>
    TLayer<TTheme>* Canvas<TTheme>::CreateLayer(const LayerPosition position, TArgs ... args)
    {
        auto normalLane = m_layers.template GetLane<typename LayerData<TTheme>::ListNormalLaneType>();
        auto insertPosition = position == LayerPosition::Bottom ? normalLane.begin() : --normalLane.end();
        return InternalCreateLayer<TLayer>(insertPosition, args...);
    }

    template<typename TTheme>
    void Canvas<TTheme>::OverrideMouseEventsUntilMouseRelease(Widget<TTheme>& widget)
    {
        m_mouseInputUpdate = &Canvas<TTheme>::UpdateModalMouseInputs;
        m_widgetOverrideMouseEvents = &widget;
    }

    template<typename TTheme>
    void Canvas<TTheme>::OverrideMouseEventsReset()
    {
        m_mouseInputUpdate = &Canvas<TTheme>::UpdateNormalMouseInputs;
        m_widgetOverrideMouseEvents = nullptr;
    }

    template<typename TTheme>
    template<template<typename> typename TLayer, typename ... TArgs>
    TLayer<TTheme>* Canvas<TTheme>::InternalCreateLayer(
        typename LayerData<TTheme>::ListNormalIterator position,
        TArgs ... args)
    {
        static_assert(std::is_base_of_v<Layer<TTheme>, TLayer<TTheme>>, "Layer<TTheme> is not a base of TLayer<TTheme>.");

        auto layerData = std::make_unique<LayerData<TTheme>>(this);
        auto* layerDataPointer = layerData.get();

        auto normalLane = m_layers.template GetLane<typename LayerData<TTheme>::ListNormalLaneType>();
        auto partialLane = m_layers.template GetLane<typename LayerData<TTheme>::ListPartialLaneType>();
        auto layerDataIt = partialLane.Insert(position, std::move(layerData));

        auto layer = std::make_unique<TLayer<TTheme>>(m_theme, *layerDataPointer, args...);
        auto* layerPointer = layer.get();

        layerDataPointer->Initialize(
            &m_layers,
            layerDataIt,
            std::move(layer));

        return layerPointer;
    }


    template<typename TTheme>
    void Canvas<TTheme>::UpdateUserInputs()
    {
        for (auto& inputEvent : m_userInputEvents)
        {
            switch (inputEvent.type)
            {
                case UserInput::EventType::Mouse: (*this.*m_mouseInputUpdate)(inputEvent); break;
                default: break;
            }
        }

        m_userInputEvents.clear();
    }

    template<typename TTheme>
    void Canvas<TTheme>::UpdateNormalMouseInputs(const UserInput::Event& mouseEvent)
    {
        switch (mouseEvent.subType)
        {
            case UserInput::EventSubType::MouseMove: HandleNormalMouseMove(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonPressed: HandleNormalMouseButtonPressed(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonReleased: HandleNormalMouseButtonReleased(mouseEvent); break;
            default: break;
        }
    }

    template<typename TTheme>
    void Canvas<TTheme>::HandleNormalMouseMove(const UserInput::Event& mouseEvent)
    {
        auto layerPartialLane = m_layers.template GetLane<typename LayerData<TTheme>::ListPartialLaneType>();
        for (auto it = layerPartialLane.rbegin(); it != layerPartialLane.rend(); it++)
        {
            auto& layerData = *it;
            if(layerData->GetLayer()->HandleUserInput(mouseEvent, m_multiLayerRepository))
            {
                return;
            }
        }

        if(m_multiLayerRepository.hoveredWidget)
        {


            m_multiLayerRepository.hoveredWidget = nullptr;
        }
    }

    template<typename TTheme>
    void Canvas<TTheme>::HandleNormalMouseButtonPressed(const UserInput::Event& mouseEvent)
    {

    }

    template<typename TTheme>
    void Canvas<TTheme>::HandleNormalMouseButtonReleased(const UserInput::Event& mouseEvent)
    {

    }


    /*
    
     */

    //template<typename TTheme>
    //void Canvas<TTheme>::HandleTreeMouseMove(const UserInput::Event& mouseEvent)
    //{
    //    /*bool hitWidget = false;

    //    m_widgetTree.template ForEachReversePreorder<typename WidgetData<TTheme>::TreePartialLaneType>(
    //        [&](auto& widgetData)
    //    {
    //        if (!widgetData->mouseEventFunction)
    //        {
    //            return true;
    //        }

    //        hitWidget = true;

    //        if (widgetData->GetGrantedBounds().Intersects(mouseEvent.mouseMoveEvent.position))
    //        {
    //            HandleMouseMoveTriggers(*widgetData, mouseEvent.mouseMoveEvent.position);
    //            return false;
    //        }

    //        return true;
    //    });

    //    if (!hitWidget && m_hoveredWidget)
    //    {
    //        TriggerMouseMoveEvent(m_hoveredWidget->GetData(), mouseEvent.mouseMoveEvent.position, WidgetEventSubType::MouseLeave);
    //        m_hoveredWidget = nullptr;
    //    }*/
    //}

    //template<typename TTheme>
    //void Canvas<TTheme>::HandleTreeMouseButtonPressed(const UserInput::Event& mouseEvent)
    //{
    //    /*bool hitWidget = false;

    //    m_widgetTree.template ForEachReversePreorder<typename WidgetData<TTheme>::TreePartialLaneType>(
    //        [&](auto& widgetData)
    //    {
    //        if (widgetData->GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
    //        {
    //            if (!widgetData->mouseEventFunction)
    //            {
    //                return true;
    //            }

    //            hitWidget = true;

    //            HandleMouseMoveTriggers(*widgetData, mouseEvent.mouseButtonEvent.position);

    //            WidgetEvent widgetEvent;
    //            widgetEvent.type = WidgetEventType::Mouse;
    //            widgetEvent.subType = WidgetEventSubType::MouseButtonPressed;
    //            widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;
    //            widgetEvent.mouseEvent.button = mouseEvent.mouseButtonEvent.button;
    //            m_pressedWidget = widgetData->mouseEventFunction(widgetEvent);

    //            return false;
    //        }

    //        if (!hitWidget && m_hoveredWidget)
    //        {
    //            TriggerMouseMoveEvent(m_hoveredWidget->GetData(), mouseEvent.mouseMoveEvent.position, WidgetEventSubType::MouseLeave);
    //            m_hoveredWidget = nullptr;
    //        }

    //        return true;
    //    });*/
    //}

    //template<typename TTheme>
    //void Canvas<TTheme>::HandleTreeMouseButtonReleased(const UserInput::Event& mouseEvent)
    //{
    //    /*if (!m_pressedWidget)
    //    {
    //        return;
    //    }

    //    WidgetEvent widgetEvent;
    //    widgetEvent.type = WidgetEventType::Mouse;
    //    widgetEvent.mouseEvent.button = mouseEvent.mouseButtonEvent.button;
    //    widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;

    //    auto& widgetData = m_pressedWidget->GetData();

    //    if (widgetData.GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
    //    {
    //        widgetEvent.subType = WidgetEventSubType::MouseButtonReleasedIn;
    //    }
    //    else
    //    {
    //        widgetEvent.subType = WidgetEventSubType::MouseButtonReleasedOut;
    //    }

    //    widgetData.GetMouseEventFunction()(widgetEvent);

    //    m_pressedWidget = nullptr;*/
    //}

    template<typename TTheme>
    void Canvas<TTheme>::UpdateModalMouseInputs(const UserInput::Event& mouseEvent)
    {
        /*if (m_widgetOverrideMouseEvents == nullptr)
        {
            m_mouseInputUpdate = &Canvas<TTheme>::UpdateModalMouseInputs;
            UpdateTreeMouseInputs(mouseEvent);
        }

        switch (mouseEvent.subType)
        {
            case UserInput::EventSubType::MouseMove: HandleModalMouseMove(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonPressed: HandleModalMouseButtonPressed(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonReleased: HandleModalMouseButtonReleased(mouseEvent); break;
            default: break;
        }*/
    }

    //template<typename TTheme>
    //void Canvas<TTheme>::HandleModalMouseMove(const UserInput::Event& mouseEvent)
    //{
    //    const auto& position = mouseEvent.mouseMoveEvent.position;
    //    TriggerMouseMoveEvent(m_pressedWidget->GetData(), position, WidgetEventSubType::MouseMove);
    //}
    //
    //template<typename TTheme>
    //void Canvas<TTheme>::HandleModalMouseButtonPressed(const UserInput::Event& mouseEvent)
    //{
    //    // Nothing here.
    //}
    //
    //template<typename TTheme>
    //void Canvas<TTheme>::HandleModalMouseButtonReleased(const UserInput::Event& mouseEvent)
    //{
    //    /*OverrideMouseEventsReset();

    //    if (!m_pressedWidget)
    //    {
    //        return;
    //    }

    //    WidgetEvent widgetEvent;
    //    widgetEvent.type = WidgetEventType::Mouse;
    //    widgetEvent.mouseEvent.button = mouseEvent.mouseButtonEvent.button;
    //    widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;

    //    auto& widgetData = m_pressedWidget->GetData();

    //    if (widgetData.GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
    //    {
    //        widgetEvent.subType = WidgetEventSubType::MouseButtonReleasedIn;
    //    }
    //    else
    //    {
    //        widgetEvent.subType = WidgetEventSubType::MouseButtonReleasedOut;
    //    }

    //    widgetData.CallMouseEventFunction(widgetEvent);

    //    m_pressedWidget = nullptr;*/
    //}

    //template<typename TTheme>
    //void Canvas<TTheme>::HandleMouseMoveTriggers(WidgetData<TTheme>& widgetData, const Vector2i32& position)
    //{
    //    /*if (m_hoveredWidget != widgetData.widget)
    //    {
    //        if (m_hoveredWidget)
    //        {
    //            TriggerMouseMoveEvent(m_hoveredWidget->GetData(), position, WidgetEventSubType::MouseLeave);
    //        }

    //        TriggerMouseMoveEvent(widgetData, position, WidgetEventSubType::MouseEnter);
    //    }

    //    TriggerMouseMoveEvent(widgetData, position, WidgetEventSubType::MouseMove);
    //    m_hoveredWidget = widgetData.widget;*/
    //}

    //template<typename TTheme>
    //void Canvas<TTheme>::TriggerMouseMoveEvent(WidgetData<TTheme>& widgetData, const Vector2f32& position, const WidgetEventSubType subType)
    //{
    //    /*WidgetEvent widgetEvent;
    //    widgetEvent.type = WidgetEventType::Mouse;
    //    widgetEvent.subType = subType;
    //    widgetEvent.mouseEvent.position = position;
    //    widgetData.CallMouseEventFunction(widgetEvent);*/
    //}

}