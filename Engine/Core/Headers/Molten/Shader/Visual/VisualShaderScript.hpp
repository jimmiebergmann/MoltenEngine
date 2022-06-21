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

#ifndef MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERSCRIPT_HPP
#define MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERSCRIPT_HPP

#include "Molten/Shader/Visual/VisualShaderConstant.hpp"
#include "Molten/Shader/Visual/VisualShaderDescriptorSet.hpp"
#include "Molten/Shader/Visual/VisualShaderPushConstants.hpp"
#include <vector>
#include <set>

namespace Molten::Shader::Visual
{

    /** Type definition of shader stage inputs. */
    using InputInterface = OutputStructure<NodeType::Input, void,
        bool, float, int32_t, Vector2f32, Vector3f32, Vector4f32, Matrix4x4f32>;

    /** Type definition of shader stage outputs. */
    using OutputInterface = InputStructure<NodeType::Output, void,
        bool, float, int32_t, Vector2f32, Vector3f32, Vector4f32, Matrix4x4f32>;

    /** Type definition of vertex output node.
     *  This type is only available and used by the vertex script. 
     */
    using VertexOutput = SinglePinNodeWithType<NodeType::VertexOutput, InputPin, Vector4f32>;


    /** Visual shader script class, used for generating shaders via visual node based system. */
    class MOLTEN_CORE_API Script
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

        /** Get all global nodes of this script. */
        /**@{*/
        virtual std::vector<Node*> GetNodes() = 0;
        virtual std::vector<const Node*> GetNodes() const = 0;
        /**@}*/

        /** Get descriptor sets container. */
        /**@{*/
        virtual DescriptorSetsBase& GetDescriptorSetsBase() = 0;
        virtual const DescriptorSetsBase& GetDescriptorSetsBase() const = 0;
        /**@}*/

        /** Get interface block for input variables.
         *  Members of this block is sent from the previous shader stage or from the vertex buffer.
         */
        /**@{*/
        virtual InputInterface& GetInputInterface() = 0;
        virtual const InputInterface& GetInputInterface() const = 0;
        /**@}*/

        /** Get interface block for output variables.
         *  Data in this block is sent to the next shader stage or to the framebuffer.
         */
        /**@{*/
        virtual OutputInterface& GetOutputInterface() = 0;
        virtual const OutputInterface& GetOutputInterface() const = 0;
        /**@}*/

        /** Get push constants base container. */
        /**@{*/
        virtual PushConstantsBase& GetPushConstantsBase() = 0;
        virtual const PushConstantsBase& GetPushConstantsBase() const = 0;
        /**@}*/

        /** Get vertex output node. */
        /*@{*/
        virtual VertexOutput* GetVertexOutput();
        virtual const VertexOutput* GetVertexOutput() const;
        /**@}*/
    };


    /** Visual vertex shader script. */
    class MOLTEN_CORE_API VertexScript : public Script
    {

    public:

        /** Constructor. */
        VertexScript();

        /** Destructor. */
        ~VertexScript();


        /** Create new constant node and append it to this script.
         *
         * @tparam TDataType Data type of constant node to create.
         */
        template<typename TDataType>
        Constant<TDataType>& CreateConstant(const TDataType& value = VariableTrait<TDataType>::DefaultValue);

        /** Create new function node and append it to this script.
         *
         * @tparam TFunction Function node object to create and data types for parameters.
         */
        template<typename TFunction>
        TFunction& CreateFunction();

        /** Create new operator node and append it to the shader script.
         *
         * @tparam TOperator Operator node object to create.
         */
        template<typename TOperator>
        TOperator& CreateOperator();

        /** Create new composite node and append it to the shader script.
         *
         * @tparam TComposite Composite node object to create.
         */
        template<typename TComposite>
        TComposite& CreateComposite();

        /** Get type of shader script. */
        Type GetType() const override;

        /** Removes node from script, disconnects all connections of node and deallocates the pointer. */
        void DestroyNode(Node* node) override;

        /** Get number of nodes in this script. */
        size_t GetNodeCount() const override;

        /** Get all global nodes of this script. */
        /**@{*/
        std::vector<Node*> GetNodes() override;
        std::vector<const Node*> GetNodes() const override;
        /**@}*/

        /** Get descriptor sets container. */
        /**@{*/
        VertexDescriptorSets& GetDescriptorSets();
        const VertexDescriptorSets& GetDescriptorSets() const;
        DescriptorSetsBase& GetDescriptorSetsBase() override;
        const DescriptorSetsBase& GetDescriptorSetsBase() const override;
        /**@}*/

        /** Get interface block for input variables.
         *  Members of this block is sent from the previous shader stage or from the vertex buffer.
         */
        /**@{*/
        InputInterface& GetInputInterface() override;
        const InputInterface& GetInputInterface() const override;
        /**@}*/

        /** Get interface block for output variables.
         *  Data in this block is sent to the next shader stage or to the framebuffer.
         */
        /**@{*/
        OutputInterface& GetOutputInterface() override;
        const OutputInterface& GetOutputInterface() const override;
        /**@}*/

        /** Get push constants container. */
        /**@{*/
        VertexPushConstants& GetPushConstants();
        const VertexPushConstants& GetPushConstants() const;
        PushConstantsBase& GetPushConstantsBase() override;
        const PushConstantsBase& GetPushConstantsBase() const override;
        /**@}*/

        /** Get vertex output node. */
        /**@{*/
        VertexOutput* GetVertexOutput() override;
        const VertexOutput* GetVertexOutput() const override;
        /**@}*/

    private:

        using Nodes = std::set<Node*>;

        Nodes m_nodes; 
        VertexDescriptorSets m_descriptorSets;
        InputInterface m_inputInterface;
        OutputInterface m_outputInterface;
        VertexPushConstants m_pushConstants;
        VertexOutput m_vertexOutput;
    };


    /**
    * @brief Fragment shader script.
    */
    class MOLTEN_CORE_API FragmentScript : public Script
    {

    public:

        /** Constructor. */
        FragmentScript();

        /** Destructor. */
        ~FragmentScript();


        /** Create new constant node and append it to this script.
         *
         * @tparam TDataType Data type of constant node to create.
         */
        template<typename TDataType>
        Constant<TDataType>& CreateConstant(const TDataType& value = VariableTrait<TDataType>::DefaultValue);

        /** Create new function node and append it to this script.
         *
         * @tparam TFunction Function node object to create and data types for parameters.
         */
        template<typename TFunction>
        TFunction& CreateFunction();

        /** Create new operator node and append it to the shader script.
         *
         * @tparam TOperator Operator node object to create.
         */
        template<typename TOperator>
        TOperator& CreateOperator();

        /** Create new composite node and append it to the shader script.
         *
         * @tparam TComposite Composite node object to create.
         */
        template<typename TComposite>
        TComposite& CreateComposite();

        /** Get type of shader script. */
        Type GetType() const override;

        /** Removes node from script, disconnects all connections of node and deallocates the pointer. */
        void DestroyNode(Node* node) override;

        /** Get number of nodes in this script. */
        size_t GetNodeCount() const override;

        /** Get all global nodes of this script. */
        /**@{*/
        std::vector<Node*> GetNodes() override;
        std::vector<const Node*> GetNodes() const override;
        /**@}*/

        /** Get descriptor sets container. */
        /**@{*/
        FragmentDescriptorSets& GetDescriptorSets();
        const FragmentDescriptorSets& GetDescriptorSets() const;
        DescriptorSetsBase& GetDescriptorSetsBase() override;
        const DescriptorSetsBase& GetDescriptorSetsBase() const override;
        /**@}*/

        /** Get interface block for input variables.
         *  Members of this block is sent from the previous shader stage or from the vertex buffer.
         */
        /**@{*/
        InputInterface& GetInputInterface() override;
        const InputInterface& GetInputInterface() const override;
        /**@}*/

        /** Get interface block for output variables.
         *  Data in this block is sent to the next shader stage or to the framebuffer.
         */
        /**@{*/
        OutputInterface& GetOutputInterface() override;
        const OutputInterface& GetOutputInterface() const override;
        /**@}*/

        /** Get push constants container. */
        /**@{*/
        FragmentPushConstants& GetPushConstants();
        const FragmentPushConstants& GetPushConstants() const;
        PushConstantsBase& GetPushConstantsBase() override;
        const PushConstantsBase& GetPushConstantsBase() const override;
        /**@}*/

    private:

        using Nodes = std::set<Node*>;

        Nodes m_nodes;
        FragmentDescriptorSets m_descriptorSets;
        InputInterface m_inputInterface;
        OutputInterface m_outputInterface;
        FragmentPushConstants m_pushConstants;

    };

}

#include "Molten/Shader/Visual/VisualShaderScript.inl"

#endif