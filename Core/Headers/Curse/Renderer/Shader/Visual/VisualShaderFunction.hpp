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

#ifndef CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERFUNCTION_HPP
#define CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERFUNCTION_HPP

#include "Curse/Renderer/Shader/Visual/VisualShaderNode.hpp"

namespace Curse::Shader::Visual
{

    /** Enumerator of functions. */
    enum class FunctionType : uint16_t
    {
        // Constructors
        CreateVec2,
        CreateVec3,
        CreateVec4,
        /*CreateMat3x3,
        CreateMat4x4,*/

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

        // Sampler
        Texture2D,
        Texture3D


    };


    /** Visual script function node base class. */
    class CURSE_API FunctionBase : public Node
    {

    public:

        /** Get type of node. */
        virtual NodeType GetType() const override;

        /** Get function type. */
        virtual FunctionType GetFunctionType() const = 0;

    protected:

        FunctionBase(Script& script);
        FunctionBase(const FunctionBase&) = delete;
        FunctionBase(FunctionBase&&) = delete;
        virtual ~FunctionBase() = default;

    };


    /** Visual script function node class. */
    template<FunctionType TFunctionType, typename TOutputType, typename ... TInputTypes>
    class Function : public FunctionBase
    {

    public:

        /** Get function type. */
        virtual FunctionType GetFunctionType() const override;

        /** Get number of input pins. */
        virtual size_t GetInputPinCount() const override;

        /**  Get number of output pins.*/
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

        explicit Function(Script& script);
        Function(const Function&) = delete;
        Function(Function&&) = delete;
        ~Function() = default;

    private:

        static constexpr size_t OutputPinCount = 1;
        static constexpr size_t InputPinCount = sizeof...(TInputTypes);

        /**
        * @brief C++17 template loop, to iterate the template types and create the input pins.
        */
        /**@{*/
        template<typename ... TLoopTypes, typename TCallback>
        void LoopEachInputPin(TCallback&& callback);

        template<typename T>
        struct InputPinTypeWrapper
        {
            using Type = T;
        };
        /**@}*/

        std::array<std::unique_ptr<Pin>, InputPinCount> m_inputs;
        std::unique_ptr<Pin> m_output;

        friend class VertexScript;
        friend class FragmentScript;

    };

}

#include "Curse/Renderer/Shader/Visual/VisualShaderFunction.inl"

#endif