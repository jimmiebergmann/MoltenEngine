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

    // Function node implementations.
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline FunctionType Function<TFunctionType, TOutputType, TInputTypes...>::GetFunctionType() const
    {
        return TFunctionType;
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline size_t Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPinCount() const
    {
        return InputPinCount;
    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline size_t Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPinCount() const
    {
        return OutputPinCount;
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline Pin* Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPin(const size_t index)
    {
        if (index >= m_inputs.size())
        {
            return nullptr;
        }
        return m_inputs[index].get();
    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline const Pin* Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPin(const size_t index) const
    {
        if (index >= m_inputs.size())
        {
            return nullptr;
        }
        return m_inputs[index].get();
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline std::vector<Pin*> Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPins()
    {
        std::vector<Pin*> pins;
        for (const auto& pin : m_inputs)
        {
            pins.push_back(pin.get());
        }

        return pins;
    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline std::vector<const Pin*> Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPins() const
    {
        std::vector<const Pin*> pins;
        for (const auto& pin : m_inputs)
        {
            pins.push_back(pin.get());
        }

        return pins;
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline Pin* Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return m_output.get();
    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline const Pin* Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return m_output.get();
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline std::vector<Pin*> Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPins()
    {
        auto output = m_output.get();
        if (!output)
        {
            return {};
        }
        return { output };
    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline std::vector<const Pin*> Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPins() const
    {
        auto output = m_output.get();
        if (!output)
        {
            return {};
        }
        return { output };
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    inline Function<TFunctionType, TOutputType, TInputTypes...>::Function(Script& script) :
        FunctionBase(script),
        m_output(std::make_unique<OutputPin<TOutputType> >(*this))
    {
        size_t index = 0;
        ForEachTemplateArgument<TInputTypes...>([&](auto t)
        {
            using currentPinType = typename decltype(t)::Type;
            m_inputs[index] = std::move(std::make_unique<InputPin<currentPinType> >(*this));
            index++;
        });
    }

}