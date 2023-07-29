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

#include "Molten/EditorFramework/FileFormat/Image/TgaImageFile.hpp"
#include <fstream>
#include <array>
#include <optional>

namespace Molten::EditorFramework
{

    Expected<TgaImageFile, ReadTgaImageFileError> ReadTgaImageFile(
        const std::filesystem::path& filename)
    {
        std::ifstream file(filename, std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(ReadTgaImageFileError::OpenFileError);
        }

        return ReadTgaImageFile(file);
    }

    Expected<TgaImageFile, ReadTgaImageFileError> ReadTgaImageFile(
        std::istream& stream)
    {
        const auto fileStart = stream.tellg();
        stream.seekg(0, std::ios::end);
        const auto fileEnd = stream.tellg();
        stream.seekg(fileStart, std::ios::beg);

        const auto fileSize = static_cast<size_t>(fileEnd - fileStart);
        auto remainingFileSize = fileSize;

        // Read header
        if (remainingFileSize < 18)
        {
            return Unexpected(ReadTgaImageFileError::UnexpectedEndOfFile);
        }

        auto tgaFile = TgaImageFile{};

        auto& tgaHeader = tgaFile.header;
        stream.read(reinterpret_cast<char*>(&tgaHeader.imageIdLength), sizeof(tgaHeader.imageIdLength));
        stream.read(reinterpret_cast<char*>(&tgaHeader.colorMapType), sizeof(tgaHeader.colorMapType));
        stream.read(reinterpret_cast<char*>(&tgaHeader.imageType), sizeof(tgaHeader.imageType));
        remainingFileSize -= 3;

        auto& tgaColorMapSpecification = tgaHeader.colorMapSpecification;
        stream.read(reinterpret_cast<char*>(&tgaColorMapSpecification.firstEntryIndex), sizeof(tgaColorMapSpecification.firstEntryIndex));
        stream.read(reinterpret_cast<char*>(&tgaColorMapSpecification.colorMapLength), sizeof(tgaColorMapSpecification.colorMapLength));
        stream.read(reinterpret_cast<char*>(&tgaColorMapSpecification.colorMapEntrySize), sizeof(tgaColorMapSpecification.colorMapEntrySize));
        remainingFileSize -= 5;

        auto& tgaImageSpecification = tgaHeader.imageSpecification;
        stream.read(reinterpret_cast<char*>(&tgaImageSpecification.originX), sizeof(tgaImageSpecification.originX));
        stream.read(reinterpret_cast<char*>(&tgaImageSpecification.originY), sizeof(tgaImageSpecification.originY));
        stream.read(reinterpret_cast<char*>(&tgaImageSpecification.imageWidth), sizeof(tgaImageSpecification.imageWidth));
        stream.read(reinterpret_cast<char*>(&tgaImageSpecification.imageHeight), sizeof(tgaImageSpecification.imageHeight));
        stream.read(reinterpret_cast<char*>(&tgaImageSpecification.pixelDepth), sizeof(tgaImageSpecification.pixelDepth));
        stream.read(reinterpret_cast<char*>(&tgaImageSpecification.imageDescriptor), sizeof(tgaImageSpecification.imageDescriptor));
        remainingFileSize -= 10;

        if (tgaImageSpecification.pixelDepth != 8 && tgaImageSpecification.pixelDepth != 24 && tgaImageSpecification.pixelDepth != 32)
        {
            return Unexpected(ReadTgaImageFileError::UnsupportedPixelDepth);
        }

        // Read image data.
        auto& tgaData = tgaFile.data;
        if (tgaHeader.imageIdLength > 0)
        {
            auto const imageIdLength = static_cast<size_t>(tgaHeader.imageIdLength);
            if (remainingFileSize < imageIdLength)
            {
                return Unexpected(ReadTgaImageFileError::UnexpectedEndOfFile);
            }

            tgaData.imageId.resize(imageIdLength);
            stream.read(reinterpret_cast<char*>(tgaData.imageId.data()), tgaData.imageId.size());
            remainingFileSize -= imageIdLength;
        }

        const auto imageDataStart = stream.tellg();

        // Read footer
        const auto imageDataEnd = [&]()
        {
            if (remainingFileSize < 26)
            {
                return fileEnd;
            }

            auto extensionOffset = uint32_t{};
            auto developerAreaOffset = uint32_t{};
            auto footerSignature = std::array<uint8_t, 18>{};

            stream.seekg(-26, std::ios::end);
            const auto footerStart = stream.tellg();
            stream.read(reinterpret_cast<char*>(&extensionOffset), sizeof(extensionOffset));
            stream.read(reinterpret_cast<char*>(&developerAreaOffset), sizeof(developerAreaOffset));
            stream.read(reinterpret_cast<char*>(footerSignature.data()), footerSignature.size());
            stream.seekg(imageDataStart, std::ios::beg);

            static const auto expectedFooterSignature = std::array<uint8_t, 18>{
                'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', 0
            };

            const auto hasFooter = std::equal(footerSignature.begin(), footerSignature.end(), expectedFooterSignature.begin());          
            if (!hasFooter)
            {
                return fileEnd;
            }

            auto& tgaFooter = tgaFile.footer;
            tgaFooter.extensionOffset = extensionOffset;
            tgaFooter.developerAreaOffset = developerAreaOffset;

            const auto minOffset = std::min(extensionOffset, developerAreaOffset);
            if (minOffset == 0)
            {
                return footerStart;
            }

            return fileStart + static_cast<std::streampos>(minOffset);
        }();
       
        auto imageDataSizeDiff = imageDataEnd - imageDataStart;
        if (imageDataSizeDiff < 0)
        {
            return Unexpected(ReadTgaImageFileError::BadFooter);
        }

        if (imageDataSizeDiff > 0)
        {
            const auto imageDataSize = static_cast<size_t>(imageDataSizeDiff);

            const auto expectedImageDataSizeResult = [&]() -> Expected<std::optional<size_t>, ReadTgaImageFileError>
            {
                switch (tgaHeader.imageType)
                {
                    case (int8_t)TgaImageFile::ImageType::UncompressedTrueColor:
                    {
                        if (tgaImageSpecification.pixelDepth != 24 && tgaImageSpecification.pixelDepth != 32)
                        {
                            return Unexpected(ReadTgaImageFileError::UnsupportedPixelDepth);
                        }

                        return
                            static_cast<size_t>(tgaImageSpecification.imageWidth) *
                            static_cast<size_t>(tgaImageSpecification.imageHeight) *
                            (static_cast<size_t>(tgaImageSpecification.pixelDepth) / 8);

                    } break;
                    case (int8_t)TgaImageFile::ImageType::UncompressedGrayscale:
                    {
                        if (tgaImageSpecification.pixelDepth != 8)
                        {
                            return Unexpected(ReadTgaImageFileError::UnsupportedPixelDepth);
                        }

                        return 
                            static_cast<size_t>(tgaImageSpecification.imageWidth) *
                            static_cast<size_t>(tgaImageSpecification.imageHeight);
                    } break;
                    case (int8_t)TgaImageFile::ImageType::RunLengthTrueColor:
                    case (int8_t)TgaImageFile::ImageType::RunLengthGrayscale:
                    {
                        return std::nullopt;
                    } break;
                    default: break;
                }

                return Unexpected(ReadTgaImageFileError::UnsupportedImageType);
            }();

            if (!expectedImageDataSizeResult)
            {
                return Unexpected(expectedImageDataSizeResult.Error());
            }

            const auto expectedImageDataSize = *expectedImageDataSizeResult;
 
            if (expectedImageDataSize)
            {
                if (expectedImageDataSize.value() != imageDataSize)
                {
                    return Unexpected(ReadTgaImageFileError::UnexpectedEndOfFile);
                }
                if (remainingFileSize < expectedImageDataSize.value())
                {
                    return Unexpected(ReadTgaImageFileError::UnexpectedEndOfFile);
                }
            }

            tgaData.imageData.resize(imageDataSize);
            stream.read(reinterpret_cast<char*>(tgaData.imageData.data()), tgaData.imageData.size());
            remainingFileSize -= imageDataSize;
        }

        return tgaFile;
    }

    Expected<std::vector<uint8_t>, DecodeTgaImageRleDataError> DecodeTgaImageRleData(
        const std::vector<uint8_t>& input,
        const uint8_t pixelDepth)
    {
        if ((pixelDepth % 8) != 0)
        {
            return Unexpected(DecodeTgaImageRleDataError::BadPixelDepth);
        }

        const auto elementSize = static_cast<size_t>(pixelDepth / 8);

        auto result = std::vector<uint8_t>{};

        auto it = input.begin();
        while (it != input.end())
        {
            const auto repetitionCountByte = static_cast<uint8_t>(*(it++));

            const bool isRunLengthPacket = repetitionCountByte & 0b10000000;
            const auto repetitionCount = static_cast<size_t>(repetitionCountByte & 0b01111111) + 1;
            const auto remainingSize = static_cast<size_t>(input.end() - it);

            if (isRunLengthPacket)
            {
                if (remainingSize < elementSize)
                {
                    return Unexpected(DecodeTgaImageRleDataError::BufferOutOfBounds);
                }

                const auto elementItBegin = it;
                it += elementSize;

                for (size_t i = 0; i < repetitionCount; ++i)
                {
                    result.insert(result.end(), elementItBegin, it);
                }
            }
            else
            {
                const auto elementsSize = repetitionCount * elementSize;
                if (remainingSize < elementsSize)
                {
                    return Unexpected(DecodeTgaImageRleDataError::BufferOutOfBounds);
                }

                const auto elementItBegin = it;
                it += elementsSize;

                result.insert(result.end(), elementItBegin, it);
            }
        }

        return result;

    }

}

