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
#include "Molten/EditorFramework/FileFormat/Asset/AssetFile.hpp"
#include <fstream>
#include <cstring>

namespace Molten::EditorFramework
{

    TEST(FileFormat, AssetFile_ReadWrite)
    {
        const auto dir = Molten::Test::CreateTestDirectory("FileFormat_AssetFile");
        const auto path = dir / "AssetFile_ReadWrite.asset";
          
        const auto assetFile = AssetFileHeader{
            .engineVersion = Version{ 456789012, 567890123, 678901234 },
            .assetType = AssetType::Scene,
            .fileVersion = Version{ 123456789, 234567890, 345678901 },
            .globalId = { 123456789012345ULL, 234567890123456ULL }
        };

        auto writeResult = WriteAssetFileHeader(path, assetFile);
        ASSERT_TRUE(writeResult);

        auto readAssetFile = ReadAssetFileHeader(path);
        ASSERT_TRUE(readAssetFile);

        EXPECT_EQ(readAssetFile->engineVersion, assetFile.engineVersion);
        EXPECT_EQ(readAssetFile->assetType, assetFile.assetType);
        EXPECT_EQ(readAssetFile->fileVersion, assetFile.fileVersion);
        EXPECT_EQ(readAssetFile->globalId, assetFile.globalId);
        
    }

}