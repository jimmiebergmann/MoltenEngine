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

namespace Curse
{

    namespace Shader
    {

        // Operator node base implementations.
        inline NodeType OperatorNodeBase::GetType() const
        {
            return NodeType::Operator;
        }

        inline OperatorNodeBase::OperatorNodeBase(Script& script) :
            Node(script)
        { }

        inline OperatorNodeBase::~OperatorNodeBase()
        { }


        // Arithmetic operator node base implementations.
        inline OperatorType ArithmeticOperatorNodeBase::GetOperatorType() const
        {
            return OperatorType::Arithmetic;
        }

        inline ArithmeticOperatorNodeBase::ArithmeticOperatorNodeBase(Script& script) :
            OperatorNodeBase(script)
        { }

        inline ArithmeticOperatorNodeBase::~ArithmeticOperatorNodeBase()
        { }


        // Shader script operator node implementations.
        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline ArithmeticOperatorType ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetArithmeticOperatorType() const
        {
            return Operator;
        }

        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline size_t ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetInputPinCount() const
        {
            return 2;
        }

        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline size_t ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetOutputPinCount() const
        {
            return 1;
        }

        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline Pin* ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetInputPin(const size_t index)
        {
            if (index > 1)
            {
                return nullptr;
            }
            return std::vector<Pin*>{&m_inputLeft, & m_inputRight} [index] ;
        }
        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline const Pin* ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetInputPin(const size_t index) const
        {
            if (index > 1)
            {
                return nullptr;
            }
            return std::vector<const Pin*>{&m_inputLeft, & m_inputRight} [index] ;
        }

        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline std::vector<Pin*> ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetInputPins()
        {
            return { &m_inputLeft, &m_inputRight };
        }
        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline std::vector<const Pin*> ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetInputPins() const
        {
            return { &m_inputLeft, &m_inputRight };
        }

        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline Pin* ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetOutputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }
        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline const Pin* ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetOutputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }

        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline std::vector<Pin*> ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetOutputPins()
        {
            return { &m_output };
        }
        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline std::vector<const Pin*> ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetOutputPins() const
        {
            return { &m_output };
        }

        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::ArithmeticOperatorNode(Script& script) :
            ArithmeticOperatorNodeBase(script),
            m_inputLeft(*this),
            m_inputRight(*this),
            m_output(*this)
        { }

        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::~ArithmeticOperatorNode()
        { }

    }

}