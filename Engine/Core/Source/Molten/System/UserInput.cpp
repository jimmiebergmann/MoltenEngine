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


#include "Molten/System/UserInput.hpp"
#include "Molten/Platform/Win32Headers.hpp"

namespace Molten
{

    // Mouse implementations.
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
    
    bool Mouse::IsDown(const Button button)
    {
        return ((::GetKeyState(UserInput::ConvertToWin32Button(button)) & 0x100) != 0);
    }
#elif MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX

    bool Mouse::IsDown(const Button)
    {
        return false;
    }
        
#endif

    // Keyboard implementations.
    /* bool Keyboard::IsDown(const Key key)
    {
        return false;
    }*/

    // Event of user input implementations.
    UserInput::Event::Event() :
        type(EventType::None),
        subType(EventSubType::None)
    {}

    UserInput::Event::Event(const EventType type, const EventSubType subType) :
        type(type),
        subType(subType)
    {}

    // User input implementations.
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
    int UserInput::ConvertToWin32Button(const Mouse::Button button)
    {
        switch (button)
        {
            case Mouse::Button::Left: return VK_LBUTTON;
            case Mouse::Button::Middle: return VK_MBUTTON;
            case Mouse::Button::Right: return VK_RBUTTON;
            case Mouse::Button::Backward: return VK_XBUTTON1;
            case Mouse::Button::Forward: return VK_XBUTTON2;
        }
        return 0;
    }

    bool UserInput::ConvertFromWin32Key(const uint32_t input, Keyboard::Key& output)
    {
        switch (input)
        {
            case '0': output = Keyboard::Key::Num0; break;
            case '1': output = Keyboard::Key::Num1; break;
            case '2': output = Keyboard::Key::Num2; break;
            case '3': output = Keyboard::Key::Num3; break;
            case '4': output = Keyboard::Key::Num4; break;
            case '5': output = Keyboard::Key::Num5; break;
            case '6': output = Keyboard::Key::Num6; break;
            case '7': output = Keyboard::Key::Num7; break;
            case '8': output = Keyboard::Key::Num8; break;
            case '9': output = Keyboard::Key::Num9; break;
            case 'A': output = Keyboard::Key::A; break;
            case 'B': output = Keyboard::Key::B; break;
            case 'C': output = Keyboard::Key::C; break;
            case 'D': output = Keyboard::Key::D; break;
            case 'E': output = Keyboard::Key::E; break;
            case 'F': output = Keyboard::Key::F; break;
            case 'G': output = Keyboard::Key::G; break;
            case 'H': output = Keyboard::Key::H; break;
            case 'I': output = Keyboard::Key::I; break;
            case 'J': output = Keyboard::Key::J; break;
            case 'K': output = Keyboard::Key::K; break;
            case 'L': output = Keyboard::Key::L; break;
            case 'M': output = Keyboard::Key::M; break;
            case 'N': output = Keyboard::Key::N; break;
            case 'O': output = Keyboard::Key::O; break;
            case 'P': output = Keyboard::Key::P; break;
            case 'Q': output = Keyboard::Key::Q; break;
            case 'R': output = Keyboard::Key::R; break;
            case 'S': output = Keyboard::Key::S; break;
            case 'T': output = Keyboard::Key::T; break;
            case 'U': output = Keyboard::Key::U; break;
            case 'V': output = Keyboard::Key::V; break;
            case 'W': output = Keyboard::Key::W; break;
            case 'X': output = Keyboard::Key::X; break;
            case 'Y': output = Keyboard::Key::Y; break;
            case 'Z': output = Keyboard::Key::Z; break;
            case VK_SPACE: output = Keyboard::Key::Space; break;

            // Function keys.
            case VK_F1: output = Keyboard::Key::F1; break;
            case VK_F2: output = Keyboard::Key::F2; break;
            case VK_F3: output = Keyboard::Key::F3; break;
            case VK_F4: output = Keyboard::Key::F4; break;
            case VK_F5: output = Keyboard::Key::F5; break;
            case VK_F6: output = Keyboard::Key::F6; break;
            case VK_F7: output = Keyboard::Key::F7; break;
            case VK_F8: output = Keyboard::Key::F8; break;
            case VK_F9: output = Keyboard::Key::F9; break;
            case VK_F10: output = Keyboard::Key::F10; break;
            case VK_F11: output = Keyboard::Key::F11; break;
            case VK_F12: output = Keyboard::Key::F12; break;

            // Navigation keys.
            case VK_DOWN: output = Keyboard::Key::Down; break;
            case VK_LEFT: output = Keyboard::Key::Left; break;
            case VK_RIGHT: output = Keyboard::Key::Right; break;
            case VK_UP: output = Keyboard::Key::Up; break;
            case VK_HOME: output = Keyboard::Key::Home; break;
            case VK_END: output = Keyboard::Key::End; break;
            case VK_NEXT: output = Keyboard::Key::PageDown; break;
            case VK_PRIOR: output = Keyboard::Key::PageUp; break;
            case VK_TAB: output = Keyboard::Key::Tab; break;

            // Enter and editing keys.
            case VK_BACK: output = Keyboard::Key::Backspace; break;
            case VK_DELETE: output = Keyboard::Key::Delete; break;
            case VK_RETURN: output = Keyboard::Key::EnterLeft; break;
            //case VK_RETURN: output = Keyboard::Key::EnterRight; break;
            case VK_INSERT: output = Keyboard::Key::Insert; break;

            // Modifier keys.
            case VK_MENU: output = Keyboard::Key::Alt; break;
            //case VK_MENU: output = Keyboard::Key::AltGr; break;
            case VK_CONTROL: output = Keyboard::Key::ControlLeft; break;
            //case VK_CONTROL: output = Keyboard::Key::ControlRight; break;
            case VK_SHIFT: output = Keyboard::Key::ShiftLeft; break;
            //case VK_SHIFT: output = Keyboard::Key::ShiftRight; break;

            // Lock keys.
            case VK_CAPITAL: output = Keyboard::Key::CapsLock; break;
            case VK_NUMLOCK: output = Keyboard::Key::NumLock; break;

            // System keys.
            case VK_ESCAPE: output = Keyboard::Key::Escape; break;
            case VK_PRINT: output = Keyboard::Key::PrintScreen; break;
            case VK_PAUSE: output = Keyboard::Key::Pause; break;
            case VK_LWIN: output = Keyboard::Key::SuperLeft; break;
            case VK_RWIN: output = Keyboard::Key::SuperRight; break;
            default: return false;
        }

        return true;
    }
#endif

    UserInput::UserInput()
    { }

    void UserInput::Begin()
    {
        m_prevPressedKeys = m_pressedKeys;
        m_prevPressedMouseButtons = m_pressedMouseButtons;

        m_events.clear();
    }

    void UserInput::End()
    {
        if (m_prevPressedKeys.size())
        {
            Event event(EventType::Keyboard, EventSubType::KeyDown);
            for (auto& key : m_prevPressedKeys)
            {
                event.keyboardEvent.key = key;
                m_events.push_back(event);
            }
        }
        if (m_prevPressedMouseButtons.size())
        {
            Event event(EventType::Mouse, EventSubType::MouseButtonDown);
            event.mouseButtonEvent.position = m_mousePosition;
            for (auto& button : m_prevPressedMouseButtons)
            {
                event.mouseButtonEvent.button = button;
                m_events.push_back(event);
            }
        }
    }

    bool UserInput::PollEvent(Event& event)
    {
        if (!m_events.size())
        {
            return false;
        }

        event = m_events.front();
        m_events.pop_front();
        return true;
    }

    void UserInput::PressKey(const Keyboard::Key key)
    {
        auto it = m_pressedKeys.find(key);
        if (it != m_pressedKeys.end())
        {
            return;
        }

        m_pressedKeys.insert(key);

        Event event(EventType::Keyboard, EventSubType::KeyPressed);
        event.keyboardEvent.key = key;

        m_events.push_back(event);
        event.subType = EventSubType::KeyDown;
        m_events.push_back(event);
    }

    void UserInput::PressMouseButton(const Mouse::Button button, const Vector2i32& position)
    {
        m_mousePosition = position;

        auto it = m_pressedMouseButtons.find(button);
        if (it != m_pressedMouseButtons.end())
        {
            return;
        }

        m_pressedMouseButtons.insert(button);

        Event event(EventType::Mouse, EventSubType::MouseButtonPressed);
        event.mouseButtonEvent.button = button;
        event.mouseButtonEvent.position = position;
 
        m_events.push_back(event);
        event.subType = EventSubType::MouseButtonDown;
        m_events.push_back(event);  
    }

    void UserInput::ReleaseKey(const Keyboard::Key key)
    {
        m_pressedKeys.erase(key);
        m_prevPressedKeys.erase(key);

        Event event(EventType::Keyboard, EventSubType::KeyReleased);
        event.keyboardEvent.key = key;
        m_events.push_back(event);
    }

    void UserInput::ReleaseMouseButton(const Mouse::Button button, const Vector2i32& position)
    {
        m_mousePosition = position;

        m_pressedMouseButtons.erase(button);
        m_prevPressedMouseButtons.erase(button);

        Event event(EventType::Mouse, EventSubType::MouseButtonReleased);
        event.mouseButtonEvent.button = button;
        event.mouseButtonEvent.position = position;
        m_events.push_back(event);      
    }

    void UserInput::MoveMouse(const Vector2i32& position)
    {
        m_mousePosition = position;

        Event event(EventType::Mouse, EventSubType::MouseMove);
        event.mouseMoveEvent.position = position;
        m_events.push_back(event);
    }

    size_t UserInput::GetEventCount() const
    {
        return m_events.size();
    }

    Vector2i32 UserInput::GetMousePosition() const
    {
        return m_mousePosition;
    }  

}


