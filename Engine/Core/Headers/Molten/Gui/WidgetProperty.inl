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

namespace Molten::Gui
{

    template<typename T>
    WidgetProperty<T>::WidgetProperty(SignalDispatcher& signalDispatcher) :
        onChange(signalDispatcher),
        m_value{}
    {}

    template<typename T>
    WidgetProperty<T>::WidgetProperty(
        SignalDispatcher& signalDispatcher,
        const T& value
    ) :
        onChange(signalDispatcher),
        m_value{ value }
    {}

    template<typename T>
    WidgetProperty<T>::WidgetProperty(
        SignalDispatcher& signalDispatcher,
        T&& value
    ) :
        onChange(signalDispatcher),
        m_value{ std::move(value) }
    {}

    template<typename T>
    void WidgetProperty<T>::Set(const T& value)
    {
        m_value = value;
        onChange();
    }

    template<typename T>
    void WidgetProperty<T>::Set(T&& value)
    {
        m_value = std::move(value);
        onChange();
    }

    template<typename T>
    WidgetProperty<T>& WidgetProperty<T>::operator = (const T& value)
    {
        m_value = value;
        onChange();
        return *this;
    }

    template<typename T>
    WidgetProperty<T>& WidgetProperty<T>::operator = (T&& value) noexcept
    {
        m_value = std::move(value);
        onChange();
        return *this;
    }

    template<typename T>
    const T& WidgetProperty<T>::Get() const
    {
        return m_value;
    }

    template<typename T>
    const T& WidgetProperty<T>::operator ()() const
    {
        return m_value;
    }

}
