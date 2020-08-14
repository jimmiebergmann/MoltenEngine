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

    // Variable meta data base implementations.
    template<typename TMetaData>
    template<typename ... TMetaDataParams>
    inline VariableMetaBase<TMetaData>::VariableMetaBase(Script& script, TMetaDataParams ... metaDataParameters) :
        VariableBase(script),
        TMetaData(metaDataParameters...)
    {}

    inline VariableMetaBase<void>::VariableMetaBase(Script& script) :
        VariableBase(script)
    {}


    // Input variable base implementations.
    template<typename TMetaData>
    template<typename ... TMetaDataParams>
    inline InputVariableBase<TMetaData>::InputVariableBase(Script& script, TMetaDataParams ... metaDataParameters) :
        VariableMetaBase<TMetaData>(script, metaDataParameters...)
    {}


    // Output variable base implementations.
    template<typename TMetaData>
    template<typename ... TMetaDataParams>
    inline OutputVariableBase<TMetaData>::OutputVariableBase(Script& script, TMetaDataParams ... metaDataParameters) :
        VariableMetaBase<TMetaData>(script, metaDataParameters...)
    {}


    // Constant variable base implementations.
    template<typename TMetaData>
    template<typename ... TMetaDataParams>
    inline ConstantVariableBase<TMetaData>::ConstantVariableBase(Script& script, TMetaDataParams ... metaDataParameters) :
        VariableMetaBase<TMetaData>(script, metaDataParameters...)
    {}


    // Input variable implementations.
    template<typename TDataType, typename TMetaData>
    template<typename ... TMetaDataParams>
    inline InputVariable<TDataType, TMetaData>::InputVariable(Script& script, TMetaDataParams ... metaDataParameters) :
        InputVariableBase<TMetaData>(script, metaDataParameters...),
        m_outputPin(*this)
    {}

    template<typename TDataType, typename TMetaData>
    inline size_t InputVariable<TDataType, TMetaData>::GetOutputPinCount() const
    {
        return 1;
    }

    template<typename TDataType, typename TMetaData>
    inline Pin* InputVariable<TDataType, TMetaData>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }
    template<typename TDataType, typename TMetaData>
    inline const Pin* InputVariable<TDataType, TMetaData>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }

    template<typename TDataType, typename TMetaData>
    inline std::vector<Pin*> InputVariable<TDataType, TMetaData>::GetOutputPins()
    {
        return { &m_outputPin };
    }
    template<typename TDataType, typename TMetaData>
    inline std::vector<const Pin*> InputVariable<TDataType, TMetaData>::GetOutputPins() const
    {
        return { &m_outputPin };
    }

    template<typename TDataType, typename TMetaData>
    inline VariableDataType InputVariable<TDataType, TMetaData>::GetDataType() const
    {
        return m_outputPin.GetDataType();
    }

    template<typename TDataType, typename TMetaData>
    inline VariableType InputVariable<TDataType, TMetaData>::GetVariableType() const
    {
        return VariableType::Input;
    }

    template<typename TDataType, typename TMetaData>
    inline size_t InputVariable<TDataType, TMetaData>::GetSizeOf() const
    {
        return VariableTrait<TDataType>::dataSize;
    }

    template<typename TDataType, typename TMetaData>
    inline size_t InputVariable<TDataType, TMetaData>::GetPaddedSizeOf() const
    {
        return VariableTrait<TDataType>::paddedDataSize;
    }


    // Output variable implementations.
    template<typename TDataType, typename TMetaData>
    template<typename ... TMetaDataParams>
    inline OutputVariable<TDataType, TMetaData>::OutputVariable(Script& script, TMetaDataParams ... metaDataParameters) :
        OutputVariableBase<TMetaData>(script, metaDataParameters...),
        m_inputPin(*this)
    {}

    template<typename TDataType, typename TMetaData>
    inline size_t OutputVariable<TDataType, TMetaData>::GetInputPinCount() const
    {
        return 1;
    }

    template<typename TDataType, typename TMetaData>
    inline Pin* OutputVariable<TDataType, TMetaData>::GetInputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_inputPin;
    }
    template<typename TDataType, typename TMetaData>
    inline const Pin* OutputVariable<TDataType, TMetaData>::GetInputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_inputPin;
    }

    template<typename TDataType, typename TMetaData>
    inline std::vector<Pin*> OutputVariable<TDataType, TMetaData>::GetInputPins()
    {
        return { &m_inputPin };
    }
    template<typename TDataType, typename TMetaData>
    inline std::vector<const Pin*> OutputVariable<TDataType, TMetaData>::GetInputPins() const
    {
        return { &m_inputPin };
    }

    template<typename TDataType, typename TMetaData>
    inline VariableDataType OutputVariable<TDataType, TMetaData>::GetDataType() const
    {
        return m_inputPin.GetDataType();
    }

    template<typename TDataType, typename TMetaData>
    inline VariableType OutputVariable<TDataType, TMetaData>::GetVariableType() const
    {
        return VariableType::Output;
    }

    template<typename TDataType, typename TMetaData>
    inline size_t OutputVariable<TDataType, TMetaData>::GetSizeOf() const
    {
        return VariableTrait<TDataType>::dataSize;
    }

    template<typename TDataType, typename TMetaData>
    inline size_t OutputVariable<TDataType, TMetaData>::GetPaddedSizeOf() const
    {
        return VariableTrait<TDataType>::paddedDataSize;
    }


    // Constant variable implementations.
    template<typename TDataType, typename TMetaData>
    template<typename ... TMetaDataParams>
    inline ConstantVariable<TDataType, TMetaData>::ConstantVariable(Script& script, const DataType& value, TMetaDataParams ... metaDataParameters) :
        ConstantVariableBase<TMetaData>(script, metaDataParameters...),
        m_outputPin(*this),
        m_value(value)
    {}

    template<typename TDataType, typename TMetaData>
    template<typename ... TMetaDataParams>
    inline ConstantVariable<TDataType, TMetaData>::ConstantVariable(Script& script, DataType&& value, TMetaDataParams ... metaDataParameters) :
        ConstantVariableBase<TMetaData>(script, metaDataParameters...),
        m_outputPin(*this),
        m_value(std::move(value))
    {}

    template<typename TDataType, typename TMetaData>
    inline size_t ConstantVariable<TDataType, TMetaData>::GetOutputPinCount() const
    {
        return 1;
    }

    template<typename TDataType, typename TMetaData>
    inline Pin* ConstantVariable<TDataType, TMetaData>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }
    template<typename TDataType, typename TMetaData>
    inline const Pin* ConstantVariable<TDataType, TMetaData>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }

    template<typename TDataType, typename TMetaData>
    inline std::vector<Pin*> ConstantVariable<TDataType, TMetaData>::GetOutputPins()
    {
        return { &m_outputPin };
    }
    template<typename TDataType, typename TMetaData>
    inline std::vector<const Pin*> ConstantVariable<TDataType, TMetaData>::GetOutputPins() const
    {
        return { &m_outputPin };
    }

    template<typename TDataType, typename TMetaData>
    inline VariableDataType ConstantVariable<TDataType, TMetaData>::GetDataType() const
    {
        return m_outputPin.GetDataType();
    }

    template<typename TDataType, typename TMetaData>
    inline VariableType ConstantVariable<TDataType, TMetaData>::GetVariableType() const
    {
        return VariableType::Constant;
    }

    template<typename TDataType, typename TMetaData>
    inline size_t ConstantVariable<TDataType, TMetaData>::GetSizeOf() const
    {
        return VariableTrait<TDataType>::dataSize;
    }

    template<typename TDataType, typename TMetaData>
    inline size_t ConstantVariable<TDataType, TMetaData>::GetPaddedSizeOf() const
    {
        return VariableTrait<TDataType>::paddedDataSize;
    }

    template<typename TDataType, typename TMetaData>
    inline const TDataType& ConstantVariable<TDataType, TMetaData>::GetValue() const
    {
        return m_value;
    }

    template<typename TDataType, typename TMetaData>
    inline void ConstantVariable<TDataType, TMetaData>::SetValue(const DataType& value)
    {
        m_value = value;
    }
    template<typename TDataType, typename TMetaData>
    inline void ConstantVariable<TDataType, TMetaData>::SetValue(DataType&& value)
    {
        m_value = std::move(value);
    }

}