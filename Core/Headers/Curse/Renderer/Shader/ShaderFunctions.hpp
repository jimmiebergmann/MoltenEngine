/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#ifndef CURSE_CORE_RENDERER_SHADER_SHADERFUNCTIONS_HPP
#define CURSE_CORE_RENDERER_SHADER_SHADERFUNCTIONS_HPP

#include "Curse/Renderer/Shader/Node/ShaderFunctionNode.hpp"

namespace Curse
{

    namespace Shader
    {

        namespace Function
        {
            // Constructors
            using CompsToVec2f32 = FunctionNode<FunctionType::CreateVec2, Curse::Vector2f32, float, float>;
            using CompsToVec3f32 = FunctionNode<FunctionType::CreateVec3, Curse::Vector3f32, float, float, float>;
            using CompsToVec4f32 = FunctionNode<FunctionType::CreateVec4, Curse::Vector4f32, float, float, float, float>;

            using Vec2ToVec3f32 = FunctionNode<FunctionType::CreateVec3, Curse::Vector3f32, Curse::Vector2f32, float>;
            using Vec2ToVec4f32 = FunctionNode<FunctionType::CreateVec4, Curse::Vector4f32, Curse::Vector2f32, float, float>;
            using Vec3ToVec4f32 = FunctionNode<FunctionType::CreateVec4, Curse::Vector4f32, Curse::Vector3f32, float>;
            using Vec3ToVec2f32 = FunctionNode<FunctionType::CreateVec2, Curse::Vector2f32, Curse::Vector3f32>;
            using Vec4ToVec2f32 = FunctionNode<FunctionType::CreateVec2, Curse::Vector2f32, Curse::Vector4f32>;
            using Vec4ToVec3f32 = FunctionNode<FunctionType::CreateVec3, Curse::Vector3f32, Curse::Vector4f32>;

            /*using Mat4x4ToMat3x3f32 = FunctionNode<FunctionType::CreateMat3x3, Curse::Matrix3x3f32, Curse::Matrix4x4f32>;*/

            // Trigonometry
            using Cosf32     = FunctionNode<FunctionType::Cos, float, float>;
            using CosVec2f32 = FunctionNode<FunctionType::Cos, Curse::Vector2f32, Curse::Vector2f32>;
            using CosVec3f32 = FunctionNode<FunctionType::Cos, Curse::Vector3f32, Curse::Vector3f32>;
            using CosVec4f32 = FunctionNode<FunctionType::Cos, Curse::Vector4f32, Curse::Vector4f32>;

            using Sinf32     = FunctionNode<FunctionType::Sin, float, float>;
            using SinVec2f32 = FunctionNode<FunctionType::Sin, Curse::Vector2f32, Curse::Vector2f32>;
            using SinVec3f32 = FunctionNode<FunctionType::Sin, Curse::Vector3f32, Curse::Vector3f32>;
            using SinVec4f32 = FunctionNode<FunctionType::Sin, Curse::Vector4f32, Curse::Vector4f32>;

            using Tanf32     = FunctionNode<FunctionType::Tan, float, float>;
            using TanVec2f32 = FunctionNode<FunctionType::Tan, Curse::Vector2f32, Curse::Vector2f32>;
            using TanVec3f32 = FunctionNode<FunctionType::Tan, Curse::Vector3f32, Curse::Vector3f32>;
            using TanVec4f32 = FunctionNode<FunctionType::Tan, Curse::Vector4f32, Curse::Vector4f32>;

            // Mathematics.
            using Maxf32     = FunctionNode<FunctionType::Max, float, float, float>;
            using MaxVec2f32 = FunctionNode<FunctionType::Max, Curse::Vector2f32, Curse::Vector2f32, Curse::Vector2f32>;
            using MaxVec3f32 = FunctionNode<FunctionType::Max, Curse::Vector3f32, Curse::Vector3f32, Curse::Vector3f32>;
            using MaxVec4f32 = FunctionNode<FunctionType::Max, Curse::Vector4f32, Curse::Vector4f32, Curse::Vector4f32>;

            using Minf32     = FunctionNode<FunctionType::Min, float, float, float>;
            using MinVec2f32 = FunctionNode<FunctionType::Min, Curse::Vector2f32, Curse::Vector2f32, Curse::Vector2f32>;
            using MinVec3f32 = FunctionNode<FunctionType::Min, Curse::Vector3f32, Curse::Vector3f32, Curse::Vector3f32>;
            using MinVec4f32 = FunctionNode<FunctionType::Min, Curse::Vector4f32, Curse::Vector4f32, Curse::Vector4f32>;

            // Vector.
            using CrossVec3f32 = FunctionNode<FunctionType::Cross, Curse::Vector3f32, Curse::Vector3f32, Curse::Vector3f32>;

            using DotVec2f32 = FunctionNode<FunctionType::Dot, float, Curse::Vector2f32, Curse::Vector2f32>;
            using DotVec3f32 = FunctionNode<FunctionType::Dot, float, Curse::Vector3f32, Curse::Vector3f32>;
            using DotVec4f32 = FunctionNode<FunctionType::Dot, float, Curse::Vector4f32, Curse::Vector4f32>;

            // Sampler
            using Texture2D = FunctionNode<FunctionType::Texture2D, Curse::Vector4f32, Sampler2D, Curse::Vector2f32>;
            using Texture3D = FunctionNode<FunctionType::Texture3D, Curse::Vector4f32, Sampler3D, Curse::Vector3f32>;
        }

    }

}

#endif