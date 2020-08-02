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
        //m_pushConstantInterface(*this),
        m_vertexOutputVariable(*this)
    {}

    VertexScript::~VertexScript()
    {
        for (auto* node : m_allNodes)
        {
            delete node;
        }
    }

    /*UniformBlock* VertexScript::CreateUniformBlock(const uint32_t id)
    {
        if (m_uniformBlocks.find(id) != m_uniformBlocks.end())
        {
            return nullptr;
        }

        auto uniformBlock = new UniformBlock(*this, id);
        m_uniformBlocks.insert({ id, uniformBlock });
        return uniformBlock;
    }*/

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

    /*InputStructure& VertexScript::GetPushConstantInterface()
    {
        return m_pushConstantInterface;
    }
    const InputStructure& VertexScript::GetPushConstantInterface() const
    {
        return m_pushConstantInterface;
    }*/

    VertexOutputVariable* VertexScript::GetVertexOutputVariable()
    {
        return &m_vertexOutputVariable;
    }
    const VertexOutputVariable* VertexScript::GetVertexOutputVariable() const
    {
        return &m_vertexOutputVariable;
    }

    /*const size_t VertexScript::GetUniformBlockCount() const
    {
        return m_uniformBlocks.size();
    }

    std::vector<UniformBlock*> VertexScript::GetUniformBlocks()
    {
        std::vector<UniformBlock*> uniformBlocks;
        for (auto& pair : m_uniformBlocks)
        {
            uniformBlocks.push_back(pair.second);
        }
        return uniformBlocks;
    }
    std::vector<const UniformBlock*> VertexScript::GetUniformBlocks() const
    {
        std::vector<const UniformBlock*> uniformBlocks;
        for (auto& pair : m_uniformBlocks)
        {
            uniformBlocks.push_back(pair.second);
        }
        return uniformBlocks;
    }*/


    // Fragment shader script implementations.
    FragmentScript::FragmentScript() :
        m_inputInterface(*this),
        m_outputInterface(*this)//,
        //m_pushConstantInterface(*this)
    {}

    FragmentScript::~FragmentScript()
    {
        for (auto* node : m_allNodes)
        {
            delete node;
        }
    }

    /*UniformBlock* FragmentScript::CreateUniformBlock(const uint32_t id)
    {
        if (m_uniformBlocks.find(id) != m_uniformBlocks.end())
        {
            return nullptr;
        }

        auto uniformBlock = new UniformBlock(*this, id);
        m_uniformBlocks.insert({ id, uniformBlock });
        return uniformBlock;
    }*/

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

    /*InputStructure& FragmentScript::GetPushConstantInterface()
    {
        return m_pushConstantInterface;
    }
    const InputStructure& FragmentScript::GetPushConstantInterface() const
    {
        return m_pushConstantInterface;
    }

    const size_t FragmentScript::GetUniformBlockCount() const
    {
        return m_uniformBlocks.size();
    }

    std::vector<UniformBlock*> FragmentScript::GetUniformBlocks()
    {
        std::vector<UniformBlock*> uniformBlocks;
        for (auto& pair : m_uniformBlocks)
        {
            uniformBlocks.push_back(pair.second);
        }
        return uniformBlocks;
    }
    std::vector<const UniformBlock*> FragmentScript::GetUniformBlocks() const
    {
        std::vector<const UniformBlock*> uniformBlocks;
        for (auto& pair : m_uniformBlocks)
        {
            uniformBlocks.push_back(pair.second);
        }
        return uniformBlocks;
    }*/

}