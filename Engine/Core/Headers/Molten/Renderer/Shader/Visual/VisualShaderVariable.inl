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

    // Input variable implementations.
    template<typename TDataType>
    inline InputVariable<TDataType>::InputVariable(Script& script) :
        InputVariableBase(script),
        m_outputPin(*this)
    {}

    template<typename TDataType>
    inline size_t InputVariable<TDataType>::GetOutputPinCount() const
    {
        return 1;
    }

    template<typename TDataType>
    inline Pin* InputVariable<TDataType>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }
    template<typename TDataType>
    inline const Pin* InputVariable<TDataType>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }

    template<typename TDataType>
    inline std::vector<Pin*> InputVariable<TDataType>::GetOutputPins()
    {
        return { &m_outputPin };
    }
    template<typename TDataType>
    inline std::vector<const Pin*> InputVariable<TDataType>::GetOutputPins() const
    {
        return { &m_outputPin };
    }

    template<typename TDataType>
    inline VariableDataType InputVariable<TDataType>::GetDataType() const
    {
        return m_outputPin.GetDataType();
    }

    template<typename TDataType>
    inline VariableType InputVariable<TDataType>::GetVariableType() const
    {
        return VariableType::Input;
    }


    // Output variable implementations.
    template<typename TDataType>
    inline OutputVariable<TDataType>::OutputVariable(Script& script) :
        OutputVariableBase(script),
        m_inputPin(*this)
    {}

    template<typename TDataType>
    inline OutputVariable<TDataType>::OutputVariable(Script& script, const DataType& defaultValue) :
        OutputVariableBase(script),
        m_inputPin(*this, defaultValue)
    {}

    template<typename TDataType>
    inline size_t OutputVariable<TDataType>::GetInputPinCount() const
    {
        return 1;
    }

    template<typename TDataType>
    inline Pin* OutputVariable<TDataType>::GetInputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_inputPin;
    }
    template<typename TDataType>
    inline const Pin* OutputVariable<TDataType>::GetInputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_inputPin;
    }

    template<typename TDataType>
    inline std::vector<Pin*> OutputVariable<TDataType>::GetInputPins()
    {
        return { &m_inputPin };
    }
    template<typename TDataType>
    inline std::vector<const Pin*> OutputVariable<TDataType>::GetInputPins() const
    {
        return { &m_inputPin };
    }

    template<typename TDataType>
    inline VariableDataType OutputVariable<TDataType>::GetDataType() const
    {
        return m_inputPin.GetDataType();
    }

    template<typename TDataType>
    inline VariableType OutputVariable<TDataType>::GetVariableType() const
    {
        return VariableType::Output;
    }


    // Constant variable implementations.
    template<typename TDataType>
    inline ConstantVariable<TDataType>::ConstantVariable(Script& script, const DataType& value) :
        ConstantVariableBase(script),
        m_outputPin(*this),
        m_value(value)
    {}

    template<typename TDataType>
    inline ConstantVariable<TDataType>::ConstantVariable(Script& script, DataType&& value) :
        ConstantVariableBase(script),
        m_outputPin(*this),
        m_value(std::move(value))
    {}

    template<typename TDataType>
    inline size_t ConstantVariable<TDataType>::GetOutputPinCount() const
    {
        return 1;
    }

    template<typename TDataType>
    inline Pin* ConstantVariable<TDataType>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }
    template<typename TDataType>
    inline const Pin* ConstantVariable<TDataType>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }

    template<typename TDataType>
    inline std::vector<Pin*> ConstantVariable<TDataType>::GetOutputPins()
    {
        return { &m_outputPin };
    }
    template<typename TDataType>
    inline std::vector<const Pin*> ConstantVariable<TDataType>::GetOutputPins() const
    {
        return { &m_outputPin };
    }

    template<typename TDataType>
    inline VariableDataType ConstantVariable<TDataType>::GetDataType() const
    {
        return m_outputPin.GetDataType();
    }

    template<typename TDataType>
    inline VariableType ConstantVariable<TDataType>::GetVariableType() const
    {
        return VariableType::Constant;
    }

    template<typename TDataType>
    inline const TDataType& ConstantVariable<TDataType>::GetValue() const
    {
        return m_value;
    }

    template<typename TDataType>
    inline void ConstantVariable<TDataType>::SetValue(const DataType& value)
    {
        m_value = value;
    }
    template<typename TDataType>
    inline void ConstantVariable<TDataType>::SetValue(DataType&& value)
    {
        m_value = std::move(value);
    }

}