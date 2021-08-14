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

#ifndef MOLTEN_CORE_UTILITY_TEMPLATE_HPP
#define MOLTEN_CORE_UTILITY_TEMPLATE_HPP

#include <tuple>

namespace Molten
{

    /**
    * @brief Loop each template type in a parameter pack.
    * Privide the types and a callback function, being called for each type.
    * 
    * Example:
    * ForEachTemplateType<int, double>([](auto type)
    * {
    *   using Type = typename decltype(type)::Type;
    * }
    */
    template<typename ... TTypes, typename TCallback>
    constexpr void ForEachTemplateArgument(TCallback&& callback);

    /**
    * @brief Loop each template type in a parameter pack.
    * Privide the types and a callback function, being called for each type.
    *
    * Same as ForEachTemplateType, but also provides the loop index(starts at 0) to the calback function.
    *
    * Example:
    * ForEachTemplateTypeIndexed<int, double>([](auto type, size_t index)
    * {
    *   using Type = typename decltype(type)::Type;
    * }
    */
    template<typename ... TTypes, typename TCallback>
    constexpr void ForEachTemplateArgumentIndexed(TCallback&& callback);

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

}

#include "Molten/Utility/Template.inl"

#endif