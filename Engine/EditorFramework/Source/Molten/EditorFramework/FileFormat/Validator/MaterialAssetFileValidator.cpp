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

#include "Molten/EditorFramework/FileFormat/Validator/MaterialAssetFileValidator.hpp"

namespace Molten::EditorFramework
{

    Expected<void, ValidateMaterialAssetFileError> ValidateMaterialAssetFile(
        const MaterialAssetFile& materialAssetFile)
    {
        using Validator = MaterialAssetFileValidator;

        auto validateNodeArgument = [&](const MaterialAssetFile::NodeArgument& nodeArgument, const uint64_t nodeCount)
            -> Expected<void, ValidateMaterialAssetFileError>
        {
            return std::visit([&](const auto& value)
                -> Expected<void, ValidateMaterialAssetFileError>
            {
                using Type = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<Type, MaterialAssetFile::NodeLink> == true)
                {
                    if (value.nodeIndex >= nodeCount)
                    {
                        return Unexpected(ValidateMaterialAssetFileError::BadNodeIndex);
                    }

                    return {};
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::ConstantValue> == true)
                {
                    return {};
                }
                else
                {
                    static_assert(AlwaysFalse<Type>, "Missing node argument type check.");
                }

            }, nodeArgument);

        };

        auto validateNodeArguments = [&](const MaterialAssetFile::NodeArguments& nodeArguments, const uint64_t nodeCount)
            -> Expected<void, ValidateMaterialAssetFileError>
        {
            for (const auto& nodeArgument : nodeArguments)
            {
                if (auto result = validateNodeArgument(nodeArgument, nodeCount); !result)
                {
                    return result;
                }
            }
            return {};
        };

        auto validateNode = [&](const MaterialAssetFile::NodeVariant& node, const uint64_t nodeCount)
            -> Expected<void, ValidateMaterialAssetFileError>
        {
            return std::visit([&](const auto& value) -> Expected<void, ValidateMaterialAssetFileError>
            {
                using Type = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<Type, MaterialAssetFile::VertexInputNode> == true)
                {
                    if (!Validator::ValidateVertexInputType(value.inputType))
                    {
                        return Unexpected(ValidateMaterialAssetFileError::BadVertexInputType);
                    }
                    return {};
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::ParameterNode> == true)
                {
                    if (!Validator::ValidateDataType(value.dataType))
                    {
                        return Unexpected(ValidateMaterialAssetFileError::BadDataType);
                    }
                    return {};
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::ConstantNode> == true)
                {
                    return {};
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::CompositeNode> == true)
                {
                    if (!Validator::ValidateDataType(value.dataType) || !Validator::ValidateDataTypes(value.parameters))
                    {
                        return Unexpected(ValidateMaterialAssetFileError::BadDataType);
                    }
                    if (value.parameters.size() < value.arguments.size())
                    {
                        return Unexpected(ValidateMaterialAssetFileError::MissingParameter);
                    }
                    if (value.parameters.size() > value.arguments.size())
                    {
                        return Unexpected(ValidateMaterialAssetFileError::MissingArgument);
                    }
                    if (auto result = validateNodeArguments(value.arguments, nodeCount); !result)
                    {
                        return result;
                    }

                    return {};
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::ComponentNode> == true)
                {
                    if (!Validator::ValidateDataType(value.dataType) || !Validator::ValidateDataType(value.parameter))
                    {
                        return Unexpected(ValidateMaterialAssetFileError::BadDataType);
                    }
                    return {};
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::OperatorNode> == true)
                {
                    if (!Validator::ValidateOperatorType(value.operatorType))
                    {
                        return Unexpected(ValidateMaterialAssetFileError::BadOperatorType);
                    }
                    if (!Validator::ValidateDataType(value.dataType) || !Validator::ValidateDataTypes(value.parameters))
                    {
                        return Unexpected(ValidateMaterialAssetFileError::BadDataType);
                    }
                    if (value.parameters.size() < value.arguments.size())
                    {
                        return Unexpected(ValidateMaterialAssetFileError::MissingParameter);
                    }
                    if (value.parameters.size() > value.arguments.size())
                    {
                        return Unexpected(ValidateMaterialAssetFileError::MissingArgument);
                    }
                    if (auto result = validateNodeArguments(value.arguments, nodeCount); !result)
                    {
                        return result;
                    }

                    return {};
                }
                else if constexpr (std::is_same_v<Type, MaterialAssetFile::FunctionNode> == true)
                {
                    if (!Validator::ValidateDataTypes(value.parameters))
                    {
                        return Unexpected(ValidateMaterialAssetFileError::BadDataType);
                    }
                    if (value.parameters.size() < value.arguments.size())
                    {
                        return Unexpected(ValidateMaterialAssetFileError::MissingParameter);
                    }
                    if (value.parameters.size() > value.arguments.size())
                    {
                        return Unexpected(ValidateMaterialAssetFileError::MissingArgument);
                    }
                    if (auto result = validateNodeArguments(value.arguments, nodeCount); !result)
                    {
                        return result;
                    }

                    return {};
                }
                else
                {
                    static_assert(AlwaysFalse<Type>, "Missing node type check.");
                }
            }, node);
        };

        auto validateNodes = [&validateNode](const MaterialAssetFile::NodeVariants& nodes)
            -> Expected<void, ValidateMaterialAssetFileError>
        {
            const uint64_t nodeCount = static_cast<uint64_t>(nodes.size());

            for (const auto& node : nodes)
            {
                if (auto result = validateNode(node, nodeCount); !result)
                {
                    return Unexpected(result.Error());
                }
            }

            return {};
        };

        const uint64_t entryPointNodeCount = static_cast<uint64_t>(materialAssetFile.entryPoint.nodes.size());
        for (const auto& outputNode : materialAssetFile.entryPoint.outputNodes)
        {
            if (!Validator::ValidateEntryPointOutputType(outputNode.outputType))
            {
                return Unexpected(ValidateMaterialAssetFileError::BadDataType);
            }
            if (auto result = validateNodeArgument(outputNode.argument, entryPointNodeCount); !result)
            {
                return result;
            }
        }

        if (auto result = validateNodes(materialAssetFile.entryPoint.nodes); !result)
        {
            return Unexpected(result.Error());
        }

        for (const auto function : materialAssetFile.functions)
        {
            const uint64_t functionNodeCount = static_cast<uint64_t>(function.nodes.size());
            for (const auto& outputNode : function.outputNodes)
            {
                if (!Validator::ValidateDataType(outputNode.parameter))
                {
                    return Unexpected(ValidateMaterialAssetFileError::BadDataType);
                }
                if (auto result = validateNodeArgument(outputNode.argument, functionNodeCount); !result)
                {
                    return result;
                }
            }

            if (auto result = validateNodes(function.nodes); !result)
            {
                return Unexpected(result.Error());
            }
        }

        return {};
    }

}
