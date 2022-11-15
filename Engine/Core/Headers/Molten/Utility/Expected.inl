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

namespace Molten
{

    
    template<typename TValue, typename TError>
    template<typename T>
    Expected<TValue, TError>::Expected(const T& value) requires (!IsTemplateInstance<T, Unexpected>::value) :
        m_variant(std::in_place_index<1>, value)
    {}

    template<typename TValue, typename TError>
    template<typename T>
    Expected<TValue, TError>& Expected<TValue, TError>::operator = (const T& value) requires (!IsTemplateInstance<T, Unexpected>::value)
    {
        m_variant = { std::in_place_index<1>, value };
        return *this;
    }

    template<typename TValue, typename TError>
    template<typename T>
    Expected<TValue, TError>::Expected(T&& value) requires (!IsTemplateInstance<T, Unexpected>::value) :
        m_variant(std::in_place_index<1>, std::move(value))
    {}

    template<typename TValue, typename TError>
    template<typename T>
    Expected<TValue, TError>& Expected<TValue, TError>::operator = (T&& value) requires (!IsTemplateInstance<T, Unexpected>::value)
    {
        m_variant = { std::in_place_index<1>, std::move(value) };
        return *this;
    }

    template<typename TValue, typename TError>
    template<typename T>
    Expected<TValue, TError>::Expected(const Unexpected<T>& unexpected) :
        m_variant(std::in_place_index<2>, unexpected.m_error)
    {}

    template<typename TValue, typename TError>
    template<typename T>
    Expected<TValue, TError>& Expected<TValue, TError>::operator = (const Unexpected<T>& unexpected)
    {
        m_variant = { std::in_place_index<2>, unexpected.m_error };
        return *this;
    }

    template<typename TValue, typename TError>
    template<typename T>
    Expected<TValue, TError>::Expected(Unexpected<T>&& unexpected) :
        m_variant(std::in_place_index<2>, std::move(unexpected.m_error))
    {}

    template<typename TValue, typename TError>
    template<typename T>
    Expected<TValue, TError>& Expected<TValue, TError>::operator = (Unexpected<T>&& unexpected)
    {
        m_variant = { std::in_place_index<2>, std::move(unexpected.m_error) };
        return *this;
    }

    template<typename TValue, typename TError>
    Expected<TValue, TError>::operator bool() const {
        return HasValue();
    }
    template<typename TValue, typename TError>
    bool Expected<TValue, TError>::HasValue() const {
        return m_variant.index() == 1;
    }

    template<typename TValue, typename TError>
    Expected<TValue, TError>::ValueType& Expected<TValue, TError>::Value()
    {
        return std::get<1>(m_variant);
    }
    template<typename TValue, typename TError>
    const typename Expected<TValue, TError>::ValueType& Expected<TValue, TError>::Value() const
    {
        return std::get<1>(m_variant);
    }

    template<typename TValue, typename TError>
    typename Expected<TValue, TError>::ErrorType& Expected<TValue, TError>::Error()
    {
        return std::get<2>(m_variant);
    }
    template<typename TValue, typename TError>
    const typename Expected<TValue, TError>::ErrorType& Expected<TValue, TError>::Error() const
    {
        return std::get<2>(m_variant);
    }

    
    // Void expected implementations.
    template<typename TValue, typename TError>
        requires std::is_void_v<TValue>
    template<typename T>
    Expected<TValue, TError>::Expected(const Unexpected<T>& unexpected) :
        m_error(unexpected.m_error)
    {}

    template<typename TValue, typename TError>
        requires std::is_void_v<TValue>
    template<typename T>
    Expected<TValue, TError>& Expected<TValue, TError>::operator = (const Unexpected<T>& unexpected)
    {
        m_error = unexpected.m_error;
        return *this;
    }

    template<typename TValue, typename TError>
        requires std::is_void_v<TValue>
    template<typename T>
    Expected<TValue, TError>::Expected(Unexpected<T>&& unexpected) :
        m_error(std::move(unexpected.m_error))
    {}

    template<typename TValue, typename TError>
        requires std::is_void_v<TValue>
    template<typename T>
    Expected<TValue, TError>& Expected<TValue, TError>::operator = (Unexpected<T>&& unexpected)
    {
        m_error = std::move(unexpected.m_error);
        return *this;
    }

    template<typename TValue, typename TError>
        requires std::is_void_v<TValue>
    Expected<TValue, TError>::operator bool() const {
        return HasValue();
    }

    template<typename TValue, typename TError>
        requires std::is_void_v<TValue>
    bool Expected<TValue, TError>::HasValue() const {
        return !m_error.has_value();
    }

    template<typename TValue, typename TError>
        requires std::is_void_v<TValue>
    typename Expected<TValue, TError>::ErrorType& Expected<TValue, TError>::Error() {
        return m_error.value();
    }

    template<typename TValue, typename TError>
        requires std::is_void_v<TValue>
    const typename Expected<TValue, TError>::ErrorType& Expected<TValue, TError>::Error() const {
        return m_error.value();
    }


    // Unspected implementations.
    template<typename TError>
    template<typename T>
    Unexpected<TError>::Unexpected(const T& error) :
        m_error(error)
    {}

    template<typename TError>
    template<typename T>
    Unexpected<TError>& Unexpected<TError>::operator = (const T& error)
    {
        m_error = error;
        return *this;
    }

    template<typename TError>
    template<typename T>
    Unexpected<TError>::Unexpected(T&& error) :
        m_error(std::move(error))
    {}

    template<typename TError>
    template<typename T>
    Unexpected<TError>& Unexpected<TError>::operator = (T&& error)
    {
        m_error = std::move(error);
        return *this;
    }

    template<typename TError>
    typename Unexpected<TError>::ErrorType& Unexpected<TError>::Error()
    {
        return m_error;
    }

    template<typename TError>
    const typename Unexpected<TError>::ErrorType& Unexpected<TError>::Error() const
    {
        return m_error;
    }

}