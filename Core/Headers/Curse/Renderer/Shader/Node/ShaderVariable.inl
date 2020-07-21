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

namespace Curse::Shader
{

    // Variable implementations.
    template<typename TDataType>
    Variable<TDataType>::Variable(Script& script) :
        VariableBase(script),
        m_inputPin(*this),
        m_outputPin(*this)
    {}

    template<typename TDataType>
    size_t Variable<TDataType>::GetInputPinCount() const
    {
        return 1;
    }

    template<typename TDataType>
    size_t Variable<TDataType>::GetOutputPinCount() const
    {
        return 1;
    }

    template<typename TDataType>
    Pin* Variable<TDataType>::GetInputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_inputPin;
    }
    template<typename TDataType>
    const Pin* Variable<TDataType>::GetInputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_inputPin;
    }

    template<typename TDataType>
    std::vector<Pin*> Variable<TDataType>::GetInputPins()
    {
        return { &m_inputPin };
    }
    template<typename TDataType>
    std::vector<const Pin*> Variable<TDataType>::GetInputPins() const
    {
        return { &m_inputPin };
    }

    template<typename TDataType>
    Pin* Variable<TDataType>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }
    template<typename TDataType>
    const Pin* Variable<TDataType>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }

    template<typename TDataType>
    std::vector<Pin*> Variable<TDataType>::GetOutputPins()
    {
        return { &m_outputPin };
    }
    template<typename TDataType>
    std::vector<const Pin*> Variable<TDataType>::GetOutputPins() const
    {
        return { &m_outputPin };
    }


    // Input variable implementations.
    template<typename TDataType>
    InputVariable<TDataType>::InputVariable(Script& script) :
        InputVariableBase(script),
        m_outputPin(*this)
    {}

    template<typename TDataType>
    size_t InputVariable<TDataType>::GetOutputPinCount() const
    {
        return 1;
    }

    template<typename TDataType>
    Pin* InputVariable<TDataType>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }
    template<typename TDataType>
    const Pin* InputVariable<TDataType>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_outputPin;
    }

    template<typename TDataType>
    std::vector<Pin*> InputVariable<TDataType>::GetOutputPins()
    {
        return { &m_outputPin };
    }
    template<typename TDataType>
    std::vector<const Pin*> InputVariable<TDataType>::GetOutputPins() const
    {
        return { &m_outputPin };
    }


    // Output variable implementations.
    template<typename TDataType>
    OutputVariable<TDataType>::OutputVariable(Script& script) :
        OutputVariableBase(script),
        m_inputPin(*this)
    {}

    template<typename TDataType>
    size_t OutputVariable<TDataType>::GetInputPinCount() const
    {
        return 1;
    }

    template<typename TDataType>
    Pin* OutputVariable<TDataType>::GetInputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_inputPin;
    }
    template<typename TDataType>
    const Pin* OutputVariable<TDataType>::GetInputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_inputPin;
    }

    template<typename TDataType>
    std::vector<Pin*> OutputVariable<TDataType>::GetInputPins()
    {
        return { &m_inputPin };
    }
    template<typename TDataType>
    std::vector<const Pin*> OutputVariable<TDataType>::GetInputPins() const
    {
        return { &m_inputPin };
    }

}