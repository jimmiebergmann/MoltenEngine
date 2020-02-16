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

#include "Curse/Renderer/Shader/ShaderScript.hpp"
#include "Curse/System/Exception.hpp"
#include <stack>
#include <map>
#include <memory>
#include <algorithm>

#if defined(CURSE_ENABLE_GLSLANG)
#include <glslang/public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>
#endif

#define CURSE_SHADERSCRIPT_LOG(severity, message) if(logger){ logger->Write(severity, message); }

namespace Curse
{

    namespace Shader
    {

        // Shader script implementations.
        Script::~Script()
        {
        }

        VertexOutputNode* Script::GetVertexOutputNode()
        {
            return nullptr;
        }
        const VertexOutputNode* Script::GetVertexOutputNode() const
        {
            return nullptr;
        }


        // Vertex shader script implementations.
        VertexScript::VertexScript() :
            m_inputBlock(*this),
            m_outputBlock(*this),
            m_vertexOutputNode(*this)
        {
        }

        VertexScript::~VertexScript()
        {
            for (auto* node : m_allNodes)
            {
                delete node;
            }
        }

        UniformBlock* VertexScript::CreateUniformBlock(const uint32_t id)
        {
            if (m_uniformBlocks.find(id) != m_uniformBlocks.end())
            {
                return nullptr;
            }

            auto uniformBlock = new UniformBlock(*this, id);
            m_uniformBlocks.insert({ id, uniformBlock });
            return uniformBlock;
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

        std::vector<Node*> VertexScript::GetNodes()
        {
            return { m_allNodes.begin(), m_allNodes.end() };
        }
        std::vector<const Node*> VertexScript::GetNodes() const
        {
            return { m_allNodes.begin(), m_allNodes.end() };
        }

        InputBlock& VertexScript::GetInputBlock()
        {
            return m_inputBlock;
        }
        const InputBlock& VertexScript::GetInputBlock() const
        {
            return m_inputBlock;
        }

        OutputBlock& VertexScript::GetOutputBlock()
        {
            return m_outputBlock;
        }
        const OutputBlock& VertexScript::GetOutputBlock() const
        {
            return m_outputBlock;
        }

        VertexOutputNode* VertexScript::GetVertexOutputNode()
        {
            return &m_vertexOutputNode;
        }
        const VertexOutputNode* VertexScript::GetVertexOutputNode() const
        {
            return &m_vertexOutputNode;
        }

        const size_t VertexScript::GetUniformBlockCount() const
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
        }


        // Fragment shader script implementations.
        FragmentScript::FragmentScript() :
            m_inputBlock(*this),
            m_outputBlock(*this)           
        {
        }

        FragmentScript::~FragmentScript()
        {
            for (auto* node : m_allNodes)
            {
                delete node;
            }
        }

        UniformBlock* FragmentScript::CreateUniformBlock(const uint32_t id)
        {
            if (m_uniformBlocks.find(id) != m_uniformBlocks.end())
            {
                return nullptr;
            }

            auto uniformBlock = new UniformBlock(*this, id);
            m_uniformBlocks.insert({ id, uniformBlock });
            return uniformBlock;
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

        std::vector<Node*> FragmentScript::GetNodes()
        {
            return { m_allNodes.begin(), m_allNodes.end() };
        }
        std::vector<const Node*> FragmentScript::GetNodes() const
        {
            return { m_allNodes.begin(), m_allNodes.end() };
        }

        InputBlock& FragmentScript::GetInputBlock()
        {
            return m_inputBlock;
        }
        const InputBlock& FragmentScript::GetInputBlock() const
        {
            return m_inputBlock;
        }

        OutputBlock& FragmentScript::GetOutputBlock()
        {
            return m_outputBlock;
        }
        const OutputBlock& FragmentScript::GetOutputBlock() const
        {
            return m_outputBlock;
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
        }

    }
}