/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#include "Molten/FileFormat/Image/BmpFormat.hpp"
#include <fstream>

namespace Molten::FileFormat::Image::Bmp
{
    ReadResult ReadFile(std::istream& stream)
    {
        const auto startPos = static_cast<size_t>(stream.tellg());
        stream.seekg(0, std::ios::end);
        const auto endPos = static_cast<size_t>(stream.tellg());
        const auto streamSize = endPos - startPos;
        stream.seekg(startPos, std::ios::beg);

        // Read header
        if (streamSize < Header::packedSize)
        {
            return Unexpected(ReadErrorCode::InvalidHeaderSize);
        }        

        File result;
        auto& header = result.header;

        stream.read(reinterpret_cast<char*>(&header.signature), sizeof(header.signature));
        if (header.signature[0] != 'B' || header.signature[1] != 'M') {
            return Unexpected(ReadErrorCode::InvalidHeaderSignature);
        }
  
        stream.read(reinterpret_cast<char*>(&header.fileSize), sizeof(header.fileSize));
        // Validate fileSize

        stream.read(reinterpret_cast<char*>(&header.reserved1), sizeof(header.reserved1));
        stream.read(reinterpret_cast<char*>(&header.reserved2), sizeof(header.reserved2));
        stream.read(reinterpret_cast<char*>(&header.dataOffset), sizeof(header.dataOffset));

        // Read info header.
        if (streamSize < Header::packedSize + sizeof(InfoHeader::headerSize))
        {
            return Unexpected(ReadErrorCode::MissingInfoHeader);
        }

        auto& infoHeader = result.infoHeader;
        stream.read(reinterpret_cast<char*>(&infoHeader.headerSize), sizeof(infoHeader.headerSize));
        if (infoHeader.headerSize != InfoHeader::packedSize) // Only supporting BITMAPINFOHEADER
        {
            return Unexpected(ReadErrorCode::UnsupportedInfoHeader);
        }

        if (streamSize < Header::packedSize + InfoHeader::packedSize)
        {
            return Unexpected(ReadErrorCode::UnexpectedEndOfFile);
        }

        stream.read(reinterpret_cast<char*>(&infoHeader.width), sizeof(infoHeader.width));
        stream.read(reinterpret_cast<char*>(&infoHeader.height), sizeof(infoHeader.height));
        stream.read(reinterpret_cast<char*>(&infoHeader.planes), sizeof(infoHeader.planes));
        stream.read(reinterpret_cast<char*>(&infoHeader.bitsPerPixel), sizeof(infoHeader.bitsPerPixel));
        if ((infoHeader.bitsPerPixel ) % 8 != 0)
        {
            return Unexpected(ReadErrorCode::UnsupportedBitsPerPixel);
        }
        stream.read(reinterpret_cast<char*>(&infoHeader.compression), sizeof(infoHeader.compression));
        if (infoHeader.compression != 0)
        {
            return Unexpected(ReadErrorCode::UnsupportedCompression);
        }
        stream.read(reinterpret_cast<char*>(&infoHeader.imageSize), sizeof(infoHeader.imageSize));
        stream.read(reinterpret_cast<char*>(&infoHeader.xPixelsPerM), sizeof(infoHeader.xPixelsPerM));
        stream.read(reinterpret_cast<char*>(&infoHeader.yPixelsPerM), sizeof(infoHeader.yPixelsPerM));
        stream.read(reinterpret_cast<char*>(&infoHeader.colorsUsed), sizeof(infoHeader.colorsUsed));
        stream.read(reinterpret_cast<char*>(&infoHeader.importantColorsUsed), sizeof(infoHeader.importantColorsUsed));

        const size_t bytesPerPixel = infoHeader.bitsPerPixel / 8;
        const size_t rawScanLineSize = static_cast<size_t>(infoHeader.width) * bytesPerPixel;
        const size_t scanLinePaddingSize = rawScanLineSize % 4;
        const size_t scanLineSize = rawScanLineSize + scanLinePaddingSize;
        
        const size_t minDataSize = scanLineSize * static_cast<size_t>(infoHeader.height);
        if (streamSize < Header::packedSize + InfoHeader::packedSize + minDataSize)
        {
            return Unexpected(ReadErrorCode::UnexpectedEndOfFile);
        }

        // Read data
        const size_t minRawDataSize = rawScanLineSize * static_cast<size_t>(infoHeader.height);
        auto& data = result.data;
        
        data.resize(minRawDataSize);
        auto* currentData = data.data();
        for (size_t i = 0; i < infoHeader.height; i++)
        {
            stream.read(reinterpret_cast<char*>(currentData), rawScanLineSize);
            currentData += rawScanLineSize;

            if (scanLinePaddingSize > 0)
            {
                stream.seekg(scanLinePaddingSize, std::ios::cur);
            }   
        }

        return result;
    }

    ReadResult ReadFile(const std::filesystem::path& path)
    {
        std::ifstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(OpenFileError{});
        }

        return ReadFile(file);
    }

    WriteResult WriteFile(const File& bmpFile, std::ostream& stream)
    {
        const auto& header = bmpFile.header;
        const auto& infoHeader = bmpFile.infoHeader;
        const auto& data = bmpFile.data;

        if ((bmpFile.infoHeader.bitsPerPixel % 8) != 0)
        {
            return Unexpected(WriteErrorCode::UnsupportedBitsPerPixel);
        }
        if (bmpFile.infoHeader.compression != 0)
        {
            return Unexpected(WriteErrorCode::UnsupportedCompression);
        }

        const size_t bytesPerPixel = static_cast<size_t>(infoHeader.bitsPerPixel) / 8;
        const size_t scanLineSize = static_cast<size_t>(infoHeader.width) * bytesPerPixel;
        const size_t minRawDataSize = scanLineSize * static_cast<size_t>(infoHeader.height);
        if (minRawDataSize < data.size())
        {
            return Unexpected(WriteErrorCode::InvalidDataSize);
        }

        const size_t scanLinePaddingSize = scanLineSize % 4;
        const size_t minDataSize = (scanLineSize + scanLinePaddingSize) * static_cast<size_t>(infoHeader.height);

        const decltype(header.fileSize) fileSize =
            static_cast<uint32_t>(Header::packedSize) +
            static_cast<uint32_t>(InfoHeader::packedSize) +
            static_cast<uint32_t>(minDataSize);

        // Write header      
        stream.write(reinterpret_cast<const char*>(&header.signature), sizeof(header.signature));
        stream.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
        stream.write(reinterpret_cast<const char*>(&header.reserved1), sizeof(header.reserved1));
        stream.write(reinterpret_cast<const char*>(&header.reserved2), sizeof(header.reserved2));
        stream.write(reinterpret_cast<const char*>(&header.dataOffset), sizeof(header.dataOffset));      

        // Write info header
        stream.write(reinterpret_cast<const char*>(&infoHeader.headerSize), sizeof(infoHeader.headerSize));
        stream.write(reinterpret_cast<const char*>(&infoHeader.width), sizeof(infoHeader.width));
        stream.write(reinterpret_cast<const char*>(&infoHeader.height), sizeof(infoHeader.height));
        stream.write(reinterpret_cast<const char*>(&infoHeader.planes), sizeof(infoHeader.planes));
        stream.write(reinterpret_cast<const char*>(&infoHeader.bitsPerPixel), sizeof(infoHeader.bitsPerPixel));
        stream.write(reinterpret_cast<const char*>(&infoHeader.compression), sizeof(infoHeader.compression));
        stream.write(reinterpret_cast<const char*>(&infoHeader.imageSize), sizeof(infoHeader.imageSize));
        stream.write(reinterpret_cast<const char*>(&infoHeader.xPixelsPerM), sizeof(infoHeader.xPixelsPerM));
        stream.write(reinterpret_cast<const char*>(&infoHeader.yPixelsPerM), sizeof(infoHeader.yPixelsPerM));
        stream.write(reinterpret_cast<const char*>(&infoHeader.colorsUsed), sizeof(infoHeader.colorsUsed));
        stream.write(reinterpret_cast<const char*>(&infoHeader.importantColorsUsed), sizeof(infoHeader.importantColorsUsed)); 

        // Write data  
        const std::array<uint8_t, 4> paddingBytes = { 0, 0, 0, 0 };

        const auto* currentData = data.data();
        for (size_t i = 0; i < infoHeader.height; i++)
        {
            stream.write(reinterpret_cast<const char*>(currentData), scanLineSize);
            currentData += scanLineSize;

            if (scanLinePaddingSize > 0)
            {
                stream.write(reinterpret_cast<const char*>(paddingBytes.data()), scanLinePaddingSize);
            }
        }        
       
        return {};
    }

    WriteResult WriteFile(const File& bmpFile, const std::filesystem::path& path)
    {
        std::ofstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(OpenFileError{});
        }

        return WriteFile(bmpFile, file);
    }


}