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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERCONSTANT_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERCONSTANT_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderNode.hpp"

namespace Molten::Shader::Visual
{

    class MOLTEN_API ConstantBase : public Node
    {

    public:

        /** Constructor. */
        ConstantBase(Script& script);

        /** Get type of node. */
        NodeType GetType() const override;

        /** Get data type of constant. */
        virtual VariableDataType GetDataType() const = 0;

    };


    /** Constant value node. Used for creating custom constant values in visual shader script.*/
    template<typename TDataType>
    class Constant : public ConstantBase
    {

    public:

        using DataType = TDataType;
        using PinType = OutputPin<DataType>;


        /** Constructor. */
        Constant(Script& script, const TDataType& value);

        /** Get data type of constant. */
        VariableDataType GetDataType() const override;


        /**  Get number of output pins.*/
        size_t GetOutputPinCount() const override;

        /**
         * Get output pin by index.
         *
         * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
         */
         /**@{*/
        Pin* GetOutputPin(const size_t index = 0) override;
        const Pin* GetOutputPin(const size_t index = 0) const override;
        /**@}*/

        /** Get all output pins, wrapped in a vector. */
        /**@{*/
        std::vector<Pin*> GetOutputPins() override;
        std::vector<const Pin*> GetOutputPins() const override;
        /**@}*/


        /** Get value of constant. */
        const TDataType& GetValue() const;

        /** Set value of constant. */
        void SetValue(const TDataType& value);

    private:

        TDataType m_value;
        OutputPin<TDataType> m_pin;

    };

}

#include "Molten/Renderer/Shader/Visual/VisualShaderConstant.inl"

#endif