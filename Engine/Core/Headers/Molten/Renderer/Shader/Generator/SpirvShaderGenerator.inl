/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

namespace Molten::Shader
{

    template<typename T>
    SpirvGenerator::Constant<T>::Constant(
        const Spirv::Id id,
        DataTypeDefinitionPointer dataTypeDefinition,
        const T value
    ) :
        id(id),
        dataTypeDefinition(std::move(dataTypeDefinition)),
        value(value)
    {}

    template<typename T>
    SpirvGenerator::ConstantPointer<T> SpirvGenerator::GetOrCreateConstantScalar(const T value)
    {
        auto& storage = std::get<ConstantScalarStorage<T>>(m_constantScalarStorages);

        if (auto it = storage.constants.find(value); it != storage.constants.end())
        {
            return it->second;
        }

        auto dataTypeDefinition = GetOrCreateDataType(VariableTrait<T>::dataType);
        auto constant = std::make_shared<Constant<T>>(GetNextId(), dataTypeDefinition, value);
        auto it = storage.constants.insert({ value, constant }).first;
        return it->second;
    }

    template<size_t VDimensions, typename T>
    SpirvGenerator::ConstantPointer<Vector<VDimensions, T>> SpirvGenerator::GetOrCreateConstantVector(const Vector<VDimensions, T>& value)
    {
        using VectorType = Vector<VDimensions, T>;

        auto& storage = std::get<ConstantVectorStorage<VDimensions, T>>(m_constantVectorStorages);

        if (auto it = storage.constants.find(value); it != storage.constants.end())
        {
            return it->second.constant;
        }

        Vector<VDimensions, Spirv::Id> components;
        for (size_t i = 0; i < VectorType::Dimensions; i++)
        {
            components.c[i] = GetOrCreateConstantScalar(value.c[i])->id;
        }

        auto dataTypeDefinition = GetOrCreateDataType(VariableTrait<VectorType>::dataType);
        auto constant = std::make_shared<Constant<VectorType>>(GetNextId(), dataTypeDefinition, value);
        auto it = storage.constants.insert({ value, { constant, components } }).first;
        return it->second.constant;
    }

}