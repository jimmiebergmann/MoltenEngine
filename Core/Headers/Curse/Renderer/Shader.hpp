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

#ifndef CURSE_CORE_RENDERER_SHADER_HPP
#define CURSE_CORE_RENDERER_SHADER_HPP

#include "Curse/Renderer/Resource.hpp"
#include <string>

namespace Curse
{

    /**
    * @brief Base shader class.
    */
    class Shader
    {

    public:

        /**
        * @brief Enumerator of shader types.
        */
        enum class Type : uint8_t
        {
            Vertex,
            Fragment
        };

        /**
        * @brief Get the type of shader.
        */
        Type GetType() const;

    protected:

        Shader() = default;
        Shader(const Shader&) = delete;
        Shader(Shader&&) = delete;
        virtual ~Shader() = default;

        Resource resource;
        Type type;

    };

    /**
    * @brief Descriptor class of shader class.
    *        It's possible to load shaders via files or raw data.
    */
    class ShaderDescriptor
    {

    public:

        ShaderDescriptor();
        ShaderDescriptor(const Shader::Type type, const char* filename);
        ShaderDescriptor(const Shader::Type type, const uint8_t* data, const size_t dataSize);

        const Shader::Type type;    ///< Type of shader.
        const char* filename;       ///< Full path + filename of shader file.
        const uint8_t* data;        ///< Raw pointer of data buffer.
        const size_t dataSize;      ///< Size of data buffer.
    };

}

#endif
