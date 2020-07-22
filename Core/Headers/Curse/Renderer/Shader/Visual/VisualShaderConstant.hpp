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

#ifndef CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERCONSTANT_HPP
#define CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERCONSTANT_HPP

#include "Curse/Renderer/Shader/Visual/VisualShaderNode.hpp"

namespace Curse::Shader::Visual
{

    /** Constant node base class. */
    class CURSE_API ConstantBase : public Node
    {

    public:

        /** Get type of node. */
        virtual NodeType GetType() const override;

        /** Get data type of constant. */
        virtual VariableDataType GetDataType() const = 0;

    protected:

        explicit ConstantBase(Script& script);
        ConstantBase(const ConstantBase&) = delete;
        ConstantBase(ConstantBase&&) = delete;
        virtual ~ConstantBase() = default;

    };


    /**
     * Constant node of shader script.
     * The stored value is not constant and can be changed at any time,
     * but it's used as a constant in the generated shader code.
     *
     * @tparam T Data type of constant node.
     */
    template<typename T>
    class Constant : public ConstantBase
    {

    public:

        /**  Get data type of constant. */
        virtual VariableDataType GetDataType() const override;

        /** Get number of output pins. */
        virtual size_t GetOutputPinCount() const override;

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

        /** Get value of constant node. */
        const T& GetValue() const;

        /** Set value of constant node. */
        void SetValue(const T& value);

    protected:

        explicit Constant(Script& script, const T& value);
        Constant(const Constant&) = delete;
        Constant(Constant&&) = delete;
        ~Constant() = default;

    private:

        OutputPin<T> m_output;
        T m_value;

        friend class VertexScript;
        friend class FragmentScript;

    };

}

#include "Curse/Renderer/Shader/Visual/VisualShaderConstant.inl"

#endif