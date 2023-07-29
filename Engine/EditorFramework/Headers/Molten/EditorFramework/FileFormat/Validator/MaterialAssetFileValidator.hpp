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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_VALIDATOR_MATERIALASSETFILEVALIDATOR_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_VALIDATOR_MATERIALASSETFILEVALIDATOR_HPP

#include "Molten/EditorFramework/FileFormat/Asset/MaterialAssetFile.hpp"
#include "Molten/Utility/StaticVector.hpp"
#include <string>

namespace Molten::EditorFramework
{

    struct MaterialAssetFileValidator
    {
        using File = MaterialAssetFile;

        using DataType = File::DataType;
        using DataTypes = File::DataTypes;
        using VertexInputType = File::VertexInputType;
        using OperatorType = File::OperatorType;
        using BuiltInFunctionType = File::BuiltInFunctionType;
        using EntryPointOutputType = File::EntryPointOutputType;

        using VertexInputNode = File::VertexInputNode;
        using CompositeNode = File::CompositeNode;

        using Functions = File::Functions;

        // Overrides
        using CompositeOverride = StaticVector<DataType, 4>;

        struct OperatorOverride
        {
            StaticVector<DataType, 2> parameters;
            DataType returnType;
        };

        struct FunctionOverride
        {
            StaticVector<DataType, 4> parameters;
            DataType returnType;      
        };

        // Traits
        template<typename T>
        struct TypeTraits;

        template<DataType VDataType>
        struct DataTypeTraits;

        template<VertexInputType VVertexInputType>
        struct VertexInputTypeTraits;

        template<DataType VDataType>
        struct CompositeTraits;

        template<DataType VDataType, size_t componentCount>
        struct ComponentTraits;

        template<OperatorType VOperatorType>
        struct OperatorTraits;

        template<BuiltInFunctionType VBuiltInFunctionType>
        struct BuiltInFunctionTraits;

        template<EntryPointOutputType VEntryPointOutput>
        struct EntryPointOutputTraits;

        template<typename T>
        struct NodeTraits;

        // Regex       
        /*static constexpr auto materialNameRegex = "[A-Za-z0-9]([A-Za-z0-9_.-]*)";
        static constexpr auto materialNameMaxLength = size_t{ 255 };

        static constexpr auto functionNameRegex = "[A-Za-z]([A-Za-z0-9_]*)";
        static constexpr auto functionNameMaxLength = size_t{ 255 };

        static constexpr auto parameterNameRegex = "[A-Za-z]([A-Za-z0-9_]*)";
        static constexpr auto parameterNameMaxLength = size_t{ 255 };*/

        // Validation errors.
        enum class FunctionAddError 
        {
            InvalidName,
            Duplicate
        };

        // Validation methods.
        static constexpr bool ConstantHasOutput(DataType dataType);

        template<DataType VCompositDataType>
        static constexpr bool CompositeHasOverride(const DataTypes& parameters);
        static constexpr bool CompositeHasOverride(DataType compositDataType, const DataTypes& parameters);

        template<OperatorType VOperatorType>
        static constexpr bool OperatorHasOverride(const DataTypes& parameters);
        static constexpr bool OperatorHasOverride(OperatorType operatorType, const DataTypes& parameters);
        template<OperatorType VOperatorType>
        static constexpr std::optional<OperatorOverride> GetOperatorOverride(const DataTypes& parameters);
        static constexpr std::optional<OperatorOverride> GetOperatorOverride(OperatorType operatorType, const DataTypes& parameters);

        template<BuiltInFunctionType VBuiltInFunctionType>
        static constexpr bool FunctionHasOverride(const DataTypes& parameters);
        static constexpr bool FunctionHasOverride(BuiltInFunctionType builtInFunctionType, const DataTypes& parameters);
        template<BuiltInFunctionType VBuiltInFunctionType>
        static constexpr std::optional<FunctionOverride> GetFunctionOverride(const DataTypes& parameters);
        static constexpr std::optional<FunctionOverride> GetFunctionOverride(BuiltInFunctionType builtInFunctionType, const DataTypes& parameters);

        //static constexpr bool ValidateFunctionName(const std::string& name);
        static constexpr bool ValidateFunctionDuplicate(const Functions& existingFunctions, const std::string& name);

        static constexpr bool ValidateDataType(const DataType dataType);
        static constexpr bool ValidateDataTypes(const DataTypes& dataTypes);
        static constexpr bool ValidateVertexInputType(const VertexInputType vertexInputType);
        static constexpr bool ValidateEntryPointOutputType(const EntryPointOutputType entryPointOutputType);
        static constexpr bool ValidateOperatorType(const OperatorType operatorType);
        static constexpr bool ValidateBuiltInFunctionType(const BuiltInFunctionType builtInFunctionType);

    };

    enum class ValidateMaterialAssetFileError
    {
        BadDataType,
        BadOperatorType,
        BadVertexInputType,
        BadEntryPointOutputType,
        BadNodeIndex,
        MissingParameter,
        MissingArgument
    };

    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, ValidateMaterialAssetFileError> ValidateMaterialAssetFile(
        const MaterialAssetFile& materialAssetFile);

}

#include "Molten/EditorFramework/FileFormat/Validator/MaterialAssetFileValidator.inl"

#endif