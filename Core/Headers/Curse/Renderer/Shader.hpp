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

#include "Curse/Core.hpp"
#include "Curse/Math/Vector.hpp"
#include "Curse/Math/Matrix.hpp"

namespace Curse
{

    namespace Shader
    {

        /**
        * @brief Enumerator of shader types.
        */
        enum class Type : uint8_t
        {
            Vertex,
            Fragment
        };


        /**
        * @brief Enumerator of shader source formats.
        */
        enum class SourceFormat : uint8_t
        {
            Glsl,
            Hlsl,
            SpirV
        };


        /**
        * @brief Data type for 2D sampler.
        */
        struct Sampler2D { };

        /**
        * @brief Data type for 3D sampler.
        */
        struct Sampler3D { };


        /**
        * @brief Enumerator of available variable data types.
        */
        enum class VariableDataType : uint8_t
        {
            Bool,
            Int32,
            Float32,
            Vector2f32,
            Vector3f32,
            Vector4f32,
            Matrix4x4f32,
            Sampler2D,
            Sampler3D
        };


        /**
        * @brief Helper wrapper class for aligning any data type by 16 bytes.
        */
        CURSE_PADDED_STRUCT_BEGIN;
        template<typename T>
        class alignas(16) PaddedType : public T
        {

        public:

            PaddedType();
            PaddedType(const T& type);

        };
        CURSE_PADDED_STRUCT_END;


        /**
        * @brief Type trait for statically checking pin information of a certain data type.
        */
        template<typename T>
        struct VariableTrait
        {
            inline static const bool Supported = false; ///< Set to true if T is accepted as a data type of pin.
            inline static const T DefaultValue = { }; ///< Set to the default value of pin data type T.
            inline static const VariableDataType DataType = VariableDataType::Bool; ///< Set to the corresponding VariableDataType of T.
        };

    }

}
 
#include "Shader.inl"

#endif
