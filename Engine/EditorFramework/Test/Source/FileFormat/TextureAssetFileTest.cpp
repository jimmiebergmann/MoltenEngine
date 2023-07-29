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

#include "Test.hpp"
#include "Molten/EditorFramework/FileFormat/Asset/TextureAssetFile.hpp"
#include <fstream>
#include <cstring>

namespace Molten::EditorFramework
{

    TEST(FileFormat, TextureAssetFile_ReadWrite)
    {
        const auto dir = Molten::Test::CreateTestDirectory("FileFormat_TextureAssetFile");
        const auto path = dir / "TextureAssetFile_ReadWrite.asset";

        const auto textureAssetFile = TextureAssetFile{
            .globalId = { 0x0102030405060708ULL, 0x1112131415161718ULL },
            .header = TextureAssetFile::Header {
                .dimensions = Vector3ui32{ 2, 2, 1 },
                .imageFormat = TextureAssetFile::ImageFormat::Red8Green8Blue8Alpha8,
                .compressionType = TextureAssetFile::CompressionType::None
            },
            .imageData = TextureAssetFile::ImageData{
                .data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}
            }
        };

        ASSERT_EQ(textureAssetFile.imageData.data.size(), size_t{ 32 });

        auto writeResult = WriteTextureAssetFile(path, textureAssetFile);
        ASSERT_TRUE(writeResult);

        auto readTextureAssetFile = ReadTextureAssetFile(path);
        ASSERT_TRUE(readTextureAssetFile);

        EXPECT_EQ(readTextureAssetFile->globalId, textureAssetFile.globalId);
        EXPECT_EQ(readTextureAssetFile->header.dimensions, textureAssetFile.header.dimensions);
        EXPECT_EQ(readTextureAssetFile->header.imageFormat, textureAssetFile.header.imageFormat);
        EXPECT_EQ(readTextureAssetFile->header.compressionType, textureAssetFile.header.compressionType);
        ASSERT_EQ(readTextureAssetFile->imageData.data.size(), textureAssetFile.imageData.data.size());
        EXPECT_EQ(std::memcmp(readTextureAssetFile->imageData.data.data(), readTextureAssetFile->imageData.data.data(), textureAssetFile.imageData.data.size()), int{ 0 });
    }

}