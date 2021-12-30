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

#ifndef MOLTEN_CORE_GUI_CANVAS_HPP
#define MOLTEN_CORE_GUI_CANVAS_HPP

#include "Molten/Gui/Layers/MultiRootLayer.hpp"
#include "Molten/Gui/Widget.hpp"
#include "Molten/Gui/WidgetEventTracker.hpp"
#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Time.hpp"
#include "Molten/System/UserInput.hpp"
#include "Molten/System/Signal.hpp"
#include <vector>
#include <type_traits>

namespace Molten
{
    class Logger;
}

namespace Molten::Gui
{

    class CanvasRenderer;


    template<typename TTheme>
    class Canvas
    {

    public:

        template<typename ... TThemeArgs>
        explicit Canvas(CanvasRenderer& canvasRenderer, TThemeArgs&& ... themeArgs);
        ~Canvas() = default;

        Canvas(Canvas&&) = delete;
        Canvas(const Canvas&) = delete;
        Canvas& operator= (Canvas&&) = delete;
        Canvas& operator= (const Canvas&) = delete;

        void PushUserInputEvent(const UserInput::Event& inputEvent);

        void Update(const Time& deltaTime);

        void Draw();

        void SetSize(const Vector2f32& size);
        void SetScale(const Vector2f32& scale);

        [[nodiscard]] const Vector2f32& GetSize() const;
        [[nodiscard]] const Vector2f32& GetScale() const;

        /** Creates a new layer and inserts it at a given position. */
        template<template<typename> typename TLayer, typename ... TArgs>
        [[nodiscard]] TLayer<TTheme>* CreateLayer(const LayerPosition position, TArgs ... args);

        bool DestroyWidget(Widget<TTheme>* widget);

        /** Creates a new widget in the overlay layer.
         * ManagedWidget is automatically destroy and removed from overlay at destruction.
         */
        /*template<template<typename> typename TWidget, typename ... TArgs>
        [[nodiscard]] ManagedWidget<TTheme, TWidget> CreateOverlayChild(TArgs ... args);*/

        /** This function call makes the provided widget to receive all mouse events, 
         * regardless of layers above or child widgets on top being hovered or pressed, until the mouse button is released.
         */
        void OverrideMouseEventsUntilMouseRelease(
            Widget<TTheme>& widget,
            Mouse::Button button);

        //void OverrideMouseEventsReset();

    private:

        /**
         * Internal function for creating and inserting a new layer at provided iterator position.
         */
        template<template<typename> typename TLayer, typename ... TArgs>
        [[nodiscard]] TLayer<TTheme>* InternalCreateLayer(
            typename LayerData<TTheme>::ListNormalIterator position,
            TArgs ... args);

        void UpdateUserInputs(); 
        
        void UpdateNormalMouseInputs(const UserInput::Event& mouseEvent);
        void HandleNormalMouseMove(const UserInput::Event& mouseEvent);
        void HandleNormalMouseButtonPressed(const UserInput::Event& mouseEvent);
        void HandleNormalMouseButtonReleased(const UserInput::Event& mouseEvent);
        
        void UpdateModalMouseInputs(const UserInput::Event& mouseEvent);
        void HandleModalMouseMove(const UserInput::Event& mouseEvent);
        void HandleModalMouseButtonPressed(const UserInput::Event& mouseEvent);
        void HandleModalMouseButtonReleased(const UserInput::Event& mouseEvent);

        CanvasRenderer& m_canvasRenderer;
     
        Vector2f32 m_size;
        Vector2f32 m_scale;

        TTheme m_theme;
        typename LayerData<TTheme>::List m_layers;
        //MultiRootLayer<TTheme>* m_overlayLayer;

        SignalDispatcher m_propertyChangeDispatcher;

        void(Canvas<TTheme>::* m_mouseInputUpdate)(const UserInput::Event&);
        std::vector<UserInput::Event> m_userInputEvents;
        WidgetMouseEventTracker<TTheme> m_mouseEventTracker;

        Widget<TTheme>* m_overrideWidgetMouseEventWidget;
        Mouse::Button m_overrideWidgetMouseEventButton;

    };

}

#include "Molten/Gui/Canvas.inl"

#endif