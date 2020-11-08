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

#include "Molten/Utility/Template.hpp"

namespace Molten::Shader::Visual
{

    // Descriptor binding sampler base implementations.
    template<typename TOutputType>
    inline DescriptorBindingSamplerBase<TOutputType>::DescriptorBindingSamplerBase(Script& script, const uint32_t id) :
        DescriptorBindingBase(script),
        m_id(id),
        m_pin(*this)
    {}

    template<typename TOutputType>
    inline uint32_t DescriptorBindingSamplerBase<TOutputType>::GetId() const
    {
        return m_id;
    }

    template<typename TOutputType>
    inline DescriptorBindingType DescriptorBindingSamplerBase<TOutputType>::GetBindingType() const
    {
        return DescriptorBindingSamplerTraits<TOutputType>::bindingType;
    }

    template<typename TOutputType>
    size_t DescriptorBindingSamplerBase<TOutputType>::GetOutputPinCount() const
    {
        return 1;
    }

    template<typename TOutputType>
    Pin* DescriptorBindingSamplerBase<TOutputType>::GetOutputPin(const size_t index)
    {
        return &m_pin;
    }
    template<typename TOutputType>
    const Pin* DescriptorBindingSamplerBase<TOutputType>::GetOutputPin(const size_t index) const
    {
        return &m_pin;
    }

    template<typename TOutputType>
    std::vector<Pin*> DescriptorBindingSamplerBase<TOutputType>::GetOutputPins()
    {
        return { &m_pin };
    }
    template<typename TOutputType>
    std::vector<const Pin*> DescriptorBindingSamplerBase<TOutputType>::GetOutputPins() const
    {
        return { &m_pin };
    }


    // Descriptor binding Sampler1D specializing implementations.
    inline DescriptorBinding<Sampler1D>::DescriptorBinding(Script& script, const uint32_t id) :
        DescriptorBindingSamplerBase<Sampler1D::Handle>(script, id)
    {}

    // Descriptor binding Sampler2D specializing implementations.
    inline DescriptorBinding<Sampler2D>::DescriptorBinding(Script& script, const uint32_t id) :
        DescriptorBindingSamplerBase<Sampler2D::Handle>(script, id)
    {}

    // Descriptor binding Sampler3D specializing implementations.
    inline DescriptorBinding<Sampler3D>::DescriptorBinding(Script& script, const uint32_t id) :
        DescriptorBindingSamplerBase<Sampler3D::Handle>(script, id)
    {}


    // Descriptor binding UnfiformBuffer specializing implementations.
    template<typename ... TAllowedDataTypes>
    inline DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::DescriptorBinding(Script& script, const uint32_t id) :
        DescriptorBindingBase(script),
        m_id(id),
        m_pinWrappers{},
        m_sizeOf(0)
    {}

    template<typename ... TAllowedDataTypes>
    inline uint32_t DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::GetId() const
    {
        return m_id;
    }

    template<typename ... TAllowedDataTypes>
    inline DescriptorBindingType DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::GetBindingType() const
    {
        return DescriptorBindingType::UniformBuffer;
    }



    template<typename ... TAllowedDataTypes>
    inline size_t DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::GetOutputPinCount() const
    {
        return m_pinWrappers.size();
    }

    template<typename ... TAllowedDataTypes>
    inline Pin* DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::GetOutputPin(const size_t index)
    {
        if (index >= m_pinWrappers.size())
        {
            return nullptr;
        }
        return m_pinWrappers[index].pin;
    }
    template<typename ... TAllowedDataTypes>
    inline const Pin* DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::GetOutputPin(const size_t index) const
    {
        if (index >= m_pinWrappers.size())
        {
            return nullptr;
        }
        return m_pinWrappers[index].pin;
    }
    
    template<typename ... TAllowedDataTypes>
    inline std::vector<Pin*> DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::GetOutputPins()
    {
        return CreatePinVector<Pin>();
    }

    template<typename ... TAllowedDataTypes>
    inline std::vector<const Pin*> DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::GetOutputPins() const
    {
        return CreatePinVector<const Pin>();
    }
    
    template<typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline OutputPin<TDataType>& DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::AddPin()
    {
        static_assert(TemplateArgumentsContains<TDataType, TAllowedDataTypes...>(),
            "Provided TDataType is not allowed for this descriptor binding.");

        auto pin = new OutputPin<TDataType>(*this);
        m_pinWrappers.push_back({ pin, sizeof(TDataType) });
        m_sizeOf += sizeof(TDataType);
        return *pin;
    }

    template<typename ... TAllowedDataTypes>
    inline void DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::RemovePin(const size_t index)
    {
        if (index >= m_pinWrappers.size())
        {
            return;
        }

        auto it = m_pinWrappers.begin() + index;
        m_sizeOf -= it->dataTypeSize;
        delete it->pin;
        m_pinWrappers.erase(it);
    }

    template<typename ... TAllowedDataTypes>
    inline void DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::RemoveAllPins()
    {
        for (auto& pinWrapper : m_pinWrappers)
        {
            delete pinWrapper.pin;
        }
        m_pinWrappers.clear();
        m_sizeOf = 0;
    }

    template<typename ... TAllowedDataTypes>
    inline size_t DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::GetPinCount() const
    {
        return m_pinWrappers.size();
    }

    template<typename ... TAllowedDataTypes>
    inline size_t DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::GetSizeOf() const
    {
        return m_sizeOf;
    }

    template<typename ... TAllowedDataTypes>
    inline DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::~DescriptorBinding()
    {
        RemoveAllPins();
    }

    template<typename ... TAllowedDataTypes>
    template<typename T>
    inline std::vector<T*> DescriptorBinding<UniformBuffer<TAllowedDataTypes...>>::CreatePinVector() const
    {
        std::vector<T*> pins(m_pinWrappers.size(), nullptr);
        std::transform(m_pinWrappers.begin(), m_pinWrappers.end(), pins.begin(),
            [](auto& pinWrapper) -> T*
            {
                return pinWrapper.pin;
            }
        );
        return pins;
    }


    // Descriptor binding traits implementations.
    template<>
    struct DescriptorBindingSamplerTraits<Sampler1D::Handle>
    {
        static constexpr DescriptorBindingType bindingType = DescriptorBindingType::Sampler1D;
    };

    template<>
    struct DescriptorBindingSamplerTraits<Sampler2D::Handle>
    {
        static constexpr DescriptorBindingType bindingType = DescriptorBindingType::Sampler2D;
    };

    template<>
    struct DescriptorBindingSamplerTraits<Sampler3D::Handle>
    {
        static constexpr DescriptorBindingType bindingType = DescriptorBindingType::Sampler3D;
    };

}