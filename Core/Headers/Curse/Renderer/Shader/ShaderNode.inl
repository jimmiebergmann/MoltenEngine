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

        // Shader script constant node implementations.
        template<typename T>
        inline const T& ConstantNode<T>::GetValue() const
        {
            return m_value;
        }

        template<typename T>
        inline void ConstantNode<T>::SetValue(const T& value)
        {
            m_value = value;
        }

        template<typename T>
        inline VariableDataType ConstantNode<T>::GetDataType() const
        {
            return m_output.GetDataType();
        }

        template<typename T>
        inline size_t ConstantNode<T>::GetOutputPinCount() const
        {
            return 1;
        }

        template<typename T>
        inline Pin* ConstantNode<T>::GetOutputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }
        template<typename T>
        inline const Pin* ConstantNode<T>::GetOutputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }

        template<typename T>
        inline std::vector<Pin*> ConstantNode<T>::GetOutputPins()
        {
            return { &m_output };
        }
        template<typename T>
        inline std::vector<const Pin*> ConstantNode<T>::GetOutputPins() const
        {
            return { &m_output };
        }

        template<typename T>
        inline ConstantNode<T>::ConstantNode(Script& script, const T& value) :
            ConstantNodeBase(script),
            m_output(*this),
            m_value(value)
        { }

        template<typename T>
        inline ConstantNode<T>::~ConstantNode()
        { }


        // Function node implementations.
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        FunctionType FunctionNode<_FunctionType, OutputType, InputTypes...>::GetFunctionType() const
        {
            return _FunctionType;
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline size_t FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPinCount() const
        {
            return InputPinCount;
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline size_t FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPinCount() const
        {
            return OutputPinCount;
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline Pin* FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPin(const size_t index)
        {
            if (index > m_inputs.size())
            {
                return nullptr;
            }
            return m_inputs[index].get();
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline const Pin* FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPin(const size_t index) const
        {
            if (index > m_inputs.size())
            {
                return nullptr;
            }
            return m_inputs[index].get();
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline std::vector<Pin*> FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPins()
        {
            std::vector<Pin*> pins;
            for (const auto& pin : m_inputs)
            {
                pins.push_back(pin.get());
            }

            return pins;
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline std::vector<const Pin*> FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPins() const
        {
            std::vector<const Pin*> pins;
            for (const auto& pin : m_inputs)
            {
                pins.push_back(pin.get());
            }

            return pins;
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline Pin* FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return m_output.get();
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline const Pin* FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return m_output.get();
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline std::vector<Pin*> FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPins()
        {
            auto output = m_output.get();
            if (!output)
            {
                return {};
            }
            return { output };
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline std::vector<const Pin*> FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPins() const
        {
            auto output = m_output.get();
            if (!output)
            {
                return {};
            }
            return { output };
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline FunctionNode<_FunctionType, OutputType, InputTypes...>::FunctionNode(Script& script) :
            FunctionNodeBase(script),
            m_output(std::make_unique<OutputPin<OutputType> >(*this))
        {
            InitInputPin<InputTypes...>();
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline FunctionNode<_FunctionType, OutputType, InputTypes...>::~FunctionNode()
        {
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        template<typename CurrentType, typename ...>
        inline void FunctionNode<_FunctionType, OutputType, InputTypes...>::InitInputPin(const size_t index)
        {
            if (index == InputPinCount)
            {
                return;
            }

            auto inputPin = std::make_unique<InputPin<CurrentType> >(*this);
            m_inputs[index] = std::move(inputPin);

            InitInputPin<CurrentType>(index + 1);
        }       


        // Shader script operator node implementations.
        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        ArithmeticOperatorType ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetArithmeticOperatorType() const
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
            return std::vector<Pin*>{&m_inputLeft, & m_inputRight}[index];
        }
        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        inline const Pin* ArithmeticOperatorNode<Operator, OutputType, LeftType, RightType>::GetInputPin(const size_t index) const
        {
            if (index > 1)
            {
                return nullptr;
            }
            return std::vector<const Pin*>{&m_inputLeft, &m_inputRight}[index];
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


        // Input node implementations.
        template<typename T>
        inline size_t InputNode<T>::GetOutputPinCount() const
        {
            return 1;
        }

        template<typename T>
        inline Pin* InputNode<T>::GetOutputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }
        template<typename T>
        inline const Pin* InputNode<T>::GetOutputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }

        template<typename T>
        inline std::vector<Pin*> InputNode<T>::GetOutputPins()
        {
            return { &m_output };
        }
        template<typename T>
        inline std::vector<const Pin*> InputNode<T>::GetOutputPins() const
        {
            return { &m_output };
        }

        template<typename T>
        inline InputNode<T>::InputNode(Script& script) :
            InputNodeBase(script),
            m_output(*this)
        { }

        template<typename T>
        inline InputNode<T>::~InputNode()
        { }


        // Input block implementations.
        template<typename T>
        inline InputNode<T>* InputBlock::AppendNode()
        {
            auto inputNode = new InputNode<T>(m_script);
            m_nodes.push_back(inputNode);
            m_pinCount += inputNode->GetOutputPinCount();
            return inputNode;
        }


        // Output node implementations.
        template<typename T>
        inline size_t OutputNode<T>::GetInputPinCount() const
        {
            return 1;
        }

        template<typename T>
        inline Pin* OutputNode<T>::GetInputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_input;
        }
        template<typename T>
        inline const Pin* OutputNode<T>::GetInputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_input;
        }
   
        template<typename T>
        inline std::vector<Pin*> OutputNode<T>::GetInputPins()
        {
            return { &m_input };
        }
        template<typename T>
        inline std::vector<const Pin*> OutputNode<T>::GetInputPins() const
        {
            return { &m_input };
        }

        template<typename T>
        inline OutputNode<T>::OutputNode(Script& script) :
            OutputNodeBase(script),
            m_input(*this)
        { }

        template<typename T>
        inline OutputNode<T>::~OutputNode()
        { }


        // Output block implementations.
        template<typename T>
        inline OutputNode<T>* OutputBlock::AppendNode()
        {
            auto outputNode = new OutputNode<T>(m_script);
            m_nodes.push_back(outputNode);
            m_pinCount += outputNode->GetInputPinCount();
            return outputNode;
        }


        // Uniform node implementations.
        template<typename T>
        size_t UniformNode<T>::GetOutputPinCount() const
        {
            return 1;
        }

        template<typename T>
        Pin* UniformNode<T>::GetOutputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }
        template<typename T>
        const Pin* UniformNode<T>::GetOutputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }
        
        template<typename T>
        std::vector<Pin*> UniformNode<T>::GetOutputPins()
        {
            return { &m_output };
        }
        template<typename T>
        std::vector<const Pin*> UniformNode<T>::GetOutputPins() const
        {
            return { &m_output };
        }
        
        template<typename T>
        UniformNode<T>::UniformNode(Script& script) :
            UniformNodeBase(script),
            m_output(*this)
        { }

        template<typename T>
        UniformNode<T>::~UniformNode()
        { }


        // Uniform block implementations.
        template<typename T>
        inline UniformNode<T>* UniformBlock::AppendNode()
        {
            auto uniformNode = new UniformNode<T>(m_script);
            m_nodes.push_back(uniformNode);
            m_pinCount += uniformNode->GetOutputPinCount();
            return uniformNode;
        }




        //// Shader script uniform base implementations.
        //inline UniformNodeBase::~UniformNodeBase()
        //{ }

        //inline NodeType UniformNodeBase::GetType() const
        //{
        //    return NodeType::Uniform;
        //}

        //inline UniformNodeBase::UniformNodeBase(Script& script) :
        //    Node(script)
        //{ }

        //// Shader script uniform base implementations.
        //template<typename T>
        //inline size_t UniformNode<T>::GetOutputPinCount() const
        //{
        //    return 1;
        //}

        //template<typename T>
        //inline Pin* UniformNode<T>::GetOutputPin(const size_t index)
        //{
        //    if (index != 0)
        //    {
        //        return nullptr;
        //    }
        //    return &m_output;
        //}
        //template<typename T>
        //inline const Pin* UniformNode<T>::GetOutputPin(const size_t index) const
        //{
        //    if (index != 0)
        //    {
        //        return nullptr;
        //    }
        //    return &m_output;
        //}

        //template<typename T>
        //inline std::vector<Pin*> UniformNode<T>::GetOutputPins()
        //{
        //    return { &m_output };
        //}
        //template<typename T>
        //inline std::vector<const Pin*> UniformNode<T>::GetOutputPins() const
        //{
        //    return { &m_output };
        //}

        //template<typename T>
        //inline UniformNode<T>::UniformNode(Script& script) :
        //    UniformNodeBase(script),
        //    m_output(*this)
        //{ }

        //// Uniform block implementations.
        //inline UniformBlock::~UniformBlock()
        //{
        //    for (auto* uniform : m_uniformNodes)
        //    {
        //        delete uniform;
        //    }
        //}

        //template<typename T>
        //inline UniformNode<T>* UniformBlock::AppendUniformNode()
        //{
        //    auto uniformNode = new UniformNode<T>(m_script);
        //    m_uniformNodes.push_back(uniformNode);
        //    return uniformNode;
        //}

        //inline std::vector<UniformNodeBase*> UniformBlock::GetUniformNodes()
        //{
        //    return m_uniformNodes;
        //}

        //inline std::vector<const UniformNodeBase*> UniformBlock::GetUniformNodes() const
        //{
        //    return { m_uniformNodes.begin(), m_uniformNodes.end() };
        //}

        //inline uint32_t UniformBlock::GetId() const
        //{
        //    return m_id;
        //}

        //inline bool UniformBlock::HasSameLayout(const UniformBlock& uniformBlock) const
        //{
        //    auto uniformNodes = uniformBlock.GetUniformNodes();
        //    if (m_uniformNodes.size() != uniformNodes.size())
        //    {
        //        return false;
        //    }

        //    for (size_t i = 0; i < m_uniformNodes.size(); i++)
        //    {
        //        if (m_uniformNodes[i]->GetOutputPin()->GetDataType() != uniformNodes[i]->GetOutputPin()->GetDataType())
        //        {
        //            return false;
        //        }
        //    }

        //    return true;
        //}

        //inline UniformBlock::UniformBlock(Script& script, const uint32_t id) :
        //    m_script(script),
        //    m_id(id)
        //{ }


        //// Shader script varying in node implementations.
        //template<typename T>
        //inline size_t VaryingInNode<T>::GetOutputPinCount() const
        //{
        //    return 1;
        //}

        //template<typename T>
        //inline Pin* VaryingInNode<T>::GetOutputPin(const size_t index)
        //{
        //    if (index != 0)
        //    {
        //        return nullptr;
        //    }
        //    return &m_pin;
        //}
        //template<typename T>
        //inline const Pin* VaryingInNode<T>::GetOutputPin(const size_t index) const
        //{
        //    if (index != 0)
        //    {
        //        return nullptr;
        //    }
        //    return &m_pin;
        //}

        //template<typename T>
        //inline std::vector<Pin*> VaryingInNode<T>::GetOutputPins()
        //{
        //    return { &m_pin };
        //}
        //template<typename T>
        //inline std::vector<const Pin*> VaryingInNode<T>::GetOutputPins() const
        //{
        //    return { &m_pin };
        //}

        //template<typename T>
        //inline NodeType VaryingInNode<T>::GetType() const
        //{
        //    return NodeType::VaryingIn;
        //}

        //template<typename T>
        //inline uint32_t VaryingInNode<T>::GetId() const
        //{
        //    return m_id;
        //}

        //template<typename T>
        //inline VaryingInNode<T>::VaryingInNode(Script& script, const uint32_t id) :
        //    Node(script),
        //    m_id(id),
        //    m_pin(*this)
        //{ }


        //// Shader script varying out node implementations.
        //template<typename T>
        //inline size_t VaryingOutNode<T>::GetInputPinCount() const
        //{
        //    return 1;
        //}

        //template<typename T>
        //inline Pin* VaryingOutNode<T>::GetInputPin(const size_t index)
        //{
        //    if (index != 0)
        //    {
        //        return nullptr;
        //    }
        //    return &m_pin;
        //}
        //template<typename T>
        //inline const Pin* VaryingOutNode<T>::GetInputPin(const size_t index) const
        //{
        //    if (index != 0)
        //    {
        //        return nullptr;
        //    }
        //    return &m_pin;
        //}

        //template<typename T>
        //inline std::vector<Pin*> VaryingOutNode<T>::GetInputPins()
        //{
        //    return { &m_pin };
        //}
        //template<typename T>
        //inline std::vector<const Pin*> VaryingOutNode<T>::GetInputPins() const
        //{
        //    return { &m_pin };
        //}

        //template<typename T>
        //inline NodeType VaryingOutNode<T>::GetType() const
        //{
        //    return NodeType::VaryingOut;
        //}

        //template<typename T>
        //inline uint32_t VaryingOutNode<T>::GetId() const
        //{
        //    return m_id;
        //}

        //template<typename T>
        //inline VaryingOutNode<T>::VaryingOutNode(Script& script, const uint32_t id) :
        //    Node(script),
        //    m_id(id),
        //    m_pin(*this)
        //{ }


        //// Shader script vertex output node implementations.
        //inline size_t VertexOutputNode::GetInputPinCount() const
        //{
        //    return 1;
        //}

        //inline Pin* VertexOutputNode::GetInputPin(const size_t index)
        //{
        //    if (index != 0)
        //    {
        //        return nullptr;
        //    }
        //    return &m_pin;
        //}     
        //inline const Pin* VertexOutputNode::GetInputPin(const size_t index) const
        //{
        //    if (index != 0)
        //    {
        //        return nullptr;
        //    }
        //    return &m_pin;
        //}

        //inline std::vector<Pin*> VertexOutputNode::GetInputPins()
        //{
        //    return { &m_pin };
        //}
        //inline std::vector<const Pin*> VertexOutputNode::GetInputPins() const
        //{
        //    return { &m_pin };
        //}

        //inline NodeType VertexOutputNode::GetType() const
        //{
        //    return NodeType::VertexOutput;
        //}

        //inline VertexOutputNode::VertexOutputNode(Script& script) :
        //    Node(script),
        //    m_pin(*this)
        //{ }

    }

}