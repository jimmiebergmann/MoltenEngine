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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERUNIFORMBUFFER_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERUNIFORMBUFFER_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderNode.hpp"

namespace Molten::Shader::Visual
{


    template<typename ... TAllowedDataTypes>
    class UniformBuffer : public Node
    {

    public:

        /** Constructor. */
        UniformBuffer(Script& script);

        /** Get type of node. */
        NodeType GetType() const override;

    private:

    };


    using FragmentUniformBuffer = UniformBuffer<bool, int32_t, float, Vector2f32, Vector3f32, Vector4f32, Matrix4x4f32>;
    using VertexUniformBuffer = UniformBuffer<bool, int32_t, float, Vector2f32, Vector3f32, Vector4f32, Matrix4x4f32>;

}

#include "Molten/Renderer/Shader/Visual/VisualShaderUniformBuffer.inl"

#endif