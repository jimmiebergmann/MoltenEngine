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

#ifndef MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERNODE_HPP
#define MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERNODE_HPP

#include "Molten/Shader/Visual/VisualShaderPin.hpp"

namespace Molten::Shader::Visual
{

    class Script;
    class VertexScript;
    class FragmentScript;
        

    /** Enumerator of node types. */
    enum class NodeType : uint8_t
    {
        Constant,           ///< Constant value in shader script.
        Composite,          ///< Composite type creation from other composites or scalars.
        Function,           ///< Built-in shader function.
        Operator,           ///< Operator node in local space.
        PushConstants,      ///< Push constant node, constants set by the client. 
        DescriptorBinding,
        VertexOutput,
        Output,
        Input
    };


    /** Visual shader script node. This type is inherited by all script nodes.
     *  A node contains 0 to N input or/an output pins.
     */
    class MOLTEN_API Node
    {

    public:

        /** Get parent script of this node. */
        /**@{*/
        Script& GetScript();
        const Script& GetScript() const;
        /**@}*/

        /** Get number of input pins. */
        virtual size_t GetInputPinCount() const;

        /**  Get number of output pins.*/
        virtual size_t GetOutputPinCount() const;

        /**
         * Get input pin by index.
         *
         * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
         */
        /**@{*/
        virtual Pin* GetInputPin(const size_t index = 0);
        virtual const Pin* GetInputPin(const size_t index = 0) const;
        /**@}*/

        /** Get all input pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetInputPins();
        virtual std::vector<const Pin*> GetInputPins() const;
        /**@}*/

        /**
         * Get output pin by index.
         *
         * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
         */
        /**@{*/
        virtual Pin* GetOutputPin(const size_t index = 0);
        virtual const Pin* GetOutputPin(const size_t index = 0) const;
        /**@}*/

        /** Get all output pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetOutputPins();
        virtual std::vector<const Pin*> GetOutputPins() const;
        /**@}*/

        /** Get type of node. */
        virtual NodeType GetType() const = 0;

    protected:

        Node(Script& script);
        virtual ~Node() = default;

        Node(const Node&) = delete;
        Node(Node&&) = delete;
        Node& operator =(const Node&) = delete;
        Node& operator =(Node&&) = delete;

    private:

        Script& m_script; ///< Parent shader script.

        friend class VertexScript;
        friend class FragmentScript;

    };


    /** Helper class for creating a node with a type. */
    template<NodeType TypeOfNode>
    class NodeWithType : public Node
    {

    public:

        /** Constructor. */
        explicit NodeWithType(Script& script);

        /** Destructor. */
        virtual ~NodeWithType() = default;


        /** Get type of node. */
        NodeType GetType() const override;
    };


    /** Helper class for constructing a single pin node. */
    template<template<typename> typename TPinType, typename TPinDataType>
    class SinglePinNode : public Node
    {

        static_assert(PinTraits<TPinType>::isInputPin || PinTraits<TPinType>::isOutputPin,
            "Provided TPinType is not of type InputPin or OutputPin");

    public:

        using Base = Node;
        using PinType = TPinType<TPinDataType>;


        SinglePinNode(Script& script);
        virtual ~SinglePinNode() = default;


        /** Get number of input pins. */
        virtual size_t GetInputPinCount() const override;

        /**  Get number of output pins.*/
        virtual size_t GetOutputPinCount() const override;

        /**
         * Get input pin by index.
         *
         * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
         */
         /**@{*/
        virtual Pin* GetInputPin(const size_t index = 0) override;
        virtual const Pin* GetInputPin(const size_t index = 0) const override;
        /**@}*/

        /** Get all input pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetInputPins() override;
        virtual std::vector<const Pin*> GetInputPins() const override;
        /**@}*/

        /**
         * Get output pin by index.
         *
         * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
         */
         /**@{*/
        virtual Pin* GetOutputPin(const size_t index = 0) override;
        virtual const Pin* GetOutputPin(const size_t index = 0) const override;
        /**@}*/

        /** Get all output pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetOutputPins() override;
        virtual std::vector<const Pin*> GetOutputPins() const override;
        /**@}*/

    protected:

        PinType m_pin;

    };


    template<NodeType VTypeOfNode, template<typename> typename TPinType, typename TPinDataType>
    class NodeWithSinglePin : public Node, public TPinType<TPinDataType>
    {

    public:

        NodeWithSinglePin(Script& script);

        /** Get type of node. */
        NodeType GetType() const override;

    };


    /** Helper class for creating a single pin node with a type. */
    template<NodeType TypeOfNode, template<typename> typename TPinType, typename TPinDataType>
    class SinglePinNodeWithType : public SinglePinNode<TPinType, TPinDataType>
    {

    public:

        /** Constructor. */
        explicit SinglePinNodeWithType(Script& script);

        /** Destructor. */
        virtual ~SinglePinNodeWithType() = default;


        /** Get type of node. */
        NodeType GetType() const override;
    };

}

#include "Molten/Shader/Visual/VisualShaderNode.inl"

#endif