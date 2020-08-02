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
/*
#include "Molten/Renderer/Shader/Node/ShaderPushConstantNode.hpp"

namespace Molten
{

    namespace Shader
    {

        // Uniform base class implementations.
        NodeType PushConstantNodeBase::GetType() const
        {
            return NodeType::PushConstant;
        }

        bool PushConstantNodeBase::IsArray() const
        {
            return false;
        }

        PushConstantNodeBase::PushConstantNodeBase(Script& script) :
            Node(script)
        { }

        PushConstantNodeBase::~PushConstantNodeBase()
        { }


        // Uniform block implementations.
        void PushConstantBlock::DestroyNode(PushConstantNodeBase* uniformNode)
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

        size_t PushConstantBlock::GetNodeCount() const
        {
            return m_nodes.size();
        }

        size_t PushConstantBlock::GetOutputPinCount() const
        {
            return m_pinCount;
        }

        std::vector<PushConstantNodeBase*> PushConstantBlock::GetNodes()
        {
            return m_nodes;
        }
        std::vector<const PushConstantNodeBase*> PushConstantBlock::GetNodes() const
        {
            return { m_nodes.begin(), m_nodes.end() };
        }*/

        /*bool PushConstantBlock::CheckCompability(const UniformBlock& block) const
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
        }*/
        /*
        PushConstantBlock::PushConstantBlock(Script& script) :
            m_script(script),
            m_pinCount(0)
        { }

        PushConstantBlock::~PushConstantBlock()
        {
            for (auto node : m_nodes)
            {
                delete node;
            }
        }

    }

}*/