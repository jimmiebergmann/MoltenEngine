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

#include "Molten/EditorFramework/FileFormat/Asset/TextureAssetFile.hpp"
#include "Molten/EditorFramework/FileFormat/BinaryFile.hpp"
#include <fstream>

namespace Molten::EditorFramework
{

    Expected<TextureAssetFile, ReadTextureAssetFileError> ReadTextureAssetFile(
        const std::filesystem::path& path,
        const ReadTextureAssetFileOptions& options)
    {
        std::ifstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(ReadTextureAssetFileError::OpenFileError);
        }

        return ReadTextureAssetFile(file, options);
    }

    Expected<TextureAssetFile, ReadTextureAssetFileError> ReadTextureAssetFile(
        std::istream& stream,
        const ReadTextureAssetFileOptions& options)
    {
        if (!options.ignoreHeader)
        {
            const auto assetFileHeader = ReadAssetFileHeader(stream);
            if (assetFileHeader.type != AssetType::Texture)
            {
                return Unexpected(ReadTextureAssetFileError::BadAssetHeader);
            }
        }

        auto texturehAssetFile = TextureAssetFile{};

        auto rootBlock = BinaryFile::Parser::ReadBlock(stream);
        if (!rootBlock)
        {
            return Unexpected(ReadTextureAssetFileError::BinaryFileError);
        }

        auto headerBlock = rootBlock->ReadBlockProperty();
        if (!headerBlock || headerBlock->name != "header")
        {
            return Unexpected(ReadTextureAssetFileError::BinaryFileError);
        }

        auto headerProperties = headerBlock->ReadProperties<uint32_t, uint32_t, uint32_t, TextureAssetFile::ImageFormat, TextureAssetFile::CompressionType>();
        if (!headerProperties)
        {
            return Unexpected(ReadTextureAssetFileError::BinaryFileError);
        }

        const auto& [width, height, depth, imageFormat, compressionType] = *headerProperties;
       
        auto& header = texturehAssetFile.header;
        header.dimensions = { width, height, depth };
        header.imageFormat = imageFormat;
        header.compressionType = compressionType;

        auto imageDataBlock = rootBlock->ReadBlockProperty();
        if (!imageDataBlock || imageDataBlock->name != "image_data")
        {
            return Unexpected(ReadTextureAssetFileError::BinaryFileError);
        }

        auto dataArray = imageDataBlock->ReadArrayProperty();
        if (!dataArray)
        {
            return Unexpected(ReadTextureAssetFileError::BinaryFileError);
        }

        auto data = dataArray->ReadScalarElements<uint8_t>();
        if (!data)
        {
            return Unexpected(ReadTextureAssetFileError::BinaryFileError);
        }

        auto& imageData = texturehAssetFile.imageData;
        imageData.data = std::move(*data);

        return texturehAssetFile;
    }

    Expected<void, WriteTextureAssetFileError> WriteTextureAssetFile(
        const std::filesystem::path& path,
        const TextureAssetFile& textureAssetFile,
        const WriteTextureAssetFileOptions& options)
    {
        std::ofstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(WriteTextureAssetFileError::OpenFileError);
        }

        return WriteTextureAssetFile(file, textureAssetFile, options);
    }

    Expected<void, WriteTextureAssetFileError> WriteTextureAssetFile(
        std::ostream& stream,
        const TextureAssetFile& textureAssetFile,
        const WriteTextureAssetFileOptions& options)
    {
        if (!options.ignoreHeader)
        {
            const auto assetFileHeader = AssetFileHeader{
                .type = AssetType::Texture
            };

            WriteAssetFileHeader(stream, assetFileHeader);
        }

        const auto& header = textureAssetFile.header;

        auto headerBlock = BinaryFile::BlockView{
            .name = "header",
            .properties = {
                header.dimensions.x,
                header.dimensions.y,
                header.dimensions.z,
                static_cast<std::underlying_type_t<decltype(header.imageFormat)>>(header.imageFormat),
                static_cast<std::underlying_type_t<decltype(header.compressionType)>>(header.compressionType)
            }
        };

        const auto& data = textureAssetFile.imageData;

        auto imageDataBlock = BinaryFile::BlockView{
           .name = "image_data",
           .properties = {
                BinaryFile::ArrayView{ data.data }
           }
        };

        const auto textureBlock = BinaryFile::BlockView{
            .name = "texture",
            .properties = {
                std::move(headerBlock),
                std::move(imageDataBlock)
            }
        };

        auto writeResult = WriteBinaryFile(stream, textureBlock);
        if (!writeResult)
        {
            switch (writeResult.Error())
            {
                case WriteBinaryFileError::OpenFileError: return Unexpected(WriteTextureAssetFileError::OpenFileError);
                case WriteBinaryFileError::InternalError: return Unexpected(WriteTextureAssetFileError::InternalError);
            }
        }

        return {};
    }

}
