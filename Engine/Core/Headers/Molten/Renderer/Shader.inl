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

#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Matrix.hpp"

namespace Molten::Shader
{

    // Data type padded implementations.
    template<typename T>
    PaddedType<T>::PaddedType() :
        value{}
    { }

    template<typename T>
    PaddedType<T>::PaddedType(const T& newValue) :
        value(newValue)
    { }

    template<typename T>
    PaddedType<T>::PaddedType(T&& newValue) :
        value(std::move(newValue))
    {}

    template<typename T>
    PaddedType<T>& PaddedType<T>::operator =(const T& newValue)
    {
        value = newValue;
        return *this;
    }

    template<typename T>
    PaddedType<T>& PaddedType<T>::operator =(T&& newValue)
    {
        value = std::move(newValue);
        return *this;
    }

    template<typename T>
    T& PaddedType<T>::operator()()
    {
        return value;
    }

    template<typename T>
    const T& PaddedType<T>::operator()() const
    {
        return value;
    }


    // VariableTrait implementations.
    template<> struct VariableTrait<bool>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ true };
        inline static const bool defaultValue{ false };
        inline static const VariableDataType dataType = VariableDataType::Bool;
        inline static const size_t dataSize = 1;
        inline static const size_t paddedDataSize = 4;
    };
    template<> struct VariableTrait<int32_t>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ true };
        inline static const int32_t defaultValue{ 0 };
        inline static const VariableDataType dataType = VariableDataType::Int32;
        inline static const size_t dataSize = 4;
        inline static const size_t paddedDataSize = 4;
    };
    template<> struct VariableTrait<float>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ true };
        inline static const float defaultValue{ 0.0f };
        inline static const VariableDataType dataType = VariableDataType::Float32;
        inline static const size_t dataSize = 4;
        inline static const size_t paddedDataSize = 4;
    };
    template<> struct VariableTrait<Vector2f32>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ true };
        inline static const Vector2f32 defaultValue{ 0.0f };
        inline static const VariableDataType dataType = VariableDataType::Vector2f32;
        inline static const size_t dataSize = 8;
        inline static const size_t paddedDataSize = 8;
    };
    template<> struct VariableTrait<Vector3f32>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ true };
        inline static const Vector3f32 defaultValue{ 0.0f };
        inline static const VariableDataType dataType = VariableDataType::Vector3f32;
        inline static const size_t dataSize = 12;
        inline static const size_t paddedDataSize = 12;
    };
    template<> struct VariableTrait<Vector4f32>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ true };
        inline static const Vector4f32 defaultValue{ 0.0f };
        inline static const VariableDataType dataType = VariableDataType::Vector4f32;
        inline static const size_t dataSize = 16;
        inline static const size_t paddedDataSize = 16;
    };
    template<> struct VariableTrait<Matrix4x4f32>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ true };
        inline static const Matrix4x4f32 defaultValue{ 0.0f };
        inline static const VariableDataType dataType = VariableDataType::Matrix4x4f32;
        inline static const size_t dataSize = 64;
        inline static const size_t paddedDataSize = 64;
    };
    template<> struct VariableTrait<Sampler1D>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ false };
        inline static const Sampler1D defaultValue{ };
        inline static const VariableDataType dataType = VariableDataType::Sampler1D;
    };
    template<> struct VariableTrait<Sampler2D>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ false };
        inline static const Sampler2D defaultValue{ };
        inline static const VariableDataType dataType = VariableDataType::Sampler2D;
    };
    template<> struct VariableTrait<Sampler3D>
    {
        inline static const bool supported = true;
        inline static const bool hasDefaultValue{ false };
        inline static const Sampler3D defaultValue{ };
        inline static const VariableDataType dataType = VariableDataType::Sampler3D;
    };

    

}