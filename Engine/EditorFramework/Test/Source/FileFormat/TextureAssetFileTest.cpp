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
#include "Molten/EditorFramework/FileFormat/TextureAssetFile.hpp"
#include <fstream>
#include <cstring>

namespace Molten::EditorFramework
{

    TEST(FileFormat, TextureAssetFile_ReadWrite)
    {
        const auto dir = Molten::Test::CreateTestDirectory("FileFormat_TextureAssetFile");
        const auto path = dir / "TextureAssetFile_ReadWrite.asset";
          
        const auto assetFileHeader = AssetFileHeader{
            .globalId = { 123456789012345ULL, 234567890123456ULL },
            .type = AssetType::Texture
        };

        const auto textureAssetFile = TextureAssetFile{
            .dimensions = Vector3ui32{ 2, 2, 2 },
            .format = TextureAssetFormat::Red8Green8Blue8Alpha8,
            .data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}
        };

        ASSERT_EQ(textureAssetFile.data.size(), size_t{ 32 });

        std::ofstream fileStream(path, std::ios::binary);
        ASSERT_TRUE(fileStream.is_open());
        WriteTextureAssetFileWithHeader(fileStream, assetFileHeader, textureAssetFile);

        fileStream.close();

        auto readTextureAssetFile = ReadTextureAssetFileWithHeader(path);

        EXPECT_EQ(readTextureAssetFile.dimensions, textureAssetFile.dimensions);
        EXPECT_EQ(readTextureAssetFile.format, textureAssetFile.format);
        ASSERT_EQ(readTextureAssetFile.data.size(), textureAssetFile.data.size());
        EXPECT_EQ(std::memcmp(readTextureAssetFile.data.data(), textureAssetFile.data.data(), readTextureAssetFile.data.size()), int{ 0 });
    }

}