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
#include <fstream>

namespace Molten::EditorFramework
{

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
        if (!options.ignoreHeader)
        {
            const auto assetFileHeader = AssetFileHeader{
                .type = AssetType::Material
            };

            WriteAssetFileHeader(stream, assetFileHeader);
        }
/*
        auto nodesBlockResult = [&materialAssetFile]() -> Expected<BinaryFile::BlockView, WriteMaterialAssetFileError>
        {
            auto nodeBlocks = std::vector<BinaryFile::BlockView>{};

            for (const auto& node : materialAssetFile.nodes)
            {
                auto nodeProperties = std::visit([](auto& value) -> Expected<BinaryFile::PropertiesView, WriteMaterialAssetFileError>
                {
                    using type = std::decay_t<decltype(value)>;
                    if constexpr (std::is_same_v<type, MaterialAssetFile::InputNode> == true)
                    {
                        return BinaryFile::PropertiesView{
                            static_cast<std::underlying_type_t<decltype(value.inputType)>>(value.inputType),
                            value.inputIndex
                        };
                    }
                    else if constexpr (std::is_same_v<type, MaterialAssetFile::OutputNode> == true)
                    {
                        return BinaryFile::PropertiesView{
                            static_cast<std::underlying_type_t<decltype(value.outputType)>>(value.outputType)
                        };
                    }
                    else if constexpr (std::is_same_v<type, MaterialAssetFile::ParameterNode> == true)
                    {
                        return BinaryFile::PropertiesView{
                            static_cast<std::underlying_type_t<decltype(value.dataType)>>(value.dataType),
                            value.name
                        };
                    }
                    else if constexpr (std::is_same_v<type, MaterialAssetFile::ConstantNode> == true)
                    {
                        auto properties = BinaryFile::PropertiesView{
                            static_cast<std::underlying_type_t<decltype(value.dataType)>>(value.dataType),
                        };

                        std::visit([&properties](auto& value)
                        {
                            properties.emplace_back(value);
                        }, value.value);

                        return properties;
                    }
                    else if constexpr (std::is_same_v<type, MaterialAssetFile::OperatorNode> == true)
                    {
                        auto properties = BinaryFile::PropertiesView{
                            static_cast<std::underlying_type_t<decltype(value.operatorType)>>(value.operatorType),
                        };

                        for (auto dataType : value.dataTypes)
                        {
                            properties.emplace_back(static_cast<std::underlying_type_t<decltype(dataType)>>(dataType));
                        }

                        return properties;
                    }
                    else if constexpr (std::is_same_v<type, MaterialAssetFile::FunctionNode> == true)
                    {
                        auto properties = BinaryFile::PropertiesView{
                            static_cast<std::underlying_type_t<decltype(value.functionType)>>(value.functionType),
                        };

                        for (auto parameter : value.parameters)
                        {
                            properties.emplace_back(static_cast<std::underlying_type_t<decltype(parameter)>>(parameter));
                        }

                        return properties;
                    }
                    else if constexpr (std::is_same_v<type, MaterialAssetFile::SamplerNode> == true)
                    {
                        return BinaryFile::PropertiesView{
                            static_cast<std::underlying_type_t<decltype(value.samplerType)>>(value.samplerType),
                            ToString(value.assetGlobalId)
                        };
                    }
                    else
                    {
                        static_assert(AlwaysFalse<type>, "Missing data type case in WriteBinaryFile::CalculateBlockSize.");
                    }
                }, node);

                if (!nodeProperties)
                {
                    return Unexpected(nodeProperties.Error());
                }

                nodeBlocks.emplace_back(BinaryFile::BlockView{
                    .name = "node",
                    .properties = {
                        std::move(*nodeProperties)
                    }
                });
            }

            return BinaryFile::BlockView{
                .name = "nodes",
                .properties = {
                    BinaryFile::ArrayView{ std::move(nodeBlocks) }
                }
            };
        }();

        if (!nodesBlockResult)
        {
            return Unexpected(nodesBlockResult.Error());
        }

        auto linksBlockResult = [&materialAssetFile]() -> Expected<BinaryFile::BlockView, WriteMaterialAssetFileError>
        {
            auto linkBlocks = std::vector<BinaryFile::BlockView>{};

            for (const auto& link : materialAssetFile.links)
            {
                if (link.sourceNodeIndex >= static_cast<uint64_t>(materialAssetFile.nodes.size()) ||
                    link.targetNodeIndex >= static_cast<uint64_t>(materialAssetFile.nodes.size()))
                {
                    return Unexpected(WriteMaterialAssetFileError::BadNodeIndex);
                }

                linkBlocks.emplace_back(BinaryFile::BlockView{
                    .name = "link",
                    .properties = {
                        link.sourceNodeIndex,
                        link.sourceOutputIndex,
                        link.targetNodeIndex,
                        link.targetInputIndex
                    }
                });
            }

            return BinaryFile::BlockView{
                .name = "links",
                .properties = {
                    BinaryFile::ArrayView{ std::move(linkBlocks) }
                }
            };
        }();

        if (!linksBlockResult)
        {
            return Unexpected(nodesBlockResult.Error());
        }

        const auto rootBlock = BinaryFile::BlockView{
            .name = "material",
            .properties = {
                materialAssetFile.name,
                std::move(*nodesBlockResult),
                std::move(*linksBlockResult)
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
        }*/

        return {};
    }

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
        if (!options.ignoreHeader)
        {
            const auto assetFileHeader = ReadAssetFileHeader(stream);
            if (assetFileHeader.type != AssetType::Material)
            {
                return Unexpected(ReadMaterialAssetFileError::BadAssetHeader);
            }
        }

        auto materialAssetFile = MaterialAssetFile{};

        auto rootBlock = BinaryFile::Parser::ReadBlock(stream);
        if (!rootBlock)
        {
            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
        }

        auto materialName = rootBlock->ReadScalarProperty<std::string>();
        if (!materialName)
        {
            return Unexpected(ReadMaterialAssetFileError::BinaryFileError);
        }
        materialAssetFile.name = materialName->empty() ? "unnamed" : std::move(*materialName);



        return materialAssetFile;
    }

}