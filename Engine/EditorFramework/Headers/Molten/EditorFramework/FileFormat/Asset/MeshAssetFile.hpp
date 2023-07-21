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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_ASSET_MESHASSETFILE_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_ASSET_MESHASSETFILE_HPP

#include "Molten/EditorFramework/FileFormat/Asset/AssetFile.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Utility/Expected.hpp"

namespace Molten::EditorFramework
{

    struct MOLTEN_EDITOR_FRAMEWORK_API MeshAssetFile
    {
        struct Buffer
        {
            std::vector<uint8_t> data = {};
        };

        struct BufferView
        {
            uint64_t bufferIndex;
            uint64_t bufferOffset;
            uint64_t bufferStride;
        };

        enum class VertexAttributeType : uint32_t
        {
            Position,
            Normal,
            Uv,
            Other
        };

        struct VertexAttribute
        {
            VertexAttributeType type;
            uint64_t bufferViewIndex;
        };

        struct SubMesh
        {
            std::string name;
            uint64_t indexBufferViewIndex;
            std::vector<VertexAttribute> vertexAttributes;
        };

        std::string name = {};
        std::vector<SubMesh> subMeshes = {};
        std::vector<Buffer> buffers = {};
        std::vector<BufferView> bufferViews = {};
    };


    struct WriteMeshAssetFileOptions
    {
        bool ignoreHeader = false;
    };

    struct ReadMeshAssetFileOptions
    {
        bool ignoreHeader = false;
    };

    enum class WriteMeshAssetFileError
    {
        OpenFileError,
        InternalError,
        BadBufferIndex,
        BadBufferViewIndex
    };

    enum class ReadMeshAssetFileError
    {
        OpenFileError,
        BadAssetHeader,
        BinaryFileError,
        MissingSubMeshes,
        MissingBuffers,
        MissingBufferViews,
        BadBufferIndex,
        BadBufferViewIndex
    };


    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteMeshAssetFileError> WriteMeshAssetFile(
        const std::filesystem::path& path,
        const MeshAssetFile& meshAssetFile,
        const WriteMeshAssetFileOptions& options = {});

    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteMeshAssetFileError> WriteMeshAssetFile(
        std::ostream& stream,
        const MeshAssetFile& meshAssetFile,
        const WriteMeshAssetFileOptions& options = {});


    MOLTEN_EDITOR_FRAMEWORK_API Expected<MeshAssetFile, ReadMeshAssetFileError> ReadMeshAssetFile(
        const std::filesystem::path& path,
        const ReadMeshAssetFileOptions& options = {});

    MOLTEN_EDITOR_FRAMEWORK_API Expected<MeshAssetFile, ReadMeshAssetFileError> ReadMeshAssetFile(
        std::istream& stream,
        const ReadMeshAssetFileOptions& options = {});

}

#endif