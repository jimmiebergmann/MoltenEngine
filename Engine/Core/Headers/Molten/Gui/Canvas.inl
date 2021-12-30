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

    template<typename TTheme>
    template<typename ... TThemeArgs>
    Canvas<TTheme>::Canvas(CanvasRenderer& canvasRenderer, TThemeArgs&& ... themeArgs) :
        m_canvasRenderer(canvasRenderer),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f),
        m_theme(m_canvasRenderer, std::forward<TThemeArgs>(themeArgs)...),
        m_mouseInputUpdate(&Canvas<TTheme>::UpdateNormalMouseInputs),
        m_overrideWidgetMouseEventWidget(nullptr),
        m_overrideWidgetMouseEventButton(Mouse::Button::Left)
        //m_overlayLayer(nullptr)
    {
        //auto normalLane = m_layers.template GetLane<typename LayerData<TTheme>::ListNormalLaneType>();
        //m_overlayLayer = InternalCreateLayer<MultiRootLayer>(normalLane.end());
    }

    template<typename TTheme>
    void Canvas<TTheme>::PushUserInputEvent(const UserInput::Event& inputEvent)
    {
        m_userInputEvents.push_back(inputEvent);
    }

    template<typename TTheme>
    void Canvas<TTheme>::Update(const Time& /*deltaTime*/)
    {
        m_propertyChangeDispatcher.Execute();

        UpdateUserInputs();

        auto layerPartialLane = m_layers.template GetLane<typename LayerData<TTheme>::ListPartialLaneType>();
        for (auto& layerData : layerPartialLane)
        {
            auto* layer = layerData->GetLayer();
            layer->SetSize(m_size);
            layer->Update({});
        }

        m_theme.Update();
    }

    template<typename TTheme>
    void Canvas<TTheme>::Draw()
    {
        m_canvasRenderer.DrawRect(AABB2f32{ { 0.0f, 0.0f }, m_size }, m_theme.backgroundColor);

        auto layerPartialLane = m_layers.template GetLane<typename LayerData<TTheme>::ListPartialLaneType>();
        for(auto& layer : layerPartialLane)
        {
            layer->GetLayer()->Draw();
        }
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

    template<typename TTheme>
    template<template<typename> typename TLayer, typename ... TArgs>
    TLayer<TTheme>* Canvas<TTheme>::CreateLayer(const LayerPosition position, TArgs ... args)
    {
        auto normalLane = m_layers.template GetLane<typename LayerData<TTheme>::ListNormalLaneType>();
        auto insertPosition = position == LayerPosition::Bottom ? normalLane.begin() : (normalLane.GetSize() > 0 ? std::prev(normalLane.end()) : normalLane.end());
        return InternalCreateLayer<TLayer>(insertPosition, args...);
    }

    template<typename TTheme>
    bool Canvas<TTheme>::DestroyWidget(Widget<TTheme>* widget)
    {
        if(!widget)
        {
            return false;
        }  

        auto& widgetData = widget->GetData();

        if(auto* parentWidget = widgetData.GetParentWidget(); parentWidget)
        {
            parentWidget->OnRemoveChild(widgetData);
        }

        auto it = widgetData.GetTreeIterator();
        widgetData.GetTree()->Erase(it);
        return true;
    }

    //template<typename TTheme>
    //template<template<typename> typename TWidget, typename ... TArgs>
    //ManagedWidget<TTheme, TWidget> Canvas<TTheme>::CreateOverlayChild(TArgs ... args)
    //{
    //    auto* widget = m_overlayLayer->template CreateChild<TWidget>(std::forward<TArgs>(args)...);
    //    return { *m_overlayLayer, *widget };
    //}

    template<typename TTheme>
    void Canvas<TTheme>::OverrideMouseEventsUntilMouseRelease(
        Widget<TTheme>& widget,
        Mouse::Button button)
    {
        m_mouseInputUpdate = &Canvas<TTheme>::UpdateModalMouseInputs;
        m_overrideWidgetMouseEventWidget = &widget;
        m_overrideWidgetMouseEventButton = button;
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

        auto partialLane = m_layers.template GetLane<typename LayerData<TTheme>::ListPartialLaneType>();
        auto layerDataIt = partialLane.Insert(position, std::move(layerData));

        auto layer = std::make_unique<TLayer<TTheme>>(m_theme, *layerDataPointer, m_propertyChangeDispatcher, args...);
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
        for (auto it = layerPartialLane.rbegin(); it != layerPartialLane.rend(); ++it)
        {
            auto& layerData = *it;
            if(layerData->GetLayer()->HandleUserInput(mouseEvent, m_mouseEventTracker))
            {
                return;
            }
        }

        if (m_mouseEventTracker.IsHoveringWidget())
        {
            m_mouseEventTracker.ResetHoveredWidget(mouseEvent.mouseMoveEvent.position);
        }
    }

    template<typename TTheme>
    void Canvas<TTheme>::HandleNormalMouseButtonPressed(const UserInput::Event& mouseEvent)
    {
        auto layerPartialLane = m_layers.template GetLane<typename LayerData<TTheme>::ListPartialLaneType>();
        for (auto it = layerPartialLane.rbegin(); it != layerPartialLane.rend(); ++it)
        {
            auto& layerData = *it;
            if (layerData->GetLayer()->HandleUserInput(mouseEvent, m_mouseEventTracker))
            {
                return;
            }
        }
    }

    template<typename TTheme>
    void Canvas<TTheme>::HandleNormalMouseButtonReleased(const UserInput::Event& mouseEvent)
    {
        auto layerPartialLane = m_layers.template GetLane<typename LayerData<TTheme>::ListPartialLaneType>();
        for (auto it = layerPartialLane.rbegin(); it != layerPartialLane.rend(); ++it)
        {
            auto& layerData = *it;
            if (layerData->GetLayer()->HandleUserInput(mouseEvent, m_mouseEventTracker))
            {
                return;
            }
        }
    }

    template<typename TTheme>
    void Canvas<TTheme>::UpdateModalMouseInputs(const UserInput::Event& mouseEvent)
    {
        if (m_overrideWidgetMouseEventWidget == nullptr)
        {
            m_mouseInputUpdate = &Canvas<TTheme>::UpdateNormalMouseInputs;
            (*this.*m_mouseInputUpdate)(mouseEvent);
            return;
        }

        switch (mouseEvent.subType)
        {
            case UserInput::EventSubType::MouseMove: HandleModalMouseMove(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonPressed: HandleModalMouseButtonPressed(mouseEvent); break;
            case UserInput::EventSubType::MouseButtonReleased: HandleModalMouseButtonReleased(mouseEvent); break;
            default: break;
        }
    }

    template<typename TTheme>
    void Canvas<TTheme>::HandleModalMouseMove(const UserInput::Event& mouseEvent)
    {
        auto& mouseMoveEvent = mouseEvent.mouseMoveEvent;
        m_mouseEventTracker.HandleMouseMove(m_overrideWidgetMouseEventWidget, mouseMoveEvent.position);
    }

    template<typename TTheme>
    void Canvas<TTheme>::HandleModalMouseButtonPressed(const UserInput::Event& mouseEvent)
    {
        auto& mouseButtonEvent = mouseEvent.mouseButtonEvent;
        m_mouseEventTracker.HandleMouseButtonPress(m_overrideWidgetMouseEventWidget, mouseButtonEvent.position, mouseButtonEvent.button);
    }

    template<typename TTheme>
    void Canvas<TTheme>::HandleModalMouseButtonReleased(const UserInput::Event& mouseEvent)
    {
        auto& mouseButtonEvent = mouseEvent.mouseButtonEvent;
        m_mouseEventTracker.HandleMouseButtonRelease(mouseButtonEvent.position, mouseButtonEvent.button);
        
        if (mouseEvent.mouseButtonEvent.button == m_overrideWidgetMouseEventButton)
        {
            m_mouseInputUpdate = &Canvas<TTheme>::UpdateNormalMouseInputs;
            m_overrideWidgetMouseEventWidget = nullptr;
        }
    }

}
