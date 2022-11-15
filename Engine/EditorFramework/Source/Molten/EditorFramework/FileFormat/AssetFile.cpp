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

#include "Molten/EditorFramework/FileFormat/AssetFile.hpp"
#include <fstream>

namespace Molten
{

    AssetFileHeader ReadAssetFileHeader(std::istream& stream)
    {
        const auto startPos = static_cast<size_t>(stream.tellg());
        stream.seekg(0, std::ios::end);
        const auto endPos = static_cast<size_t>(stream.tellg());
        stream.seekg(startPos, std::ios::beg);

        const auto size = endPos - startPos;
        if (size < AssetFileHeader::packedSize)
        {
            return {};
        }  

        AssetFileHeader result = {};
        stream.read(result.magic.data(), result.magic.size());
        stream.read(reinterpret_cast<char*>(&result.fileVersion), sizeof(result.fileVersion));
        stream.read(reinterpret_cast<char*>(&result.engineVersion), sizeof(result.engineVersion));
        stream.read(reinterpret_cast<char*>(&result.globalId), sizeof(result.globalId));
        stream.read(reinterpret_cast<char*>(&result.type), sizeof(result.type));
        return result;
    }

    AssetFileHeader ReadAssetFileHeader(const std::filesystem::path& path)
    {
        std::ifstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return {};
        }

        return ReadAssetFileHeader(file);
    }

    void WriteAssetFileHeader(
        std::ostream& stream,
        const AssetFileHeader& assetFileHeader)
    {
        stream.write(assetFileHeader.magic.data(), assetFileHeader.magic.size());
        stream.write(reinterpret_cast<const char*>(&assetFileHeader.fileVersion), sizeof(assetFileHeader.fileVersion));
        stream.write(reinterpret_cast<const char*>(&assetFileHeader.engineVersion), sizeof(assetFileHeader.engineVersion));
        stream.write(reinterpret_cast<const char*>(&assetFileHeader.globalId), sizeof(assetFileHeader.globalId));
        stream.write(reinterpret_cast<const char*>(&assetFileHeader.type), sizeof(assetFileHeader.type));
    }

}
