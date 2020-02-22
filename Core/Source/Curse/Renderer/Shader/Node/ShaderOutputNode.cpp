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

#include "Curse/Renderer/Shader/Node/ShaderOutputNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderInputNode.hpp"

namespace Curse
{

    namespace Shader
    {

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

    }

}