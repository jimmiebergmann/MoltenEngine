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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_ASSET_MATERIALASSETFILE_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_ASSET_MATERIALASSETFILE_HPP

#include "Molten/EditorFramework/FileFormat/Asset/AssetFile.hpp"
#include "Molten/Utility/Expected.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Matrix.hpp"
#include <vector>

namespace Molten::EditorFramework
{
   
    struct MOLTEN_EDITOR_FRAMEWORK_API MaterialAssetFile
    {
        struct Sampler1D
        {
            Uuid assetGlobalId;
        };

        struct Sampler2D
        {
            Uuid assetGlobalId;
        };

        struct Sampler3D
        {
            Uuid assetGlobalId;
        };

        struct MaterialFunction
        {
            uint64_t index;
        };

        struct LibraryFunction
        {
            Uuid assetGlobalId;
            std::string name;
        };

        enum class DataType : uint8_t
        {
            Bool,
            Int32,
            Float32,
            Vector2f32,
            Vector3f32,
            Vector4f32,
            Matrix4x4f32,
            Sampler1D,
            Sampler2D,
            Sampler3D
        };

        using DataTypes = std::vector<DataType>;

        enum class NodeType : uint8_t
        {
            VertexInput,
            Parameter,            
            Constant,
            Composite,
            Operator,
            Function,
            EntryPointOutput,
            FunctionOutput
        };

        enum class VertexInputType : uint8_t
        {
            Position,
            Uv,
            Normal
        };

        enum class EntryPointOutputType : uint8_t
        {
            Color,
            Opacity,
            Normal
        };

        enum class OperatorType : uint8_t
        {
            // Arithmetic
            Addition,
            Subtraction,
            Multiplication,
            Division
            
        };

        enum class BuiltInFunctionType : uint16_t
        {
            // Sampling
            SampleTexture,

            // Trigonometry
            Sin,
            Cos,
            Tan,

            // Mathematics.
            Min,
            Max,      

            // Vector.
            Cross,
            Dot
        };

        enum class FunctionType : uint16_t
        {
            BuiltIn,
            Material,
            Library
        };

        using ConstantValue = std::variant<
            bool,
            int32_t,
            float,
            Vector2f32,
            Vector3f32,
            Vector4f32,
            Matrix4x4f32,
            Sampler1D,
            Sampler2D,
            Sampler3D>;

        using NodeArgumentValue = std::variant<
            bool,
            int32_t,
            float,
            Vector2f32,
            Vector3f32,
            Vector4f32,
            Matrix4x4f32,
            Sampler1D,
            Sampler2D,
            Sampler3D>;

        struct NodeLink
        {
            uint64_t nodeIndex;
            uint64_t outputIndex;
        };

        using NodeArgument = std::variant<
            NodeLink,
            NodeArgumentValue>;

        using NodeArguments = std::vector<NodeArgument>;

        struct VertexInputNode
        {
            VertexInputType inputType;
            uint8_t inputIndex;
        };

        struct ParameterNode
        {
            DataType dataType;
            std::string name;
        };

        struct ConstantNode
        {
            ConstantValue value;
        };

        struct CompositeNode
        {
            DataType dataType;
            DataTypes parameters;
            NodeArguments arguments;
        };

        struct OperatorNode
        {
            OperatorType operatorType;
            DataType dataType;
            DataTypes parameters;
            NodeArguments arguments;
        };

        using FunctionVariant = std::variant<
            BuiltInFunctionType,
            MaterialFunction,
            LibraryFunction>;

        struct FunctionNode
        {
            FunctionVariant function;
            DataTypes parameters;
            NodeArguments arguments;
        };

        struct EntryPointOutputNode
        {
            EntryPointOutputType outputType;
            NodeArgument argument;
        };

        using EntryPointOutputNodes = std::vector<EntryPointOutputNode>;

        struct FunctionOutputNode
        {
            DataType dataType;
            NodeArgument argument;
        };

        using FunctionOutputNodes = std::vector<FunctionOutputNode>;

        using NodeVariant = std::variant<
            VertexInputNode,
            ParameterNode,
            ConstantNode,
            CompositeNode,
            OperatorNode,
            FunctionNode>;

        using NodeVariants = std::vector<NodeVariant>;

        struct Function
        {
            std::string name = {};
            FunctionOutputNodes outputNodes = {};
            NodeVariants nodes = {};
        };

        using Functions = std::vector<Function>;

        struct EntryPoint
        {
            EntryPointOutputNodes outputNodes = {};
            NodeVariants nodes = {};
        };

        std::string name = {};
        EntryPoint entryPoint = {};
        Functions functions = {};
    };


    struct WriteMaterialAssetFileOptions
    {
        bool ignoreHeader = false;
    };

    struct ReadMaterialAssetFileOptions
    {
        bool ignoreHeader = false;
    };

    enum class WriteMaterialAssetFileError
    {
        OpenFileError,
        InternalError,
        BadNodeIndex
    };

    enum class ReadMaterialAssetFileError
    {
        OpenFileError,
        BadAssetHeader,
        BinaryFileError
    };


    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteMaterialAssetFileError> WriteMaterialAssetFile(
        const std::filesystem::path& path,
        const MaterialAssetFile& materialAssetFile,
        const WriteMaterialAssetFileOptions& options = {});
    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteMaterialAssetFileError> WriteMaterialAssetFile(
        std::ostream& stream,
        const MaterialAssetFile& materialAssetFile,
        const WriteMaterialAssetFileOptions& options = {});


    MOLTEN_EDITOR_FRAMEWORK_API Expected<MaterialAssetFile, ReadMaterialAssetFileError> ReadMaterialAssetFile(
        const std::filesystem::path& path,
        const ReadMaterialAssetFileOptions& options = {});

    MOLTEN_EDITOR_FRAMEWORK_API Expected<MaterialAssetFile, ReadMaterialAssetFileError> ReadMaterialAssetFile(
        std::istream& stream,
        const ReadMaterialAssetFileOptions& options = {});

}

#endif