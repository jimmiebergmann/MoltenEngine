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

namespace Molten::Gui
{

    // Layer implementations.
    template<typename TTheme>
    Layer<TTheme>::Layer(const LayerDescriptor<TTheme>& descriptor) :
        m_theme(descriptor.theme),
        m_canvas(descriptor.canvas),
        m_widgetPropertyDispatcher(descriptor.propertyDispatcher),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f)
    {}

    template<typename TTheme>
    bool Layer<TTheme>::HandleUserInput(
        const UserInput::Event& userInputEvent,
        WidgetMouseEventTracker<TTheme>& mouseEventTracker)
    {
        if (userInputEvent.type == UserInput::EventType::Mouse)
        {
            switch (userInputEvent.subType)
            {
                case UserInput::EventSubType::MouseMove: return HandleMouseMoveEvent(userInputEvent.mouseMoveEvent, mouseEventTracker);
                case UserInput::EventSubType::MouseButtonPressed: return HandleMouseButtonPressedEvent(userInputEvent.mouseButtonEvent, mouseEventTracker);
                case UserInput::EventSubType::MouseButtonReleased: return HandleMouseButtonReleasedEvent(userInputEvent.mouseButtonEvent, mouseEventTracker);
                default: break;
            }
        }

        return false;
    }

    template<typename TTheme>
    void Layer<TTheme>::Update(const Time&)
    {
        m_widgetDrawQueue.clear();
        auto updateContext = WidgetUpdateContext<TTheme>{ m_widgetDrawQueue };

        for (auto& child : m_children)
        {
            child->m_bounds.position = { 0.0f, 0.0f };
            child->m_grantedSize = m_size;
            child->OnUpdate(updateContext);
            updateContext.DrawChild(*child);
        }

        m_overlayWidgetDrawQueue.clear();
        auto overlayUpdateContext = WidgetUpdateContext<TTheme>{ m_overlayWidgetDrawQueue };

        for (auto& overlayWidget : m_overlayChildren)
        {
            overlayWidget->m_bounds.position = { 0.0f, 0.0f };
            overlayWidget->m_grantedSize = m_size;
            overlayWidget->OnUpdate(overlayUpdateContext);
            overlayUpdateContext.DrawChild(*overlayWidget);
        }

        m_visibilityTracker.Update();
    }

    template<typename TTheme>
    void Layer<TTheme>::Draw()
    {
        for (auto it = m_widgetDrawQueue.rbegin(); it != m_widgetDrawQueue.rend(); ++it)
        {
            (*it)->m_skinBase->Draw();
        }
        for (auto it = m_overlayWidgetDrawQueue.rbegin(); it != m_overlayWidgetDrawQueue.rend(); ++it)
        {
            (*it)->m_skinBase->Draw();
        }
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
        return CreateChildInternal<TWidget>(m_children, nullptr, std::forward<TArgs>(args)...);
    }

    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    TWidget<TTheme>* Layer<TTheme>::CreateChildFor(Widget<TTheme>* parent, TArgs ... args)
    {
        return CreateChildInternal<TWidget>(parent->m_children, parent, std::forward<TArgs>(args)...);
    }

    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    ManagedWidget<TTheme, TWidget> Layer<TTheme>::CreateOverlayChild(TArgs ... args)
    {
        auto* widget = CreateChildInternal<TWidget>(m_overlayChildren, nullptr, std::forward<TArgs>(args)...);
        return ManagedWidget<TTheme, TWidget>(this, widget);
    }

    template<typename TTheme>
    template<template<typename> typename TWidget>
    void Layer<TTheme>::DestroyOverlayChild(ManagedWidget<TTheme, TWidget>& managedWidget)
    {
        auto* widget = managedWidget.m_widget;
        managedWidget.m_widget = nullptr;
        managedWidget.m_layer = nullptr;
        
        if (auto it = std::find_if(m_overlayChildren.begin(), m_overlayChildren.end(), [&widget](const auto& rhs) {
            return rhs.get() == widget; }); it != m_overlayChildren.end())
        {
            m_overlayChildren.erase(it);
        }
    }

    template<typename TTheme>
    bool Layer<TTheme>::HandleMouseMoveEvent(
        const UserInput::MouseMoveEvent& mouseMoveEvent,
        WidgetMouseEventTracker<TTheme>& mouseEventTracker)
    {
        for (auto* widget : m_overlayWidgetDrawQueue)
        {
            if (mouseEventTracker.HandleMouseMove(widget, mouseMoveEvent.position))
            {
                return true;
            }
        }

        for (auto* widget : m_widgetDrawQueue)
        {
            if (mouseEventTracker.HandleMouseMove(widget, mouseMoveEvent.position))
            {
                return true;
            }
        }

        return false;
    }

    template<typename TTheme>
    bool Layer<TTheme>::HandleMouseButtonPressedEvent(
        const UserInput::MouseButtonEvent& mouseButtonEvent,
        WidgetMouseEventTracker<TTheme>& mouseEventTracker)
    {
        for (auto* widget : m_overlayWidgetDrawQueue)
        {
            if (mouseEventTracker.HandleMouseButtonPress(widget, mouseButtonEvent.position, mouseButtonEvent.button))
            {
                return true;
            }
        }

        for (auto* widget : m_widgetDrawQueue)
        {
            if (mouseEventTracker.HandleMouseButtonPress(widget, mouseButtonEvent.position, mouseButtonEvent.button))
            {
                return true;
            }
        }

        return false;
    }

    template<typename TTheme>
    bool Layer<TTheme>::HandleMouseButtonReleasedEvent(
        const UserInput::MouseButtonEvent& mouseButtonEvent,
        WidgetMouseEventTracker<TTheme>& mouseEventTracker)
    {
        mouseEventTracker.HandleMouseButtonRelease(mouseButtonEvent.position, mouseButtonEvent.button);
        return false;
    }

    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    TWidget<TTheme>* Layer<TTheme>::CreateChildInternal(
        WidgetChildren<TTheme>& childContainer,
        Widget<TTheme>* parent,
        TArgs ... args)
    {
        WidgetMixinDescriptor<TTheme, TWidget> widgetDescriptor = {
            WidgetDescriptor<TTheme>{
                .parent = parent,
                .canvas = m_canvas,
                .layer = this,
                .theme = m_theme,
                .propertyDispatcher = m_widgetPropertyDispatcher,
                .visibilityTracker = m_visibilityTracker
            },
            nullptr
        };

        auto widget = std::make_unique<TWidget<TTheme>>(widgetDescriptor, std::forward<TArgs>(args)...);
        auto* widgetPtr = widget.get();

        auto widgetSkin = m_theme.template Create<TWidget>(*widgetPtr);
        auto* widgetSkinPtr = widgetSkin.get();
        widgetPtr->m_skin = std::move(widgetSkin);
        widgetPtr->m_skinBase = widgetSkinPtr;

        widgetPtr->m_mouseEventFunction = CreateChildMouseEventFunction(parent, widgetPtr);

        childContainer.push_back(std::move(widget));

        if (parent)
        {
            parent->OnAddChild(*widgetPtr);
        }

        widgetPtr->OnCreate();

        return widgetPtr;
    }

    template<typename TTheme>
    template<template<typename> typename TWidget>
    WidgetMouseEventFunction<TTheme> Layer<TTheme>::CreateChildMouseEventFunction(
        [[maybe_unused]] Widget<TTheme>* parent,
        [[maybe_unused]] TWidget<TTheme>* widget)
    {
        if constexpr (std::is_base_of_v<WidgetMouseEventHandler, TWidget<TTheme>>)
        {
            auto* eventHandler = static_cast<WidgetMouseEventHandler*>(widget);

            return [childHandler = eventHandler, widgetPtr = widget](const WidgetMouseEvent& widgetEvent) -> Widget<TTheme>*
            {
                return childHandler->OnMouseEvent(widgetEvent) ? widgetPtr : nullptr;
            };
        }
        else
        {
            return nullptr;
        }
    }
   
}
