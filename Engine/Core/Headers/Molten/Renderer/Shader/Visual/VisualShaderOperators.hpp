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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADEROPERATORS_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADEROPERATORS_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderOperator.hpp"

namespace Molten::Shader::Visual::Operators
{

    // Addition
    using AddInt32   = ArithmeticOperator<ArithmeticOperatorType::Addition, int32_t>;
    using AddFloat32 = ArithmeticOperator<ArithmeticOperatorType::Addition, float>;
    using AddVec2f32 = ArithmeticOperator<ArithmeticOperatorType::Addition, Vector2f32>;
    using AddVec3f32 = ArithmeticOperator<ArithmeticOperatorType::Addition, Vector3f32>;
    using AddVec4f32 = ArithmeticOperator<ArithmeticOperatorType::Addition, Vector4f32>;

    // Division
    using DivInt32   = ArithmeticOperator<ArithmeticOperatorType::Division, int32_t>;
    using DivFloat32 = ArithmeticOperator<ArithmeticOperatorType::Division, float>;
    using DivVec2f32 = ArithmeticOperator<ArithmeticOperatorType::Division, Vector2f32>;
    using DivVec3f32 = ArithmeticOperator<ArithmeticOperatorType::Division, Vector3f32>;
    using DivVec4f32 = ArithmeticOperator<ArithmeticOperatorType::Division, Vector4f32>;

    // Multiplication
    using MultInt32       = ArithmeticOperator<ArithmeticOperatorType::Multiplication, int32_t>;
    using MultFloat32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, float>;
    using MultVec2f32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Vector2f32>;
    using MultVec3f32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Vector3f32>;
    using MultVec4f32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Vector4f32>;
    using MultMat4f32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Matrix4x4f32>;
    using MultVec4Mat4f32 = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Vector4f32, Matrix4x4f32>;

    // Subtraction
    using SubInt32   = ArithmeticOperator<ArithmeticOperatorType::Subtraction, int32_t>;
    using SubFloat32 = ArithmeticOperator<ArithmeticOperatorType::Subtraction, float>;
    using SubVec2f32 = ArithmeticOperator<ArithmeticOperatorType::Subtraction, Vector2f32>;
    using SubVec3f32 = ArithmeticOperator<ArithmeticOperatorType::Subtraction, Vector3f32>;
    using SubVec4f32 = ArithmeticOperator<ArithmeticOperatorType::Subtraction, Vector4f32>;


    /** Type trait for statically checking information of a certain operator type. */
    template<typename TOperator>
    struct Trait
    {
        inline static constexpr bool Supported = false; ///< Set to true if Op is an accepted operator node.
    };

}

#include "Molten/Renderer/Shader/Visual/VisualShaderOperators.inl"

#endif