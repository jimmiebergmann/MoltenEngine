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

#ifndef CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADEROPERATOR_HPP
#define CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADEROPERATOR_HPP

#include "Curse/Renderer/Shader/Visual/VisualShaderNode.hpp"

namespace Curse::Shader::Visual
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
    class CURSE_API OperatorBase : public Node
    {

    public:

        /** Get type of node. */
        virtual NodeType GetType() const override;

        /* Get operator type. */
        virtual OperatorType GetOperatorType() const = 0;

    protected:

        explicit OperatorBase(Script& script);
        OperatorBase(const OperatorBase&) = delete;
        OperatorBase(OperatorBase&&) = delete;
        virtual ~OperatorBase() = default;

    };


    /**
    * Arithmetic operator node base class of shader script. Inherited by all arithemic operator nodes.
    */
    class CURSE_API ArithmeticOperatorBase : public OperatorBase
    {

    public:

        /* Get operator type. */
        virtual OperatorType GetOperatorType() const override;

        /* Get arithmetic operator type. */
        virtual ArithmeticOperatorType GetArithmeticOperatorType() const = 0;

    protected:

        explicit ArithmeticOperatorBase(Script& script);
        ArithmeticOperatorBase(const ArithmeticOperatorBase&) = delete;
        ArithmeticOperatorBase(ArithmeticOperatorBase&&) = delete;
        virtual ~ArithmeticOperatorBase() = default;

    };


    /**
    * @brief Operator node of shader script.
    */
    template<ArithmeticOperatorType TOperator, typename TOutputType, typename TLeftType, typename TRightType>
    class ArithmeticOperator : public ArithmeticOperatorBase
    {

    public:

        /* Get arithmetic operator type. */
        virtual ArithmeticOperatorType GetArithmeticOperatorType() const override;

        /** Get number of input pins. */
        virtual size_t GetInputPinCount() const override;

        /** Get number of output pins. */
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

        explicit ArithmeticOperator(Script& script);
        ArithmeticOperator(const ArithmeticOperator&) = delete;
        ArithmeticOperator(ArithmeticOperator&&) = delete;
        ~ArithmeticOperator() = default;

    private:

        InputPin<TLeftType> m_inputLeft;
        InputPin<TRightType> m_inputRight;
        OutputPin<TOutputType> m_output;

        friend class VertexScript;
        friend class FragmentScript;

    };

}

#include "Curse/Renderer/Shader/Visual/VisualShaderOperator.inl"

#endif