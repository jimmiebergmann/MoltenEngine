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

#ifndef CURSE_CORE_RENDERER_SHADER_SHADERSCRIPT_HPP
#define CURSE_CORE_RENDERER_SHADER_SHADERSCRIPT_HPP

#include "Curse/Renderer/Shader/Node/ShaderConstantNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderFunctionNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderInputNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderOperatorNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderOutputNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderUniformNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderVertexOutputNode.hpp"
#include "Curse/Renderer/Shader/ShaderFunctions.hpp"
#include "Curse/Renderer/Shader/ShaderOperators.hpp"
#include "Curse/Logger.hpp"
#include <vector>
#include <set>
#include <map>
#include <string>

namespace Curse
{

    namespace Shader
    {

        /**
        * @brief Shader script class, used for generating shaders via virtual script.
        */
        class CURSE_API Script
        {

        public:

            /**
            * @brief Destructor.
            */
            virtual ~Script();

            /**
            * @brief Get type of shader script.
            */
            virtual Type GetType() const = 0;

            /**
            * @brief Removes the node from the script, disconnects all connections of node
            *        and deallocates the pointer.
            */
            virtual void DestroyNode(Node * node) = 0;

            /**
            * @brief Get number of nodes in script.
            */
            virtual size_t GetNodeCount() const = 0;

            /**
            * @brief Get all nodes of shader script.
            */
            /**@{*/
            virtual std::vector<Node*> GetNodes() = 0;
            virtual std::vector<const Node*> GetNodes() const = 0;
            /**@}*/

            /**
            * @brief Get output block.
            */
            /**@{*/
            virtual InputBlock& GetInputBlock() = 0;
            virtual const InputBlock& GetInputBlock() const = 0;
            /**@}*/

            /**
            * @brief Get output block.
            */
            /**@{*/
            virtual OutputBlock& GetOutputBlock() = 0;
            virtual const OutputBlock& GetOutputBlock() const = 0;
            /**@}*/

            /**
            * @brief Get number of uniform blocks.
            */
            virtual const size_t GetUniformBlockCount() const = 0;

            /**
            * @brief Get all uniform blocks in this script.
            */
            /**@{*/
            virtual std::vector<UniformBlock*> GetUniformBlocks() = 0;
            virtual std::vector<const UniformBlock*> GetUniformBlocks() const = 0;
            /**@}*/ 

        protected:

            /**
            * @brief Get vertex output node.
            */
            /**@{*/
            virtual VertexOutputNode* GetVertexOutputNode();
            virtual const VertexOutputNode* GetVertexOutputNode() const;
            /**@}*/

        };


        /**
        * @brief Vertex shader script.
        */
        class CURSE_API VertexScript : public Script
        {

        public:

            /**
            * @brief Constructor.
            */
            VertexScript();

            /**
            * @brief Destructor.
            */
            ~VertexScript();

            /**
            * @brief Create new constant node and append it to the shader script.
            *
            * @tparam T Constant node object to create.
            */
            template<typename T>
            ConstantNode<T>* CreateConstantNode(const T& value = VariableTrait<T>::DefaultValue);

            /**
            * @brief Create new function node and append it to the shader script.
            *
            * @tparam T Function node object to create and data types for parameters.
            */
            template<typename Func>
            Func* CreateFunctionNode();

            /**
            * @brief Create new operator node and append it to the shader script.
            *
            * @tparam T Operator node object to create.
            */
            template<typename Op>
            Op* CreateOperatorNode();

            /**
            * @brief Create a new uniform block.
            *
            * @param id Id of the block. The id is used for sharing uniform data between shader stages.
            *
            * @return Pointer to the new block.
            */
            UniformBlock* CreateUniformBlock(const uint32_t id);

            /**
            * @brief Get type of shader script.
            */
            virtual Type GetType() const override;

            /**
            * @brief Removes the node from the script, disconnects all connections of node
            *        and deallocates the pointer.
            */
            virtual void DestroyNode(Node* node) override;

            /**
            * @brief Get number of nodes in script.
            */
            virtual size_t GetNodeCount() const override;

            /**
            * @brief Get all nodes of shader script.
            */
            /**@{*/
            virtual std::vector<Node*> GetNodes() override;
            virtual std::vector<const Node*> GetNodes() const override;
            /**@}*/

            /**
            * @brief Get output block.
            */
            /**@{*/
            virtual InputBlock& GetInputBlock() override;
            virtual const InputBlock& GetInputBlock() const override;
            /**@}*/

            /**
            * @brief Get output block.
            */
            /**@{*/
            virtual OutputBlock& GetOutputBlock() override;
            virtual const OutputBlock& GetOutputBlock() const override;
            /**@}*/
            
            /**
            * @brief Get vertex output node.
            */
            /**@{*/
            virtual VertexOutputNode* GetVertexOutputNode() override;
            virtual const VertexOutputNode* GetVertexOutputNode() const override;
            /**@}*/

            /**
            * @brief Get number of uniform blocks.
            */
            virtual const size_t GetUniformBlockCount() const override;

            /**
            * @brief Get all uniform blocks in this script.
            */
            /**@{*/
            virtual std::vector<UniformBlock*> GetUniformBlocks() override;
            virtual std::vector<const UniformBlock*> GetUniformBlocks() const override;
            /**@}*/     

        private:

            std::set<Node*> m_allNodes;
            InputBlock m_inputBlock;
            OutputBlock m_outputBlock;
            VertexOutputNode m_vertexOutputNode;
            std::map<uint32_t, UniformBlock*> m_uniformBlocks;

        };


        /**
        * @brief Fragment shader script.
        */
        class CURSE_API FragmentScript : public Script
        {

        public:

            /**
            * @brief Constructor.
            */
            FragmentScript();

            /**
            * @brief Destructor.
            */
            ~FragmentScript();

            /**
            * @brief Create new constant node and append it to the shader script.
            *
            * @tparam T Constant node object to create.
            */
            template<typename T>
            ConstantNode<T>* CreateConstantNode(const T& value = VariableTrait<T>::DefaultValue);

            /**
            * @brief Create new function node and append it to the shader script.
            *
            * @tparam T Function node object to create and data types for parameters.
            */
            template<typename Func>
            Func* CreateFunctionNode();

            /**
            * @brief Create new operator node and append it to the shader script.
            *
            * @tparam T Operator node object to create.
            */
            template<typename Op>
            Op* CreateOperatorNode();

            /**
            * @brief Create a new uniform block.
            *
            * @param id Id of the block. The id is used for sharing uniform data between shader stages.
            *
            * @return Pointer to the new block.
            */
            UniformBlock* CreateUniformBlock(const uint32_t id);

            /**
            * @brief Get type of shader script.
            */
            virtual Type GetType() const override;

            /**
            * @brief Removes the node from the script, disconnects all connections of node
            *        and deallocates the pointer.
            */
            virtual void DestroyNode(Node* node) override;

            /**
            * @brief Get number of nodes in script.
            */
            virtual size_t GetNodeCount() const override;

            /**
            * @brief Get all nodes of shader script.
            */
            /**@{*/
            virtual std::vector<Node*> GetNodes() override;
            virtual std::vector<const Node*> GetNodes() const override;
            /**@}*/

            /**
            * @brief Get output block.
            */
            /**@{*/
            virtual InputBlock& GetInputBlock() override;
            virtual const InputBlock& GetInputBlock() const override;
            /**@}*/

            /**
            * @brief Get output block.
            */
            /**@{*/
            virtual OutputBlock& GetOutputBlock() override;
            virtual const OutputBlock& GetOutputBlock() const override;
            /**@}*/

            /**
            * @brief Get number of uniform blocks.
            */
            virtual const size_t GetUniformBlockCount() const override;

            /**
            * @brief Get all uniform blocks in this script.
            */
            /**@{*/
            virtual std::vector<UniformBlock*> GetUniformBlocks() override;
            virtual std::vector<const UniformBlock*> GetUniformBlocks() const override;
            /**@}*/

        private:

            std::set<Node*> m_allNodes;
            InputBlock m_inputBlock;
            OutputBlock m_outputBlock;
            std::map<uint32_t, UniformBlock*> m_uniformBlocks;

        };

    }

}

#include "Curse/Renderer/Shader/ShaderScript.inl"

#endif