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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERPUSHCONSTANTS_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERPUSHCONSTANTS_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Matrix.hpp"

namespace Molten::Shader::Visual
{

    /** Type definition of push constant strucutre. */
    using PushConstantsStructure = 
        Structure<
            NodeType::PushConstants,
            OutputPin, 
            bool, int32_t, float, Vector2f32, Vector3f32, Vector4f32, Matrix4x4f32
        >;


    /** Push constant container node of visual script. */
    template<typename ... TAllowedDataTypes>
    class PushConstants : public Structure<NodeType::PushConstants, OutputPin, TAllowedDataTypes...>
    {

    public:

        /** Constructor. */
        PushConstants(Script& script);

    };


    /** Base type for push constants. */
    using PushConstantsBase = typename PushConstantsStructure::Base;


    /** Push constant container definition for vertex shader. */
    using VertexPushConstants =
        PushConstants<
            bool, int32_t, float, Vector2f32, Vector3f32, Vector4f32, Matrix4x4f32
        >;

    /** Push constant container definition for fragment shader. */
    using FragmentPushConstants =
        PushConstants<
            bool, int32_t, float, Vector2f32, Vector3f32, Vector4f32, Matrix4x4f32
        >;

}

#include "Molten/Renderer/Shader/Visual/VisualShaderPushConstants.inl"

#endif