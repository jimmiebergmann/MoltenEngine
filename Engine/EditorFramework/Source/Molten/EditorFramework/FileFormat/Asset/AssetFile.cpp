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

#include "Molten/EditorFramework/FileFormat/Asset/AssetFile.hpp"
#include "Molten/EditorFramework/FileFormat/Validator/AssetFileHeaderValidator.hpp"
#include <fstream>

namespace Molten::EditorFramework
{

    static const auto g_assetFileSignature = std::array<char, 12>{ 'm', 'o', 'l', 't', 'e', 'n', '.', 'a', 's', 's', 'e', 't', };

    Expected<AssetFileHeader, ReadAssetFileHeaderError> ReadAssetFileHeader(const std::filesystem::path& path)
    {
        std::ifstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(ReadAssetFileHeaderError::OpenFileError);
        }

        return ReadAssetFileHeader(file);
    }

    Expected<AssetFileHeader, ReadAssetFileHeaderError> ReadAssetFileHeader(std::istream& stream)
    {
        const auto startPos = static_cast<size_t>(stream.tellg());
        stream.seekg(0, std::ios::end);
        const auto endPos = static_cast<size_t>(stream.tellg());
        stream.seekg(startPos, std::ios::beg);

        const auto size = endPos - startPos;
        if (size < 56)
        {
            return Unexpected(ReadAssetFileHeaderError::UnexpectedEndOfFile);
        }

        auto fileSignature = std::array<char, 12>{};
        stream.read(fileSignature.data(), fileSignature.size());
        const auto validSignature = std::equal(fileSignature.begin(), fileSignature.end(), g_assetFileSignature.begin());
        if (!validSignature)
        {
            return Unexpected(ReadAssetFileHeaderError::BadAssetFileSignature);
        }

        auto result = AssetFileHeader{};

        stream.read(reinterpret_cast<char*>(&result.engineVersion), sizeof(result.engineVersion));
        stream.read(reinterpret_cast<char*>(&result.assetType), sizeof(result.assetType));
        stream.read(reinterpret_cast<char*>(&result.fileVersion), sizeof(result.fileVersion));
        stream.read(reinterpret_cast<char*>(&result.globalId), sizeof(result.globalId));
        
        if (!AssetFileHeaderValidator::ValidateAssetType(result.assetType))
        {
            return Unexpected(ReadAssetFileHeaderError::BadAssetType);
        }

        return result;
    }

    Expected<void, WriteAssetFileHeaderError> WriteAssetFileHeader(
        const std::filesystem::path& path,
        const AssetFileHeader& assetFileHeader)
    {
        std::ofstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(WriteAssetFileHeaderError::OpenFileError);
        }

        return WriteAssetFileHeader(file, assetFileHeader);
    }

    Expected<void, WriteAssetFileHeaderError> WriteAssetFileHeader(
        std::ostream& stream,
        const AssetFileHeader& assetFileHeader)
    {
        stream.write(g_assetFileSignature.data(), g_assetFileSignature.size());
        stream.write(reinterpret_cast<const char*>(&assetFileHeader.engineVersion), sizeof(assetFileHeader.engineVersion));
        stream.write(reinterpret_cast<const char*>(&assetFileHeader.assetType), sizeof(assetFileHeader.assetType));
        stream.write(reinterpret_cast<const char*>(&assetFileHeader.fileVersion), sizeof(assetFileHeader.fileVersion));      
        stream.write(reinterpret_cast<const char*>(&assetFileHeader.globalId), sizeof(assetFileHeader.globalId));

        return {};
    }

}
