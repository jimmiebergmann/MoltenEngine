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

#ifndef CURSE_CORE_GUI_CANVAS_HPP
#define CURSE_CORE_GUI_CANVAS_HPP

#include "Curse/Gui/Context.hpp"
#include "Curse/Gui/TemplatedWidget.hpp"
#include "Curse/Gui/WidgetTemplate.hpp"
#include "Curse/Gui/Behaviors/BaseWidget.hpp"
#include "Curse/Gui/Systems/MouseWidgetSystem.hpp"
#include "Curse/Gui/Systems/KeyboardWidgetSystem.hpp"
#include <memory>
#include <vector>

namespace Curse
{

    // Forward declarations...
    class Renderer;
    class Logger;

    namespace Gui
    {


        class CURSE_API Canvas
        {

        public:

            Canvas();

            bool Load(Renderer* renderer, Logger* logger = nullptr);

            void Unload();

            void Update();

            void Draw();

            template<typename System>
            void RegisterSystem(System& system);

            template<typename Template, typename ... Behaviors, typename ... ConstructorArgs>
            TemplatedWidgetPointer<Template> Add(WidgetPointer parent, ConstructorArgs ... args);

            bool Move(WidgetPointer widget, WidgetPointer parent);

            WidgetPointer GetRoot();

        private:

            Canvas(Canvas&&) = delete;
            Canvas(const Canvas&) = delete;
            Canvas& operator= (Canvas&&) = delete;
            Canvas& operator= (const Canvas&) = delete;

            void TraversalWidgetSizeUpdate(WidgetPointer startWidget);

            Renderer* m_renderer;
            Logger* m_logger;
            std::unique_ptr<Private::Context> m_context;
            std::unique_ptr<KeyboardSystem> m_keyboardSystem;
            std::unique_ptr<MouseSystem> m_mouseSystem;
            WidgetPointer m_rootWidget;
        };

    }

}

#include "Curse/Gui/Canvas.inl"

#endif