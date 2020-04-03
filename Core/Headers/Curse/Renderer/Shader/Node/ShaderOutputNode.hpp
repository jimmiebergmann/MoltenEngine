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

#ifndef CURSE_CORE_RENDERER_SHADER_NODE_SHADEROUTPUTNODE_HPP
#define CURSE_CORE_RENDERER_SHADER_NODE_SHADEROUTPUTNODE_HPP

#include "Curse/Renderer/Shader/ShaderNode.hpp"

namespace Curse
{

    namespace Shader
    {

        // Forward declarations.
        class OutputBlock;
        class VertexScript;
        class FragmentScript;


        /**
        * @brief Base clas of output node, of shader script.
        */
        class CURSE_API OutputNodeBase : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

        protected:

            OutputNodeBase(Script& script);
            OutputNodeBase(const OutputNodeBase&) = delete;
            OutputNodeBase(OutputNodeBase&&) = delete;
            virtual ~OutputNodeBase();

            friend class OutputBlock;

        };


        /**
        * @brief Output node of shader script.
        */
        template<typename T>
        class OutputNode : public OutputNodeBase
        {

        public:

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            /**@{*/
            virtual Pin* GetInputPin(const size_t index = 0) override;
            virtual const Pin* GetInputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetInputPins() override;
            virtual std::vector<const Pin*> GetInputPins() const override;
            /**@}*/

        private:

            OutputNode(Script& script);
            OutputNode(const OutputNode&) = delete;
            OutputNode(OutputNode&&) = delete;
            ~OutputNode();

            InputPin<T> m_input;

            friend class OutputBlock;

        };


        /**
        * @brief Output node block node.
        */
        class CURSE_API OutputBlock
        {

        public:

            /**
            * @brief Append new output node to this block.
            */
            template<typename T>
            OutputNode<T>* AppendNode();

            /**
            * @brief Removes the node from the block, disconnects all connections of node
            *        and deallocates the pointer.
            */
            void DestroyNode(OutputNodeBase* outputNode);

            /**
            * @brief Get number of nodes in this block.
            */
            size_t GetNodeCount() const;

            /**
            * @brief Get number of input pins in this block.
            */
            size_t GetInputPinCount() const;

            /**
            * @brief Get all output nodes.
            */
            /**@{*/
            std::vector<OutputNodeBase*> GetNodes();
            std::vector<const OutputNodeBase*> GetNodes() const;
            /**@}*/

        private:

            OutputBlock(Script& script);
            OutputBlock(const OutputBlock&) = delete;
            OutputBlock(OutputBlock&&) = delete;
            ~OutputBlock();

            Script& m_script;
            std::vector<OutputNodeBase*> m_nodes;
            size_t m_pinCount;

            friend class VertexScript;
            friend class FragmentScript;

        };

    }

}

#include "Curse/Renderer/Shader/Node/ShaderOutputNode.inl"

#endif