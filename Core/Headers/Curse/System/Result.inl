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

namespace Curse
{

    template<typename T, typename E>
    inline Result<T, E>::Result() :
        m_data{ std::in_place_index<1> }
    { }

    template<typename T, typename E>
    inline Result<T, E>::Result(const Result& result) :
        m_data{result.m_data}
    { }

    template<typename T, typename E>
    inline Result<T, E>::Result(Result&& result) noexcept :
        m_data{std::move(result.m_data)}
    { }
 
    template<typename T, typename E>
    inline Result<T, E>::operator bool() const
    {
        return m_data.index() == 0;
    }

    template<typename T, typename E>
    inline Result<T, E>& Result<T, E>::operator=(const Result& result)
    {
        m_data = result.m_data;
        return *this;
    }

    template<typename T, typename E>
    inline Result<T, E>& Result<T, E>::operator=(Result&& result) noexcept
    {
        if (*this == result)
        {
            return *this;
        }
        m_data = std::move(result.m_data);
        return *this;
    }

    template<typename T, typename E>
    inline E& Result<T, E>::Error()
    {
        return std::get<1>(m_data);
    }
    template<typename T, typename E>
    inline const E& Result<T, E>::Error() const
    {
        return std::get<1>(m_data);
    }

    template<typename T, typename E>
    inline T& Result<T, E>::Value()
    {
        return std::get<0>(m_data);
    }
    template<typename T, typename E>
    inline const T& Result<T, E>::Value() const
    {
        return std::get<0>(m_data);
    }

    template<typename T, typename E>
    inline Result<T, E> Result<T, E>::Value(T&& type)
    {
        return VariantType(std::in_place_index<0>, type);
    }

    template<typename T, typename E>
    inline Result<T, E> Result<T, E>::Error(E&& error)
    {
        return VariantType( std::in_place_index<1>, error);
    }

    template<typename T, typename E>
    inline Result<T, E>::Result(std::variant<T, E>&& variant) :
        m_data(std::move(variant))
    { }

}
