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

namespace Molten
{

    template<typename T, typename TError>
    inline Result<T, TError> Result<T, TError>::CreateSuccess(T&& type)
    {
        return Result{ VariantType(std::in_place_index<0>, std::move(type)) };
    }

    template<typename T, typename TError>
    inline Result<T, TError> Result<T, TError>::CreateError(TError&& error)
    {
        return Result{ VariantType(std::in_place_index<1>, std::move(error)) };
    }

    template<typename T, typename TError>
    inline Result<T, TError>::Result(Result&& result) noexcept :
        m_data{std::move(result.m_data)}
    {}

    template<typename T, typename TError>
    inline Result<T, TError>& Result<T, TError>::operator=(Result&& result) noexcept
    {
        m_data = std::move(result.m_data);
        return *this;
    }
 
    template<typename T, typename TError>
    inline Result<T, TError>::operator bool() const
    {
        return IsValid();
    }
    template<typename T, typename TError>
    inline bool Result<T, TError>::IsValid() const
    {
        return m_data.index() == 0;
    }

    template<typename T, typename TError>
    inline TError& Result<T, TError>::Error()
    {
        return std::get<1>(m_data);
    }
    template<typename T, typename TError>
    inline const TError& Result<T, TError>::Error() const
    {
        return std::get<1>(m_data);
    }

    template<typename T, typename TError>
    inline T& Result<T, TError>::Value()
    {
        return std::get<0>(m_data);
    }
    template<typename T, typename TError>
    inline const T& Result<T, TError>::Value() const
    {
        return std::get<0>(m_data);
    }

    template<typename T, typename TError>
    inline T& Result<T, TError>::operator *()
    {
        return std::get<0>(m_data);
    }
    template<typename T, typename TError>
    inline const T& Result<T, TError>::operator *() const
    {
        return std::get<0>(m_data);
    }

    template<typename T, typename TError>
    inline T* Result<T, TError>::operator ->()
    {
        return &std::get<0>(m_data);
    }
    template<typename T, typename TError>
    inline const T* Result<T, TError>::operator ->() const
    {
        return &std::get<0>(m_data);
    }

    template<typename T, typename TError>
    inline Result<T, TError>::Result(VariantType&& variant) :
        m_data(std::move(variant))
    {}

}
