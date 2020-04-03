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

#ifndef CURSE_CORE_RENDERER_SHADER_NODE_SHADEROPERATORNODE_HPP
#define CURSE_CORE_RENDERER_SHADER_NODE_SHADEROPERATORNODE_HPP

#include "Curse/Renderer/Shader/ShaderNode.hpp"

namespace Curse
{

    namespace Shader
    {

        /**
        * @brief Enumerator of operator types.
        */
        enum class OperatorType : uint8_t
        {
            Arithmetic
        };


        /**
        * @brief Enumerator of arithmetic operator types.
        */
        enum class ArithmeticOperatorType : uint8_t
        {
            Addition,
            Division,
            Multiplication,
            Subtraction
        };


        /**
        * @brief Operator node base class of shader script.
        */
        class CURSE_API OperatorNodeBase : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

            /*
            * @brief Get operator type.
            */
            virtual OperatorType GetOperatorType() const = 0;

        protected:

            OperatorNodeBase(Script& script);
            OperatorNodeBase(const OperatorNodeBase&) = delete;
            OperatorNodeBase(OperatorNodeBase&&) = delete;
            virtual ~OperatorNodeBase();

        };


        /**
        * @brief Arithmetic operator node base class of shader script.
        */
        class CURSE_API ArithmeticOperatorNodeBase : public OperatorNodeBase
        {

        public:

            /*
            * @brief Get operator type.
            */
            virtual OperatorType GetOperatorType() const override;

            /*
            * @brief Get arithmetic operator type.
            */
            virtual ArithmeticOperatorType GetArithmeticOperatorType() const = 0;

        protected:

            ArithmeticOperatorNodeBase(Script& script);
            ArithmeticOperatorNodeBase(const ArithmeticOperatorNodeBase&) = delete;
            ArithmeticOperatorNodeBase(ArithmeticOperatorNodeBase&&) = delete;
            virtual ~ArithmeticOperatorNodeBase();

        };


        /**
        * @brief Operator node of shader script.
        */
        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        class ArithmeticOperatorNode : public ArithmeticOperatorNodeBase
        {

        public:

            /*
            * @brief Get arithmetic operator type.
            */
            virtual ArithmeticOperatorType GetArithmeticOperatorType() const override;

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const override;

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

            ArithmeticOperatorNode(Script& script);
            ArithmeticOperatorNode(const ArithmeticOperatorNode&) = delete;
            ArithmeticOperatorNode(ArithmeticOperatorNode&&) = delete;
            ~ArithmeticOperatorNode();

        private:

            InputPin<LeftType> m_inputLeft;
            InputPin<RightType> m_inputRight;
            OutputPin<OutputType> m_output;

            friend class VertexScript;
            friend class FragmentScript;

        };

    }

}

#include "Curse/Renderer/Shader/Node/ShaderOperatorNode.inl"

#endif