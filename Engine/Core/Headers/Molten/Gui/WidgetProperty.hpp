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

#ifndef MOLTEN_CORE_GUI_WIDGETPROPERTY_HPP
#define MOLTEN_CORE_GUI_WIDGETPROPERTY_HPP

#include "Molten/System/Signal.hpp"

namespace Molten::Gui
{

    template<typename T>
    class WidgetProperty
    {

    public:

        DispatchSignal<> onChange;

        explicit WidgetProperty(SignalDispatcher& signalDispatcher);
        WidgetProperty(
            SignalDispatcher& signalDispatcher,
            const T& value);
        WidgetProperty(
            SignalDispatcher& signalDispatcher,
            T&& value);

        ~WidgetProperty() = default;

        /** Deleted copy and move constructors/operators. */
        /**@{*/
        WidgetProperty(const WidgetProperty&) = delete;
        WidgetProperty(WidgetProperty&&) = delete;
        WidgetProperty& operator = (const WidgetProperty&) = delete;
        WidgetProperty& operator = (WidgetProperty&&) = delete;
        /**@}*/

        /** Set Property value. */
        /**@{*/
        void Set(const T& value);
        void Set(T&& value);
        WidgetProperty& operator = (const T& value);
        WidgetProperty& operator = (T&& value) noexcept;
        /**@}*/

        /** Get Property value. */
        /**@{*/
        const T& Get() const;
        const T& operator ()() const;
        /**@}*/

    private:

        T m_value;

    };


}

#include "Molten/Gui/WidgetProperty.inl"

#endif