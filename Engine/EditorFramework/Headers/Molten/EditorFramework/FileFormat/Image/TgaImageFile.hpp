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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_IMAGE_TGAIMAGEFILE_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_IMAGE_TGAIMAGEFILE_HPP

#include "Molten/EditorFramework/Build.hpp"
#include "Molten/Utility/Expected.hpp"
#include <filesystem>

namespace Molten::EditorFramework
{

    struct TgaImageFile
    {
        enum class ColorMapType : uint8_t
        {
            Absent = 0,
            Present = 1
        };

        enum class ImageType : uint8_t
        {
            NoData = 0,
            UncompressedColorMapped = 1,
            UncompressedTrueColor = 2,
            UncompressedGrayscale = 3,
            RunLengthColorMapped = 9,
            RunLengthTrueColor = 10,
            RunLengthGrayscale = 11
        };

        struct ImageColorMapSpecification
        {
            uint16_t firstEntryIndex = 0;
            uint16_t colorMapLength = 0;
            uint8_t colorMapEntrySize = 0;
        };

        struct ImageSpecification
        {
            uint16_t originX = 0;
            uint16_t originY = 0;
            uint16_t imageWidth;
            uint16_t imageHeight;
            uint8_t pixelDepth;
            uint8_t imageDescriptor = 0;
        };

        struct Header
        {
            uint8_t imageIdLength = 0;
            uint8_t colorMapType;
            uint8_t imageType;
            ImageColorMapSpecification colorMapSpecification;
            ImageSpecification imageSpecification;
        };

        struct ImageData
        {
            std::vector<uint8_t> imageId;
            std::vector<uint8_t> colorMapData;
            std::vector<uint8_t> imageData;
        };

        struct Footer
        {
            uint32_t extensionOffset = 0;
            uint32_t developerAreaOffset = 0;
        };

        Header header;
        ImageData data;
        Footer footer;
    };


    enum class ReadTgaImageFileError
    {
        OpenFileError,
        UnexpectedEndOfFile,
        UnsupportedImageType,
        UnsupportedColorMap,
        UnsupportedPixelDepth, 
        BadFooter
    };

    enum class DecodeTgaImageRleDataError
    {
        BadPixelDepth,
        BufferOutOfBounds
    };


    MOLTEN_EDITOR_FRAMEWORK_API Expected<TgaImageFile, ReadTgaImageFileError> ReadTgaImageFile(
        const std::filesystem::path& filename);

    MOLTEN_EDITOR_FRAMEWORK_API Expected<TgaImageFile, ReadTgaImageFileError> ReadTgaImageFile(
        std::istream& stream);

    MOLTEN_EDITOR_FRAMEWORK_API Expected<std::vector<uint8_t>, DecodeTgaImageRleDataError> DecodeTgaImageRleData(
        const std::vector<uint8_t>& input,
        const uint8_t pixelDepth);

}

#endif