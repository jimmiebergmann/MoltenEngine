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

#ifndef CURSE_CORE_RENDERER_SHADER_NODE_SHADERCONSTANTNODE_HPP
#define CURSE_CORE_RENDERER_SHADER_NODE_SHADERCONSTANTNODE_HPP

#include "Curse/Renderer/Shader/ShaderNode.hpp"

namespace Curse
{

    namespace Shader
    {

        /**
        * @brief Base clas of constant node, of shader script.
        */
        class CURSE_API ConstantNodeBase : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

            /**
            * @brief Get data type of constant.
            */
            virtual VariableDataType GetDataType() const = 0;

        protected:

            ConstantNodeBase(Script& script);
            ConstantNodeBase(const ConstantNodeBase&) = delete;
            ConstantNodeBase(ConstantNodeBase&&) = delete;
            virtual ~ConstantNodeBase();

        };


        /**
        * @brief Constant node of shader script.
        *        The stored value is not constant and can be changed at any time,
        *        but it's used as a constant in the generated shader code.
        */
        template<typename T>
        class ConstantNode : public ConstantNodeBase
        {

        public:

            /**
            * @brief Get value of constant node.
            */
            const T& GetValue() const;

            /**
            * @brief Set value of constant node.
            */
            void SetValue(const T& value);

            /**
            * @brief Get data type of constant.
            */
            virtual VariableDataType GetDataType() const override;

            /**
            * @brief Get number of output pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get output pin by index.
            *
            * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
            */
            /**@{*/
            virtual Pin* GetOutputPin(const size_t index = 0) override;
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all output pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetOutputPins() override;
            virtual std::vector<const Pin*> GetOutputPins() const override;
            /**@}*/

        protected:

            ConstantNode(Script& script, const T& value);
            ConstantNode(const ConstantNode&) = delete;
            ConstantNode(ConstantNode&&) = delete;
            ~ConstantNode();

        private:

            OutputPin<T> m_output;
            T m_value;

            friend class VertexScript;
            friend class FragmentScript;

        };

    }

}

#include "ShaderConstantNode.inl"

#endif