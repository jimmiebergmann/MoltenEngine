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

#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"

namespace Molten::Shader::Visual
{

    // Shader script implementations.
    VertexOutputVariable* Script::GetVertexOutputVariable()
    {
        return nullptr;
    }
    const VertexOutputVariable* Script::GetVertexOutputVariable() const
    {
        return nullptr;
    }


    // Vertex shader script implementations.
    VertexScript::VertexScript() :
        m_inputInterface(*this),
        m_outputInterface(*this),
        m_uniformInterfaces(*this),
        m_pushConstantInterface(*this),
        m_vertexOutputVariable(*this)
    {}

    VertexScript::~VertexScript()
    {
        for (auto* node : m_allNodes)
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
        auto itAll = m_allNodes.find(node);
        if (itAll == m_allNodes.end())
        {
            return;
        }

        m_allNodes.erase(itAll);

        delete node;
    }

    size_t VertexScript::GetNodeCount() const
    {
        return m_allNodes.size();
    }

    std::vector<Node*> VertexScript::GetAllNodes()
    {
        return { m_allNodes.begin(), m_allNodes.end() };
    }
    std::vector<const Node*> VertexScript::GetAllNodes() const
    {
        return { m_allNodes.begin(), m_allNodes.end() };
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

    UniformInterfaces& VertexScript::GetUniformInterfaces()
    {
        return m_uniformInterfaces;
    }
    const UniformInterfaces& VertexScript::GetUniformInterfaces() const
    {
        return m_uniformInterfaces;
    }

    PushConstantInterface& VertexScript::GetPushConstantInterface()
    {
        return m_pushConstantInterface;
    }
    const PushConstantInterface& VertexScript::GetPushConstantInterface() const
    {
        return m_pushConstantInterface;
    }

    VertexOutputVariable* VertexScript::GetVertexOutputVariable()
    {
        return &m_vertexOutputVariable;
    }
    const VertexOutputVariable* VertexScript::GetVertexOutputVariable() const
    {
        return &m_vertexOutputVariable;
    }


    // Fragment shader script implementations.
    FragmentScript::FragmentScript() :
        m_inputInterface(*this),
        m_outputInterface(*this),
        m_uniformInterfaces(*this),
        m_pushConstantInterface(*this)
    {}

    FragmentScript::~FragmentScript()
    {
        for (auto* node : m_allNodes)
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
        auto itAll = m_allNodes.find(node);
        if (itAll == m_allNodes.end())
        {
            return;
        }

        m_allNodes.erase(itAll);

        delete node;
    }

    size_t FragmentScript::GetNodeCount() const
    {
        return m_allNodes.size();
    }

    std::vector<Node*> FragmentScript::GetAllNodes()
    {
        return { m_allNodes.begin(), m_allNodes.end() };
    }
    std::vector<const Node*> FragmentScript::GetAllNodes() const
    {
        return { m_allNodes.begin(), m_allNodes.end() };
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

    UniformInterfaces& FragmentScript::GetUniformInterfaces()
    {
        return m_uniformInterfaces;
    }
    const UniformInterfaces& FragmentScript::GetUniformInterfaces() const
    {
        return m_uniformInterfaces;
    }

    PushConstantInterface& FragmentScript::GetPushConstantInterface()
    {
        return m_pushConstantInterface;
    }
    const PushConstantInterface& FragmentScript::GetPushConstantInterface() const
    {
        return m_pushConstantInterface;
    }

}