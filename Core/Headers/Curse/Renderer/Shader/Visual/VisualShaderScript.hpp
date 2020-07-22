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

#ifndef CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERSCRIPT_HPP
#define CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERSCRIPT_HPP

//#include "Curse/Renderer/Shader/Visual/VisualShaderConstant.hpp"
#include "Curse/Renderer/Shader/Visual/VisualShaderFunction.hpp"
//#include "Curse/Renderer/Shader/Visual/VisualShaderUniform.hpp"
//#include "Curse/Renderer/Shader/Visual/VisualShaderPushConstant.hpp"
#include "Curse/Renderer/Shader/Visual/VisualShaderFunctions.hpp"
#include "Curse/Renderer/Shader/Visual/VisualShaderOperators.hpp"
#include "Curse/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include "Curse/Logger.hpp"
#include <vector>
#include <set>
#include <map>
#include <string>

namespace Curse::Shader::Visual
{

    /**
     * Type definition of vertex output variable node.
     * This type is used by the vertex shader. 
     */
    using VertexOutputVariable = OutputVariable<Curse::Vector4f32>;


    /** Visual shader script class, used for generating shaders via visual node based system. */
    class CURSE_API Script
    {

    public:

        /**  Destructor. */
        virtual ~Script() = default;

        /** Get type of shader script. */
        virtual Type GetType() const = 0;

        /** Removes node from script, disconnects all connections of node and deallocates the pointer. */
        virtual void DestroyNode(Node * node) = 0;

        /** Get number of nodes in this script. */
        virtual size_t GetNodeCount() const = 0;

        /** Get all nodes of this script. */
        /**@{*/
        virtual std::vector<Node*> GetAllNodes() = 0;
        virtual std::vector<const Node*> GetAllNodes() const = 0;
        /**@}*/

        /**
         * Get interface block for input variables.
         * Members of this block is sent from the previous shader stage or from the vertex buffer.
         */
        /**@{*/
        virtual InputStructure& GetInputInterface() = 0;
        virtual const InputStructure& GetInputInterface() const = 0;
        /**@}*/

        /**
         * Get interface block for output variables.
         * Data in this block is sent to the next shader stage or to the framebuffer.
         */
        /**@{*/
        virtual OutputStructure& GetOutputInterface() = 0;
        virtual const OutputStructure& GetOutputInterface() const = 0;
        /**@}*/

        /**
         * Get interface block for push constant variables.
         *  Data in this block is sent from the client at runtime.
         *  Maximum number of bytes available in this structure is 128.
         */
        /**@{*/
        //virtual InputStructure& GetPushConstantInterface() = 0;
        //virtual const InputStructure& GetPushConstantInterface() const = 0;
        /**@}*/

        /** Get number of uniform blocks in this script. */
        //virtual const size_t GetUniformBlockCount() const = 0;

        /** Get all uniform blocks in this script. */
        /**@{*/
        //virtual std::vector<UniformBlock*> GetUniformBlocks() = 0;
        //virtual std::vector<const UniformBlock*> GetUniformBlocks() const = 0;
        /**@}*/ 

    protected:

        /** Get vertex output variable. */
        /**@{*/
        virtual VertexOutputVariable* GetVertexOutputVariable();
        virtual const VertexOutputVariable* GetVertexOutputVariable() const;
        /**@}*/

    };


    /** Visual vertex shader script. */
    class CURSE_API VertexScript : public Script
    {

    public:

        /** Constructor. */
        VertexScript();

        /** Destructor. */
        ~VertexScript();

        /**
         * Create new variable node and append it to this script.
         *
         * @tparam TDataType Data type of variable node to create.
         */
        template<typename TDataType>
        ConstantVariable<TDataType>* CreateConstantVariable(const TDataType& value = VariableTrait<TDataType>::DefaultValue);

        /**
         * Create new function node and append it to this script.
         *
         * @tparam TFunction Function node object to create and data types for parameters.
         */
        template<typename TFunction>
        TFunction* CreateFunction();

        /**
         * Create new operator node and append it to the shader script.
         *
         * @tparam TOperator Operator node object to create.
         */
        template<typename TOperator>
        TOperator* CreateOperator();

        /**
         * Create a new uniform block.
         *
         * @param id Id of the block. The id is used for sharing uniform data between shader stages.
         *
         * @return Pointer to the new block.
         */
        //UniformBlock* CreateUniformBlock(const uint32_t id);

        /** Get type of shader script. */
        virtual Type GetType() const override;

        /** Removes node from script, disconnects all connections of node and deallocates the pointer. */
        virtual void DestroyNode(Node* node) override;

        /** Get number of nodes in this script. */
        virtual size_t GetNodeCount() const override;

        /** Get all nodes of this script. */
        /**@{*/
        virtual std::vector<Node*> GetAllNodes() override;
        virtual std::vector<const Node*> GetAllNodes() const override;
        /**@}*/

        /**
         * Get interface block for input variables.
         * Members of this block is sent from the previous shader stage or from the vertex buffer.
         */
         /**@{*/
        virtual InputStructure& GetInputInterface() override;
        virtual const InputStructure& GetInputInterface() const override;
        /**@}*/

        /**
         * Get interface block for output variables.
         * Data in this block is sent to the next shader stage or to the framebuffer.
         */
         /**@{*/
        virtual OutputStructure& GetOutputInterface() override;
        virtual const OutputStructure& GetOutputInterface() const override;
        /**@}*/

        /**
         * Get interface block for push constant variables.
         *  Data in this block is sent from the client at runtime.
         *  Maximum number of bytes available in this structure is 128.
         */
         /**@{*/
        //virtual InputStructure& GetPushConstantInterface() override;
        //virtual const InputStructure& GetPushConstantInterface() const override;
        /**@}*/

        /** Get number of uniform blocks in this script. */
        //virtual const size_t GetUniformBlockCount() const override;

        /** Get all uniform blocks in this script. */
        /**@{*/
        //virtual std::vector<UniformBlock*> GetUniformBlocks() override;
        //virtual std::vector<const UniformBlock*> GetUniformBlocks() const override;
        /**@}*/

        /** Get vertex output variable. */
        /**@{*/
        virtual VertexOutputVariable* GetVertexOutputVariable() override;
        virtual const VertexOutputVariable* GetVertexOutputVariable() const override;
        /**@}*/

    private:

        std::set<Node*> m_allNodes;
        
        //std::map<uint32_t, UniformBlock*> m_uniformBlocks;

        InputStructure m_inputInterface;
        OutputStructure m_outputInterface;
        //InputStructure m_pushConstantInterface;
        VertexOutputVariable m_vertexOutputVariable;
    };


    /**
    * @brief Fragment shader script.
    */
    class CURSE_API FragmentScript : public Script
    {

    public:

        /** Constructor. */
        FragmentScript();

        /** Destructor. */
        ~FragmentScript();

        /**
          * Create new variable node and append it to this script.
          *
          * @tparam TDataType Data type of variable node to create.
          */
        template<typename TDataType>
        ConstantVariable<TDataType>* CreateConstantVariable(const TDataType& value = VariableTrait<TDataType>::DefaultValue);

        /**
         * Create new function node and append it to this script.
         *
         * @tparam TFunction Function node object to create and data types for parameters.
         */
        template<typename TFunction>
        TFunction* CreateFunction();

        /**
         * Create new operator node and append it to the shader script.
         *
         * @tparam TOperator Operator node object to create.
         */
        template<typename TOperator>
        TOperator* CreateOperator();

        /**
         * Create a new uniform block.
         *
         * @param id Id of the block. The id is used for sharing uniform data between shader stages.
         *
         * @return Pointer to the new block.
         */
        //UniformBlock* CreateUniformBlock(const uint32_t id);

        /** Get type of shader script. */
        virtual Type GetType() const override;

        /** Removes node from script, disconnects all connections of node and deallocates the pointer. */
        virtual void DestroyNode(Node* node) override;

        /** Get number of nodes in this script. */
        virtual size_t GetNodeCount() const override;

        /** Get all nodes of this script. */
        /**@{*/
        virtual std::vector<Node*> GetAllNodes() override;
        virtual std::vector<const Node*> GetAllNodes() const override;
        /**@}*/

        /**
         * Get interface block for input variables.
         * Members of this block is sent from the previous shader stage or from the vertex buffer.
         */
         /**@{*/
        virtual InputStructure& GetInputInterface() override;
        virtual const InputStructure& GetInputInterface() const override;
        /**@}*/

        /**
         * Get interface block for output variables.
         * Data in this block is sent to the next shader stage or to the framebuffer.
         */
         /**@{*/
        virtual OutputStructure& GetOutputInterface() override;
        virtual const OutputStructure& GetOutputInterface() const override;
        /**@}*/

        /**
         * Get interface block for push constant variables.
         *  Data in this block is sent from the client at runtime.
         *  Maximum number of bytes available in this structure is 128.
         */
         /**@{*/
        //virtual InputStructure& GetPushConstantInterface() override;
        //virtual const InputStructure& GetPushConstantInterface() const override;
        /**@}*/

        /** Get number of uniform blocks in this script. */
        //virtual const size_t GetUniformBlockCount() const override;

        /** Get all uniform blocks in this script. */
        /**@{*/
        //virtual std::vector<UniformBlock*> GetUniformBlocks() override;
        //virtual std::vector<const UniformBlock*> GetUniformBlocks() const override;
        /**@}*/

    private:

        std::set<Node*> m_allNodes;
        //std::map<uint32_t, UniformBlock*> m_uniformBlocks;

        InputStructure m_inputInterface;
        OutputStructure m_outputInterface;
        //InputStructure m_pushConstantInterface;

    };

}

#include "Curse/Renderer/Shader/Visual/VisualShaderScript.inl"

#endif