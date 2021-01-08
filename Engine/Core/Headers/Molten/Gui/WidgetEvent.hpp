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

#ifndef MOLTEN_CORE_GUI_WIDGETEVENT_HPP
#define MOLTEN_CORE_GUI_WIDGETEVENT_HPP

#include "Molten/System/UserInput.hpp"

namespace Molten::Gui
{

    enum class WidgetEventType : uint8_t
    {
        None,
        Mouse,
        Keyboard
    };

    enum class WidgetEventSubType : uint8_t
    {
        None,
        MouseEnter,     ///< Mouse cursur entered widget and is hovering widget.
        MouseLeave,     ///< Mouse cursur leaved the widget and is not longer hovering widget.
        MouseMove,      ///< Mouse moved over widget. This event must occur after a MouseEnter event.
        MousePress,     ///< Mouse button was pressed while hovering widget.
        MouseRelease    ///< Mouse button was released while hovering widget.
    };


    /** User input event handled by widget. */
    struct WidgetEvent
    {
        struct MouseEvent
        {
            Vector2i32 position;
            Mouse::Button button;
        };

        WidgetEvent() :
            type(WidgetEventType::None),
            subType(WidgetEventSubType::None)
        {}

        WidgetEventType type;
        WidgetEventSubType subType;

        union
        {
            MouseEvent mouseEvent;
        };
    };


    /** Base class for all widget types that handles user input events. */
    class WidgetEventHandler
    {

    public:

        virtual ~WidgetEventHandler() = default;

        virtual bool HandleEvent(const WidgetEvent& widgetEvent) = 0;

    };



}

#endif