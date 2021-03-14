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

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Gui/Widgets/DockerWidget.hpp"
#include "Molten/Gui/WidgetData.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Gui/Layer.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Time.hpp"
#include "Molten/System/UserInput.hpp"
#include <memory>
#include <list>
#include <set>
#include <vector>
#include <functional>

namespace Molten
{
    class Renderer;
    class Logger;
}

namespace Molten::Gui
{

    class CanvasRenderer;


    template<typename TTheme>
    class Canvas
    {

    public:

        explicit Canvas(Renderer& backendRenderer, CanvasRendererPointer renderer = nullptr);
        ~Canvas();

        void SetRenderer(CanvasRendererPointer renderer);
        CanvasRendererPointer GetRenderer();
        const CanvasRendererPointer GetRenderer() const;

        void PushUserInputEvent(const UserInput::Event& inputEvent);

        void Update(const Time& deltaTime);

        void Draw();

        void SetSize(const Vector2f32& size);
        void SetScale(const Vector2f32& scale);

        const Vector2f32& GetSize() const;
        const Vector2f32& GetScale() const;

        template<template<typename> typename TWidget, typename ... TArgs>
        WidgetTypePointer<TWidget<TTheme>> CreateChild(TArgs ... args);

        template<template<typename> typename TWidget, typename ... TArgs>
        WidgetTypePointer<TWidget<TTheme>> CreateChild(Widget<TTheme>& parent, TArgs ... args);

        /** This function call makes the provided widget to receive all mouse events, 
          * regardless of child widgets on top being hovered or pressed, until the mouse button is released.
          */
        void OverrideMouseEventsUntilMouseRelease(Widget<TTheme>& widget);
        void OverrideMouseEventsReset();

        /** This function makes it possible to draw on top of all other drawn widgets in the canvas.
          * Draw commands are executed in Draw() after the regular widget tree draw calls are made.
          * Pushed commands are removed from the draw command queue after drawn.
          */
        void PushTopDrawCommand(std::function<void()> && drawCommand);

    private:

        Canvas(Canvas&&) = delete;
        Canvas(const Canvas&) = delete;
        Canvas& operator= (Canvas&&) = delete;
        Canvas& operator= (const Canvas&) = delete;

        void UpdateUserInputs(); 
        
        void UpdateTreeMouseInputs(const UserInput::Event& mouseEvent);
        void HandleTreeMouseMove(const UserInput::Event& mouseEvent);
        void HandleTreeMouseButtonPressed(const UserInput::Event& mouseEvent);
        void HandleTreeMouseButtonReleased(const UserInput::Event& mouseEvent);
        
        void UpdateModalMouseInputs(const UserInput::Event& mouseEvent);
        void HandleModalMouseMove(const UserInput::Event& mouseEvent);
        void HandleModalMouseButtonPressed(const UserInput::Event& mouseEvent);
        void HandleModalMouseButtonReleased(const UserInput::Event& mouseEvent);

        void HandleMouseMoveTriggers(WidgetData<TTheme>& widgetData, const Vector2i32& position);
        void TriggerMouseMoveEvent(WidgetData<TTheme>& widgetData, const Vector2f32& position, const WidgetEventSubType subType);

        void UpdateWidgetSkins();

        Renderer& m_backendRenderer;
        CanvasRendererPointer m_renderer;

        TTheme m_skin;
        typename WidgetData<TTheme>::Tree m_widgetTree;    
     
        Vector2f32 m_size;
        Vector2f32 m_scale;

        std::vector<UserInput::Event> m_userInputEvents;
        WidgetPointer<TTheme> m_hoveredWidget;
        Widget<TTheme>* m_pressedWidget;
        Widget<TTheme>* m_widgetOverrideMouseEvents;
        void(Canvas<TTheme>::* m_mouseInputUpdate)(const UserInput::Event&);
        std::vector<std::function<void()>> m_topDrawCommands;
    };

}

#include "Molten/Gui/Canvas.inl"

#endif