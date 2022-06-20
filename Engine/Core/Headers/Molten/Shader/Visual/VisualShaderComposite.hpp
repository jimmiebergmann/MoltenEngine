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

#ifndef MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERCOMPOSITE_HPP
#define MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERCOMPOSITE_HPP

#include "Molten/Shader/Visual/VisualShaderNode.hpp"
#include "Molten/Utility/Template.hpp"
#include <memory>
#include <array>

namespace Molten::Shader::Visual
{

    /** Enumerator of composite types. */
    enum class CompositeType : uint16_t
    {
        Vector2f32,
        Vector3f32,
        Vector4f32
    };


    /** Visual script composite node base class. */
    class MOLTEN_API CompositeBase : public Node
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        CompositeBase(const CompositeBase&) = delete;
        CompositeBase(CompositeBase&&) = delete;
        CompositeBase& operator = (const CompositeBase&) = delete;
        CompositeBase& operator = (CompositeBase&&) = delete;
        /**@}*/

        /** Get type of node. */
        [[nodiscard]] NodeType GetType() const override;

        /** Get composite type. */
        [[nodiscard]] virtual CompositeType GetCompositeType() const = 0;

    protected:

        explicit CompositeBase(Script& script);
        ~CompositeBase() override = default;

    };


    /** Visual script composite node class. */
    template<CompositeType VCompositeType, typename TOutputType, typename ... TInputTypes>
    class Composite : public CompositeBase
    {

    public:

        template<size_t VIndex>
        using InputTypeAt = GetTemplateArgumentTypeAt<VIndex, TInputTypes...>;

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        Composite(const Composite&) = delete;
        Composite(Composite&&) = delete;
        Composite& operator = (const Composite&) = delete;
        Composite& operator = (Composite&&) = delete;
        /**@}*/

        /** Get output pin of composite as reference. */
        /**@{*/
        [[nodiscard]] OutputPin<TOutputType>& GetOutput();
        [[nodiscard]] const OutputPin<TOutputType>& GetOutput() const;
        /**@}*/

        /** Get input pins of composite as reference. */
        /**@{*/
        template<size_t VIndex>
        [[nodiscard]] InputPin<InputTypeAt<VIndex>>& GetInput();
        template<size_t VIndex>
        [[nodiscard]] const InputPin<InputTypeAt<VIndex>>& GetInput() const;
        /**@}*/

        /** Get composite type. */
        [[nodiscard]] CompositeType GetCompositeType() const override;

        /** Get number of input pins. */
        [[nodiscard]] size_t GetInputPinCount() const override;

        /**  Get number of output pins.*/
        [[nodiscard]] size_t GetOutputPinCount() const override;

        /** Get input pin by index.
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

    protected:

        explicit Composite(Script& script);
        ~Composite() override = default;

    private:

        static constexpr size_t OutputPinCount = 1;
        static constexpr size_t InputPinCount = sizeof...(TInputTypes);

        std::tuple<std::unique_ptr<InputPin<TInputTypes>>...> m_inputs;
        OutputPin<TOutputType> m_output;

        friend class VertexScript;
        friend class FragmentScript;

    };


}

#include "Molten/Shader/Visual/VisualShaderComposite.inl"

#endif