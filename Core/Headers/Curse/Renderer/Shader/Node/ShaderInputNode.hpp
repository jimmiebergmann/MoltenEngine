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

#ifndef CURSE_CORE_RENDERER_SHADER_NODE_SHADERINPUTNODE_HPP
#define CURSE_CORE_RENDERER_SHADER_NODE_SHADERINPUTNODE_HPP

#include "Curse/Renderer/Shader/ShaderNode.hpp"

namespace Curse
{

    namespace Shader
    {

        // Forward declarations.
        class InputBlock;
        class OutputBlock;
        class VertexScript;
        class FragmentScript;


        /**
        * @brief Base clas of input node, of shader script.
        */
        class CURSE_API InputNodeBase : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

        protected:

            explicit InputNodeBase(Script& script);
            InputNodeBase(const InputNodeBase&) = delete;
            InputNodeBase(InputNodeBase&&) = delete;
            virtual ~InputNodeBase();

            friend class InputBlock;

        };


        /**
        * @brief Output node of shader script.
        */
        template<typename T>
        class InputNode : public InputNodeBase
        {

        public:

            /**
            * @brief Get number of output pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            /**@{*/
            virtual Pin* GetOutputPin(const size_t index = 0) override;
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetOutputPins() override;
            virtual std::vector<const Pin*> GetOutputPins() const override;
            /**@}*/

        private:

            explicit InputNode(Script& script);
            InputNode(const InputNode&) = delete;
            InputNode(InputNode&&) = delete;
            ~InputNode();

            OutputPin<T> m_output;

            friend class InputBlock;

        };


        /**
        * @brief Input node block node.
        */
        class CURSE_API InputBlock
        {

        public:

            /**
            * @brief Append new input node to this block.
            */
            template<typename T>
            InputNode<T>* AppendNode();

            /**
            * @brief Removes the node from the block, disconnects all connections of node
            *        and deallocates the pointer.
            */
            void DestroyNode(InputNodeBase* inputNode);

            /**
            * @brief Get number of nodes in this block.
            */
            size_t GetNodeCount() const;

            /**
            * @brief Get number of output pins in this block.
            */
            size_t GetOutputPinCount() const;

            /**
            * @brief Get all output nodes.
            */
            /**@{*/
            std::vector<InputNodeBase*> GetNodes();
            std::vector<const InputNodeBase*> GetNodes() const;
            /**@}*/

            /**
            * @brief Compare and check the layout compability between this and target output block.
            *
            * @return True if layouts are compatible, else false.
            */
            bool CheckCompability(const OutputBlock& block) const;

        private:

            InputBlock(Script& script);
            InputBlock(const InputBlock&) = delete;
            InputBlock(InputBlock&&) = delete;
            ~InputBlock();

            Script& m_script;
            std::vector<InputNodeBase*> m_nodes;
            size_t m_pinCount;

            friend class VertexScript;
            friend class FragmentScript;

        };

    }

}

#include "Curse/Renderer/Shader/Node/ShaderInputNode.inl"

#endif