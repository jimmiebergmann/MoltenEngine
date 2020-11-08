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

namespace Molten::Shader::Visual
{

    // Single Pin Node implementations.
    template<template<typename> typename TPinType, typename TPinDataType>
    inline SinglePinNode<TPinType, TPinDataType>::SinglePinNode(Script& script) :
        Node(script)
    {}

    template<template<typename> typename TPinType, typename TPinDataType>
    inline size_t SinglePinNode<TPinType, TPinDataType>::GetInputPinCount() const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    template<template<typename> typename TPinType, typename TPinDataType>
    inline size_t SinglePinNode<TPinType, TPinDataType>::GetOutputPinCount() const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    template<template<typename> typename TPinType, typename TPinDataType>
    inline Pin* SinglePinNode<TPinType, TPinDataType>::GetInputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_pin;
        }
        else
        {
            return nullptr;
        }
    }
    template<template<typename> typename TPinType, typename TPinDataType>
    inline const Pin* SinglePinNode<TPinType, TPinDataType>::GetInputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_pin;
        }
        else
        {
            return nullptr;
        }
    }

    template<template<typename> typename TPinType, typename TPinDataType>
    inline std::vector<Pin*> SinglePinNode<TPinType, TPinDataType>::GetInputPins()
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return { &m_pin };
        }
        else
        {
            return {};
        }
    }
    template<template<typename> typename TPinType, typename TPinDataType>
    inline std::vector<const Pin*> SinglePinNode<TPinType, TPinDataType>::GetInputPins() const
    {
        if constexpr (PinTraits<TPinType>::isInputPin == true)
        {
            return { &m_pin };
        }
        else
        {
            return {};
        }
    }
    

    template<template<typename> typename TPinType, typename TPinDataType>
    inline Pin* SinglePinNode<TPinType, TPinDataType>::GetOutputPin(const size_t index)
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_pin;
        }
        else
        {
            return nullptr;
        }
    }
    template<template<typename> typename TPinType, typename TPinDataType>
    inline const Pin* SinglePinNode<TPinType, TPinDataType>::GetOutputPin(const size_t index) const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_pin;
        }
        else
        {
            return nullptr;
        }
    }

    template<template<typename> typename TPinType, typename TPinDataType>
    inline std::vector<Pin*> SinglePinNode<TPinType, TPinDataType>::GetOutputPins()
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return { &m_pin };
        }
        else
        {
            return {};
        }
    }
    template<template<typename> typename TPinType, typename TPinDataType>
    inline std::vector<const Pin*> SinglePinNode<TPinType, TPinDataType>::GetOutputPins() const
    {
        if constexpr (PinTraits<TPinType>::isOutputPin == true)
        {
            return { &m_pin };
        }
        else
        {
            return {};
        }
    }
    
}