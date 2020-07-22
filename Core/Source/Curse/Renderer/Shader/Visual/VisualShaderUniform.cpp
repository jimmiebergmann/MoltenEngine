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

#include "Curse/Renderer/Shader/Node/ShaderUniformNode.hpp"

namespace Curse
{

    namespace Shader
    {

        // Uniform base class implementations.
        NodeType UniformNodeBase::GetType() const
        {
            return NodeType::Uniform;
        }

        bool UniformNodeBase::IsArray() const
        {
            return false;
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