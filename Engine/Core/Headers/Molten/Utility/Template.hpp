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

#ifndef MOLTEN_CORE_UTILITY_TEMPLATE_HPP
#define MOLTEN_CORE_UTILITY_TEMPLATE_HPP

#include <type_traits>
#include <tuple>
#include <variant>

namespace Molten
{

    /**
    * @brief Loop each template type in a parameter pack.
    * Privide the types and a callback function, being called for each type.
    * 
    * Example:
    * ForEachTemplateType<int, double>([](auto type)
    * {
    *   using Param = decltype(type);
    *   using Type = typename Param::Type;
    *  
    *   size_t index = Param::index;
    * }
    */
    template<typename ... TTypes, typename TCallback>
    constexpr void ForEachTemplateType(TCallback&& callback);

    template<typename T, T ... TTypes, typename TCallback>
    constexpr void ForEachTemplateValue(TCallback&& callback);

    /**
    * @brief Checks if a set of template argument types contains Type.
    * @return True if Type is contained in Types, else false.
    */
    template<typename TType, typename ... TTypes>
    constexpr bool TemplateArgumentsContains();


    /** Gets data type of variadic template parameter at index. */
    template<size_t VIndex, typename ... TTypes>
    struct GetTemplateArgumentAt
    {

        static constexpr size_t GetIndex();

        using Type = std::tuple_element_t<GetIndex(), std::tuple<TTypes...>>;
        
    };

    template<size_t VIndex, typename ... TTypes>
    using GetTemplateArgumentTypeAt = typename GetTemplateArgumentAt <VIndex, TTypes...>::Type;


    /** Checks if currently held value of variant is of type T. */
    template<typename T, typename ... TVariantTypes>
    [[nodiscard]] constexpr bool VariantEqualsType(const std::variant<TVariantTypes...>& variant);

    /** Checks if currently held value of variant is of same value and type T. */
    template<typename T, typename ... TVariantTypes>
    [[nodiscard]] constexpr bool VariantEqualsValue(const std::variant<TVariantTypes...>& variant, const T& value);


    /** Checks if first type is an instation of a template class(second parameter). */
    /**@{*/
    template <typename, template <typename, typename...> typename>
    struct IsTemplateInstance : public std::false_type {};

    template <typename...Ts, template <typename, typename...> typename U>
    struct IsTemplateInstance<U<Ts...>, U> : public std::true_type {};
    /**@}*/


    /** Always false value, used for static_assert. */
    template<typename T>
    constexpr bool AlwaysFalse = false;
}

#include "Molten/Utility/Template.inl"

#endif