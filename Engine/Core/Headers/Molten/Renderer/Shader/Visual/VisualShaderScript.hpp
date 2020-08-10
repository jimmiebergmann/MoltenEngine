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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERSCRIPT_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERSCRIPT_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderUniform.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderPushConstant.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderFunctions.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderOperators.hpp"
#include "Molten/Logger.hpp"
#include <vector>
#include <set>

namespace Molten::Shader::Visual
{

    using InputInterface = InputStructure; ///< Input interface structure block.
    using OutputInterface = OutputStructure; ///< Output interface structure block.

    /**
     * Type definition of vertex output variable node.
     * This type is only available and used by the vertex shader. 
     */
    using VertexOutputVariable = OutputVariable<Vector4f32>;
    

    /** Visual shader script class, used for generating shaders via visual node based system. */
    class MOLTEN_API Script
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
        virtual InputInterface& GetInputInterface() = 0;
        virtual const InputInterface& GetInputInterface() const = 0;
        /**@}*/

        /**
         * Get interface block for output variables.
         * Data in this block is sent to the next shader stage or to the framebuffer.
         */
        /**@{*/
        virtual OutputInterface& GetOutputInterface() = 0;
        virtual const OutputInterface& GetOutputInterface() const = 0;
        /**@}*/

        /**
         * Get interface blocks for uniform variables.
         * This interface contains 0 or more uniform interfaces. UniformInterfaces is simply a container.
         * Data in these blocks are sent from client to any shader stage.
         */
        /**@{*/
        virtual UniformInterfaces& GetUniformInterfaces() = 0;
        virtual const UniformInterfaces& GetUniformInterfaces() const = 0;
        /**@}*/

        /**
         * Get interface block for push constant variables.
         * Data in this block is sent from client to any shader stage at runtime.
         */
         /**@{*/
        virtual PushConstantInterface& GetPushConstantInterface() = 0;
        virtual const PushConstantInterface& GetPushConstantInterface() const = 0;
        /**@}*/

        /** Get vertex output variable. */
        /**@{*/
        virtual VertexOutputVariable* GetVertexOutputVariable();
        virtual const VertexOutputVariable* GetVertexOutputVariable() const;
        /**@}*/

    };


    /** Visual vertex shader script. */
    class MOLTEN_API VertexScript : public Script
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
        virtual InputInterface& GetInputInterface() override;
        virtual const InputInterface& GetInputInterface() const override;
        /**@}*/

        /**
         * Get interface block for output variables.
         * Data in this block is sent to the next shader stage or to the framebuffer.
         */
         /**@{*/
        virtual OutputInterface& GetOutputInterface() override;
        virtual const OutputInterface& GetOutputInterface() const override;
        /**@}*/

        /**
         * Get interface blocks for uniform variables.
         * This interface contains 0 or more uniform interfaces. UniformInterfaces is simply a container.
         * Data in these blocks are sent from client to any shader stage.
         */
        /**@{*/
        virtual UniformInterfaces& GetUniformInterfaces() override;
        virtual const UniformInterfaces& GetUniformInterfaces() const override;
        /**@}*/

        /**
         * Get interface block for push constant variables.
         * Data in this block is sent from client to any shader stage at runtime.
         */
         /**@{*/
        virtual PushConstantInterface& GetPushConstantInterface() override;
        virtual const PushConstantInterface& GetPushConstantInterface() const override;
        /**@}*/

        /** Get vertex output variable. */
        /**@{*/
        virtual VertexOutputVariable* GetVertexOutputVariable() override;
        virtual const VertexOutputVariable* GetVertexOutputVariable() const override;
        /**@}*/

    private:

        std::set<Node*> m_allNodes;
        InputInterface m_inputInterface;
        OutputInterface m_outputInterface;
        UniformInterfaces m_uniformInterfaces;
        PushConstantInterface m_pushConstantInterface;
        VertexOutputVariable m_vertexOutputVariable;
    };


    /**
    * @brief Fragment shader script.
    */
    class MOLTEN_API FragmentScript : public Script
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
        virtual InputInterface& GetInputInterface() override;
        virtual const InputInterface& GetInputInterface() const override;
        /**@}*/

        /**
         * Get interface block for output variables.
         * Data in this block is sent to the next shader stage or to the framebuffer.
         */
         /**@{*/
        virtual OutputInterface& GetOutputInterface() override;
        virtual const OutputInterface& GetOutputInterface() const override;
        /**@}*/

        /**
         * Get interface blocks for uniform variables.
         * This interface contains 0 or more uniform interfaces. UniformInterfaces is simply a container.
         * Data in these blocks are sent from client to any shader stage.
         */
        /**@{*/
        virtual UniformInterfaces& GetUniformInterfaces() override;
        virtual const UniformInterfaces& GetUniformInterfaces() const override;
        /**@}*/

        /**
         * Get interface block for push constant variables.
         * Data in this block is sent from client to any shader stage at runtime.
         */
         /**@{*/
        virtual PushConstantInterface& GetPushConstantInterface() override;
        virtual const PushConstantInterface& GetPushConstantInterface() const override;
        /**@}*/

    private:

        using Script::GetVertexOutputVariable;

        std::set<Node*> m_allNodes;
        InputInterface m_inputInterface;
        OutputInterface m_outputInterface;
        UniformInterfaces m_uniformInterfaces;
        PushConstantInterface m_pushConstantInterface;

    };

}

#include "Molten/Renderer/Shader/Visual/VisualShaderScript.inl"

#endif