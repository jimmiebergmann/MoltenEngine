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

    // Constant implementations.
    template<typename TDataType>
    inline Constant<TDataType>::Constant(Script& script, const TDataType& value) :
        ConstantBase(script),
        m_value(value),
        m_pin(*this)
    {}

    template<typename TDataType>
    inline VariableDataType Constant<TDataType>::GetDataType() const
    {
        return VariableTrait<TDataType>::dataType;
    }

    template<typename TDataType>
    size_t Constant<TDataType>::GetOutputPinCount() const
    {
        return 1;
    }

    template<typename TDataType>
    Pin* Constant<TDataType>::GetOutputPin(const size_t index)
    {
        if (index >= 1)
        {
            return nullptr;
        }
        return &m_pin;
    }

    template<typename TDataType>
    const Pin* Constant<TDataType>::GetOutputPin(const size_t index) const
    {
        if (index >= 1)
        {
            return nullptr;
        }
        return &m_pin;
    }

    template<typename TDataType>
    std::vector<Pin*> Constant<TDataType>::GetOutputPins()
    {
        return { &m_pin };
    }
    template<typename TDataType>
    std::vector<const Pin*> Constant<TDataType>::GetOutputPins() const
    {
        return { &m_pin };
    }




    template<typename TDataType>
    inline const TDataType& Constant<TDataType>::GetValue() const
    {
        return m_value;
    }

    template<typename TDataType>
    inline void Constant<TDataType>::SetValue(const TDataType& value)
    {
        m_value = value;
    }

}