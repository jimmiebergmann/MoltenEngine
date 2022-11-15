/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#include "Molten/EditorFramework/FileFormat/TextureAssetFile.hpp"
#include <fstream>

namespace Molten
{

    size_t GetTextureAssetFormatDepth(const TextureAssetFormat format)
    {
        switch (format)
        {
            case TextureAssetFormat::Red8: return 1;
            case TextureAssetFormat::Red8Green8Blue8: return 3;
            case TextureAssetFormat::Red8Green8Blue8Alpha8: return 4;
            case TextureAssetFormat::SrgbRed8Green8Blue8: return 3;
            case TextureAssetFormat::SrgbRed8Green8Blue8Alpha8: return 4;
        }
        return 0;
    }

    TextureAssetFile ReadTextureAssetFile(std::istream& stream)
    {
        TextureAssetFile result = {};

        const auto startPos = static_cast<size_t>(stream.tellg());
        stream.seekg(0, std::ios::end);
        const auto endPos = static_cast<size_t>(stream.tellg());
        stream.seekg(startPos, std::ios::beg);
        const auto size = endPos - startPos;
        auto sizeLeft = size;

        const auto expectedPreDataSize = sizeof(result.dimensions) + sizeof(result.format);
        if (sizeLeft < expectedPreDataSize)
        {
            return {};
        }

        stream.read(reinterpret_cast<char*>(&result.dimensions), sizeof(result.dimensions));
        stream.read(reinterpret_cast<char*>(&result.format), sizeof(result.format));
        uint64_t dataSize = 0;
        stream.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
        sizeLeft -= expectedPreDataSize;

        const auto formatDepth = GetTextureAssetFormatDepth(result.format);
        if (formatDepth == 0)
        {
            return {};
        }

        const auto expectedDataSize =
            static_cast<size_t>(result.dimensions.x) *
            static_cast<size_t>(result.dimensions.y) *
            static_cast<size_t>(result.dimensions.z) *
            formatDepth;

        if (sizeLeft < expectedDataSize || sizeLeft < static_cast<size_t>(dataSize))
        {
            return {};
        }

        result.data.resize(dataSize);
        stream.read(reinterpret_cast<char*>(result.data.data()), static_cast<size_t>(result.data.size()));

        return result;
    }

    TextureAssetFile ReadTextureAssetFileWithHeader(std::istream& stream)
    {
        const auto startPos = static_cast<size_t>(stream.tellg());
        stream.seekg(0, std::ios::end);
        const auto endPos = static_cast<size_t>(stream.tellg());
        const auto size = endPos - startPos;
        if (size <= AssetFileHeader::packedSize)
        {
            return {};
        }
        stream.seekg(startPos + AssetFileHeader::packedSize, std::ios::beg);

        return ReadTextureAssetFile(stream);
    }

    TextureAssetFile ReadTextureAssetFileWithHeader(const std::filesystem::path& path)
    {
        std::ifstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return {};
        }

        return ReadTextureAssetFileWithHeader(file);
    }

    void WriteTextureAssetFile(
        std::ostream& stream,
        const TextureAssetFile& textureAssetFile)
    {
        stream.write(reinterpret_cast<const char*>(&textureAssetFile.dimensions), sizeof(textureAssetFile.dimensions));
        stream.write(reinterpret_cast<const char*>(&textureAssetFile.format), sizeof(textureAssetFile.format));
        const auto dataSize = static_cast<uint64_t>(textureAssetFile.data.size());
        stream.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));

        if (dataSize > 0)
        {
            stream.write(reinterpret_cast<const char*>(textureAssetFile.data.data()), static_cast<size_t>(textureAssetFile.data.size()));
        }
    }

    void WriteTextureAssetFileWithHeader(
        std::ostream& stream,
        const AssetFileHeader& assetFileHeader,
        const TextureAssetFile& textureAssetFile)
    {
        WriteAssetFileHeader(stream, assetFileHeader);
        WriteTextureAssetFile(stream, textureAssetFile);
    }

    void WriteTextureAssetFileWithHeader(
        const std::filesystem::path& path,
        const AssetFileHeader& assetFileHeader,
        const TextureAssetFile& textureAssetFile)
    {
        std::ofstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return;
        }

        WriteTextureAssetFileWithHeader(
            file,
            assetFileHeader,
            textureAssetFile);
    }

}
