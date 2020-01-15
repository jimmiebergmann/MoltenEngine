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

#ifndef CURSE_CORE_RENDERER_SHADER_SHADERPROGRAM_HPP
#define CURSE_CORE_RENDERER_SHADER_SHADERPROGRAM_HPP

#include "Curse/Renderer/Resource.hpp"
#include "Curse/Renderer/Shader.hpp"
#include <string>
#include <vector>

namespace Curse
{

    namespace Shader
    {

        /**
        * @brief Shader program base class.
        */
        class CURSE_API Program
        {

        public:

            /**
            * @brief Compile shader program.
            *        Compilation of shaders makes it possible to convert from GLSL to SPIR-V.
            */
            static std::vector<uint8_t> Compile(const ShaderFormat inputFormat, const ShaderType inputType,
                const std::vector<uint8_t>& inputData, const ShaderFormat outputFormat,
                std::string& errorMessage);

            /**
            * @brief Get the type of shader.
            */
            ShaderType GetType() const;

        protected:

            Program() = default;
            Program(const Program&) = delete;
            Program(Program&&) = delete;
            virtual ~Program() = default;

            Resource resource;
            ShaderType type;

        };

        /**
        * @brief Descriptor class of shader program class.
        *        It's possible to load shaders via files or raw data.
        */
        class CURSE_API ProgramDescriptor
        {

        public:

            ProgramDescriptor();
            ProgramDescriptor(const ShaderType type, const char* filename);
            ProgramDescriptor(const ShaderType type, const uint8_t* data, const size_t dataSize);

            const ShaderType type;  ///< Type of shader.
            const char* filename;   ///< Full path + filename of shader file.
            const uint8_t* data;    ///< Raw pointer of data buffer.
            const size_t dataSize;  ///< Size of data buffer.
        };

    }

}

#endif
