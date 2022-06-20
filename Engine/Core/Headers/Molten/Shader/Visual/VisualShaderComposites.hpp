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

#ifndef MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERCOMPOSITES_HPP
#define MOLTEN_CORE_SHADER_VISUAL_VISUALSHADERCOMPOSITES_HPP

#include "Molten/Shader/Visual/VisualShaderComposite.hpp"

namespace Molten::Shader::Visual::Composites
{

    // Vector2f32 composite.
    using Vec2f32FromFloat32 = Composite<CompositeType::Vector2f32, Vector2f32, float, float>;

    // Vector3f32 composite.
    using Vec3f32FromFloat32 = Composite<CompositeType::Vector3f32, Vector3f32, float, float, float>;
    using Vec3f32FromFloat32Vec2f32 = Composite<CompositeType::Vector3f32, Vector3f32, float, Vector2f32>;
    using Vec3f32FromVec2f32Float32 = Composite<CompositeType::Vector3f32, Vector3f32, Vector2f32, float>;

    // Vector4f32 composite.
    using Vec4f32FromFloat32 = Composite<CompositeType::Vector4f32, Vector4f32, float, float, float, float>;
    using Vec4f32FromFloat32Vec2f32 = Composite<CompositeType::Vector4f32, Vector4f32, float, float, Vector2f32>;
    using Vec4f32FromFloat32Vec2f32Float32 = Composite<CompositeType::Vector4f32, Vector4f32, float, Vector2f32, float>;
    using Vec4f32FromFloat32Vec3f32 = Composite<CompositeType::Vector4f32, Vector4f32, float, Vector3f32>;
    using Vec4f32FromVec2f32 = Composite<CompositeType::Vector4f32, Vector4f32, Vector2f32, Vector2f32>;
    using Vec4f32FromVec2f32Float32 = Composite<CompositeType::Vector4f32, Vector4f32, Vector2f32, float, float>;
    using Vec4f32FromVec3f32Float32 = Composite<CompositeType::Vector4f32, Vector4f32, Vector3f32, float>;

}

#endif