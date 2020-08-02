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

#ifndef MOLTEN_CORE_GUI_CANVAS_HPP
#define MOLTEN_CORE_GUI_CANVAS_HPP

#include "Molten/Gui/Context.hpp"
#include "Molten/Gui/TemplatedWidget.hpp"
#include "Molten/Gui/WidgetTemplate.hpp"
#include "Molten/Gui/Behaviors/BaseWidget.hpp"
#include "Molten/Gui/Systems/MouseWidgetSystem.hpp"
#include "Molten/Gui/Systems/KeyboardWidgetSystem.hpp"
#include <memory>
#include <vector>

namespace Molten
{
    class Renderer;
    class Logger;
}

namespace Molten::Gui
{

    class Renderer;

    class MOLTEN_API Canvas
    {

    public:

        Canvas();
        ~Canvas();

        bool Load(Molten::Renderer* backendRenderer, Logger* logger = nullptr);

        void Unload();

        void Update();

        void Draw();

        template<typename System>
        void RegisterSystem(System& system);

        template<typename TemplateType, typename ... Behaviors, typename ... ConstructorArgs>
        TemplatedWidgetPointer<TemplateType> Add(WidgetPointer parent, ConstructorArgs ... args);

        bool Move(WidgetPointer widget, WidgetPointer parent);

        WidgetPointer GetRoot();

    private:

        Canvas(Canvas&&) = delete;
        Canvas(const Canvas&) = delete;
        Canvas& operator= (Canvas&&) = delete;
        Canvas& operator= (const Canvas&) = delete;

        void TraversalWidgetSizeUpdate(WidgetPointer startWidget);

        void DrawChild(Widget& widget);

        Renderer* m_renderer;
        Logger* m_logger;
        std::unique_ptr<Private::Context> m_context;
        std::unique_ptr<KeyboardSystem> m_keyboardSystem;
        std::unique_ptr<MouseSystem> m_mouseSystem;
        WidgetPointer m_rootWidget;
    };

}

#include "Molten/Gui/Canvas.inl"

#endif