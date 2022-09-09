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

#ifndef MOLTEN_GRAPHICS_GUI_LAYER_HPP
#define MOLTEN_GRAPHICS_GUI_LAYER_HPP

#include "Molten/Graphics/Gui/Widget.hpp"
#include "Molten/Graphics/Gui/WidgetEventTracker.hpp"
#include "Molten/Graphics/Gui/WidgetVisibilityTracker.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Time.hpp"
#include "Molten/System/UserInput.hpp"
#include "Molten/System/Signal.hpp"
#include <vector>

namespace Molten::Gui
{

    enum class LayerPosition
    {
        Top,
        Bottom
    };

    template<typename TTheme>
    struct LayerDescriptor
    {
        Canvas<TTheme>* canvas;
        TTheme& theme;
        SignalDispatcher& propertyDispatcher;
    };


    /** Layer base class. Layers should inherit from LayerMixin instead of this class template.*/
    template<typename TTheme>
    class Layer
    {

    public:

        explicit Layer(const LayerDescriptor<TTheme>& descriptor);

        virtual ~Layer() = default;

        Layer(Layer&&) = delete;
        Layer(const Layer&) = delete;
        Layer& operator= (Layer&&) = delete;
        Layer& operator= (const Layer&) = delete;

        virtual bool HandleUserInput(
            const UserInput::Event& userInputEvent,
            WidgetMouseEventTracker<TTheme>& mouseEventTracker);

        virtual void Update(const Time& deltaTime);

        virtual void Draw();

        virtual void SetSize(const Vector2f32& size);

        virtual void SetScale(const Vector2f32& scale);

        virtual bool AllowsMultipleRoots() = 0;

        template<template<typename> typename TWidget, typename ... TArgs>
        TWidget<TTheme>* CreateChild(TArgs ... args);

        template<template<typename> typename TWidget, typename ... TArgs>
        TWidget<TTheme>* CreateChildFor(Widget<TTheme>* parent, TArgs ... args);

        template<template<typename> typename TWidget, typename ... TArgs>
        [[nodiscard]] ManagedWidget<TTheme, TWidget> CreateOverlayChild(TArgs ... args);

        template<template<typename> typename TWidget>
        void DestroyOverlayChild(ManagedWidget<TTheme, TWidget>& managedWidget);

    protected:

        [[nodiscard]] bool HandleMouseMoveEvent(
            const UserInput::MouseMoveEvent& mouseMoveEvent,
            WidgetMouseEventTracker<TTheme>& mouseEventTracker);
        [[nodiscard]] bool HandleMouseButtonPressedEvent(
            const UserInput::MouseButtonEvent& mouseButtonEvent,
            WidgetMouseEventTracker<TTheme>& mouseEventTracker);
        [[nodiscard]] bool HandleMouseButtonReleasedEvent(
            const UserInput::MouseButtonEvent& mouseButtonEvent,
            WidgetMouseEventTracker<TTheme>& mouseEventTracker);

        TTheme& m_theme;
        Canvas<TTheme>* m_canvas;
        SignalDispatcher& m_widgetPropertyDispatcher;
        WidgetChildren<TTheme> m_children;
        WidgetChildren<TTheme> m_overlayChildren;
        WidgetVisibilityTracker m_visibilityTracker;
        Vector2f32 m_size;
        Vector2f32 m_scale;
        WidgetPointers<TTheme> m_widgetDrawQueue;
        WidgetPointers<TTheme> m_overlayWidgetDrawQueue;

    private:

        template<typename> friend class Widget;

        template<template<typename> typename TWidget, typename ... TArgs>
        [[nodiscard]] TWidget<TTheme>* CreateChildInternal(
            WidgetChildren<TTheme>& childContainer,
            Widget<TTheme>* parent,
            TArgs ... args);

        template<template<typename> typename TWidget>
        [[nodiscard]] WidgetMouseEventFunction<TTheme> CreateChildMouseEventFunction(
            Widget<TTheme>* parent,
            TWidget<TTheme>* widget);

    };

}

#include "Molten/Graphics/Gui/Layer.inl"

#endif