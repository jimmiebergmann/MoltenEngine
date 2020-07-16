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

#ifndef CURSE_CORE_GUI_SYSTEMS_KEYBOARDWIDGETSYSTEM_HPP
#define CURSE_CORE_GUI_SYSTEMS_KEYBOARDWIDGETSYSTEM_HPP

#include "Curse/Gui/WidgetSystem.hpp"
#include "Curse/Gui/Widget.hpp"
#include "Curse/Gui/Behaviors/KeyboardListenerWidget.hpp"
#include "Curse/Logger.hpp"

namespace Curse
{

    namespace Gui
    {

        class CURSE_API KeyboardSystem : public WidgetSystem<KeyboardSystem, KeyboardListener>
        {

        public:

            KeyboardSystem(Logger& logger) :
                m_logger(logger)
            { }

            void OnRegister() override
            {
                m_logger.Write(Logger::Severity::Info, "Creating mouse keyboard, number of widgets: " + std::to_string(GetEntityCount()));
            }

            void OnCreateEntity(WidgetEntity widget) override
            {
                m_logger.Write(Logger::Severity::Info, "Added widget to keyboard system, number of widgets: " + std::to_string(GetEntityCount()));
            }
    
            void OnDestroyEntity(WidgetEntity widget) override
            {
            }

            void Process(const Time& deltaTime) override
            {
               /* for (size_t i = 0; i < GetEntityCount(); i++)
               {
                
               }*/
            }

            Logger& m_logger;

        };

    }

}

#endif