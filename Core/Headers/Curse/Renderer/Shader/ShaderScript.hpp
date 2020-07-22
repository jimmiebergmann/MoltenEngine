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

#ifndef CURSE_CORE_RENDERER_SHADER_SHADERSCRIPT_HPP
#define CURSE_CORE_RENDERER_SHADER_SHADERSCRIPT_HPP

#include "Curse/Renderer/Shader/Node/ShaderConstantNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderFunctionNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderUniformNode.hpp"
#include "Curse/Renderer/Shader/Node/ShaderPushConstantNode.hpp"
#include "Curse/Renderer/Shader/ShaderFunctions.hpp"
#include "Curse/Renderer/Shader/ShaderOperators.hpp"
#include "Curse/Renderer/Shader/ShaderStructure.hpp"
#include "Curse/Logger.hpp"
#include <vector>
#include <set>
#include <map>
#include <string>

namespace Curse::Shader
{

    using VertexOutputNode = OutputVariable<Curse::Vector4f32>;

    /**
    * @brief Shader script class, used for generating shaders via virtual script.
    */
    class CURSE_API Script
    {

    public:

        /**
        * @brief Destructor.
        */
        virtual ~Script() = default;

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
        virtual std::vector<Node*> GetAllNodes() = 0;
        virtual std::vector<const Node*> GetAllNodes() const = 0;
        /**@}*/

        /**
        * @brief Get input variables interface block/structure.
        *        Data in this block is sent from the previous shader stage or from the vertex buffer.
        */
        /**@{*/
        virtual InputStructure& GetInputInterface() = 0;
        virtual const InputStructure& GetInputInterface() const = 0;
        /**@}*/

        /**
        * @brief Get output variables interface block/structure.
        *        Data in this block is sent to the next shader stage or to the frame buffer.
        */
        /**@{*/
        virtual OutputStructure& GetOutputInterface() = 0;
        virtual const OutputStructure& GetOutputInterface() const = 0;
        /**@}*/

        /**
        * @brief Get push constant interface block/structure.
        *        Data in this block is sent from the client at runtime.
        *        Maximum number of bytes available in this structure is 128.
        */
        /**@{*/
        virtual InputStructure& GetPushConstantInterface() = 0;
        virtual const InputStructure& GetPushConstantInterface() const = 0;
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
        * @brief Get vertex output variable.
        */
        /**@{*/
        virtual VertexOutputNode* GetVertexOutputVariable();
        virtual const VertexOutputNode* GetVertexOutputVariable() const;
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
        virtual std::vector<Node*> GetAllNodes() override;
        virtual std::vector<const Node*> GetAllNodes() const override;
        /**@}*/

        /**
        * @brief Get input variables interface block/structure.
        *        Data in this block is sent from the previous shader stage or from the vertex buffer.
        */
        /**@{*/
        virtual InputStructure& GetInputInterface() override;
        virtual const InputStructure& GetInputInterface() const override;
        /**@}*/

        /**
        * @brief Get output variables interface block/structure.
        *        Data in this block is sent to the next shader stage or to the frame buffer.
        */
        /**@{*/
        virtual OutputStructure& GetOutputInterface() override;
        virtual const OutputStructure& GetOutputInterface() const override;
        /**@}*/
       
        /**
        * @brief Get push constant interface block/structure.
        *        Data in this block is sent from the client at runtime.
        *        Maximum number of bytes available in this structure is 128.
        */
        /**@{*/
        virtual InputStructure& GetPushConstantInterface() override;
        virtual const InputStructure& GetPushConstantInterface() const override;
        /**@}*/

        /**
        * @brief Get vertex output variable.
        */
        /**@{*/
        virtual VertexOutputNode* GetVertexOutputVariable() override;
        virtual const VertexOutputNode* GetVertexOutputVariable() const override;
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
        
        std::map<uint32_t, UniformBlock*> m_uniformBlocks;

        InputStructure m_inputInterface;
        OutputStructure m_outputInterface;
        InputStructure m_pushConstantInterface;
        VertexOutputNode m_vertexOutputVariable;
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
        virtual std::vector<Node*> GetAllNodes() override;
        virtual std::vector<const Node*> GetAllNodes() const override;
        /**@}*/

        /**
        * @brief Get input variables interface block/structure.
        *        Data in this block is sent from the previous shader stage or from the vertex buffer.
        */
        /**@{*/
        virtual InputStructure& GetInputInterface() override;
        virtual const InputStructure& GetInputInterface() const override;
        /**@}*/

        /**
        * @brief Get output variables interface block/structure.
        *        Data in this block is sent to the next shader stage or to the frame buffer.
        */
        /**@{*/
        virtual OutputStructure& GetOutputInterface() override;
        virtual const OutputStructure& GetOutputInterface() const override;
        /**@}*/

        /**
        * @brief Get push constant interface block/structure.
        *        Data in this block is sent from the client at runtime.
        *        Maximum number of bytes available in this structure is 128.
        */
        /**@{*/
        virtual InputStructure& GetPushConstantInterface() override;
        virtual const InputStructure& GetPushConstantInterface() const override;
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
        std::map<uint32_t, UniformBlock*> m_uniformBlocks;

        InputStructure m_inputInterface;
        OutputStructure m_outputInterface;
        InputStructure m_pushConstantInterface;

    };

}

#include "Curse/Renderer/Shader/ShaderScript.inl"

#endif