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

#ifndef MOLTEN_GRAPHICS_SHADERPROGRAM_HPP
#define MOLTEN_GRAPHICS_SHADERPROGRAM_HPP

#include "Molten/Graphics/Build.hpp"
#include "Molten/Shader/Visual/VisualShaderScript.hpp"

namespace Molten
{

    /** Shader module resource object, representing a compiled shader program. */
    class MOLTEN_GRAPHICS_API ShaderProgram
    {

    public:

        ShaderProgram() = default;
        virtual ~ShaderProgram() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram(ShaderProgram&&) = delete;
        ShaderProgram& operator =(const ShaderProgram&) = delete;
        ShaderProgram& operator =(ShaderProgram&&) = delete;
        /**@}*/

    };

    /** Descriptor class of shader program class. */
    struct MOLTEN_GRAPHICS_API VisualShaderProgramDescriptor
    {

        VisualShaderProgramDescriptor();
        VisualShaderProgramDescriptor(
            Shader::Visual::VertexScript* vertexScript,
            Shader::Visual::FragmentScript* fragmentScript);

        ~VisualShaderProgramDescriptor() = default;

        VisualShaderProgramDescriptor(const VisualShaderProgramDescriptor&) = default;
        VisualShaderProgramDescriptor(VisualShaderProgramDescriptor&&) = default;
        VisualShaderProgramDescriptor& operator =(const VisualShaderProgramDescriptor&) = default;
        VisualShaderProgramDescriptor& operator =(VisualShaderProgramDescriptor&&) = default;

        Shader::Visual::VertexScript* vertexScript;
        Shader::Visual::FragmentScript* fragmentScript;

    };

}

#endif
