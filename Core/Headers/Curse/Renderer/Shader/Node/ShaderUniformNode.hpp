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

#ifndef CURSE_CORE_RENDERER_SHADER_NODE_SHADERUNIFORMNODE_HPP
#define CURSE_CORE_RENDERER_SHADER_NODE_SHADERUNIFORMNODE_HPP

#include "Curse/Renderer/Shader/ShaderNode.hpp"
#include <memory>

namespace Curse
{

    namespace Shader
    {

        // Forward declarations.
        class InputBlock;
        class VertexScript;
        class FragmentScript;


        /**
        * @brief Base clas of uniform node, of shader script.
        */
        class CURSE_API UniformNodeBase : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

            /**
            * @brief Checks if this uniform node is an array.
            *
            * @return True if this uniform node is an array, else false.
            */
            virtual bool IsArray() const;

        protected:

            UniformNodeBase(Script& script);
            UniformNodeBase(const UniformNodeBase&) = delete;
            UniformNodeBase(UniformNodeBase&&) = delete;
            virtual ~UniformNodeBase();

            friend class UniformBlock;

        };


        /**
        * @brief Uniform node of shader script.
        */
        template<typename T>
        class UniformNode : public UniformNodeBase
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

            UniformNode(Script& script);
            UniformNode(const UniformNode&) = delete;
            UniformNode(UniformNode&&) = delete;
            ~UniformNode();

            OutputPin<T> m_output;

            friend class UniformBlock;

        };


        /**
        * @brief Uniform array node of shader script.
        */
        template<typename T, size_t Size>
        class UniformArrayNode : public UniformNodeBase
        {

            static_assert(Size != 0, "Size of uniform array cannot be 0.");

        public:

            /**
            * @brief Checks if this uniform node is an array.
            *
            * @return True if this uniform node is an array, else false.
            */
            virtual bool IsArray() const override;

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

            UniformArrayNode(Script& script);
            UniformArrayNode(const UniformArrayNode&) = delete;
            UniformArrayNode(UniformArrayNode&&) = delete;
            ~UniformArrayNode();

            void InitOutputPins();

            std::array<std::unique_ptr<OutputPin<T> >, Size> m_outputs;

            friend class UniformBlock;

        };


        /**
        * @brief Uniform block node.
        */
        class CURSE_API UniformBlock
        {

        public:

            /**
            * @brief Append new uniform node to this block.
            */
            template<typename T>
            UniformNode<T>* AppendNode();

            /**
            * @brief Append new uniform array node to this block.
            */
            template<typename T, size_t Size>
            UniformArrayNode<T, Size>* AppendNode();

            /**
            * @brief Removes the node from the block, disconnects all connections of node
            *        and deallocates the pointer.
            */
            void DestroyNode(UniformNodeBase* uniformNode);

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
            std::vector<UniformNodeBase*> GetNodes();
            std::vector<const UniformNodeBase*> GetNodes() const;
            /**@}*/

            /**
            * @brief Get id of this block.
            */
            uint32_t GetId() const;

            /**
            * @brief Compare and check the layout compability between this and target uniform block.
            *
            * @return True if layouts are compatible, else false.
            */
            bool CheckCompability(const UniformBlock& block) const;

        private:

            UniformBlock(Script& script, const uint32_t id);
            UniformBlock(const UniformBlock&) = delete;
            UniformBlock(UniformBlock&&) = delete;
            ~UniformBlock();

            Script& m_script;
            uint32_t m_id;
            std::vector<UniformNodeBase*> m_nodes;
            size_t m_pinCount;

            friend class VertexScript;
            friend class FragmentScript;

        };

    }

}

#include "ShaderUniformNode.inl"

#endif