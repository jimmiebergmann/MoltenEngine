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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VULKANSHADERGENERATOR_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VULKANSHADERGENERATOR_HPP

#include "Molten/Renderer/Shader.hpp"
#include <vector>

namespace Molten
{
    class Logger;
}
namespace Molten::Shader::Visual
{
    class Script;
}

namespace Molten::Shader
{

    /**
    * Vulkan shader code generator.
    *
    * Generation is performed in two steps:
    *   1. Generate GLSL code, compatible with Spri-V.
    *   2. Converting the GLSL code into SPIR-V.
    */
    class MOLTEN_API VulkanGenerator
    {

    public:

        /** Generate GLSL code, compatible with Spri-V, from a visual shader script. */
        static std::vector<uint8_t> GenerateGlsl(const Visual::Script& script, Logger* logger = nullptr);

        /** Converting GLSL code into SPIR-V code. */
        static std::vector<uint8_t> ConvertGlslToSpriV(const std::vector<uint8_t> & code, Type shaderType, Logger* logger = nullptr);

    };

}

#endif