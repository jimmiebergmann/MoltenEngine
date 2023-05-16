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

#ifndef MOLTEN_GRAPHICS_GUI2_WIDGETEVENT_HPP
#define MOLTEN_GRAPHICS_GUI2_WIDGETEVENT_HPP

#include "Molten/Graphics/Gui2/Gui2Namespace.hpp"
#include "Molten/System/UserInput.hpp"

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    enum class WidgetMouseEventType : uint8_t
    {
        MouseEnter,             ///< Mouse cursor entered widget and is hovering widget.
        MouseLeave,             ///< Mouse cursor leaved the widget and is not longer hovering widget. This event can only occur after a MouseEnter event.
        MouseMove,              ///< Mouse moved over widget. This event can only occur after a MouseEnter event.
        MouseButtonPressed,     ///< Mouse button was pressed while hovering widget. This event can only occur after a MouseEnter event.
        MouseButtonReleasedIn,  ///< Mouse button was released while hovering widget. This event can only occur after a MousePressed event.
        MouseButtonReleasedOut  ///< Mouse button was released while not hovering widget. This event can only occur after a MousePressed event.
    };


    struct WidgetMouseEvent
    {
        WidgetMouseEventType type = WidgetMouseEventType::MouseEnter;
        Vector2i32 position = {};
        Mouse::Button button = Mouse::Button::Left;
    };


    class MOLTEN_GRAPHICS_API WidgetMouseEventHandler
    {

    public:

        WidgetMouseEventHandler() = default;
        virtual ~WidgetMouseEventHandler() = default;

        WidgetMouseEventHandler(const WidgetMouseEventHandler&) = default;
        WidgetMouseEventHandler(WidgetMouseEventHandler&&) = default;
        WidgetMouseEventHandler& operator=(const WidgetMouseEventHandler&) = default;
        WidgetMouseEventHandler& operator=(WidgetMouseEventHandler&&) = default;

        virtual bool OnMouseEvent(const WidgetMouseEvent& mouseEvent) = 0;

    };
   
}

#endif