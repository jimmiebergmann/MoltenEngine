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

#ifndef MOLTEN_CORE_FILEFORMAT_IMAGE_BMPFORMAT_HPP
#define MOLTEN_CORE_FILEFORMAT_IMAGE_BMPFORMAT_HPP

#include "Molten/FileFormat/FileFormatResult.hpp"

#include "Molten/Utility/Expected.hpp"
#include "Molten/Math/Vector.hpp"
#include <array>
#include <vector>
#include <filesystem>
#include <istream>


namespace Molten::FileFormat::Image::Bmp
{

    /** Data type of BMP data bytes. */
    using Data = std::vector<uint8_t>;

    /** BMP Info Header class, containing information about bitmap data. */
    struct MOLTEN_CORE_API InfoHeader
    {
        inline static constexpr size_t packedSize = 40;

        uint32_t headerSize = 40;
        uint32_t width = 0;
        uint32_t height = 0;
        uint16_t planes = 1;
        uint16_t bitsPerPixel = 24;
        uint32_t compression = 0;
        uint32_t imageSize = 0;
        uint32_t xPixelsPerM = 0;
        uint32_t yPixelsPerM = 0;
        uint32_t colorsUsed = 0;
        uint32_t importantColorsUsed = 0;
    };

    /** BMP Header class, containing basic file information. */
    struct MOLTEN_CORE_API Header
    {
        inline static constexpr size_t packedSize = 14;

        std::array<uint8_t, 2> signature = { 'B', 'M' };
        uint32_t fileSize = packedSize + InfoHeader::packedSize;
        uint16_t reserved1 = 0;
        uint16_t reserved2 = 0;
        uint32_t dataOffset = packedSize + InfoHeader::packedSize;
    };

    /** BMP Color Table class. This section is optional, but used for padding indices to color values. */
    //struct MOLTEN_CORE_API ColorTable
    //{
    //    std::vector<Vector4<uint8_t>> colors = {};
    //};


    /** BMP File class contains all data necessary for reading and writing BMP image files. */
    struct MOLTEN_CORE_API File
    {
        Header header = {};
        InfoHeader infoHeader = {};
        //ColorTable colorTable = {};
        Data data = {}; ///< Raw data without scan line padding.
    };


    enum class ReadErrorCode {
        UnexpectedEndOfFile,
        InvalidHeaderSignature, ///< Signature consists of two characters: 'BM'.
        InvalidHeaderSize, ///< Size of stream is less than 16 bytes.
        MissingInfoHeader, ///< Unexpected EOF.
        UnsupportedInfoHeader, ///< Currently only supporting BITMAPINFOHEADER (40 bytes).
        UnsupportedBitsPerPixel, ///< Currently only supporting values divisible by 8.
        UnsupportedCompression ///< No compression is currently supported.
    };

    using ReadError = std::variant<ReadErrorCode, OpenFileError>;
    using ReadResult = Expected<File, ReadError>;

    MOLTEN_CORE_API ReadResult ReadFile(std::istream& stream);
    MOLTEN_CORE_API ReadResult ReadFile(const std::filesystem::path& path);


    enum class WriteErrorCode {
        UnsupportedBitsPerPixel, ///< Currently only supporting values divisible by 8.
        UnsupportedCompression, ///< No compression is currently supported.
        InvalidDataSize
    };

    using WriteError = std::variant<WriteErrorCode, OpenFileError>;
    using WriteResult = Expected<void, WriteError>;

    MOLTEN_CORE_API WriteResult WriteFile(const File& bmpFile, std::ostream& stream);
    MOLTEN_CORE_API WriteResult WriteFile(const File& bmpFile, const std::filesystem::path& path);
    
}

namespace Molten
{
    using BmpImageFile = Molten::FileFormat::Image::Bmp::File;
}

#endif