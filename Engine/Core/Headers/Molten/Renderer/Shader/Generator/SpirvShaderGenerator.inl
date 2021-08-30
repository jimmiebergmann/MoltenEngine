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

    // Constant implementations.
    template<typename T>
    SpirvGenerator::Constant::Constant(
        DataTypePointer dataType, 
        const T& value,
        ConstantPointers componentConstants
    ) :
        id(0),
        dataType(std::move(dataType)),
        value(value),
        componentConstants(std::move(componentConstants))
    {}

    // Constant storage implementations.
    template<typename T>
    SpirvGenerator::ConstantPointer SpirvGenerator::ConstantStorage::Get(const T& value)
    {
        const auto variableDataType = VariableTrait<T>::dataType;

        auto constantsIt = m_constants.find(variableDataType);
        if(constantsIt == m_constants.end())
        {
            return nullptr;
        }

        auto& constants = constantsIt->second;
        auto constantIt = std::find_if(constants.begin(), constants.end(), [&](const auto& constant)
        {
            if (const auto variantValue = std::get_if<T>(&constant->value); variantValue)
            {
                return *variantValue == value;
            }

            return false;
        });

        return constantIt != constants.end() ? *constantIt : nullptr;
    }

    template<typename T>
    SpirvGenerator::ConstantPointer SpirvGenerator::ConstantStorage::GetOrCreate(DataTypeStorage& dataTypeStorage, const T& value)
    {
        const auto variableDataType = VariableTrait<T>::dataType;

        auto constantsIt = m_constants.find(variableDataType);
        if (constantsIt == m_constants.end())
        {
            constantsIt = m_constants.insert({ variableDataType , {}}).first;
        }

        auto& constants = constantsIt->second;
        auto constantIt = std::find_if(constants.begin(), constants.end(), [&](const auto& constant)
        {
            if (const auto variantValue = std::get_if<T>(&constant->value); variantValue)
            {
                return *variantValue == value;
            }

            return false;
        });

        if(constantIt != constants.end())
        {
            return *constantIt;
        }

        ConstantPointers componentConstants;

        if constexpr (std::is_same_v<T, Vector2f32> == true)
        {
            for (size_t i = 0; i < Vector2f32::Dimensions; i++)
            {
                componentConstants.push_back(GetOrCreate(dataTypeStorage, value.c[i]));
            }
        }
        else if constexpr (std::is_same_v<T, Vector3f32> == true)
        {
            for (size_t i = 0; i < Vector3f32::Dimensions; i++)
            {
                componentConstants.push_back(GetOrCreate(dataTypeStorage, value.c[i]));
            }
        }
        else if constexpr (std::is_same_v<T, Vector4f32> == true)
        {
            for (size_t i = 0; i < Vector4f32::Dimensions; i++)
            {
                componentConstants.push_back(GetOrCreate(dataTypeStorage, value.c[i]));
            }
        }
        else if constexpr (std::is_same_v<T, Matrix4x4f32> == true)
        {
            for (size_t i = 0; i < Matrix4x4f32::Components; i++)
            {
                componentConstants.push_back(GetOrCreate(dataTypeStorage, value.e[i]));
            }
        }

        auto dataType = dataTypeStorage.GetOrCreate(variableDataType);

        auto newConstant = std::make_shared<Constant>(std::move(dataType), value, std::move(componentConstants));
        constants.push_back(newConstant);
        return newConstant;
    }

}