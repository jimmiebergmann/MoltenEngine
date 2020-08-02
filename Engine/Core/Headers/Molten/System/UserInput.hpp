/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_SYSTEM_USERINPUT_HPP
#define MOLTEN_CORE_SYSTEM_USERINPUT_HPP

#include "Molten/Math/Vector.hpp"
#include <list>
#include <set>

namespace Molten
{

    /**
    * @brief Generic mouse class.
    */
    class MOLTEN_API Mouse
    {

    public:

        enum class Button : uint8_t
        {
            Left,
            Middle,
            Right,
            Forward,
            Backward
        };

        /**
        * @brief Checks if mouse button is currently pressed down.
        */
        static bool IsDown(const Button button);

    };


    /**
    * @brief Generic keyboard class.
    */
    class MOLTEN_API Keyboard
    {

    public:

        enum class Key : uint16_t
        {          
            // Characters keys.
            Num0,
            Num1,
            Num2,
            Num3,
            Num4,
            Num5,
            Num6,
            Num7,
            Num8,
            Num9,
            A,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,
            Space,

            // Function keys.
            F1,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,

            // Navigation keys.
            Down,
            Left,
            Right,
            Up,
            Home,
            End,
            PageDown,
            PageUp,
            Tab,

            // Enter and editing keys.
            Backspace,
            Delete,
            EnterLeft,
            EnterRight,
            Insert,

            // Modifier keys.
            Alt,
            AltGr,
            ControlLeft,
            ControlRight,
            ShiftLeft,
            ShiftRight,

            // Lock keys.
            CapsLock,
            NumLock,

            // System keys.
            Escape,
            PrintScreen,
            Pause,
            SuperLeft,
            SuperRight
        };

        /**
        * @brief Checks if mouse button is currently pressed down.
        */
        static bool IsDown(const Key key);

    };


    /**
    * @brief User input class.
    *        A generic system to handle user input events.
    *        The *-Pressed events are automatically pushed and
    *        *-Down events are supposed to be pushed every update if the key or button is being held down.
    */
    class MOLTEN_API UserInput
    {

    public:

        /**
        * @brief Mouse event, describing the following event types:
        *   - MouseButtonPressed
        *   - MouseButtonDown
        *   - MouseButtonReleased
        */
        struct MOLTEN_API MouseButtonEvent
        {
            Mouse::Button button;
            Vector2i32 position;
        };

        /**
        * @brief Mouse event, describing the following event types:
        *   - MouseMove
        */
        struct MOLTEN_API MouseMoveEvent
        {
            Vector2i32 position;
        };

        /**
        * @brief Keyboard event, describing the following event types:
        *   - KeyPressed
        *   - KeyDown
        *   - KeyReleased
        */
        struct MOLTEN_API KeyboardEvent
        {
            Keyboard::Key key;
        };


        /**
        * @brief Event object, containing information about 
        */
        class MOLTEN_API Event
        {

        public:

            /**
            * @brief User input interface enumerator.
            */
            enum class Type : uint8_t
            {
                None,                   ///< Initial value of event, ignored by user input container.

                // Mouse events.
                MouseButtonPressed,     ///< Mouse button was just pressed.
                MouseButtonDown,        ///< Mouse button is down.
                MouseButtonReleased,    ///< Mouse button was just released.
                MouseMove,              ///< Mouse cursor moved.

                // Keyboard events.
                KeyPressed,             ///< Key was just pressed.
                KeyDown,                ///< Key is down.
                KeyReleased             ///< Key was just released.
            };

            /**
            * @brief Constructor.
            */
            Event();

            /**
            * @brief Type of event.
            */
            Type type;

            /**
            * @brief Union of different events.
            *        Check "type" to select the correct union member.
            */
            union
            {
                MouseButtonEvent mouseButtonEvent;
                MouseMoveEvent mouseMoveEvent;
                KeyboardEvent keyboardEvent;
            };            

        };
    
    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
        static int ConvertToWin32Button(const Mouse::Button button);
        static bool ConvertFromWin32Key(const uint32_t input, Keyboard::Key& output);
    #endif

        /**
        * @brief Constructor.
        */
        UserInput();

        /**
        * @brief Prepare and begin to record input events.
        */
        void Begin();

        /**
        * @brief End user input recording.
        *        Pushing repeated *-Down events.
        */
        void End();

        /**
        * @brief Poll event from queue.
        *
        * @true if new event was polled, else false if no events are queued.
        */
        bool PollEvent(Event& event);

        /**
        * @brief Register keyboard press.
        */
        void PressKey(const Keyboard::Key key);

        /**
        * @brief Register mouse button press.
        */
        void PressMouseButton(const Mouse::Button button, const Vector2i32& position);

        /**
        * @brief Register keyboard release.
        */
        void ReleaseKey(const Keyboard::Key key);

        /**
        * @brief Register mouse button release.
        */
        void ReleaseMouseButton(const Mouse::Button button, const Vector2i32& position);

        /**
        * @brief Register mouse movement.
        */
        void MoveMouse(const Vector2i32& position);

        /**
        * @brief Get number of queued events.
        */
        size_t GetEventCount() const;

        /**
        * @brief Get last mouse recorded mouse position.
        */
        Vector2i32 GetMousePosition() const;

    private:

        std::list<Event> m_events;
        Vector2i32 m_mousePosition;
        std::set<Keyboard::Key> m_pressedKeys;
        std::set<Mouse::Button> m_pressedMouseButtons;
        std::set<Keyboard::Key> m_prevPressedKeys;
        std::set<Mouse::Button> m_prevPressedMouseButtons;

    };

}

#endif
