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

#ifndef MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERCONSTANT_HPP
#define MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERCONSTANT_HPP

#include "Molten/Shader/Visual/VisualShaderNode.hpp"

namespace Molten::Shader::Visual
{

    class MOLTEN_CORE_API ConstantBase : public Node
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        ConstantBase(const ConstantBase&) = delete;
        ConstantBase(ConstantBase&&) = delete;
        ConstantBase& operator =(const ConstantBase&) = delete;
        ConstantBase& operator =(ConstantBase&&) = delete;
        /**@}*/

        /** Get type of node. */
        [[nodiscard]] NodeType GetType() const override;

        /** Get data type of constant. */
        [[nodiscard]] virtual VariableDataType GetDataType() const = 0;

    protected:

        explicit ConstantBase(Script& script);

        ~ConstantBase() override = default;

    };


    /** Constant value node. Used for creating custom constant values in visual shader script.*/
    template<typename TDataType>
    class Constant : public ConstantBase
    {

    public:

        using DataType = TDataType;
        using PinType = OutputPin<DataType>;

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        Constant(const Constant&) = delete;
        Constant(Constant&&) = delete;
        Constant& operator =(const Constant&) = delete;
        Constant& operator =(Constant&&) = delete;
        /**@}*/

        /** Get output pin of function as reference. */
        /**@{*/
        [[nodiscard]] OutputPin<TDataType>& GetOutput();
        [[nodiscard]] const OutputPin<TDataType>& GetOutput() const;
        /**@}*/

        /** Get data type of constant. */
        [[nodiscard]] VariableDataType GetDataType() const override;

        /**  Get number of output pins.*/
        [[nodiscard]] size_t GetOutputPinCount() const override;

        /** Get output pin by index.
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


        /** Get value of constant. */
        [[nodiscard]] const TDataType& GetValue() const;

        /** Set value of constant. */
        void SetValue(const TDataType& value);

    protected:

        Constant(Script& script, const TDataType& value);

        ~Constant() override = default;

    private:

        TDataType m_value;
        OutputPin<TDataType> m_output;

        friend class VertexScript;
        friend class FragmentScript;

    };

}

#include "Molten/Shader/Visual/VisualShaderConstant.inl"

#endif