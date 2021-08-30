/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERFUNCTION_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERFUNCTION_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderNode.hpp"
#include "Molten/Utility/Template.hpp"
#include <memory>
#include <array>

namespace Molten::Shader::Visual
{

    /** Enumerator of function types. */
    enum class FunctionType : uint16_t
    {
        // Trigonometry
        Cos,
        Sin,
        Tan,

        // Mathematics.
        Max,
        Min,

        // Vector.
        Cross,
        Dot,

        // Texture
        Texture1D,
        Texture2D,
        Texture3D
    };


    /** Visual script function node base class. */
    class MOLTEN_API FunctionBase : public Node
    {

    public:

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        FunctionBase(const FunctionBase&) = delete;
        FunctionBase(FunctionBase&&) = delete;
        FunctionBase& operator = (const FunctionBase&) = delete;
        FunctionBase& operator = (FunctionBase&&) = delete;
        /**@}*/

        /** Get type of node. */
        [[nodiscard]] NodeType GetType() const override;

        /** Get function type. */
        [[nodiscard]] virtual FunctionType GetFunctionType() const = 0;

    protected:

        explicit FunctionBase(Script& script);
        ~FunctionBase() override = default;

    };


    /** Visual script function node class. */
    template<FunctionType VFunctionType, typename TOutputType, typename ... TInputTypes>
    class Function : public FunctionBase
    {

    public:

        template<size_t VIndex>
        using InputTypeAt = GetTemplateArgumentTypeAt<VIndex, TInputTypes...>;

        /* Deleted copy/move constructor/operators. */
        /**@{*/
        Function(const Function&) = delete;
        Function(Function&&) = delete;
        Function& operator = (const Function&) = delete;
        Function& operator = (Function&&) = delete;
        /**@}*/     

        /** Get output pin of function as reference. */
        /**@{*/
        [[nodiscard]] OutputPin<TOutputType>& GetOutput();
        [[nodiscard]] const OutputPin<TOutputType>& GetOutput() const;
        /**@}*/

        /** Get input pins of function as reference. */
        /**@{*/
        template<size_t VIndex>
        [[nodiscard]] InputPin<InputTypeAt<VIndex>>& GetInput();
        template<size_t VIndex>
        [[nodiscard]] const InputPin<InputTypeAt<VIndex>>& GetInput() const;
        /**@}*/

        /** Get function type. */
        [[nodiscard]] FunctionType GetFunctionType() const override;

        /** Get number of input pins. */
        [[nodiscard]] size_t GetInputPinCount() const override;

        /**  Get number of output pins.*/
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

        explicit Function(Script& script);
        ~Function() override = default;

    private:

        static constexpr size_t OutputPinCount = 1;
        static constexpr size_t InputPinCount = sizeof...(TInputTypes);

        std::tuple<std::unique_ptr<InputPin<TInputTypes>>...> m_inputs;
        OutputPin<TOutputType> m_output;

        friend class VertexScript;
        friend class FragmentScript;

    };

}

#include "Molten/Renderer/Shader/Visual/VisualShaderFunction.inl"

#endif