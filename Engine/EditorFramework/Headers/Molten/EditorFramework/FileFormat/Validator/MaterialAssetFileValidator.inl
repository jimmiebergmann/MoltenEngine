/*
* MIT License
*
* Copyright (c) 2023 Jimmie Bergmann
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

namespace Molten::EditorFramework
{
    // Type traits.
    template<>
    struct MaterialAssetFileValidator::TypeTraits<bool> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Bool;
    };

    template<>
    struct MaterialAssetFileValidator::TypeTraits<int32_t> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Int32;
    };

    template<>
    struct MaterialAssetFileValidator::TypeTraits<float> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Float32;
    };

    template<>
    struct MaterialAssetFileValidator::TypeTraits<Vector2f32> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Vector2f32;
    };

    template<>
    struct MaterialAssetFileValidator::TypeTraits<Vector3f32> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Vector3f32;
    };

    template<>
    struct MaterialAssetFileValidator::TypeTraits<Vector4f32> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Vector4f32;
    };

    template<>
    struct MaterialAssetFileValidator::TypeTraits<Matrix4x4f32> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Matrix4x4f32;
    };

    template<>
    struct MaterialAssetFileValidator::TypeTraits<MaterialAssetFile::Sampler1D> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Sampler1D;
    };

    template<>
    struct MaterialAssetFileValidator::TypeTraits<MaterialAssetFile::Sampler2D> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Sampler2D;
    };

    template<>
    struct MaterialAssetFileValidator::TypeTraits<MaterialAssetFile::Sampler3D> {
        static constexpr auto dataType = MaterialAssetFile::DataType::Sampler3D;
    };


    // Data type traits
    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Bool> {
        using Type = bool;
    };

    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Int32> {
        using Type = int32_t;
    };

    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Float32> {
        using Type = float;
    };

    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Vector2f32> {
        using Type = Vector2f32;
    };

    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Vector3f32> {
        using Type = Vector3f32;
    };

    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Vector4f32> {
        using Type = Vector4f32;
    };

    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Matrix4x4f32> {
        using Type = Matrix4x4f32;
    };

    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Sampler1D> {
        using Type = MaterialAssetFile::Sampler1D;
    };

    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Sampler2D> {
        using Type = MaterialAssetFile::Sampler2D;
    };

    template<>
    struct MaterialAssetFileValidator::DataTypeTraits<MaterialAssetFile::DataType::Sampler3D> {
        using Type = MaterialAssetFile::Sampler3D;
    };


    // Vertex input type traits.
    template<>
    struct MaterialAssetFileValidator::VertexInputTypeTraits<MaterialAssetFile::VertexInputType::Position>
    {
        using Type = Vector3f32;
        static constexpr auto dataType = MaterialAssetFile::DataType::Vector3f32;
    };

    template<>
    struct MaterialAssetFileValidator::VertexInputTypeTraits<MaterialAssetFile::VertexInputType::Uv>
    {
        using Type = Vector2f32;
        static constexpr auto dataType = MaterialAssetFile::DataType::Vector2f32;
    };

    template<>
    struct MaterialAssetFileValidator::VertexInputTypeTraits<MaterialAssetFile::VertexInputType::Normal>
    {
        using Type = Vector3f32;
        static constexpr auto dataType = MaterialAssetFile::DataType::Vector3f32;
    };


    // Composite traits
    template<>
    struct MaterialAssetFileValidator::CompositeTraits<MaterialAssetFile::DataType::Bool>
    {
        static constexpr auto overrides = std::array{ CompositeOverride{ DataType::Int32 } };
    };

    template<>
    struct MaterialAssetFileValidator::CompositeTraits<MaterialAssetFile::DataType::Int32>
    {
        static constexpr auto overrides = std::array{ CompositeOverride{ DataType::Float32 } };
    };

    template<>
    struct MaterialAssetFileValidator::CompositeTraits<MaterialAssetFile::DataType::Float32>
    {
        static constexpr auto overrides = std::array{ CompositeOverride{ DataType::Int32 } };
    };

    template<>
    struct MaterialAssetFileValidator::CompositeTraits<MaterialAssetFile::DataType::Vector2f32>
    {
        static constexpr auto overrides = std::array{ CompositeOverride{ DataType::Float32, DataType::Float32 } };
    };

    template<>
    struct MaterialAssetFileValidator::CompositeTraits<MaterialAssetFile::DataType::Vector3f32>
    {
        static constexpr auto overrides = std::array{ 
            CompositeOverride{ DataType::Float32, DataType::Float32, DataType::Float32 },
            CompositeOverride{ DataType::Float32, DataType::Vector2f32 },
            CompositeOverride{ DataType::Vector2f32, DataType::Float32 }
        };
    };

    template<>
    struct MaterialAssetFileValidator::CompositeTraits<MaterialAssetFile::DataType::Vector4f32>
    {
        static constexpr auto overrides = std::array{
            CompositeOverride{ DataType::Float32, DataType::Float32, DataType::Float32, DataType::Float32 },
            CompositeOverride{ DataType::Float32, DataType::Float32, DataType::Vector2f32 },
            CompositeOverride{ DataType::Float32, DataType::Vector2f32, DataType::Float32 },
            CompositeOverride{ DataType::Float32, DataType::Vector3f32 },
            CompositeOverride{ DataType::Vector2f32, DataType::Float32, DataType::Float32 },
            CompositeOverride{ DataType::Vector2f32, DataType::Vector2f32 },
            CompositeOverride{ DataType::Vector3f32, DataType::Float32 }
        };
    };

    template<>
    struct MaterialAssetFileValidator::CompositeTraits<MaterialAssetFile::DataType::Matrix4x4f32>
    {
        static constexpr auto overrides = std::array{
            CompositeOverride{ DataType::Vector4f32, DataType::Vector4f32, DataType::Vector4f32, DataType::Vector4f32 },
        };
    };


    // Operator traits.
    template<>
    struct MaterialAssetFileValidator::OperatorTraits<MaterialAssetFile::OperatorType::Addition>
    {
        static constexpr auto overrides = std::array{
            OperatorOverride{ .parameters = { DataType::Int32, DataType::Int32 }, .returnType = DataType::Int32 },
            OperatorOverride{ .parameters = { DataType::Float32, DataType::Float32 }, .returnType = DataType::Float32 },
            OperatorOverride{ .parameters = { DataType::Vector2f32, DataType::Vector2f32 }, .returnType = DataType::Vector2f32 },
            OperatorOverride{ .parameters = { DataType::Vector3f32, DataType::Vector3f32 }, .returnType = DataType::Vector3f32 },
            OperatorOverride{ .parameters = { DataType::Vector4f32, DataType::Vector4f32 }, .returnType = DataType::Vector4f32 }
        };
    };
    template<>
    struct MaterialAssetFileValidator::OperatorTraits<MaterialAssetFile::OperatorType::Subtraction>
    {
        static constexpr auto overrides = OperatorTraits<MaterialAssetFile::OperatorType::Addition>::overrides;
    };

    template<>
    struct MaterialAssetFileValidator::OperatorTraits<MaterialAssetFile::OperatorType::Multiplication>
    {
        static constexpr auto overrides = std::array{
            OperatorOverride{ .parameters = { DataType::Int32, DataType::Int32 }, .returnType = DataType::Int32 },
            OperatorOverride{ .parameters = { DataType::Float32, DataType::Float32 }, .returnType = DataType::Float32 },
            OperatorOverride{ .parameters = { DataType::Vector2f32, DataType::Vector2f32 }, .returnType = DataType::Vector2f32 },
            OperatorOverride{ .parameters = { DataType::Vector2f32, DataType::Float32 }, .returnType = DataType::Vector2f32 },
            OperatorOverride{ .parameters = { DataType::Vector3f32, DataType::Vector3f32 }, .returnType = DataType::Vector3f32 },
            OperatorOverride{ .parameters = { DataType::Vector3f32, DataType::Float32 }, .returnType = DataType::Vector3f32 },
            OperatorOverride{ .parameters = { DataType::Vector4f32, DataType::Vector4f32 }, .returnType = DataType::Vector4f32 },
            OperatorOverride{ .parameters = { DataType::Vector4f32, DataType::Float32 }, .returnType = DataType::Vector4f32 }
        };
    };
    template<>
    struct MaterialAssetFileValidator::OperatorTraits<MaterialAssetFile::OperatorType::Division>
    {
        static constexpr auto overrides = OperatorTraits<MaterialAssetFile::OperatorType::Multiplication>::overrides;
    };


    // Built-in function traits
    template<>
    struct MaterialAssetFileValidator::BuiltInFunctionTraits<MaterialAssetFile::BuiltInFunctionType::SampleTexture>
    {
        static constexpr auto overrides = std::array{
            FunctionOverride{ .parameters = { DataType::Sampler1D, DataType::Float32 }, .returnType = DataType::Vector4f32 },
            FunctionOverride{ .parameters = { DataType::Sampler2D, DataType::Vector2f32 }, .returnType = DataType::Vector4f32 },
            FunctionOverride{ .parameters = { DataType::Sampler3D, DataType::Vector3f32 }, .returnType = DataType::Vector4f32 },
        };
    };

    template<>
    struct MaterialAssetFileValidator::BuiltInFunctionTraits<MaterialAssetFile::BuiltInFunctionType::Sin>
    {
        static constexpr auto overrides = std::array{
            FunctionOverride{ .parameters = { DataType::Float32 }, .returnType = DataType::Float32 },
            FunctionOverride{ .parameters = { DataType::Vector2f32 }, .returnType = DataType::Vector2f32 },
            FunctionOverride{ .parameters = { DataType::Vector3f32 }, .returnType = DataType::Vector3f32 },
            FunctionOverride{ .parameters = { DataType::Vector4f32 }, .returnType = DataType::Vector4f32 }
        };
    };
    template<>
    struct MaterialAssetFileValidator::BuiltInFunctionTraits<MaterialAssetFile::BuiltInFunctionType::Cos>
    {
        static constexpr auto overrides = BuiltInFunctionTraits< MaterialAssetFile::BuiltInFunctionType::Sin>::overrides;
    };
    template<>
    struct MaterialAssetFileValidator::BuiltInFunctionTraits<MaterialAssetFile::BuiltInFunctionType::Tan>
    {
        static constexpr auto overrides = BuiltInFunctionTraits< MaterialAssetFile::BuiltInFunctionType::Sin>::overrides;
    };

    template<>
    struct MaterialAssetFileValidator::BuiltInFunctionTraits<MaterialAssetFile::BuiltInFunctionType::Min>
    {
        static constexpr auto overrides = std::array{
            FunctionOverride{ .parameters = { DataType::Float32, DataType::Float32 }, .returnType = DataType::Float32 },
            FunctionOverride{ .parameters = { DataType::Vector2f32, DataType::Vector2f32 }, .returnType = DataType::Vector2f32 },
            FunctionOverride{ .parameters = { DataType::Vector3f32, DataType::Vector3f32 }, .returnType = DataType::Vector3f32 },
            FunctionOverride{ .parameters = { DataType::Vector4f32, DataType::Vector4f32 }, .returnType = DataType::Vector4f32 }
        };
    };
    template<>
    struct MaterialAssetFileValidator::BuiltInFunctionTraits<MaterialAssetFile::BuiltInFunctionType::Max>
    {
        static constexpr auto overrides = BuiltInFunctionTraits< MaterialAssetFile::BuiltInFunctionType::Min>::overrides;
    };

    template<>
    struct MaterialAssetFileValidator::BuiltInFunctionTraits<MaterialAssetFile::BuiltInFunctionType::Cross>
    {
        static constexpr auto overrides = std::array{
            FunctionOverride{.parameters = { DataType::Vector3f32, DataType::Vector3f32 }, .returnType = DataType::Vector3f32 }
        };
    };

    template<>
    struct MaterialAssetFileValidator::BuiltInFunctionTraits<MaterialAssetFile::BuiltInFunctionType::Dot>
    {
        static constexpr auto overrides = std::array{
            FunctionOverride{ .parameters = { DataType::Vector2f32, DataType::Vector2f32 }, .returnType = DataType::Float32 },
            FunctionOverride{ .parameters = { DataType::Vector3f32, DataType::Vector3f32 }, .returnType = DataType::Float32 },
            FunctionOverride{ .parameters = { DataType::Vector4f32, DataType::Vector4f32 }, .returnType = DataType::Float32 }
        };
    };


    // Entry point output traits.
    template<>
    struct MaterialAssetFileValidator::EntryPointOutputTraits<MaterialAssetFile::EntryPointOutputType::Color>
    {
        using Type = Vector3f32;
        static constexpr auto dataType = MaterialAssetFile::DataType::Vector3f32;
    };

    template<>
    struct MaterialAssetFileValidator::EntryPointOutputTraits<MaterialAssetFile::EntryPointOutputType::Opacity>
    {
        using Type = float;
        static constexpr auto dataType = MaterialAssetFile::DataType::Float32;
    };

    template<>
    struct MaterialAssetFileValidator::EntryPointOutputTraits<MaterialAssetFile::EntryPointOutputType::Normal>
    {
        using Type = Vector3f32;
        static constexpr auto dataType = MaterialAssetFile::DataType::Vector3f32;
    };


    // Validation methods implementations.
    constexpr bool MaterialAssetFileValidator::ConstantHasOutput(DataType dataType)
    {
        switch (dataType)
        {
            case DataType::Bool:
            case DataType::Int32:
            case DataType::Float32:
            case DataType::Vector2f32:
            case DataType::Vector3f32:
            case DataType::Vector4f32:
            case DataType::Matrix4x4f32: return true;
            default: return false;
        }
    }

    template<MaterialAssetFile::DataType VCompositDataType>
    constexpr bool MaterialAssetFileValidator::CompositeHasOverride(const DataTypes& parameters)
    {
        const auto& compositeOverrides = CompositeTraits<VCompositDataType>::overrides;

        for (const auto& compositeOverride : compositeOverrides)
        {
            if (compositeOverride.size() != parameters.size())
            {
                continue;
            }

            if (std::equal(parameters.begin(), parameters.end(), compositeOverride.begin()))
            {
                return true;
            }
        }
        return false;
    }

    constexpr bool MaterialAssetFileValidator::CompositeHasOverride(
        DataType compositDataType,
        const DataTypes& parameters)
    {
        switch (compositDataType)
        {
            case DataType::Bool: return CompositeHasOverride<DataType::Bool>(parameters);
            case DataType::Int32: return CompositeHasOverride<DataType::Int32>(parameters);
            case DataType::Float32: return CompositeHasOverride<DataType::Float32>(parameters);
            case DataType::Vector2f32: return CompositeHasOverride<DataType::Vector2f32>(parameters);
            case DataType::Vector3f32: return CompositeHasOverride<DataType::Vector3f32>(parameters);
            case DataType::Vector4f32: return CompositeHasOverride<DataType::Vector4f32>(parameters);
            case DataType::Matrix4x4f32: return CompositeHasOverride<DataType::Matrix4x4f32>(parameters);
            default: return false;
        }
    }

    template<MaterialAssetFile::OperatorType VOperatorType>
    constexpr bool MaterialAssetFileValidator::OperatorHasOverride(const DataTypes& parameters)
    {
        const auto& operatorOverrides = OperatorTraits<VOperatorType>::overrides;

        for (const auto& operatorOverride : operatorOverrides)
        {
            if (operatorOverride.parameters.size() != parameters.size())
            {
                continue;
            }

            if (std::equal(parameters.begin(), parameters.end(), operatorOverride.parameters.begin()))
            {
                return true;
            }
        }
        return false;
    }

    constexpr bool MaterialAssetFileValidator::OperatorHasOverride(OperatorType operatorType, const DataTypes& parameters)
    {
        switch (operatorType)
        {
            case OperatorType::Addition: return OperatorHasOverride<OperatorType::Addition>(parameters);
            case OperatorType::Subtraction: return OperatorHasOverride<OperatorType::Subtraction>(parameters);
            case OperatorType::Multiplication: return OperatorHasOverride<OperatorType::Multiplication>(parameters);
            case OperatorType::Division: return OperatorHasOverride<OperatorType::Division>(parameters);
        }
    }

    template<MaterialAssetFile::OperatorType VOperatorType>
    constexpr std::optional<MaterialAssetFileValidator::OperatorOverride> MaterialAssetFileValidator::
        GetOperatorOverride(const DataTypes& parameters)
    {
        const auto& operatorOverrides = OperatorTraits<VOperatorType>::overrides;

        for (const auto& operatorOverride : operatorOverrides)
        {
            if (operatorOverride.parameters.size() != parameters.size())
            {
                continue;
            }

            if (std::equal(parameters.begin(), parameters.end(), operatorOverride.parameters.begin()))
            {
                return operatorOverride;
            }
        }
        return std::nullopt;
    }

    constexpr std::optional<MaterialAssetFileValidator::OperatorOverride> MaterialAssetFileValidator::
        GetOperatorOverride(OperatorType operatorType, const DataTypes& parameters)
    {
        switch (operatorType)
        {
            case OperatorType::Addition: return GetOperatorOverride<OperatorType::Addition>(parameters);
            case OperatorType::Subtraction: return GetOperatorOverride<OperatorType::Subtraction>(parameters);
            case OperatorType::Multiplication: return GetOperatorOverride<OperatorType::Multiplication>(parameters);
            case OperatorType::Division: return GetOperatorOverride<OperatorType::Division>(parameters);
        }
    }


    template<MaterialAssetFile::BuiltInFunctionType VBuiltInFunctionType>
    constexpr bool MaterialAssetFileValidator::FunctionHasOverride(const DataTypes& parameters)
    {
        const auto& functionOverrides = BuiltInFunctionTraits<VBuiltInFunctionType>::overrides;

        for (const auto& functionOverride : functionOverrides)
        {
            if (functionOverride.parameters.size() != parameters.size())
            {
                continue;
            }

            if (std::equal(parameters.begin(), parameters.end(), functionOverride.parameters.begin()))
            {
                return true;
            }
        }
        return false;
    }

    constexpr bool MaterialAssetFileValidator::FunctionHasOverride(BuiltInFunctionType builtInFunctionType, const DataTypes& parameters)
    {
        switch (builtInFunctionType)
        {
            case BuiltInFunctionType::SampleTexture: return FunctionHasOverride<BuiltInFunctionType::SampleTexture>(parameters);
            case BuiltInFunctionType::Sin: return FunctionHasOverride<BuiltInFunctionType::Sin>(parameters);
            case BuiltInFunctionType::Cos: return FunctionHasOverride<BuiltInFunctionType::Cos>(parameters);
            case BuiltInFunctionType::Tan: return FunctionHasOverride<BuiltInFunctionType::Tan>(parameters);
            case BuiltInFunctionType::Min: return FunctionHasOverride<BuiltInFunctionType::Min>(parameters);
            case BuiltInFunctionType::Max: return FunctionHasOverride<BuiltInFunctionType::Max>(parameters);
            case BuiltInFunctionType::Cross: return FunctionHasOverride<BuiltInFunctionType::Cross>(parameters);
            case BuiltInFunctionType::Dot: return FunctionHasOverride<BuiltInFunctionType::Dot>(parameters);
        }
    }

    template<MaterialAssetFile::BuiltInFunctionType VBuiltInFunctionType>
    constexpr std::optional<MaterialAssetFileValidator::FunctionOverride> MaterialAssetFileValidator::
        GetFunctionOverride(const DataTypes& parameters)
    {
        const auto& functionOverrides = BuiltInFunctionTraits<VBuiltInFunctionType>::overrides;

        for (const auto& functionOverride : functionOverrides)
        {
            if (functionOverride.parameters.size() != parameters.size())
            {
                continue;
            }

            if (std::equal(parameters.begin(), parameters.end(), functionOverride.parameters.begin()))
            {
                return functionOverride;
            }
        }
        return std::nullopt;
    }

    constexpr std::optional<MaterialAssetFileValidator::FunctionOverride> MaterialAssetFileValidator::
        GetFunctionOverride(BuiltInFunctionType builtInFunctionType, const DataTypes& parameters)
    {
        switch (builtInFunctionType)
        {
            case BuiltInFunctionType::SampleTexture: return GetFunctionOverride<BuiltInFunctionType::SampleTexture>(parameters);
            case BuiltInFunctionType::Sin: return GetFunctionOverride<BuiltInFunctionType::Sin>(parameters);
            case BuiltInFunctionType::Cos: return GetFunctionOverride<BuiltInFunctionType::Cos>(parameters);
            case BuiltInFunctionType::Tan: return GetFunctionOverride<BuiltInFunctionType::Tan>(parameters);
            case BuiltInFunctionType::Min: return GetFunctionOverride<BuiltInFunctionType::Min>(parameters);
            case BuiltInFunctionType::Max: return GetFunctionOverride<BuiltInFunctionType::Max>(parameters);
            case BuiltInFunctionType::Cross: return GetFunctionOverride<BuiltInFunctionType::Cross>(parameters);
            case BuiltInFunctionType::Dot: return GetFunctionOverride<BuiltInFunctionType::Dot>(parameters);
        }
    }

    bool MaterialAssetFileValidator::ValidateFunctionName(const std::string& name)
    {
        // TODO
        return true;
    }

    constexpr bool MaterialAssetFileValidator::ValidateFunctionDuplicate(const Functions& existingFunctions, const std::string& name)
    {
        return std::find_if(existingFunctions.begin(), existingFunctions.end(), [&](const auto& func)
        {
            return func.name == name;
        }) == existingFunctions.end();
    }
 
}
