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

    // Arithmetic operator node implementations.
    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline ArithmeticOperatorType ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetArithmeticOperatorType() const
    {
        return TOperator;
    }

    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline size_t ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetInputPinCount() const
    {
        return 2;
    }

    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline size_t ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetOutputPinCount() const
    {
        return 1;
    }

    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline Pin* ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetInputPin(const size_t index)
    {
        if (index > 1)
        {
            return nullptr;
        }
        return std::array<Pin*, 2>{&m_inputLeft, &m_inputRight}[index] ;
    }
    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline const Pin* ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetInputPin(const size_t index) const
    {
        if (index > 1)
        {
            return nullptr;
        }
        return std::array<const Pin*, 2>{&m_inputLeft, &m_inputRight}[index] ;
    }

    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline std::vector<Pin*> ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetInputPins()
    {
        return { &m_inputLeft, &m_inputRight };
    }
    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline std::vector<const Pin*> ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetInputPins() const
    {
        return { &m_inputLeft, &m_inputRight };
    }

    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline Pin* ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetOutputPin(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_output;
    }
    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline const Pin* ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetOutputPin(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return &m_output;
    }

    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline std::vector<Pin*> ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetOutputPins()
    {
        return { &m_output };
    }
    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline std::vector<const Pin*> ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::GetOutputPins() const
    {
        return { &m_output };
    }

    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    inline ArithmeticOperator<TOperator, TOutputType, TLeftType, TRightType>::ArithmeticOperator(Script& script) :
        ArithmeticOperatorBase(script),
        m_inputLeft(*this),
        m_inputRight(*this),
        m_output(*this)
    { }

}