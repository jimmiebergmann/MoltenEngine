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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADEROPERATOR_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADEROPERATOR_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderNode.hpp"

namespace Molten::Shader::Visual
{

    /** Enumerator of operator types. */
    enum class OperatorType : uint8_t
    {
        Arithmetic
    };


    /** Enumerator of arithmetic operator types. */
    enum class ArithmeticOperatorType : uint8_t
    {
        Addition,
        Division,
        Multiplication,
        Subtraction
    };


    /** Operator node base class of shader script. Inherited by all operator nodes.  */
    class MOLTEN_API OperatorBase : public Node
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        OperatorBase(const OperatorBase&) = delete;
        OperatorBase(OperatorBase&&) = delete;
        OperatorBase& operator = (const OperatorBase&) = delete;
        OperatorBase& operator = (OperatorBase&&) = delete;
        /**@}*/

        /** Get type of node. */
        [[nodiscard]] NodeType GetType() const override;

        /* Get operator type. */
        [[nodiscard]] virtual OperatorType GetOperatorType() const = 0;

    protected:

        explicit OperatorBase(Script& script);
        ~OperatorBase() override = default;

    };


    /**
    * Arithmetic operator node base class of shader script. Inherited by all arithemic operator nodes.
    */
    class MOLTEN_API ArithmeticOperatorBase : public OperatorBase
    {

    public:


        /* Deleted copy/move constructor/operators. */
        /**@{*/
        ArithmeticOperatorBase(const ArithmeticOperatorBase&) = delete;
        ArithmeticOperatorBase(ArithmeticOperatorBase&&) = delete;
        ArithmeticOperatorBase& operator = (const ArithmeticOperatorBase&) = delete;
        ArithmeticOperatorBase& operator = (ArithmeticOperatorBase&&) = delete;
        /**@}*/

        /* Get operator type. */
        [[nodiscard]] OperatorType GetOperatorType() const override;

        /* Get arithmetic operator type. */
        [[nodiscard]] virtual ArithmeticOperatorType GetArithmeticOperatorType() const = 0;

    protected:

        explicit ArithmeticOperatorBase(Script& script);
        ~ArithmeticOperatorBase() override = default;

    };


    /**
    * @brief Operator node of shader script.
    */
    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    class ArithmeticOperator : public ArithmeticOperatorBase
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        ArithmeticOperator(const ArithmeticOperator&) = delete;
        ArithmeticOperator(ArithmeticOperator&&) = delete;
        ArithmeticOperator& operator = (const ArithmeticOperator&) = delete;
        ArithmeticOperator& operator = (ArithmeticOperator&&) = delete;
        /**@}*/


        /** Get input pins of arithmetic operator as reference. */
        /**@{*/
        [[nodiscard]] InputPin<TLeftType>& GetLeftInput();
        [[nodiscard]] const InputPin<TLeftType>& GetLeftInput() const;

        [[nodiscard]] InputPin<TRightType>& GetRightInput();
        [[nodiscard]] const InputPin<TRightType>& GetRightInput() const;
        /**@}*/

        /** Get output pin of arithmetic operator as reference. */
        /**@{*/
        [[nodiscard]] OutputPin<TOutputType>& GetOutput();
        [[nodiscard]] const OutputPin<TOutputType>& GetOutput() const;
        /**@}*/


        /* Get arithmetic operator type. */
        [[nodiscard]] ArithmeticOperatorType GetArithmeticOperatorType() const override;


        /** Get number of input pins. */
        [[nodiscard]] size_t GetInputPinCount() const override;

        /** Get number of output pins. */
        [[nodiscard]] size_t GetOutputPinCount() const override;

        /**
         * Get input pin by index.
         *
         * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
         */
        /**@{*/
        [[nodiscard]] Pin* GetInputPin(const size_t index = 0) override;
        [[nodiscard]] const Pin* GetInputPin(const size_t index = 0) const override;
        /**@}*/

        /** Get all input pins, wrapped in a vector. */
        /**@{*/
        [[nodiscard]] std::vector<Pin*> GetInputPins() override;
        [[nodiscard]] std::vector<const Pin*> GetInputPins() const override;
        /**@}*/

        /**
         * Get output pin by index.
         *
         * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
         */
        /**@{*/
        [[nodiscard]] Pin* GetOutputPin(const size_t index = 0) override;
        [[nodiscard]] const Pin* GetOutputPin(const size_t index = 0) const override;
        /**@}*/

        /** Get all output pins, wrapped in a vector. */
        /**@{*/
        [[nodiscard]] std::vector<Pin*> GetOutputPins() override;
        [[nodiscard]] std::vector<const Pin*> GetOutputPins() const override;
        /**@}*/

    protected:

        explicit ArithmeticOperator(Script& script);
        ~ArithmeticOperator() override = default;

    private:

        InputPin<TLeftType> m_inputLeft;
        InputPin<TRightType> m_inputRight;
        OutputPin<TOutputType> m_output;

        friend class VertexScript;
        friend class FragmentScript;

    };

}

#include "Molten/Renderer/Shader/Visual/VisualShaderOperator.inl"

#endif