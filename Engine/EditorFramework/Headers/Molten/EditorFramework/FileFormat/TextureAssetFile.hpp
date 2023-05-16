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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_TEXTUREASSETFILE_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_TEXTUREASSETFILE_HPP

#include "Molten/EditorFramework/FileFormat/AssetFile.hpp"
#include "Molten/Math/Vector.hpp"
#include <memory>

namespace Molten::EditorFramework
{

    enum class TextureAssetFormat
    {
        Red8,                       ///< Linear { uint8_t red; }
        Red8Green8Blue8,            ///< Linear { uint8_t red; uint8_t green; uint8_t blue; }
        Red8Green8Blue8Alpha8,      ///< Linear { uint8_t red; uint8_t green; uint8_t blue; uint8_t alpha; }

        SrgbRed8Green8Blue8,        ///< Non-linear sRGB { int8_t red; int8_t green; int8_t blue; }
        SrgbRed8Green8Blue8Alpha8,  ///< Non-linear sRGB { int8_t red; int8_t green; int8_t blue; int8_t alpha; }
    };

    struct MOLTEN_EDITOR_FRAMEWORK_API TextureAssetFile
    {
        Vector3ui32 dimensions = {};
        TextureAssetFormat format = TextureAssetFormat::Red8;
        std::vector<uint8_t> data = {};
    };


    MOLTEN_EDITOR_FRAMEWORK_API size_t GetTextureAssetFormatDepth(const TextureAssetFormat format);


    MOLTEN_EDITOR_FRAMEWORK_API TextureAssetFile ReadTextureAssetFile(std::istream& stream);
    MOLTEN_EDITOR_FRAMEWORK_API TextureAssetFile ReadTextureAssetFileWithHeader(std::istream& stream);
    MOLTEN_EDITOR_FRAMEWORK_API TextureAssetFile ReadTextureAssetFileWithHeader(const std::filesystem::path& path);


    MOLTEN_EDITOR_FRAMEWORK_API void WriteTextureAssetFile(
        std::ostream& stream,
        const TextureAssetFile& textureAssetFile);

    MOLTEN_EDITOR_FRAMEWORK_API void WriteTextureAssetFileWithHeader(
        std::ostream& stream,
        const AssetFileHeader& assetFileHeader,
        const TextureAssetFile& textureAssetFile);

    MOLTEN_EDITOR_FRAMEWORK_API void WriteTextureAssetFileWithHeader(
        const std::filesystem::path& path,
        const AssetFileHeader& assetFileHeader,
        const TextureAssetFile& textureAssetFile);

}

#endif