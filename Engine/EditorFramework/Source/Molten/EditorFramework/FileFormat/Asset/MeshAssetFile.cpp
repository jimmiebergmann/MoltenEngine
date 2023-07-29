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

#include "Molten/EditorFramework/FileFormat/Asset/MeshAssetFile.hpp"
#include "Molten/EditorFramework/FileFormat/BinaryFile.hpp"
#include <fstream>


namespace Molten::EditorFramework
{

    Expected<MeshAssetFile, ReadMeshAssetFileError> ReadMeshAssetFile(
        const std::filesystem::path& path,
        const ReadMeshAssetFileOptions& options)
    {
        std::ifstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(ReadMeshAssetFileError::OpenFileError);
        }

        return ReadMeshAssetFile(file, options);
    }

    Expected<MeshAssetFile, ReadMeshAssetFileError> ReadMeshAssetFile(
        std::istream& stream,
        const ReadMeshAssetFileOptions& options)
    {
        const auto assetFileHeader = ReadAssetFileHeader(stream);
        if (!assetFileHeader || assetFileHeader->assetType != AssetType::Mesh)
        {
            return Unexpected(ReadMeshAssetFileError::BadAssetHeader);
        }
        
        auto meshAssetFile = MeshAssetFile{
            .globalId = assetFileHeader->globalId
        };

        auto rootBlock = BinaryFile::Parser::ReadBlock(stream);
        if (!rootBlock) 
        {
            return Unexpected(ReadMeshAssetFileError::BinaryFileError);
        }

        auto meshName = rootBlock->ReadScalarProperty<std::string>();
        if (!meshName)
        {
            return Unexpected(ReadMeshAssetFileError::BinaryFileError);
        }
        meshAssetFile.name = meshName->empty() ? "unnamed" : std::move(*meshName);

        {
            auto subMeshesBlock = rootBlock->ReadBlockProperty();
            if (!subMeshesBlock || subMeshesBlock->name != "sub_meshes")
            {
                return Unexpected(ReadMeshAssetFileError::BinaryFileError);
            }

            auto subMeshArray = subMeshesBlock->ReadArrayProperty();
            if (!subMeshArray)
            {
                return Unexpected(ReadMeshAssetFileError::BinaryFileError);
            }

            for (size_t i = 0; i < subMeshArray->elementCount; i++)
            {
                auto& subMesh = meshAssetFile.subMeshes.emplace_back();

                auto submeshBlock = subMeshArray->ReadBlockElement();
                if (!submeshBlock || submeshBlock->name != "sub_mesh")
                {
                    return Unexpected(ReadMeshAssetFileError::MissingSubMeshes);
                }

                auto subMeshName = submeshBlock->ReadScalarProperty<std::string>();
                if (!subMeshName)
                {
                    return Unexpected(ReadMeshAssetFileError::BinaryFileError);
                }
                subMesh.name = std::move(*subMeshName);

                auto indexBufferViewIndex = submeshBlock->ReadScalarProperty<uint64_t>();
                if (!indexBufferViewIndex)
                {
                    return Unexpected(ReadMeshAssetFileError::BinaryFileError);
                }
                subMesh.indexBufferViewIndex = *indexBufferViewIndex;

                auto vertexAttributeArray = submeshBlock->ReadArrayProperty();
                if (!vertexAttributeArray)
                {
                    return Unexpected(ReadMeshAssetFileError::BinaryFileError);
                }

                for (size_t j = 0; j < vertexAttributeArray->elementCount; j++)
                {
                    auto& vertexAttribute = subMesh.vertexAttributes.emplace_back();

                    auto vertexAttributeBlock = vertexAttributeArray->ReadBlockElement();
                    if (!vertexAttributeBlock || vertexAttributeBlock->name != "vertex_attribute")
                    {
                        return Unexpected(ReadMeshAssetFileError::MissingSubMeshes);
                    }

                    auto vertexAttributeProperties = vertexAttributeBlock->ReadProperties<MeshAssetFile::VertexAttributeType, uint64_t>();
                    if (!vertexAttributeProperties)
                    {
                        return Unexpected(ReadMeshAssetFileError::BinaryFileError);
                    }

                    auto& [vertexAttributeType, bufferViewIndex] = *vertexAttributeProperties;
                    vertexAttribute.type = vertexAttributeType;
                    vertexAttribute.bufferViewIndex = bufferViewIndex;
                }

            }            
        }
        {
            auto buffersBlock = rootBlock->ReadBlockProperty();
            if (!buffersBlock || buffersBlock->name != "buffers")
            {
                return Unexpected(ReadMeshAssetFileError::BinaryFileError);
            }

            auto buffersArray = buffersBlock->ReadArrayProperty();
            if (!buffersArray)
            {
                return Unexpected(ReadMeshAssetFileError::BinaryFileError);
            }

            for (size_t i = 0; i < buffersArray->elementCount; i++)
            {
                auto& buffer = meshAssetFile.buffers.emplace_back();

                auto bufferBlock = buffersArray->ReadBlockElement();
                if (!bufferBlock || bufferBlock->name != "buffer")
                {
                    return Unexpected(ReadMeshAssetFileError::MissingSubMeshes);
                }

                auto bufferArray = bufferBlock->ReadArrayProperty();
                if (!bufferArray)
                {
                    return Unexpected(ReadMeshAssetFileError::BinaryFileError);
                }

                auto data = bufferArray->ReadScalarElements<uint8_t>();
                if (!data)
                {
                    return Unexpected(ReadMeshAssetFileError::BinaryFileError);
                }

                buffer.data = std::move(*data);
            }
        }
        {
            auto bufferViewsBlock = rootBlock->ReadBlockProperty();
            if (!bufferViewsBlock || bufferViewsBlock->name != "buffer_views")
            {
                return Unexpected(ReadMeshAssetFileError::BinaryFileError);
            }

            auto bufferViewsArray = bufferViewsBlock->ReadArrayProperty();
            if (!bufferViewsArray)
            {
                return Unexpected(ReadMeshAssetFileError::BinaryFileError);
            }

            for (size_t i = 0; i < bufferViewsArray->elementCount; i++)
            {
                auto& bufferView = meshAssetFile.bufferViews.emplace_back();

                auto bufferViewBlock = bufferViewsArray->ReadBlockElement();
                if (!bufferViewBlock || bufferViewBlock->name != "buffer_view")
                {
                    return Unexpected(ReadMeshAssetFileError::MissingSubMeshes);
                }

                auto bufferViewProperties = bufferViewBlock->ReadProperties<uint64_t, uint64_t, uint64_t>();
                if (!bufferViewProperties)
                {
                    return Unexpected(ReadMeshAssetFileError::BinaryFileError);
                }

                auto& [bufferIndex, bufferOffset, bufferStride] = *bufferViewProperties;
                bufferView.bufferIndex = bufferIndex;
                bufferView.bufferOffset = bufferOffset;
                bufferView.bufferStride = bufferStride;
            }
        }
  
        return meshAssetFile;
    }

    Expected<void, WriteMeshAssetFileError> WriteMeshAssetFile(
        const std::filesystem::path& path,
        const MeshAssetFile& meshAssetFile,
        const WriteMeshAssetFileOptions& options)
    {
        std::ofstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(WriteMeshAssetFileError::OpenFileError);
        }

        return WriteMeshAssetFile(file, meshAssetFile, options);
    }

    Expected<void, WriteMeshAssetFileError> WriteMeshAssetFile(
        std::ostream& stream,
        const MeshAssetFile& meshAssetFile,
        const WriteMeshAssetFileOptions& options)
    {
        const auto assetFileHeader = AssetFileHeader{
            .engineVersion = MOLTEN_VERSION,
            .assetType = AssetType::Mesh,
            .fileVersion = Version{ 0, 1, 0 },
            .globalId = meshAssetFile.globalId
        };

        WriteAssetFileHeader(stream, assetFileHeader);

        auto subMeshBlockResult = [&meshAssetFile]()->Expected<BinaryFile::BlockView, WriteMeshAssetFileError>
        {
            auto subMeshBlocks = std::vector<BinaryFile::BlockView>{};

            for (const auto& subMesh : meshAssetFile.subMeshes)
            {
                if (subMesh.indexBufferViewIndex >= static_cast<uint64_t>(meshAssetFile.bufferViews.size()))
                {
                    return Unexpected(WriteMeshAssetFileError::BadBufferViewIndex);
                }

                auto vertexAttributeBlocks = std::vector<BinaryFile::BlockView>{};
                for (const auto& vertexAttribute : subMesh.vertexAttributes)
                {
                    if (vertexAttribute.bufferViewIndex >= static_cast<uint64_t>(meshAssetFile.bufferViews.size()))
                    {
                        return Unexpected(WriteMeshAssetFileError::BadBufferViewIndex);
                    }

                    vertexAttributeBlocks.emplace_back(BinaryFile::BlockView{
                        .name = "vertex_attribute",
                        .properties = {
                            static_cast<uint32_t>(vertexAttribute.type),
                            vertexAttribute.bufferViewIndex
                        }
                    });
                }

                subMeshBlocks.emplace_back(BinaryFile::BlockView{
                    .name = "sub_mesh",
                    .properties = {
                        subMesh.name,
                        subMesh.indexBufferViewIndex,
                        BinaryFile::ArrayView{ std::move(vertexAttributeBlocks)}
                    }
                });
            }

            return BinaryFile::BlockView{
                .name = "sub_meshes",
                .properties = {
                    BinaryFile::ArrayView{ std::move(subMeshBlocks) }
                }
            };
        }();

        if (!subMeshBlockResult)
        {
            return Unexpected(subMeshBlockResult.Error());
        }

        auto buffersBlockResult = [&meshAssetFile]() -> Expected<BinaryFile::BlockView, WriteMeshAssetFileError>
        {
            auto bufferBlocks = std::vector<BinaryFile::BlockView>{};

            for (const auto& buffer : meshAssetFile.buffers)
            {
                bufferBlocks.emplace_back(BinaryFile::BlockView{
                    .name = "buffer",
                    .properties = {
                        BinaryFile::ArrayView{ buffer.data }
                    }
                });
            }

            return BinaryFile::BlockView{
                .name = "buffers",
                .properties = {
                    BinaryFile::ArrayView{ std::move(bufferBlocks) }
                }
            };
        }();

        if (!buffersBlockResult)
        {
            return Unexpected(buffersBlockResult.Error());
        }

        auto bufferViewsBlockResult = [&meshAssetFile]() -> Expected<BinaryFile::BlockView, WriteMeshAssetFileError>
        {
            auto bufferViewBlocks = std::vector<BinaryFile::BlockView>{};

            for (const auto& bufferView : meshAssetFile.bufferViews)
            {
                if (bufferView.bufferIndex >= static_cast<uint64_t>(meshAssetFile.buffers.size()))
                {
                    return Unexpected(WriteMeshAssetFileError::BadBufferIndex);
                }

                bufferViewBlocks.emplace_back(BinaryFile::BlockView{
                    .name = "buffer_view",
                    .properties = {
                        bufferView.bufferIndex,
                        bufferView.bufferOffset,
                        bufferView.bufferStride
                    }
                });
            }

            return BinaryFile::BlockView{
                .name = "buffer_views",
                    .properties = {
                        BinaryFile::ArrayView{ std::move(bufferViewBlocks) }
                }
            };
        }();

        if (!bufferViewsBlockResult)
        {
            return Unexpected(bufferViewsBlockResult.Error());
        }

        const auto meshBlock = BinaryFile::BlockView{
            .name = "mesh",
            .properties = {
                meshAssetFile.name,
                std::move(*subMeshBlockResult),
                std::move(*buffersBlockResult),
                std::move(*bufferViewsBlockResult)
            }
        };

        auto writeResult = WriteBinaryFile(stream, meshBlock);
        if (!writeResult)
        {
            switch (writeResult.Error())
            {
                case WriteBinaryFileError::OpenFileError: return Unexpected(WriteMeshAssetFileError::OpenFileError);
                case WriteBinaryFileError::InternalError: return Unexpected(WriteMeshAssetFileError::InternalError);
            }
        }

        return {};
    }

}
