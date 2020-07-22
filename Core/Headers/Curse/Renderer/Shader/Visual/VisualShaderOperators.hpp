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

#ifndef CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADEROPERATORS_HPP
#define CURSE_CORE_RENDERER_SHADER_VISUAL_VISUALSHADEROPERATORS_HPP

#include "Curse/Renderer/Shader/Visual/VisualShaderOperator.hpp"

namespace Curse::Shader::Visual::Operators
{

    // Addition
    using AddInt32   = ArithmeticOperator<ArithmeticOperatorType::Addition, int32_t, int32_t, int32_t>;
    using AddFloat32 = ArithmeticOperator<ArithmeticOperatorType::Addition, float, float, float>;
    using AddVec2f32 = ArithmeticOperator<ArithmeticOperatorType::Addition, Curse::Vector2f32, Curse::Vector2f32, Curse::Vector2f32>;
    using AddVec3f32 = ArithmeticOperator<ArithmeticOperatorType::Addition, Curse::Vector3f32, Curse::Vector3f32, Curse::Vector3f32>;
    using AddVec4f32 = ArithmeticOperator<ArithmeticOperatorType::Addition, Curse::Vector4f32, Curse::Vector4f32, Curse::Vector4f32>;

    // Division
    using DivInt32   = ArithmeticOperator<ArithmeticOperatorType::Division, int32_t, int32_t, int32_t>;
    using DivFloat32 = ArithmeticOperator<ArithmeticOperatorType::Division, float, float, float>;

    // Multiplication
    using MultInt32       = ArithmeticOperator<ArithmeticOperatorType::Multiplication, int32_t, int32_t, int32_t>;
    using MultFloat32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, float, float, float>;
    using MultVec2f32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Curse::Vector2f32, Curse::Vector2f32, Curse::Vector2f32>;
    using MultVec3f32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Curse::Vector3f32, Curse::Vector3f32, Curse::Vector3f32>;
    using MultVec4f32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Curse::Vector4f32, Curse::Vector4f32, Curse::Vector4f32>;
    using MultMat4f32     = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Curse::Matrix4x4f32, Curse::Matrix4x4f32, Curse::Matrix4x4f32>;
    using MultMat4Vec4f32 = ArithmeticOperator<ArithmeticOperatorType::Multiplication, Curse::Vector4f32, Curse::Matrix4x4f32, Curse::Vector4f32>;

    // Subtraction
    using SubInt32   = ArithmeticOperator<ArithmeticOperatorType::Subtraction, int32_t, int32_t, int32_t>;
    using SubFloat32 = ArithmeticOperator<ArithmeticOperatorType::Subtraction, float, float, float>;
    using SubVec2f32 = ArithmeticOperator<ArithmeticOperatorType::Subtraction, Curse::Vector2f32, Curse::Vector2f32, Curse::Vector2f32>;
    using SubVec3f32 = ArithmeticOperator<ArithmeticOperatorType::Subtraction, Curse::Vector3f32, Curse::Vector3f32, Curse::Vector3f32>;
    using SubVec4f32 = ArithmeticOperator<ArithmeticOperatorType::Subtraction, Curse::Vector4f32, Curse::Vector4f32, Curse::Vector4f32>;


    /** Type trait for statically checking information of a certain operator type. */
    template<typename Op>
    struct Trait
    {
        inline static constexpr bool Supported = false; ///< Set to true if Op is an accepted operator node.
    };

}

#include "Curse/Renderer/Shader/Visual/VisualShaderOperators.inl"

#endif