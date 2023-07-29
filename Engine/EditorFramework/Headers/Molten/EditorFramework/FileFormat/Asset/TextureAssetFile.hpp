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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_ASSET_TEXTUREASSETFILE_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_ASSET_TEXTUREASSETFILE_HPP

#include "Molten/EditorFramework/FileFormat/Asset/AssetFile.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Utility/Expected.hpp"

namespace Molten::EditorFramework
{

    struct TextureAssetFile
    {

        enum class ImageFormat : uint8_t
        {
            Gray8,                       ///< Linear { uint8_t red; }
            Red8Green8Blue8,            ///< Linear { uint8_t red; uint8_t green; uint8_t blue; }
            Red8Green8Blue8Alpha8,      ///< Linear { uint8_t red; uint8_t green; uint8_t blue; uint8_t alpha; }

            SrgbRed8Green8Blue8,        ///< Non-linear sRGB { int8_t red; int8_t green; int8_t blue; }
            SrgbRed8Green8Blue8Alpha8,  ///< Non-linear sRGB { int8_t red; int8_t green; int8_t blue; int8_t alpha; }
        };

        enum class CompressionType : uint8_t
        {
            None
        };

        struct Header
        {
            Vector3ui32 dimensions = {};
            ImageFormat imageFormat;
            CompressionType compressionType = CompressionType::None;
        };

        struct ImageData
        {
            std::vector<uint8_t> data = {};
        };

        Uuid globalId = {};
        Header header = {};
        ImageData imageData = {};

    };


    struct ReadTextureAssetFileOptions
    {
        bool ignoreHeader = false;
    };

    struct WriteTextureAssetFileOptions
    {
        bool ignoreHeader = false;
    };
  
    enum class ReadTextureAssetFileError
    {
        OpenFileError,
        BadAssetHeader,
        BinaryFileError
    };

    enum class WriteTextureAssetFileError
    {
        OpenFileError,
        InternalError
    };


    MOLTEN_EDITOR_FRAMEWORK_API Expected<TextureAssetFile, ReadTextureAssetFileError> ReadTextureAssetFile(
        const std::filesystem::path& path,
        const ReadTextureAssetFileOptions& options = {});

    MOLTEN_EDITOR_FRAMEWORK_API Expected<TextureAssetFile, ReadTextureAssetFileError> ReadTextureAssetFile(
        std::istream& stream,
        const ReadTextureAssetFileOptions& options = {});

    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteTextureAssetFileError> WriteTextureAssetFile(
        const std::filesystem::path& path,
        const TextureAssetFile& textureAssetFile,
        const WriteTextureAssetFileOptions& options = {});

    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteTextureAssetFileError> WriteTextureAssetFile(
        std::ostream& stream,
        const TextureAssetFile& textureAssetFile,
        const WriteTextureAssetFileOptions& options = {});

}

#endif