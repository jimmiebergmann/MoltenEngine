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

#include "Curse/Renderer/Shader/ShaderNode.hpp"

namespace Curse
{

    namespace Shader
    {

        // Shader script node implementations.
        Script& Node::GetScript()
        {
            return m_script;
        }
        const Script& Node::GetScript() const
        {
            return m_script;
        }

        size_t Node::GetInputPinCount() const
        {
            return 0;
        }

        size_t Node::GetOutputPinCount() const
        {
            return 0;
        }

        Pin* Node::GetInputPin(const size_t)
        {
            return nullptr;
        }
        const Pin* Node::GetInputPin(const size_t) const
        {
            return nullptr;
        }

        std::vector<Pin*> Node::GetInputPins()
        {
            return {};
        }
        std::vector<const Pin*> Node::GetInputPins() const
        {
            return {};
        }

        Pin* Node::GetOutputPin(const size_t)
        {
            return nullptr;
        }
        const Pin* Node::GetOutputPin(const size_t) const
        {
            return nullptr;
        }

        std::vector<Pin*> Node::GetOutputPins()
        {
            return {};
        }
        std::vector<const Pin*> Node::GetOutputPins() const
        {
            return {};
        }

        Node::Node(Script& script) :
            m_script(script)
        { }

        Node::~Node()
        {
            auto inputs = GetInputPins();
            for (auto& input : inputs)
            {
                input->Disconnect();
            }

            auto outputs = GetOutputPins();
            for (auto& output : outputs)
            {
                output->Disconnect();
            }
        }


        // Constant node base implementations.
        NodeType ConstantNodeBase::GetType() const
        {
            return NodeType::Constant;
        }

        ConstantNodeBase::ConstantNodeBase(Script& script) :
            Node(script)
        { }

        ConstantNodeBase::~ConstantNodeBase()
        { }


        // Function node base implementations.
        NodeType FunctionNodeBase::GetType() const
        {
            return NodeType::Function;
        }

        FunctionNodeBase::FunctionNodeBase(Script& script) :
            Node(script)
        { }

        FunctionNodeBase::~FunctionNodeBase()
        { }


        // Operator node base implementations.
        NodeType OperatorNodeBase::GetType() const
        {
            return NodeType::Operator;
        }

        OperatorNodeBase::OperatorNodeBase(Script& script) :
            Node(script)
        { }

        OperatorNodeBase::~OperatorNodeBase()
        { }


        // Arithmetic operator node base implementations.
        OperatorType ArithmeticOperatorNodeBase::GetOperatorType() const
        {
            return OperatorType::Arithmetic;
        }

        ArithmeticOperatorNodeBase::ArithmeticOperatorNodeBase(Script& script) :
            OperatorNodeBase(script)
        { }

        ArithmeticOperatorNodeBase::~ArithmeticOperatorNodeBase()
        { }


        // Input node base implementations.
        NodeType InputNodeBase::GetType() const
        {
            return NodeType::Input;
        }

        InputNodeBase::InputNodeBase(Script& script) :
            Node(script)
        { }

        InputNodeBase::~InputNodeBase()
        { }


        // Input block implementations.
        void InputBlock::DestroyNode(InputNodeBase* inputNode)
        {
            for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
            {
                if (*it == inputNode)
                {
                    m_nodes.erase(it);
                    m_pinCount -= inputNode->GetOutputPinCount();
                    delete inputNode;                  
                    return;
                }
            }
        }

        size_t InputBlock::GetNodeCount() const
        {
            return m_nodes.size();
        }

        size_t InputBlock::GetOutputPinCount() const
        {
            return m_pinCount;
        }

        std::vector<InputNodeBase*> InputBlock::GetNodes()
        {
            return m_nodes;
        }
        std::vector<const InputNodeBase*> InputBlock::GetNodes() const
        {
            return { m_nodes.begin(), m_nodes.end() };
        }

        bool InputBlock::CheckCompability(const OutputBlock& block) const
        {
            auto blockNodes = block.GetNodes();

            if (m_nodes.size() != blockNodes.size())
            {
                return false;
            }

            for (size_t i = 0; i < m_nodes.size(); i++)
            {
                auto nodeA = m_nodes[i];
                auto nodeB = blockNodes[i];

                if (nodeA->GetOutputPinCount() != nodeB->GetInputPinCount())
                {
                    return false;
                }

                for (size_t j = 0; j < nodeA->GetOutputPinCount(); j++)
                {
                    auto pinA = nodeA->GetOutputPin(j);
                    auto pinB = nodeB->GetInputPin(j);

                    if (pinA->GetDataType() != pinB->GetDataType())
                    {
                        return false;
                    }
                }

            }

            return true;
        }

        InputBlock::InputBlock(Script& script) :
            m_script(script),
            m_pinCount(0)
        { }

        InputBlock::~InputBlock()
        {
            for (auto node : m_nodes)
            {
                delete node;
            }
        }


        // Output node base implementations.
        NodeType OutputNodeBase::GetType() const
        {
            return NodeType::Output;
        }

        OutputNodeBase::OutputNodeBase(Script& script) :
            Node(script)
        { }

        OutputNodeBase::~OutputNodeBase()
        { }


        // Output block implementations.
        void OutputBlock::DestroyNode(OutputNodeBase* outputNode)
        {
            for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
            {
                if (*it == outputNode)
                {
                    m_nodes.erase(it);
                    m_pinCount -= outputNode->GetInputPinCount();
                    delete outputNode;
                    return;
                }
            }
        }

        size_t OutputBlock::GetNodeCount() const
        {
            return m_nodes.size();
        }

        size_t OutputBlock::GetInputPinCount() const
        {
            return m_pinCount;
        }

        std::vector<OutputNodeBase*> OutputBlock::GetNodes()
        {
            return m_nodes;
        }
        std::vector<const OutputNodeBase*> OutputBlock::GetNodes() const
        {
            return { m_nodes.begin(), m_nodes.end() };
        }

        OutputBlock::OutputBlock(Script& script) :
            m_script(script),
            m_pinCount(0)
        { }

        OutputBlock::~OutputBlock()
        {
            for (auto node : m_nodes)
            {
                delete node;
            }
        }


        // Vertex output node implementations.
        NodeType VertexOutputNode::GetType() const
        {
            return NodeType::VertexOutput;
        }

        size_t VertexOutputNode::GetInputPinCount() const
        {
            return 1;
        }

        Pin* VertexOutputNode::GetInputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_input;
        }
        const Pin* VertexOutputNode::GetInputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_input;
        }

        std::vector<Pin*> VertexOutputNode::GetInputPins()
        {
            return { &m_input };
        }
        std::vector<const Pin*> VertexOutputNode::GetInputPins() const
        {
            return { &m_input };
        }
 
        VertexOutputNode::VertexOutputNode(Script& script) :
            Node(script),
            m_input(*this)
        { }

        VertexOutputNode::~VertexOutputNode()
        { }


        // Uniform base class implementations.
        NodeType UniformNodeBase::GetType() const
        {
            return NodeType::Uniform;
        }

        UniformNodeBase::UniformNodeBase(Script& script) :
            Node(script)
        { }

        UniformNodeBase::~UniformNodeBase()
        { }


        // Uniform block implementations.
        void UniformBlock::DestroyNode(UniformNodeBase* uniformNode)
        {
            for (auto it = m_nodes.begin(); it != m_nodes.end(); it++)
            {
                if (*it == uniformNode)
                {
                    m_nodes.erase(it);
                    m_pinCount -= uniformNode->GetOutputPinCount();
                    delete uniformNode;
                    return;
                }
            }
        }

        size_t UniformBlock::GetNodeCount() const
        {
            return m_nodes.size();
        }

        size_t UniformBlock::GetOutputPinCount() const
        {
            return m_pinCount;
        }

        std::vector<UniformNodeBase*> UniformBlock::GetNodes()
        {
            return m_nodes;
        }
        std::vector<const UniformNodeBase*> UniformBlock::GetNodes() const
        {
            return { m_nodes.begin(), m_nodes.end() };
        }

        uint32_t UniformBlock::GetId() const
        {
            return m_id;
        }

        bool UniformBlock::CheckCompability(const UniformBlock& block) const
        {
            auto& blockNodes = block.m_nodes;

            if (m_nodes.size() != blockNodes.size())
            {
                return false;
            }

            for (size_t i = 0; i < m_nodes.size(); i++)
            {
                auto nodeA = m_nodes[i];
                auto nodeB = blockNodes[i];

                if (nodeA->GetOutputPinCount() != nodeB->GetInputPinCount())
                {
                    return false;
                }

                for (size_t j = 0; j < nodeA->GetOutputPinCount(); j++)
                {
                    auto pinA = nodeA->GetOutputPin(j);
                    auto pinB = nodeB->GetInputPin(j);

                    if (pinA->GetDataType() != pinB->GetDataType())
                    {
                        return false;
                    }
                }

            }

            return true;
        }

        UniformBlock::UniformBlock(Script& script, const uint32_t id) :
            m_script(script),
            m_id(id),
            m_pinCount(0)
        { }

        UniformBlock::~UniformBlock()
        {
            for (auto node : m_nodes)
            {
                delete node;
            }
        }

    }

}