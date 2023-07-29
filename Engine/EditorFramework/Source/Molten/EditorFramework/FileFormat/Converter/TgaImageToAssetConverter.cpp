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

#include "Molten/EditorFramework/FileFormat/Converter/TgaImageToAssetConverter.hpp"
#include <tuple>

namespace Molten::EditorFramework
{

    Expected<TextureAssetFile, ConvertToTextureAssetFileError> ConvertToTextureAsset(
        const TgaImageFile& tgaImageFile,
        const Uuid globalId)
    {
        auto assetFile = TextureAssetFile{
            .globalId = globalId
        };

        auto& header = assetFile.header;

        header.dimensions = {
            static_cast<uint32_t>(tgaImageFile.header.imageSpecification.imageWidth),
            static_cast<uint32_t>(tgaImageFile.header.imageSpecification.imageHeight),
            1
        };

        if (header.dimensions.x == 0 || header.dimensions.y == 0)
        {
            return Unexpected(ConvertToTextureAssetFileError::InvalidDimensions);
        }

        const auto tgaImageType = static_cast<TgaImageFile::ImageType>(tgaImageFile.header.imageType);
        if (tgaImageType != TgaImageFile::ImageType::UncompressedTrueColor && 
            tgaImageType != TgaImageFile::ImageType::UncompressedGrayscale &&
            tgaImageType != TgaImageFile::ImageType::RunLengthTrueColor &&
            tgaImageType != TgaImageFile::ImageType::RunLengthGrayscale)
        {
            return Unexpected(ConvertToTextureAssetFileError::UnsupportedImageFormat);
        }
  
        const auto tgaPixelDepth = tgaImageFile.header.imageSpecification.pixelDepth;

        const auto convertImageFormatResult = [&]() -> Expected<std::pair<TextureAssetFile::ImageFormat, size_t>, ConvertToTextureAssetFileError>
        {
            switch (tgaPixelDepth)
            {
                case 8: return std::make_pair(TextureAssetFile::ImageFormat::Gray8, size_t{ 1 });
                case 24: return std::make_pair(TextureAssetFile::ImageFormat::Red8Green8Blue8, size_t{ 3 });
                case 32: return std::make_pair(TextureAssetFile::ImageFormat::Red8Green8Blue8Alpha8, size_t{ 4 });
                default: return Unexpected(ConvertToTextureAssetFileError::UnsupportedImageFormat);
            }
        }();

        if (!convertImageFormatResult)
        {
            return Unexpected(convertImageFormatResult.Error());
        }

        const auto [imageFormat, texelSize] = *convertImageFormatResult;
      
        header.imageFormat = imageFormat;

        // Convert image data.
        const auto isDataCompressed =
            tgaImageType == TgaImageFile::ImageType::RunLengthTrueColor ||
            tgaImageType == TgaImageFile::ImageType::RunLengthGrayscale;

        if (isDataCompressed)
        {
            auto decodedData = DecodeTgaImageRleData(tgaImageFile.data.imageData, tgaPixelDepth);
            if (!decodedData)
            {
                return Unexpected(ConvertToTextureAssetFileError::BadCompression);
            }

            assetFile.imageData.data = std::move(*decodedData);
        }
        else
        {
            assetFile.imageData.data = tgaImageFile.data.imageData;
        }

        // Validate data size.
        const auto expectedImageDataSize =
            static_cast<size_t>(header.dimensions.x) *
            static_cast<size_t>(header.dimensions.y) *
            texelSize;

        if (assetFile.imageData.data.size() < expectedImageDataSize)
        {
            return Unexpected(ConvertToTextureAssetFileError::ImageBufferUnderflow);
        }
        if (assetFile.imageData.data.size() > expectedImageDataSize)
        {
            return Unexpected(ConvertToTextureAssetFileError::ImageBufferOverflow);
        }

        return assetFile;
    }

}