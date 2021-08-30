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

#include <type_traits>

namespace Molten
{

    namespace Private
    {

        template<typename T>
        struct ForEachTemplateTypeWrapper
        {
            using Type = T;
        };

    }

    template<typename ... TTypes, typename TCallback>
    constexpr void ForEachTemplateArgument(TCallback&& callback)
    {
        (callback(Private::ForEachTemplateTypeWrapper<TTypes>{}), ...);
    }

    template<typename ... TTypes, typename TCallback>
    constexpr void ForEachTemplateArgumentIndexed(TCallback&& callback)
    {
        size_t index = 0;
        (callback(Private::ForEachTemplateTypeWrapper<TTypes>{}, index++), ...);
    }

    template<typename TType, typename ... TTypes>
    constexpr bool TemplateArgumentsContains()
    {
        return (std::is_same_v<TType, TTypes> || ...);
    }

    template<size_t VIndex, typename ... TTypes>
    constexpr size_t GetTemplateArgumentAt<VIndex, TTypes...>::GetIndex()
    {
        static_assert(VIndex < sizeof...(TTypes), "Molten::GetTemplateArgumentAt failed due to index out of bounds.");
        return VIndex;
    }

}