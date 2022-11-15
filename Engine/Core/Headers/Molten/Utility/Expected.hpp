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

#ifndef MOLTEN_CORE_UTILITY_EXPECTED_HPP
#define MOLTEN_CORE_UTILITY_EXPECTED_HPP

#include "Molten/Core.hpp"
#include "Molten/Utility/Template.hpp"
#include <type_traits>
#include <variant>
#include <optional>
#include <utility>

namespace Molten
{

    template<typename TError> class Unexpected;
   

    template<typename TValue, typename TError>
    class Expected
    {

    public:

        using ValueType = TValue;
        using ErrorType = TError;

        /** Expected(successful) value construction and assignment. */
        template<typename T = TValue>
        Expected(const T& value) :
            m_variant(std::in_place_index<1>, value)
        {}

        template<typename T = TValue>
        Expected& operator = (const T& value)
        {
            m_variant = { std::in_place_index<1>, value };
            return *this;
        }

        template<typename T = TValue>
        Expected(T&& value) :
            m_variant(std::in_place_index<1>, std::move(value))
        {}

        template<typename T = TValue>
        Expected& operator = (T&& value)
        {
            m_variant = { std::in_place_index<1>, std::move(value) };
            return *this;
        }

        /** Expected(failure) value construction and assignment from unexpected. */
        template<typename T = TError>
        Expected(const Unexpected<T>& unexpected) :
            m_variant(std::in_place_index<2>, unexpected.m_error)
        {}

        template<typename T = TError>
        Expected& operator = (const Unexpected<T>& unexpected)
        {
            m_variant = { std::in_place_index<2>, unexpected.m_error };
            return *this;
        }

        template<typename T = TError>
        Expected(Unexpected<T>&& unexpected) :
            m_variant(std::in_place_index<2>, std::move(unexpected.m_error))
        {}

        template<typename T = TError>
        Expected& operator = (Unexpected<T>&& unexpected)
        {
            m_variant = { std::in_place_index<2>, std::move(unexpected.m_error) };
            return *this;
        }

        /* Copy and move construction and assignment.*/
        Expected(const Expected<ValueType, ErrorType>&) = default;
        Expected& operator = (const Expected<ValueType, ErrorType>&) = default;
        Expected(Expected<ValueType, ErrorType>&&) noexcept = default;
        Expected& operator = (Expected<ValueType, ErrorType>&&) noexcept = default;

        ~Expected() = default;

        [[nodiscard]] operator bool() const {
            return HasValue();
        }
        [[nodiscard]] bool HasValue() const {
            return m_variant.index() == 1;
        }

        [[nodiscard]] ValueType& Value()
        {
            return std::get<1>(m_variant);
        }
        [[nodiscard]] const ValueType& Value() const
        {
            return std::get<1>(m_variant);
        }

        [[nodiscard]] ErrorType& Error()
        {
            return std::get<2>(m_variant);
        }
        [[nodiscard]] const ErrorType& Error() const
        {
            return std::get<2>(m_variant);
        }

    private:

        using VariantType = std::variant<std::monostate, ValueType, ErrorType>;
        VariantType m_variant;

    };

    template<typename TValue, typename TError>
        requires std::is_void_v<TValue>
    class Expected<TValue, TError>
    {

    public:

        using ValueType = TValue;
        using ErrorType = TError;

        /** Expected(successful) value construction and assignment. */
        Expected() = default;

        /** Expected(failure) value construction and assignment from unexpected. */
        template<typename T = TError>
        Expected(const Unexpected<T>& unexpected) :
            m_error(unexpected.m_error)
        {}

        template<typename T = TError>
        Expected& operator = (const Unexpected<T>& unexpected)
        {
            m_error = unexpected.m_error;
            return *this;
        }

        template<typename T = TError>
        Expected(Unexpected<T>&& unexpected) :
            m_error(std::move(unexpected.m_error))
        {}

        template<typename T = TError>
        Expected& operator = (Unexpected<T>&& unexpected)
        {
            m_error = std::move(unexpected.m_error);
            return *this;
        }

        ~Expected() = default;

        [[nodiscard]] operator bool() const { return HasValue(); }
        [[nodiscard]] bool HasValue() const { return !m_error.has_value(); }

        [[nodiscard]] ErrorType& Error() { return m_error.value(); }
        [[nodiscard]] const ErrorType& Error() const { return m_error.value(); }

    private:

        std::optional<ErrorType> m_error;

    };



    template<typename TError>
    class Unexpected
    {

    public:

        using ErrorType = TError;

        /** Unexpected(failure) value construction and assignment. */
        template<typename T = TError>
        Unexpected(const T& error) :
            m_error(error)
        {}

        template<typename T = TError>
        Unexpected& operator = (const T& error)
        {
            m_error = error;
            return *this;
        }

        template<typename T = TError>
        Unexpected(T&& error) :
            m_error(std::move(error))
        {}

        template<typename T = TError>
        Unexpected& operator = (T&& error)
        {
            m_error = std::move(error);
            return *this;
        }

        /* Copy and move construction and assignment.*/
        Unexpected(const Unexpected&) = default;
        Unexpected& operator = (const Unexpected&) = default;
        Unexpected(Unexpected&&) noexcept = default;
        Unexpected& operator = (Unexpected&&) noexcept = default;

        [[nodiscard]] ErrorType& Error()
        {
            return m_error;
        }
        [[nodiscard]] const ErrorType& Error() const
        {
            return m_error;
        }

    private:

        template<typename, typename>
        friend class Expected;

        TError m_error;

    };

    template <class TError>
    Unexpected(TError) -> Unexpected<TError>;

}

//#include "Molten/Utility/Expected.inl"

#endif