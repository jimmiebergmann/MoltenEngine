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

#include "Test.hpp"
#include "Molten/System/UserInput.hpp"

namespace Molten
{
    TEST(System, UserInput_Mouse)
    {
        {
            EXPECT_EQ(static_cast<uint8_t>(Mouse::Button::Left), uint8_t{ 0 });
            EXPECT_EQ(static_cast<uint8_t>(Mouse::Button::Middle), uint8_t{ 1 });
            EXPECT_EQ(static_cast<uint8_t>(Mouse::Button::Right), uint8_t{ 2 });
            EXPECT_EQ(static_cast<uint8_t>(Mouse::Button::Forward), uint8_t{ 3 });
            EXPECT_EQ(static_cast<uint8_t>(Mouse::Button::Backward), uint8_t{ 4 });
        }
        {
            EXPECT_NO_THROW(Mouse::IsDown(Mouse::Button::Left));
            EXPECT_NO_THROW(Mouse::IsDown(Mouse::Button::Middle));
            EXPECT_NO_THROW(Mouse::IsDown(Mouse::Button::Right));
            EXPECT_NO_THROW(Mouse::IsDown(Mouse::Button::Forward));
            EXPECT_NO_THROW(Mouse::IsDown(Mouse::Button::Backward));
        }
        { 
            UserInput userInput;
            UserInput::Event event;

            userInput.Begin();
            {
                EXPECT_EQ(userInput.GetEventCount(), size_t{ 0 });
                userInput.PressMouseButton(Mouse::Button::Left, Vector2i32(100, 200));
            }
            userInput.End();

            EXPECT_EQ(userInput.GetEventCount(), size_t{ 2 }); 
            EXPECT_TRUE(userInput.PollEvent(event));
            EXPECT_EQ(event.type, UserInput::EventType::Mouse);
            EXPECT_EQ(event.subType, UserInput::EventSubType::MouseButtonPressed);
            EXPECT_EQ(event.mouseButtonEvent.button, Mouse::Button::Left);
            EXPECT_EQ(event.mouseButtonEvent.position, Vector2i32(100, 200));

            EXPECT_TRUE(userInput.PollEvent(event));
            EXPECT_EQ(event.type, UserInput::EventType::Mouse);
            EXPECT_EQ(event.subType, UserInput::EventSubType::MouseButtonDown);
            EXPECT_EQ(event.mouseButtonEvent.button, Mouse::Button::Left);
            EXPECT_EQ(event.mouseButtonEvent.position, Vector2i32(100, 200));

            EXPECT_FALSE(userInput.PollEvent(event));
            EXPECT_EQ(userInput.GetEventCount(), size_t{ 0 });


            userInput.Begin();
            {
            }
            userInput.End();

            EXPECT_EQ(userInput.GetEventCount(), size_t{ 1 });
            {
                EXPECT_TRUE(userInput.PollEvent(event));
                EXPECT_EQ(event.type, UserInput::EventType::Mouse);
                EXPECT_EQ(event.subType, UserInput::EventSubType::MouseButtonDown);
                EXPECT_EQ(event.mouseButtonEvent.button, Mouse::Button::Left);
                EXPECT_EQ(event.mouseButtonEvent.position, Vector2i32(100, 200));

                EXPECT_FALSE(userInput.PollEvent(event));
                EXPECT_EQ(userInput.GetEventCount(), size_t{ 0 });
            }

            userInput.Begin();
            {
                userInput.ReleaseMouseButton(Mouse::Button::Left, Vector2i32(100, 200));
            }
            userInput.End();

            EXPECT_EQ(userInput.GetEventCount(), size_t{ 1 });
            {
                EXPECT_TRUE(userInput.PollEvent(event));
                EXPECT_EQ(event.type, UserInput::EventType::Mouse);
                EXPECT_EQ(event.subType, UserInput::EventSubType::MouseButtonReleased);
                EXPECT_EQ(event.mouseButtonEvent.button, Mouse::Button::Left);
                EXPECT_EQ(event.mouseButtonEvent.position, Vector2i32(100, 200));

                EXPECT_FALSE(userInput.PollEvent(event));
                EXPECT_EQ(userInput.GetEventCount(), size_t{ 0 });
            }

            userInput.Begin();
            {
            }
            userInput.End();
            EXPECT_EQ(userInput.GetEventCount(), size_t{ 0 });

        }
    }
}