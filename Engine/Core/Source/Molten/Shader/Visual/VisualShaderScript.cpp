/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#include "Molten/Shader/Visual/VisualShaderScript.hpp"

namespace Molten::Shader::Visual
{

    // Shader script implementations.
    VertexOutput* Script::GetVertexOutput()
    {
        return nullptr;
    }
    const VertexOutput* Script::GetVertexOutput() const
    {
        return nullptr;
    }


    // Vertex shader script implementations.
    VertexScript::VertexScript() :
        m_nodes{},   
        m_descriptorSets(*this),
        m_inputInterface(*this),
        m_outputInterface(*this),
        m_pushConstants(*this),
        m_vertexOutput(*this)
    {}

    VertexScript::~VertexScript()
    {
        for (auto* node : m_nodes)
        {
            delete node;
        }
    }

    Type VertexScript::GetType() const
    {
        return Type::Vertex;
    }

    void VertexScript::DestroyNode(Node* node)
    {
        auto it = m_nodes.find(node);
        if (it == m_nodes.end())
        {
            return;
        }
        delete node;
        m_nodes.erase(it);
    }

     size_t VertexScript::GetNodeCount() const
    {
        return m_nodes.size();
    }

    std::vector<Node*> VertexScript::GetNodes()
    {
        return { m_nodes.begin(), m_nodes.end() };
    }
    std::vector<const Node*> VertexScript::GetNodes() const
    {
        return { m_nodes.begin(), m_nodes.end() };
    }

    VertexOutput* VertexScript::GetVertexOutput()
    {
        return &m_vertexOutput;
    }
    const VertexOutput* VertexScript::GetVertexOutput() const
    {
        return &m_vertexOutput;
    }

    VertexDescriptorSets& VertexScript::GetDescriptorSets()
    {
        return m_descriptorSets;
    }
    const VertexDescriptorSets& VertexScript::GetDescriptorSets() const
    {
        return m_descriptorSets;
    }
    DescriptorSetsBase& VertexScript::GetDescriptorSetsBase()
    {
        return m_descriptorSets;
    }
    const DescriptorSetsBase& VertexScript::GetDescriptorSetsBase() const
    {
        return m_descriptorSets;
    }

    InputInterface& VertexScript::GetInputInterface()
    {
        return m_inputInterface;
    }
    const InputInterface& VertexScript::GetInputInterface() const
    {
        return m_inputInterface;
    }

    OutputInterface& VertexScript::GetOutputInterface()
    {
        return m_outputInterface;
    }
    const OutputInterface& VertexScript::GetOutputInterface() const
    {
        return m_outputInterface;
    }

    VertexPushConstants& VertexScript::GetPushConstants()
    {
        return m_pushConstants;
    }
    const VertexPushConstants& VertexScript::GetPushConstants() const
    {
        return m_pushConstants;
    }
    PushConstantsBase& VertexScript::GetPushConstantsBase()
    {
        return m_pushConstants;
    }
    const PushConstantsBase& VertexScript::GetPushConstantsBase() const
    {
        return m_pushConstants;
    }


    // Fragment shader script implementations.
    FragmentScript::FragmentScript() :
        m_nodes{},
        m_descriptorSets(*this),
        m_inputInterface(*this),
        m_outputInterface(*this),
        m_pushConstants(*this)
    {}

    FragmentScript::~FragmentScript()
    {
        for (auto* node : m_nodes)
        {
            delete node;
        }
    }

    Type FragmentScript::GetType() const
    {
        return Type::Fragment;
    }

    void FragmentScript::DestroyNode(Node* node)
    {
        auto it = m_nodes.find(node);
        if (it == m_nodes.end())
        {
            return;
        }
        delete node;
        m_nodes.erase(it);
    }

    size_t FragmentScript::GetNodeCount() const
    {
        return m_nodes.size();
    }

    std::vector<Node*> FragmentScript::GetNodes()
    {
        return { m_nodes.begin(), m_nodes.end() };
    }
    std::vector<const Node*> FragmentScript::GetNodes() const
    {
        return { m_nodes.begin(), m_nodes.end() };
    }

    FragmentDescriptorSets& FragmentScript::GetDescriptorSets()
    {
        return m_descriptorSets;
    }
    const FragmentDescriptorSets& FragmentScript::GetDescriptorSets() const
    {
        return m_descriptorSets;
    }
    DescriptorSetsBase& FragmentScript::GetDescriptorSetsBase()
    {
        return m_descriptorSets;
    }
    const DescriptorSetsBase& FragmentScript::GetDescriptorSetsBase() const
    {
        return m_descriptorSets;
    }

    InputInterface& FragmentScript::GetInputInterface()
    {
        return m_inputInterface;
    }
    const InputInterface& FragmentScript::GetInputInterface() const
    {
        return m_inputInterface;
    }

    OutputInterface& FragmentScript::GetOutputInterface()
    {
        return m_outputInterface;
    }
    const OutputInterface& FragmentScript::GetOutputInterface() const
    {
        return m_outputInterface;
    }

    FragmentPushConstants& FragmentScript::GetPushConstants()
    {
        return m_pushConstants;
    }
    const FragmentPushConstants& FragmentScript::GetPushConstants() const
    {
        return m_pushConstants;
    }
    PushConstantsBase& FragmentScript::GetPushConstantsBase()
    {
        return m_pushConstants;
    }
    const PushConstantsBase& FragmentScript::GetPushConstantsBase() const
    {
        return m_pushConstants;
    }

}