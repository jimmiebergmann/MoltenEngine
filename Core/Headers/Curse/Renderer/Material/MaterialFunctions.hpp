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

#ifndef CURSE_CORE_RENDERER_MATERIAL_FUNCTIONS_HPP
#define CURSE_CORE_RENDERER_MATERIAL_FUNCTIONS_HPP

#include "Curse/Renderer/Material/MaterialNode.hpp"
#include <vector>
#include <set>
#include <string>

namespace Curse
{

    namespace Material
    {

        namespace Function
        {

            // Mathematics.
            using Maxf32 = FunctionNode<FunctionType::Max, float, float, float>;
            using Minf32 = FunctionNode<FunctionType::Min, float, float, float>;

            // Vector.
            using CrossVec3f32 = FunctionNode<FunctionType::Cross, Vector3f32, Vector3f32, Vector3f32>;
            using DotVec2f32 = FunctionNode<FunctionType::Dot, float, Vector2f32, Vector2f32>;
            using DotVec3f32 = FunctionNode<FunctionType::Dot, float, Vector3f32, Vector3f32>;
        }

    }

}

#endif