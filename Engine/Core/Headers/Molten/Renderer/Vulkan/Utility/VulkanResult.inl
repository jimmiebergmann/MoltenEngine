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

namespace Molten::Vulkan
{

    // Result<TCustomResult> implementations.
    template<typename TCustomResult>
    Result<TCustomResult>::Result() :
        std::variant<VkResult, TCustomResult>{}
    {}

    template<typename TCustomResult>
    Result<TCustomResult>::Result(const VkResult result) :
        std::variant<VkResult, TCustomResult>{ result  }
    {}

    template<typename TCustomResult>
    Result<TCustomResult>::Result(const TCustomResult& result) :
        std::variant<VkResult, TCustomResult>{ result }
    {}

    template<typename TCustomResult>
    Result<TCustomResult>::Result(TCustomResult&& result) :
        std::variant<VkResult, TCustomResult>{ std::move(result) }
    {}

    template<typename TCustomResult>
    Result<TCustomResult>& Result<TCustomResult>::operator =(const VkResult result)
    {
        *(static_cast<std::variant<VkResult, TCustomResult>*>(this)) = result;
        return *this;
    }

    template<typename TCustomResult>
    Result<TCustomResult>& Result<TCustomResult>::operator =(const TCustomResult& result)
    {
        *(static_cast<std::variant<VkResult, TCustomResult>*>(this)) = result;
        return *this;
    }

    template<typename TCustomResult>
    Result<TCustomResult>& Result<TCustomResult>::operator =(TCustomResult&& result)
    {
        *(static_cast<std::variant<VkResult, TCustomResult>*>(this)) = std::move(result);
        return *this;
    }

    template<typename TCustomResult>
    template<size_t Index>
    typename Result<TCustomResult>::template ResultValue<Index>::Type& Result<TCustomResult>::Get()
    {
        return std::get<Index>(*static_cast<std::variant<VkResult, TCustomResult>*>(this));
    }
    template<typename TCustomResult>
    template<size_t Index>
    const typename Result<TCustomResult>::template ResultValue<Index>::Type& Result<TCustomResult>::Get() const
    {
        return std::get<Index>(*static_cast<const std::variant<VkResult, TCustomResult>*>(this));
    }

    template<typename TCustomResult>
    size_t Result<TCustomResult>::GetTypeIndex() const
    {
        return static_cast<const std::variant<VkResult, TCustomResult>*>(this)->index();
    }

    template<typename TCustomResult>
    bool Result<TCustomResult>::ExpectAny(VkResult vulkanResult, const TCustomResult& customResult)
    {
        auto& variant = *static_cast<std::variant<VkResult, TCustomResult>*>(this);
        if (variant.index() == 0)
        {
            return std::get<0>(variant) == vulkanResult;
        }
        return std::get<1>(variant) == customResult;
    }

    template<typename TCustomResult>
    bool Result<TCustomResult>::ExpectSuccessOr(const TCustomResult& customResult)
    {
        auto& variant = *static_cast<std::variant<VkResult, TCustomResult>*>(this);
        if (variant.index() == 0)
        {
            return std::get<0>(variant) == VkResult::VK_SUCCESS;
        }
        return std::get<1>(variant) == customResult;
    }


    // Result<VkResult> implementations.
    inline Result<VkResult>::Result() :
        m_value(VkResult::VK_SUCCESS)
    {}

    inline Result<VkResult>::Result(const VkResult result) :
        m_value(result)
    {}

    template<size_t Index>
    VkResult& Result<VkResult>::Get()
    {
        static_assert(Index == 0, "Cannot get value by index 0 from Result<VkResult>.");
        return m_value;
    }

    template<size_t Index>
    const VkResult& Result<VkResult>::Get() const
    {
        static_assert(Index == 0, "Cannot get value by index 0 from Result<VkResult>.");
        return m_value;
    }

    inline VkResult& Result<VkResult>::Get()
    {
        return m_value;
    }
    inline const VkResult& Result<VkResult>::Get() const
    {
        return m_value;
    }

    inline size_t Result<VkResult>::GetTypeIndex() const
    {
        return 0;
    }

    inline bool Result<VkResult>::IsSuccessful() const
    {
        return m_value == VkResult::VK_SUCCESS;
    }

    inline bool Result<VkResult>::operator ==(const Result<VkResult> result) const
    {
        return m_value == result.m_value;
    }
    inline bool Result<VkResult>::operator ==(const VkResult result) const
    {
        return m_value == result;
    }
    inline bool Result<VkResult>::operator !=(const Result<VkResult> result) const
    {
        return m_value != result.m_value;
    }
    inline bool Result<VkResult>::operator !=(const VkResult result) const
    {
        return m_value != result;
    }


    // Result type info implementations.
    inline ResultTypeInfo::ResultTypeInfo() :
        name(ResultTypeInfo::emptyString),
        description(ResultTypeInfo::emptyString)
    {}
    inline ResultTypeInfo::ResultTypeInfo(const ResultTypeInfoStorage& storage) :
        name(storage.name),
        description(storage.description)
    {}

}