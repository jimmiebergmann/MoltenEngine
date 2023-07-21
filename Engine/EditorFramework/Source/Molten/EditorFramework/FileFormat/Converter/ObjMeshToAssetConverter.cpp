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

#include "Molten/EditorFramework/FileFormat/Converter/ObjMeshToAssetConverter.hpp"
#include <set>
#include <tuple>
#include <limits>

namespace Molten::EditorFramework
{

    Expected<MeshAssetFile, ConvertToMeshAssetFileError> ConvertToMeshAssetFile(
        const ObjMeshFile::Object& objObject)
    {
        MeshAssetFile meshAssetFile{};

        struct VertexGroup
        {
            Vector3f32 position;
            Vector3f32 normal;
            Vector2f32 uv;
            size_t index;

            bool operator < (const VertexGroup& rhs) const {
                if (position.x != rhs.position.x) return position.x < rhs.position.x;
                if (position.y != rhs.position.y) return position.y < rhs.position.y;
                if (position.z != rhs.position.z) return position.z < rhs.position.z;
                if (normal.x != rhs.normal.x) return normal.x < rhs.normal.x;
                if (normal.y != rhs.normal.y) return normal.y < rhs.normal.y;
                if (normal.z != rhs.normal.z) return normal.z < rhs.normal.z;
                if (uv.x != rhs.uv.x) return uv.x < rhs.uv.x;
                return uv.y < rhs.uv.y;
            }
        };

        meshAssetFile.name = objObject.name;

        meshAssetFile.buffers = {
            MeshAssetFile::Buffer{},
            MeshAssetFile::Buffer{},
            MeshAssetFile::Buffer{}, 
            MeshAssetFile::Buffer{} 
        };

        meshAssetFile.bufferViews = {
            MeshAssetFile::BufferView{.bufferIndex = 0, .bufferOffset = 0, .bufferStride = 0 },
            MeshAssetFile::BufferView{.bufferIndex = 1, .bufferOffset = 0, .bufferStride = 12 },
            MeshAssetFile::BufferView{.bufferIndex = 2, .bufferOffset = 0, .bufferStride = 12 },
            MeshAssetFile::BufferView{.bufferIndex = 3, .bufferOffset = 0, .bufferStride = 8 } 
        };

        auto& positionBuffer = meshAssetFile.buffers.at(1);
        auto& normalBuffer = meshAssetFile.buffers.at(2);
        auto& uvBuffer = meshAssetFile.buffers.at(3);

        auto& AssetMeshSubMesh = meshAssetFile.subMeshes.emplace_back();
        AssetMeshSubMesh.indexBufferViewIndex = 0;
        AssetMeshSubMesh.vertexAttributes.emplace_back(MeshAssetFile::VertexAttributeType::Position, uint64_t{ 1 });
        AssetMeshSubMesh.vertexAttributes.emplace_back(MeshAssetFile::VertexAttributeType::Normal, uint64_t{ 2 });
        AssetMeshSubMesh.vertexAttributes.emplace_back(MeshAssetFile::VertexAttributeType::Uv, uint64_t{ 3 });

        auto vertexSet = std::set<VertexGroup>{};
        auto currentVertexBufferIndex = size_t{ 0 };

        auto tempIndexBuffer = std::vector<uint32_t>{};

        auto handleVertex = [&](const ObjMeshFile::Triangle& triangle, size_t triangleVertexIndex) -> Expected<void, ConvertToMeshAssetFileError>
        {
            auto vertexIndex = static_cast<size_t>(triangle.vertexIndices[triangleVertexIndex]) - size_t{ 1 };
            if (vertexIndex >= objObject.vertices.size())
            {
                return Unexpected(ConvertToMeshAssetFileError::VertexIndexOverflow);
            }
            auto vertex = objObject.vertices.at(vertexIndex);

            auto normalIndex = static_cast<size_t>(triangle.normalIndices[triangleVertexIndex]) - size_t{ 1 };
            if (normalIndex >= objObject.normals.size())
            {
                return Unexpected(ConvertToMeshAssetFileError::NormalIndexOverflow);
            }
            auto normal = objObject.normals.at(normalIndex).Normal();

            auto textureCoordIndex = static_cast<size_t>(triangle.textureCoordinateIndices[triangleVertexIndex]) - size_t{ 1 };
            if (textureCoordIndex >= objObject.textureCoordinates.size())
            {
                return Unexpected(ConvertToMeshAssetFileError::NormalIndexOverflow);
            }
            auto textureCoordinate = objObject.textureCoordinates.at(textureCoordIndex);

            auto vertexTuple = VertexGroup{ 
                .position = vertex, 
                .normal = normal, 
                .uv = textureCoordinate
            };

            auto appendVertexBuffer = [&](MeshAssetFile::Buffer& buffer, const auto value) 
            {
                auto& data = buffer.data;
                const auto insertPosition = data.size();
                data.insert(data.end(), sizeof(value.c), uint8_t{ 0 });
                std::memcpy(data.data() + insertPosition, value.c, sizeof(value.c));
            };

            auto it = vertexSet.find(vertexTuple);
            if (it == vertexSet.end())
            {
                vertexTuple.index = currentVertexBufferIndex;
                it = vertexSet.emplace(vertexTuple).first;
                ++currentVertexBufferIndex;

                appendVertexBuffer(positionBuffer, it->position);
                appendVertexBuffer(normalBuffer, it->normal);
                appendVertexBuffer(uvBuffer, it->uv);
            }
            
            tempIndexBuffer.push_back(static_cast<uint32_t>(it->index));

            return {};
        };

        for (auto& group : objObject.groups)
        {
            for (auto& smoothGroup : group->smoothingGroups)
            {
                for (auto& triangle : smoothGroup->triangles)
                {
                    for (size_t i = 0; i < 3; i++)
                    {
                        if (auto result = handleVertex(triangle, i); !result)
                        {
                            return Unexpected(result.Error());
                        }
                    }    
                }
            }
        }

        auto& indexBuffer = meshAssetFile.buffers.at(0);
        auto& indexBufferView = meshAssetFile.bufferViews.at(0);

        if (tempIndexBuffer.size() <= std::numeric_limits<uint16_t>::max()) // 2 bytes per index.
        {
            indexBufferView.bufferStride = 2;
            indexBuffer.data.resize(tempIndexBuffer.size() * sizeof(uint16_t));
            for (size_t i = 0; i < tempIndexBuffer.size(); i++)
            {
                const auto shortIndex = static_cast<uint16_t>(tempIndexBuffer.at(i));
                std::memcpy(indexBuffer.data.data() + (i * sizeof(uint16_t)), reinterpret_cast<const void*>(&shortIndex), sizeof(uint16_t));
            }
        }
        else // 4 bytes per index.
        {
            indexBufferView.bufferStride = 4;
            indexBuffer.data.resize(tempIndexBuffer.size() * sizeof(uint32_t));
            std::memcpy(indexBuffer.data.data(), tempIndexBuffer.data(), indexBuffer.data.size());
        }

        return meshAssetFile;
    }

}