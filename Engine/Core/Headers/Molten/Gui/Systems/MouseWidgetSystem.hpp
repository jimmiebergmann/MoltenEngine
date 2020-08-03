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

#ifndef MOLTEN_CORE_GUI_SYSTEMS_MOUSEWIDGETSYSTEM_HPP
#define MOLTEN_CORE_GUI_SYSTEMS_MOUSEWIDGETSYSTEM_HPP

#include "Molten/Gui/WidgetSystem.hpp"
#include "Molten/Gui/Widget.hpp"
#include "Molten/Gui/Behaviors/MouseListenerWidget.hpp"
#include "Molten/Logger.hpp"

namespace Molten::Gui
{

    class MOLTEN_API MouseSystem : public WidgetSystem<MouseSystem, MouseListener>
    {

    public:

        explicit MouseSystem(Logger& logger) :
            m_logger(logger)
        { }

        void OnRegister() override
        {
            m_logger.Write(Logger::Severity::Info, "Creating mouse system, number of widgets: " + std::to_string(GetEntityCount()));

        }

        void OnCreateEntity(WidgetEntity entity) override
        {
            m_logger.Write(Logger::Severity::Info, "Added widget to mouse system, number of widgets: " + std::to_string(GetEntityCount()));
        }

        void OnDestroyEntity(WidgetEntity entity) override
        {
        }

        void Process(const Time& deltaTime) override
        {
        }

        Logger& m_logger;

    };

}

#endif