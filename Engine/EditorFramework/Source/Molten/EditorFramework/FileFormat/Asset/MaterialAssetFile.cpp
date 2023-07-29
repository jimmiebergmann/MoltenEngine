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


#include "Molten/EditorFramework/FileFormat/Asset/MaterialAssetFile.hpp"
#include "Molten/EditorFramework/FileFormat/BinaryFile.hpp"
#include "Molten/EditorFramework/FileFormat/Validator/MaterialAssetFileValidator.hpp"
#include <fstream>

namespace Molten::EditorFramework
{

    Expected<MaterialAssetFile, ReadMaterialAssetFileError> ReadMaterialAssetFile(
        const std::filesystem::path& path,
        const ReadMaterialAssetFileOptions& options)
    {
        std::ifstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(ReadMaterialAssetFileError::OpenFileError);
        }

        return ReadMaterialAssetFile(file, options);
    }

    Expected<MaterialAssetFile, ReadMaterialAssetFileError> ReadMaterialAssetFile(
        std::istream& stream,
        const ReadMaterialAssetFileOptions& options)
    {
        const auto assetFileHeader = ReadAssetFileHeader(stream);
        if (!assetFileHeader || assetFileHeader->assetType != AssetType::Material)
        {
            return Unexpected(ReadMaterialAssetFileError::BadAssetHeader);
        }

        auto materialAssetFile = MaterialAssetFile{
            .globalId = assetFileHeader->globalId
        };

        auto rootBlock = BinaryFile::Parser::ReadBlock(stream);
        if (!rootBlock || rootBlock->name != "material")
        {
            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
        }

        auto materialName = rootBlock->ReadScalarProperty<std::string>();
        if (!materialName)
        {
            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
        }
        materialAssetFile.name = materialName->empty() ? "unnamed" : std::move(*materialName);

        auto readConstantValueBlock = [](BinaryFile::BlockParser& parentBlock) 
            -> Expected<MaterialAssetFile::ConstantValue, ReadMaterialAssetFileError>
        {
            auto constantValueBlock = parentBlock.ReadBlockProperty();
            if (!constantValueBlock || constantValueBlock->name != "constant_value")
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            auto dataType = constantValueBlock->ReadScalarProperty<MaterialAssetFile::DataType>();
            if (!dataType)
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            switch (*dataType)
            {
                case MaterialAssetFile::DataType::Bool:
                {
                    auto value = constantValueBlock->ReadScalarProperty<bool>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }
                    return *value;
                } break;
                case MaterialAssetFile::DataType::Int32:
                {
                    auto value = constantValueBlock->ReadScalarProperty<int32_t>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }
                    return *value;
                } break;
                case MaterialAssetFile::DataType::Float32:
                {
                    auto value = constantValueBlock->ReadScalarProperty<float>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }
                    return *value;
                } break;
                case MaterialAssetFile::DataType::Vector2f32:
                {
                    auto value = constantValueBlock->ReadScalarProperty<Vector2f32>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }
                    return *value;
                } break;
                case MaterialAssetFile::DataType::Vector3f32:
                {
                    auto value = constantValueBlock->ReadScalarProperty<Vector3f32>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }
                    return *value;
                } break;
                case MaterialAssetFile::DataType::Vector4f32:
                {
                    auto value = constantValueBlock->ReadScalarProperty<Vector4f32>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }
                    return *value;
                } break;
                case MaterialAssetFile::DataType::Matrix4x4f32:
                {
                    auto value = constantValueBlock->ReadScalarProperty<Matrix4x4f32>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }
                    return *value;
                } break;
                case MaterialAssetFile::DataType::Sampler1D:
                {
                    auto value = constantValueBlock->ReadScalarProperty<std::string>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    auto uuid = FromString<Uuid>(*value);
                    if (!uuid)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    return MaterialAssetFile::Sampler1D{ *uuid };
                } break;
                case MaterialAssetFile::DataType::Sampler2D:
                {
                    auto value = constantValueBlock->ReadScalarProperty<std::string>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    auto uuid = FromString<Uuid>(*value);
                    if (!uuid)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    return MaterialAssetFile::Sampler2D{ *uuid };
                } break;
                case MaterialAssetFile::DataType::Sampler3D:
                {
                    auto value = constantValueBlock->ReadScalarProperty<std::string>();
                    if (!value)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    auto uuid = FromString<Uuid>(*value);
                    if (!uuid)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    return MaterialAssetFile::Sampler3D{ *uuid };
                } break;
                default: break;
            }

            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
        };

        auto readNodeArgument = [&readConstantValueBlock](BinaryFile::BlockParser& argumentBlock)
            -> Expected<MaterialAssetFile::NodeArgument, ReadMaterialAssetFileError>
        {
            auto nodeType = argumentBlock.ReadScalarProperty<MaterialAssetFile::NodeArgumentType>();
            if (!nodeType)
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            switch (*nodeType)
            {
                case MaterialAssetFile::NodeArgumentType::NodeLink:
                {
                    auto innerBlock = argumentBlock.ReadBlockProperty();
                    if (!innerBlock)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    auto nodeLink = innerBlock->ReadProperties<uint64_t, uint64_t>();
                    if (!nodeLink)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    const auto& [nodeIndex, outputIndex] = *nodeLink;

                    return MaterialAssetFile::NodeLink
                    {
                        .nodeIndex = nodeIndex,
                            .outputIndex = outputIndex
                    };
                };
                case MaterialAssetFile::NodeArgumentType::ConstantValue:
                {
                    auto constantValue = readConstantValueBlock(argumentBlock);
                    if (!constantValue)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    return *constantValue;
                };
                default: break;
            }

            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
        };

        auto readNodeArgumentBlock = [&readNodeArgument](BinaryFile::BlockParser& parentBlock)
            -> Expected<MaterialAssetFile::NodeArgument, ReadMaterialAssetFileError>
        {
            auto argumentBlock = parentBlock.ReadBlockProperty();
            if (!argumentBlock || argumentBlock->name != "argument")
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            return readNodeArgument(*argumentBlock);
        };

        auto readNodeArgumentsArray = [&readNodeArgument, &readConstantValueBlock](BinaryFile::BlockParser& parentBlock)
            ->Expected<std::vector<MaterialAssetFile::NodeArgument>, ReadMaterialAssetFileError>
        {
            auto nodeArgumentArray = parentBlock.ReadArrayProperty();
            if (!nodeArgumentArray)
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            auto nodeArguments = std::vector<MaterialAssetFile::NodeArgument>{};
            nodeArguments.reserve(nodeArgumentArray->elementCount);

            for (size_t i = 0; i < nodeArgumentArray->elementCount; i++)
            {
                auto nodeArgumentBlock = nodeArgumentArray->ReadBlockElement();
                if (!nodeArgumentBlock || nodeArgumentBlock->name != "argument")
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                auto nodeArgument = readNodeArgument(*nodeArgumentBlock);
                if (!nodeArgument)
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                nodeArguments.emplace_back(std::move(*nodeArgument));
            }

            return nodeArguments;
        };

        auto readNodesBlock = [&readConstantValueBlock, &readNodeArgumentsArray, &readNodeArgumentBlock](BinaryFile::BlockParser& parentBlock)
            -> Expected<MaterialAssetFile::NodeVariants, ReadMaterialAssetFileError>
        {
            auto nodesBlock = parentBlock.ReadBlockProperty();
            if (!nodesBlock || nodesBlock->name != "nodes")
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            auto nodeArray = nodesBlock->ReadArrayProperty();
            if (!nodeArray)
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            auto nodes = MaterialAssetFile::NodeVariants{};
            nodes.reserve(static_cast<size_t>(nodeArray->elementCount));

            for (size_t i = 0; i < nodeArray->elementCount; i++)
            {
                auto nodeBlock = nodeArray->ReadBlockElement();
                if (!nodeBlock || nodeBlock->name != "node")
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                const auto nodeType = nodeBlock->ReadScalarProperty<MaterialAssetFile::NodeType>();
                if (!nodeType)
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                auto innerNodeBlock = nodeBlock->ReadBlockProperty();
                if (!innerNodeBlock)
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                switch (*nodeType)
                {
                    case MaterialAssetFile::NodeType::VertexInput: 
                    {
                        if (innerNodeBlock->name != "vertex_input_node")
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto properties = innerNodeBlock->ReadProperties<MaterialAssetFile::VertexInputType, uint8_t>();
                        if (!properties)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto& [inputType, inputIndex] = *properties;

                        nodes.emplace_back(MaterialAssetFile::VertexInputNode{
                            .inputType = inputType,
                            .inputIndex = inputIndex
                        }); 
                    } break;
                    case MaterialAssetFile::NodeType::Parameter: 
                    {
                        if (innerNodeBlock->name != "parameter_node")
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto parameterProps = innerNodeBlock->ReadProperties<MaterialAssetFile::DataType, std::string>();
                        if (!parameterProps)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto& [dataType, name] = *parameterProps;

                        nodes.emplace_back(MaterialAssetFile::ParameterNode{
                            .dataType = dataType, 
                            .name = std::move(name)
                        });
                    } break;
                    case MaterialAssetFile::NodeType::Constant: 
                    {
                        if (innerNodeBlock->name != "constant_node")
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto constantValue = readConstantValueBlock(*innerNodeBlock);
                        if (!constantValue)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        nodes.emplace_back(MaterialAssetFile::ConstantNode{
                            .value = std::move(*constantValue)
                        });
                    } break;
                    case MaterialAssetFile::NodeType::Composite:
                    {
                        if (innerNodeBlock->name != "composite_node")
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto dataType = innerNodeBlock->ReadScalarProperty<MaterialAssetFile::DataType>();
                        if (!dataType)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto parameterArray = innerNodeBlock->ReadArrayProperty();
                        if (!parameterArray)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto parameters = parameterArray->ReadScalarElements<std::underlying_type_t<MaterialAssetFile::DataType>>();
                        if (!parameters)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto argumments = readNodeArgumentsArray(*innerNodeBlock);
                        if (!argumments)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        nodes.emplace_back(MaterialAssetFile::CompositeNode{
                            .dataType = *dataType,
                            .parameters = reinterpret_cast<const std::vector<MaterialAssetFile::DataType>&>(*parameters),
                            .arguments = std::move(*argumments)
                        });
                    } break;
                    case MaterialAssetFile::NodeType::Component:
                    {
                        if (innerNodeBlock->name != "component_node")
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto properties = innerNodeBlock->ReadProperties<MaterialAssetFile::DataType, MaterialAssetFile::DataType>();
                        if (!properties)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        const auto& [dataType, parameter] = *properties;

                        auto nodeArgument = readNodeArgumentBlock(*innerNodeBlock);
                        if (!nodeArgument)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto componentIndexArray = innerNodeBlock->ReadArrayProperty();
                        if (!componentIndexArray)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto componentIndices = componentIndexArray->ReadScalarElements<uint8_t>();
                        if (!componentIndices)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        nodes.emplace_back(MaterialAssetFile::ComponentNode{
                            .dataType = dataType,
                            .parameter = parameter,
                            .argument = std::move(*nodeArgument),
                            .componentIndices = std::move(*componentIndices)
                        });
                    } break;
                    case MaterialAssetFile::NodeType::Operator: 
                    {
                        if (innerNodeBlock->name != "operator_node")
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto properties = innerNodeBlock->ReadProperties<MaterialAssetFile::OperatorType, MaterialAssetFile::DataType>();
                        if (!properties)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        const auto& [operatorType, dataType] = *properties;

                        auto parameterArray = innerNodeBlock->ReadArrayProperty();
                        if (!parameterArray)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto parameters = parameterArray->ReadScalarElements<std::underlying_type_t<MaterialAssetFile::DataType>>();
                        if (!parameters)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto argumments = readNodeArgumentsArray(*innerNodeBlock);
                        if (!argumments)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        nodes.emplace_back(MaterialAssetFile::OperatorNode{
                            .operatorType = operatorType,
                            .dataType = dataType,
                            .parameters = reinterpret_cast<const std::vector<MaterialAssetFile::DataType>&>(*parameters),
                            .arguments = std::move(*argumments)
                        });
                    } break;
                    case MaterialAssetFile::NodeType::Function:
                    {
                        if (innerNodeBlock->name != "function_node")
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto functionType = innerNodeBlock->ReadScalarProperty<MaterialAssetFile::FunctionType>();
                        if (!functionType)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto functionBlock = innerNodeBlock->ReadBlockProperty();
                        if (!functionBlock)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto function = [&functionType, &functionBlock]() -> Expected<MaterialAssetFile::FunctionVariant, ReadMaterialAssetFileError>
                        {
                            switch (*functionType)
                            {
                                case MaterialAssetFile::FunctionType::BuiltIn:
                                {
                                    if (functionBlock->name != "builtin_function")
                                    {
                                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                                    }

                                    auto builtInFunctionType = functionBlock->ReadScalarProperty<MaterialAssetFile::BuiltInFunctionType>();
                                    if (!builtInFunctionType)
                                    {
                                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                                    }

                                    return *builtInFunctionType;
                                } break;
                                case MaterialAssetFile::FunctionType::Material:
                                {
                                    if (functionBlock->name != "material_function")
                                    {
                                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                                    }

                                    auto index = functionBlock->ReadScalarProperty<uint64_t>();
                                    if (!index)
                                    {
                                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                                    }

                                    return MaterialAssetFile::MaterialFunction{
                                        .index = *index
                                    };
                                } break;
                                case MaterialAssetFile::FunctionType::Library:
                                {
                                    if (functionBlock->name != "library_function")
                                    {
                                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                                    }

                                    auto properties = functionBlock->ReadProperties<std::string, std::string>();
                                    if (!properties)
                                    {
                                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                                    }

                                    auto& [assetGlobalIdString, name] = *properties;

                                    auto assetGlobalId = FromString<Uuid>(assetGlobalIdString);
                                    if (!assetGlobalId)
                                    {
                                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                                    }

                                    return MaterialAssetFile::LibraryFunction{
                                        .assetGlobalId = std::move(*assetGlobalId),
                                        .name = std::move(name)
                                    };
                                } break;
                                default: break;
                            }

                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }();

                        if (!function)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto parameterArray = innerNodeBlock->ReadArrayProperty();
                        if (!parameterArray)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto parameters = parameterArray->ReadScalarElements<std::underlying_type_t<MaterialAssetFile::DataType>>();
                        if (!parameters)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        auto argumments = readNodeArgumentsArray(*innerNodeBlock);
                        if (!argumments)
                        {
                            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                        }

                        nodes.emplace_back(MaterialAssetFile::FunctionNode{
                            .function = *function,
                            .parameters = reinterpret_cast<const std::vector<MaterialAssetFile::DataType>&>(*parameters),
                            .arguments = std::move(*argumments)
                        });
                    } break;
                    case MaterialAssetFile::NodeType::EntryPointOutput:
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    } break;
                    case MaterialAssetFile::NodeType::FunctionOutput:
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    } break;
                }
            }

            return nodes;
        };

        auto readEntryPoint = [&rootBlock, &materialAssetFile, &readNodeArgumentBlock, &readNodesBlock]() -> Expected<void, ReadMaterialAssetFileError>
        {
            auto& entryPoint = materialAssetFile.entryPoint;

            auto entryPointBlock = rootBlock->ReadBlockProperty();
            if (!entryPointBlock || entryPointBlock->name != "entrypoint")
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            auto outputNodesBlock = entryPointBlock->ReadBlockProperty();
            if (!outputNodesBlock || outputNodesBlock->name != "output_nodes")
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            auto outputNodeArray = outputNodesBlock->ReadArrayProperty();
            if (!outputNodeArray)
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            for (size_t i = 0; i < outputNodeArray->elementCount; i++)
            {
                auto& outputNode = entryPoint.outputNodes.emplace_back();

                auto outputNodeBlock = outputNodeArray->ReadBlockElement();
                if (!outputNodeBlock || outputNodeBlock->name != "output_node")
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                auto entryPointOutputType = outputNodeBlock->ReadScalarProperty<MaterialAssetFile::EntryPointOutputType>();
                if (!entryPointOutputType)
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                outputNode.outputType = *entryPointOutputType;

                auto argument = readNodeArgumentBlock(*outputNodeBlock);
                if (!argument)
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                outputNode.argument = std::move(*argument);
            }

            auto nodesBlockResult = readNodesBlock(*entryPointBlock);
            if (!nodesBlockResult)
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }
            
            entryPoint.nodes = std::move(*nodesBlockResult);

            return {};
        };

        auto readFunctions = [&rootBlock, &materialAssetFile, &readNodeArgumentBlock, &readNodesBlock]()->Expected<void, ReadMaterialAssetFileError>
        {
            auto functionsBlock = rootBlock->ReadBlockProperty();
            if (!functionsBlock || functionsBlock->name != "functions")
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            auto functionArray = functionsBlock->ReadArrayProperty();
            if (!functionArray)
            {
                return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
            }

            auto& functions = materialAssetFile.functions;
            for (size_t i = 0; i < functionArray->elementCount; i++)
            {
                auto& function = functions.emplace_back();

                auto functionBlock = functionArray->ReadBlockElement();
                if (!functionBlock || functionBlock->name != "function")
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                auto name = functionBlock->ReadScalarProperty<std::string>();
                if(!name)
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                function.name = std::move(name);

                auto outputNodes = functionBlock->ReadBlockProperty();
                if (!outputNodes || outputNodes->name != "output_nodes")
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                auto outputNodeArray = outputNodes->ReadArrayProperty();
                if (!outputNodeArray)
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                auto& functionOutputs = function.outputNodes;
                for (size_t j = 0; j < outputNodeArray->elementCount; j++)
                {
                    auto outputNodeBlock = outputNodeArray->ReadBlockElement();
                    if (!outputNodeBlock || outputNodeBlock->name != "output_node")
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    auto parameter = outputNodeBlock->ReadScalarProperty<MaterialAssetFile::DataType>();
                    if (!parameter)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    auto argument = readNodeArgumentBlock(*outputNodeBlock);
                    if (!argument)
                    {
                        return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                    }

                    functionOutputs.emplace_back(MaterialAssetFile::FunctionOutputNode{
                        .parameter = *parameter,
                        .argument = std::move(*argument)
                    });
                }

                auto nodesBlockResult = readNodesBlock(*functionBlock);
                if (!nodesBlockResult)
                {
                    return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
                }

                function.nodes = std::move(*nodesBlockResult);
            }

            return {};
        };

        if (auto result = readEntryPoint(); !result)
        {
            return Unexpected(result.Error());
        }

        if (auto result = readFunctions(); !result)
        {
            return Unexpected(result.Error());
        }

        return materialAssetFile;
    }

    Expected<void, WriteMaterialAssetFileError> WriteMaterialAssetFile(
        const std::filesystem::path& path,
        const MaterialAssetFile& materialAssetFile,
        const WriteMaterialAssetFileOptions& options)
    {
        std::ofstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(WriteMaterialAssetFileError::OpenFileError);
        }

        return WriteMaterialAssetFile(file, materialAssetFile, options);
    }

    Expected<void, WriteMaterialAssetFileError> WriteMaterialAssetFile(
        std::ostream& stream,
        const MaterialAssetFile& materialAssetFile,
        const WriteMaterialAssetFileOptions& options)
    {
        const auto assetFileHeader = AssetFileHeader{
            .engineVersion = MOLTEN_VERSION,
            .assetType = AssetType::Material,
            .fileVersion = Version{ 0, 1, 0 },
            .globalId = materialAssetFile.globalId  
        };

        WriteAssetFileHeader(stream, assetFileHeader);

        auto createConstantValueBlock = [](const MaterialAssetFile::ConstantValue& constantValue)
        {
            return std::visit([](const auto value)
            {
                using ValueType = std::decay_t<decltype(value)>;
                const auto dataType = MaterialAssetFileValidator::TypeTraits<ValueType>::dataType;

                if constexpr (
                    std::is_same_v<ValueType, bool> == true ||
                    std::is_same_v<ValueType, int32_t> == true ||
                    std::is_same_v<ValueType, float> == true ||
                    std::is_same_v<ValueType, Vector2f32> == true ||
                    std::is_same_v<ValueType, Vector3f32> == true ||
                    std::is_same_v<ValueType, Vector4f32> == true ||
                    std::is_same_v<ValueType, Matrix4x4f32> == true)
                {
                    return BinaryFile::BlockView{
                        .name = "constant_value",
                        .properties = {
                            static_cast<std::underlying_type_t<decltype(dataType)>>(dataType),
                            value
                        }
                    };
                }
                else if constexpr (
                    std::is_same_v<ValueType, MaterialAssetFile::Sampler1D> == true ||
                    std::is_same_v<ValueType, MaterialAssetFile::Sampler2D> == true ||
                    std::is_same_v<ValueType, MaterialAssetFile::Sampler3D> == true)
                {
                    return BinaryFile::BlockView{
                        .name = "constant_value",
                        .properties = {
                            static_cast<std::underlying_type_t<decltype(dataType)>>(dataType),
                            ToString(value.assetGlobalId)
                        }
                    };
                }
                else
                {
                    static_assert(AlwaysFalse<ValueType>, "Missing node value argument type check.");
                }
            }, constantValue);
        };

        auto createNodeArgumentBlock = [&createConstantValueBlock](const MaterialAssetFile::NodeArgument& nodeArgument) -> BinaryFile::BlockView
        {
            return std::visit([&createConstantValueBlock](const auto argument)
            {
                using ArgumentType = std::decay_t<decltype(argument)>;

                if constexpr (std::is_same_v<ArgumentType, MaterialAssetFile::NodeLink> == true)
                {
                    auto nodeLink = BinaryFile::BlockView{
                        .name = "link_argument",
                        .properties = {
                            argument.nodeIndex,
                            argument.outputIndex
                        }
                    };

                    return BinaryFile::BlockView{
                        .name = "argument",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::NodeArgumentType>>(MaterialAssetFile::NodeArgumentType::NodeLink),
                            std::move(nodeLink)
                        }
                    };
                }
                else if constexpr (std::is_same_v<ArgumentType, MaterialAssetFile::ConstantValue> == true)
                {
                    return BinaryFile::BlockView{
                        .name = "argument",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::NodeArgumentType>>(MaterialAssetFile::NodeArgumentType::ConstantValue),
                            createConstantValueBlock(argument)
                        }
                    };
                }
                else
                {
                    static_assert(AlwaysFalse<ArgumentType>, "Missing node argument type check.");
                }
            }, nodeArgument);
        };

        auto createNodeBlock = [&createConstantValueBlock, &createNodeArgumentBlock](const MaterialAssetFile::NodeVariant& nodeVariant)
            -> Expected<BinaryFile::BlockView, WriteMaterialAssetFileError>
        {
            return std::visit([&createConstantValueBlock, &createNodeArgumentBlock](const auto& value)
            {
                using Type = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<Type, MaterialAssetFile::VertexInputNode> == true)
                {
                    auto node = BinaryFile::BlockView{
                        .name = "vertex_input_node",
                        .properties = {
                            static_cast<std::underlying_type_t<decltype(value.inputType)>>(value.inputType),
                            value.inputIndex
                        }
                    };

                    return BinaryFile::BlockView{
                        .name = "node",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::NodeType>>(MaterialAssetFile::NodeType::VertexInput),
                            std::move(node)
                        }
                    };
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::ParameterNode> == true)
                {
                    auto node = BinaryFile::BlockView{
                        .name = "parameter_node",
                        .properties = {
                            static_cast<std::underlying_type_t<decltype(value.dataType)>>(value.dataType),
                            value.name
                        }
                    };

                    return BinaryFile::BlockView{
                        .name = "node",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::NodeType>>(MaterialAssetFile::NodeType::Parameter),
                            std::move(node)
                        }
                    };
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::ConstantNode> == true)
                {
                    auto node = BinaryFile::BlockView{
                        .name = "constant_node",
                        .properties = {
                            createConstantValueBlock(value.value)
                        }
                    };

                    return BinaryFile::BlockView{
                        .name = "node",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::NodeType>>(MaterialAssetFile::NodeType::Constant),
                            std::move(node)
                        }
                    };
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::CompositeNode> == true)
                {
                    using UnderlyingDataType = std::underlying_type_t<MaterialAssetFile::DataType>;

                    auto argumentBlocks = std::vector<BinaryFile::BlockView>{};
                    for (const auto& argument : value.arguments)
                    {
                        argumentBlocks.emplace_back(createNodeArgumentBlock(argument));
                    }

                    auto node = BinaryFile::BlockView{
                        .name = "composite_node",
                        .properties = {
                            static_cast<std::underlying_type_t<decltype(value.dataType)>>(value.dataType),
                            BinaryFile::ArrayView{ std::cref(reinterpret_cast<const std::vector<UnderlyingDataType>&>(value.parameters)) },
                            BinaryFile::ArrayView{ std::move(argumentBlocks) }
                        }
                    };

                    return BinaryFile::BlockView{
                        .name = "node",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::NodeType>>(MaterialAssetFile::NodeType::Composite),
                            std::move(node)
                        }
                    };
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::ComponentNode> == true)
                {
                    auto node = BinaryFile::BlockView{
                        .name = "component_node",
                        .properties = {
                            static_cast<std::underlying_type_t<decltype(value.dataType)>>(value.dataType),
                            static_cast<std::underlying_type_t<decltype(value.parameter)>>(value.parameter),
                            createNodeArgumentBlock(value.argument),
                            BinaryFile::ArrayView{ value.componentIndices }
                        }
                    };

                    return BinaryFile::BlockView{
                        .name = "node",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::NodeType>>(MaterialAssetFile::NodeType::Component),
                            std::move(node)
                        }
                    };
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::OperatorNode> == true)
                {
                    using UnderlyingDataType = std::underlying_type_t<MaterialAssetFile::DataType>;

                    auto argumentBlocks = std::vector<BinaryFile::BlockView>{};
                    for (const auto& argument : value.arguments)
                    {
                        argumentBlocks.emplace_back(createNodeArgumentBlock(argument));
                    }

                    auto node = BinaryFile::BlockView{
                        .name = "operator_node",
                        .properties = {
                            static_cast<std::underlying_type_t<decltype(value.dataType)>>(value.operatorType),
                            static_cast<std::underlying_type_t<decltype(value.dataType)>>(value.dataType),
                            BinaryFile::ArrayView{ std::cref(reinterpret_cast<const std::vector<UnderlyingDataType>&>(value.parameters)) },
                            BinaryFile::ArrayView{ std::move(argumentBlocks) }
                        }
                    };

                    return BinaryFile::BlockView{
                        .name = "node",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::NodeType>>(MaterialAssetFile::NodeType::Operator),
                            std::move(node)
                        }
                    };
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::FunctionNode> == true)
                {
                    auto functionBlock = std::visit([](const auto& function)
                    {
                        using FunctionType = std::decay_t<decltype(function)>;

                        if constexpr (std::is_same_v<FunctionType, MaterialAssetFile::BuiltInFunctionType> == true)
                        {
                            return BinaryFile::BlockView{
                                .name = "builtin_function",
                                .properties = {
                                        static_cast<std::underlying_type_t<MaterialAssetFile::BuiltInFunctionType>>(function)
                                }
                            };
                        }
                        else if constexpr (std::is_same_v<FunctionType, MaterialAssetFile::MaterialFunction> == true)
                        {
                            return BinaryFile::BlockView{
                                .name = "material_function",
                                .properties = {
                                    function.index
                                }
                            };
                        }
                        else if constexpr (std::is_same_v<FunctionType, MaterialAssetFile::LibraryFunction> == true)
                        {
                            return BinaryFile::BlockView{
                                .name = "library_function",
                                .properties = {
                                    ToString(function.assetGlobalId),
                                    function.name
                                }
                            };
                        }
                        else
                        {
                            static_assert(AlwaysFalse<FunctionType>, "Missing function type check.");
                        }
                    }, value.function);

                    auto functionType = std::visit([](const auto& function)
                    {
                        using FunctionType = std::decay_t<decltype(function)>;

                        if constexpr (std::is_same_v<FunctionType, MaterialAssetFile::BuiltInFunctionType> == true)
                        {
                            return MaterialAssetFile::FunctionType::BuiltIn;
                        }
                        else if constexpr (std::is_same_v<FunctionType, MaterialAssetFile::MaterialFunction> == true)
                        {
                            return MaterialAssetFile::FunctionType::Material;
                        }
                        else if constexpr (std::is_same_v<FunctionType, MaterialAssetFile::LibraryFunction> == true)
                        {
                            return MaterialAssetFile::FunctionType::Library;
                        }
                        else
                        {
                            static_assert(AlwaysFalse<FunctionType>, "Missing function type check.");
                        }
                    }, value.function);

                    using UnderlyingDataType = std::underlying_type_t<MaterialAssetFile::DataType>;

                    auto argumentBlocks = std::vector<BinaryFile::BlockView>{};
                    for (const auto& argument : value.arguments)
                    {
                        argumentBlocks.emplace_back(createNodeArgumentBlock(argument));
                    }

                    auto node = BinaryFile::BlockView{
                        .name = "function_node",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::FunctionType>>(functionType),
                            std::move(functionBlock),
                            BinaryFile::ArrayView{ std::cref(reinterpret_cast<const std::vector<UnderlyingDataType>&>(value.parameters)) },
                            BinaryFile::ArrayView{ std::move(argumentBlocks) }
                        }
                    };

                    return BinaryFile::BlockView{
                        .name = "node",
                        .properties = {
                            static_cast<std::underlying_type_t<MaterialAssetFile::NodeType>>(MaterialAssetFile::NodeType::Function),
                            std::move(node)
                        }
                    };
                }
                else
                {
                    static_assert(AlwaysFalse<Type>, "Missing node type check.");
                }

            }, nodeVariant);
        };

        auto entryPointBlockResult = [&materialAssetFile, &createNodeArgumentBlock, &createNodeBlock]() -> Expected<BinaryFile::BlockView, WriteMaterialAssetFileError>
        {
            const auto& entryPoint = materialAssetFile.entryPoint;

            auto outputNodeBlocks = std::vector<BinaryFile::BlockView>{};
            for (const auto& outputNode : entryPoint.outputNodes)
            {
                outputNodeBlocks.emplace_back(BinaryFile::BlockView{
                    .name = "output_node",
                    .properties = {
                        static_cast<std::underlying_type_t<decltype(outputNode.outputType)>>(outputNode.outputType),
                        createNodeArgumentBlock(outputNode.argument)
                    }
                });
            }

            auto outputNodesBlock = BinaryFile::BlockView{
                .name = "output_nodes",
                .properties = {
                    BinaryFile::ArrayView{ std::move(outputNodeBlocks) }
                }
            };

            auto nodeBlocks = std::vector<BinaryFile::BlockView>{};
            for (const auto& node : entryPoint.nodes)
            {
                auto nodeBlockResult = createNodeBlock(node);
                if (!nodeBlockResult)
                {
                    return Unexpected(nodeBlockResult.Error());
                }

                nodeBlocks.emplace_back(std::move(*nodeBlockResult));
            }

            auto nodesBlock = BinaryFile::BlockView{
                .name = "nodes",
                .properties = {
                    BinaryFile::ArrayView{ std::move(nodeBlocks) }
                }
            };

            return BinaryFile::BlockView{
                .name = "entrypoint",
                .properties = {
                    std::move(outputNodesBlock),
                    std::move(nodesBlock)
                }
            };
        }();

        if (!entryPointBlockResult)
        {
            return Unexpected(entryPointBlockResult.Error());
        }

        auto functionsBlockResult = [&materialAssetFile, &createNodeBlock, &createNodeArgumentBlock]() -> Expected<BinaryFile::BlockView, WriteMaterialAssetFileError>
        {
            auto functionBlocks = std::vector<BinaryFile::BlockView>{};
            for (const auto& function : materialAssetFile.functions)
            {
                auto outputNodeBlocks = std::vector<BinaryFile::BlockView>{};
                for (const auto& outputNode : function.outputNodes)
                {
                    outputNodeBlocks.emplace_back(BinaryFile::BlockView{
                        .name = "output_node",
                        .properties = {
                            static_cast<std::underlying_type_t<decltype(outputNode.parameter)>>(outputNode.parameter),
                            createNodeArgumentBlock(outputNode.argument)
                        }
                    });
                }

                auto outputNodesBlock = BinaryFile::BlockView{
                   .name = "output_nodes",
                   .properties = {
                       BinaryFile::ArrayView{ std::move(outputNodeBlocks) }
                   }
                };

                auto nodeBlocks = std::vector<BinaryFile::BlockView>{};
                for (const auto& node : function.nodes)
                {
                    auto nodeBlockResult = createNodeBlock(node);
                    if (!nodeBlockResult)
                    {
                        return Unexpected(nodeBlockResult.Error());
                    }

                    nodeBlocks.emplace_back(std::move(*nodeBlockResult));
                }

                auto nodesBlock = BinaryFile::BlockView{
                    .name = "nodes",
                    .properties = {
                        BinaryFile::ArrayView{ std::move(nodeBlocks) }
                    }
                };

                functionBlocks.emplace_back(BinaryFile::BlockView{
                    .name = "function",
                    .properties = {
                        function.name,
                        std::move(nodesBlock),
                        std::move(outputNodesBlock)
                    }
                });
            }

            return BinaryFile::BlockView{
                .name = "functions",
                .properties = {
                    BinaryFile::ArrayView{ std::move(functionBlocks) }
                }
            };
        }();

        if (!functionsBlockResult)
        {
            return Unexpected(functionsBlockResult.Error());
        }

        const auto rootBlock = BinaryFile::BlockView{
           .name = "material",
           .properties = {
               materialAssetFile.name,
               std::move(*entryPointBlockResult),
               std::move(*functionsBlockResult)
           }
        };

        auto writeResult = WriteBinaryFile(stream, rootBlock);
        if (!writeResult)
        {
            switch (writeResult.Error())
            {
                case WriteBinaryFileError::OpenFileError: return Unexpected(WriteMaterialAssetFileError::OpenFileError);
                case WriteBinaryFileError::InternalError: return Unexpected(WriteMaterialAssetFileError::InternalError);
            }
        }

        return {};
    }

}