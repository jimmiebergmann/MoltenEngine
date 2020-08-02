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


namespace Molten
{

    namespace Shader
    {


        // Data type padded implementations.
        template<typename T>
        inline PaddedType<T>::PaddedType() :
            T()
        { }

        template<typename T>
        inline PaddedType<T>::PaddedType(const T& type) :
            T(type)
        { }


        // VariableTrait implementations.
        template<> struct VariableTrait<bool>
        {
            inline static const bool Supported = true;
            inline static const bool DefaultValue = false;
            inline static const VariableDataType DataType = VariableDataType::Bool;
        };
        template<> struct VariableTrait<int32_t>
        {
            inline static const bool Supported = true;
            inline static const int32_t DefaultValue = 0;
            inline static const VariableDataType DataType = VariableDataType::Int32;
        };
        template<> struct VariableTrait<float>
        {
            inline static const bool Supported = true;
            inline static const float DefaultValue = 0.0f;
            inline static const VariableDataType DataType = VariableDataType::Float32;
        };
        template<> struct VariableTrait<Vector2f32>
        {
            inline static const bool Supported = true;
            inline static const Vector2f32 DefaultValue = { 0.0f };
            inline static const VariableDataType DataType = VariableDataType::Vector2f32;
        };
        template<> struct VariableTrait<Vector3f32>
        {
            inline static const bool Supported = true;
            inline static const Vector3f32 DefaultValue = { 0.0f };
            inline static const VariableDataType DataType = VariableDataType::Vector3f32;
        };
        template<> struct VariableTrait<Vector4f32>
        {
            inline static const bool Supported = true;
            inline static const Vector4f32 DefaultValue = { 0.0f };
            inline static const VariableDataType DataType = VariableDataType::Vector4f32;
        };
        template<> struct VariableTrait<Matrix4x4f32>
        {
            inline static const bool Supported = true;
            inline static const Matrix4x4f32 DefaultValue = { 0.0f };
            inline static const VariableDataType DataType = VariableDataType::Matrix4x4f32;
        };
        template<> struct VariableTrait<Sampler2D>
        {
            inline static const bool Supported = true;
            inline static const Sampler2D DefaultValue = { };
            inline static const VariableDataType DataType = VariableDataType::Sampler2D;
        };
        template<> struct VariableTrait<Sampler3D>
        {
            inline static const bool Supported = true;
            inline static const Sampler3D DefaultValue = { };
            inline static const VariableDataType DataType = VariableDataType::Sampler3D;
        };

    }

}