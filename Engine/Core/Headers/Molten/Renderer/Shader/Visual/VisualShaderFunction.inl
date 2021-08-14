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

    // Function node implementations.
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    OutputPin<TOutputType>& Function<TFunctionType, TOutputType, TInputTypes...>::GetOutput()
    {
        return m_output;
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    const OutputPin<TOutputType>& Function<TFunctionType, TOutputType, TInputTypes...>::GetOutput() const
    {
        return m_output;
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    template<size_t VIndex>
    InputPin<typename Function<TFunctionType, TOutputType, TInputTypes...>::template InputTypeAt<VIndex>>&
        Function<TFunctionType, TOutputType, TInputTypes...>::GetInput()
    {
        return *std::get<VIndex>(m_inputs);
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    template<size_t VIndex>
    const InputPin<typename Function<TFunctionType, TOutputType, TInputTypes...>::template InputTypeAt<VIndex>>&
        Function<TFunctionType, TOutputType, TInputTypes...>::GetInput() const
    {
        return *std::get<VIndex>(m_inputs);
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    FunctionType Function<TFunctionType, TOutputType, TInputTypes...>::GetFunctionType() const
    {
        return TFunctionType;
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    size_t Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPinCount() const
    {
        return InputPinCount;
    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    size_t Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPinCount() const
    {
        return OutputPinCount;
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    Pin* Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPin(const size_t index)
    {
        if constexpr(InputPinCount > 0)
        {
            if (index >= InputPinCount)
            {
                return nullptr;
            }

            return std::apply([](auto&&... elems) {
                return std::array<Pin*, InputPinCount>{ static_cast<Pin*>(elems.get()) ... };
            }, m_inputs)[index];
        }
        else
        {
            return nullptr;
        }
    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    const Pin* Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPin(const size_t index) const
    {
        if constexpr (InputPinCount > 0)
        {
            if (index >= InputPinCount)
            {
                return nullptr;
            }

            return std::apply([](auto&&... elems) {
                return std::array<const Pin*, InputPinCount>{ static_cast<const Pin*>(elems.get()) ... };
            }, m_inputs)[index];
        }
        else
        {
            return nullptr;
        }
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    std::vector<Pin*> Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPins()
    {
        return std::apply([](auto&&... elems) {
            return std::vector<Pin*>{ static_cast<Pin*>(elems.get()) ... };
        }, m_inputs);

    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    std::vector<const Pin*> Function<TFunctionType, TOutputType, TInputTypes...>::GetInputPins() const
    {
        return std::apply([](auto&&... elems) {
            return std::vector<const Pin*>{ static_cast<const Pin*>(elems.get()) ... };
        }, m_inputs);
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    Pin* Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_output;
    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    const Pin* Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_output;
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    std::vector<Pin*> Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPins()
    {
        return { &m_output };
    }
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    std::vector<const Pin*> Function<TFunctionType, TOutputType, TInputTypes...>::GetOutputPins() const
    {
        return { &m_output };
    }

    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    Function<TFunctionType, TOutputType, TInputTypes...>::Function(Script& script) :
        FunctionBase(script),
        m_output(*this),
        m_inputs(std::make_tuple<std::unique_ptr<InputPin<TInputTypes>>...>(std::make_unique<InputPin<TInputTypes>>(*this)...))
    {}

}