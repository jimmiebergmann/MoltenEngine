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
#include <algorithm>

namespace Molten::Shader::Visual
{

    // Structure implementations.
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline Structure<TPinType, TAllowedDataTypes...>::Structure(Script& script) :
        Node(script),
        m_script(script),
        m_sizeOf(0)
    {
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline Structure<TPinType, TAllowedDataTypes...>::~Structure()
    {
        RemoveAllPins();
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline typename Structure<TPinType, TAllowedDataTypes...>::Iterator
        Structure<TPinType, TAllowedDataTypes...>::begin()
    {
        return m_pinWrappers.begin();
    }
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline typename Structure<TPinType, TAllowedDataTypes...>::ConstIterator
        Structure<TPinType, TAllowedDataTypes...>::begin() const
    {
        return m_pinWrappers.begin();
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline typename Structure<TPinType, TAllowedDataTypes...>::Iterator
        Structure<TPinType, TAllowedDataTypes...>::end()
    {
        return m_pinWrappers.end();
    }
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline typename Structure<TPinType, TAllowedDataTypes...>::ConstIterator
        Structure<TPinType, TAllowedDataTypes...>::end() const
    {
        return m_pinWrappers.end();
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline size_t Structure<TPinType, TAllowedDataTypes...>::GetInputPinCount() const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return m_pinWrappers.size();
        }
        else
        {
            return 0;
        }
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline size_t Structure<TPinType, TAllowedDataTypes...>::GetOutputPinCount() const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return m_pinWrappers.size();
        }
        else
        {
            return 0;
        }
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline Pin* Structure<TPinType, TAllowedDataTypes...>::GetInputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            if (index >= m_pinWrappers.size())
            {
                return nullptr;
            }
            return m_pinWrappers[index].pin;
        }
        else
        {
            return nullptr;
        }
    }
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline const Pin* Structure<TPinType, TAllowedDataTypes...>::GetInputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            if (index >= m_pinWrappers.size())
            {
                return nullptr;
            }
            return m_pinWrappers[index].pin;
        }
        else
        {
            return nullptr;
        }
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline InputPin<TDataType>* Structure<TPinType, TAllowedDataTypes...>::GetInputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return static_cast<InputPin<TDataType>*>(GetInputPin(index));
        }
        else
        {
            return nullptr;
        }
    }
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline const InputPin<TDataType>* Structure<TPinType, TAllowedDataTypes...>::GetInputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return static_cast<const InputPin<TDataType>*>(GetInputPin(index));
        }
        else
        {
            return nullptr;
        }
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline std::vector<Pin*> Structure<TPinType, TAllowedDataTypes...>::GetInputPins()
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return CreatePinVector<Pin>();
        }
        else
        {
            return {};
        }
    }
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline std::vector<const Pin*> Structure<TPinType, TAllowedDataTypes...>::GetInputPins() const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return CreatePinVector<const Pin>();
        }
        else
        {
            return {};
        }
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline Pin* Structure<TPinType, TAllowedDataTypes...>::GetOutputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            if (index >= m_pinWrappers.size())
            {
                return nullptr;
            }
            return m_pinWrappers[index].pin;
        }
        else
        {
            return nullptr;
        }
    }
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline const Pin* Structure<TPinType, TAllowedDataTypes...>::GetOutputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            if (index >= m_pinWrappers.size())
            {
                return nullptr;
            }
            return m_pinWrappers[index].pin;
        }
        else
        {
            return nullptr;
        }
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline OutputPin<TDataType>* Structure<TPinType, TAllowedDataTypes...>::GetOutputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return static_cast<OutputPin<TDataType>*>(GetOutputPin(index));
        }
        else
        {
            return nullptr;
        }
    }
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline const OutputPin<TDataType>* Structure<TPinType, TAllowedDataTypes...>::GetOutputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return static_cast<const OutputPin<TDataType>*>(GetOutputPin(index));
        }
        else
        {
            return nullptr;
        }
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline std::vector<Pin*> Structure<TPinType, TAllowedDataTypes...>::GetOutputPins()
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return CreatePinVector<Pin>();
        }
        else
        {
            return {};
        }
    }
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline std::vector<const Pin*> Structure<TPinType, TAllowedDataTypes...>::GetOutputPins() const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return CreatePinVector<const Pin>();
        }
        else
        {
            return {};
        }
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    template<typename TDataType>
    inline typename Structure<TPinType, TAllowedDataTypes...>::template PinType<TDataType>&
        Structure<TPinType, TAllowedDataTypes...>::AddPin()
    {
        static_assert(TemplateArgumentsContains<TDataType, TAllowedDataTypes...>(),
            "Provided TDataType is not allowed for this structure.");

        auto pin = new PinType<TDataType>(*this);
        m_pinWrappers.push_back({ pin, sizeof(TDataType) });
        m_sizeOf += sizeof(TDataType);
        return *pin;
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline void Structure<TPinType, TAllowedDataTypes...>::RemovePin(const size_t index)
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
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline void Structure<TPinType, TAllowedDataTypes...>::RemovePin(const Iterator it)
    {
        m_sizeOf -= it->dataTypeSize;
        delete it->pin;
        m_pinWrappers.erase(it);
    }
    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline void Structure<TPinType, TAllowedDataTypes...>::RemovePin(const ConstIterator it)
    {
        m_sizeOf -= it->dataTypeSize;
        delete it->pin;
        m_pinWrappers.erase(it);
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline void Structure<TPinType, TAllowedDataTypes...>::RemoveAllPins()
    {
        for (auto& pinWrapper : m_pinWrappers)
        {
            delete pinWrapper.pin;
        }
        m_pinWrappers.clear();
        m_sizeOf = 0;
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline size_t Structure<TPinType, TAllowedDataTypes...>::GetPinCount() const
    {
        return m_pinWrappers.size();
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    inline size_t Structure<TPinType, TAllowedDataTypes...>::GetSizeOf() const
    {
        return m_sizeOf;
    }

    template<template<typename> typename TPinType, typename ... TAllowedDataTypes>
    template<typename T>
    inline std::vector<T*> Structure<TPinType, TAllowedDataTypes...>::CreatePinVector() const
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

}