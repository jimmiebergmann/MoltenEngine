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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_ASSET_ASSETFILE_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_ASSET_ASSETFILE_HPP

#include "Molten/EditorFramework/Build.hpp"
#include "Molten/Utility/Expected.hpp"
#include "Molten/Utility/Uuid.hpp"
#include "Molten/System/Version.hpp"
#include <array>
#include <istream>
#include <ostream>
#include <filesystem>

namespace Molten::EditorFramework
{

    enum class AssetType : uint32_t
    {
        Scene,
        Mesh,
        Texture,
        Material,
        MaterialLibrary,
        Audio
    };

    struct AssetFileHeader
    {
        Version engineVersion;
        AssetType assetType;
        Version fileVersion;    
        Uuid globalId;       
    };

    enum class ReadAssetFileHeaderError
    {
        OpenFileError,
        UnexpectedEndOfFile,
        BadAssetFileSignature,
        BadAssetType
    };

    enum class WriteAssetFileHeaderError
    {
        OpenFileError
    };


    MOLTEN_EDITOR_FRAMEWORK_API Expected<AssetFileHeader, ReadAssetFileHeaderError> ReadAssetFileHeader(
        const std::filesystem::path& path);

    MOLTEN_EDITOR_FRAMEWORK_API Expected<AssetFileHeader, ReadAssetFileHeaderError> ReadAssetFileHeader(
        std::istream& stream);

    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteAssetFileHeaderError> WriteAssetFileHeader(
        const std::filesystem::path& path,
        const AssetFileHeader& assetFileHeader);

    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteAssetFileHeaderError> WriteAssetFileHeader(
        std::ostream& stream,
        const AssetFileHeader& assetFileHeader);

}

#endif