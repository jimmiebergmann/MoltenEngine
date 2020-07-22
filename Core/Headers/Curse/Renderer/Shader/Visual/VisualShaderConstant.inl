/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

namespace Curse::Shader::Visual
{

    // Shader script constant node implementations.
    template<typename T>
    inline VariableDataType Constant<T>::GetDataType() const
    {
        return m_output.GetDataType();
    }

    template<typename T>
    inline size_t Constant<T>::GetOutputPinCount() const
    {
        return 1;
    }

    template<typename T>
    inline Pin* Constant<T>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_output;
    }
    template<typename T>
    inline const Pin* Constant<T>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_output;
    }

    template<typename T>
    inline std::vector<Pin*> Constant<T>::GetOutputPins()
    {
        return { &m_output };
    }
    template<typename T>
    inline std::vector<const Pin*> Constant<T>::GetOutputPins() const
    {
        return { &m_output };
    }

    template<typename T>
    inline const T& Constant<T>::GetValue() const
    {
        return m_value;
    }

    template<typename T>
    inline void Constant<T>::SetValue(const T& value)
    {
        m_value = value;
    }

    template<typename T>
    inline Constant<T>::Constant(Script& script, const T& value) :
        ConstantBase(script),
        m_output(*this),
        m_value(value)
    { }


}