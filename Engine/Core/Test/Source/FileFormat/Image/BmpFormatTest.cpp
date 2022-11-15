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

#include "Test.hpp"
#include "Molten/FileFormat/Image/BmpFormat.hpp"

namespace Molten
{
    TEST(FileFormat, BmpFormat)
    {
        const auto dir = Molten::Test::CreateTestDirectory("FileFormat_BmpFormat");
        const auto path = dir / "custom.bmp";

        {
            Molten::FileFormat::Image::Bmp::File bmpFile;

            bmpFile.data = {
               255, 1, 2,      255, 3, 255,
               4, 5, 255,      6, 255, 7
            };

            bmpFile.header.fileSize += static_cast<uint32_t>(bmpFile.data.size());
            bmpFile.infoHeader.width = 2;
            bmpFile.infoHeader.height = 2;
            bmpFile.infoHeader.bitsPerPixel = 24;

            bmpFile.infoHeader.xPixelsPerM = 123;
            bmpFile.infoHeader.yPixelsPerM = 234;

            auto result = Molten::FileFormat::Image::Bmp::WriteFile(bmpFile, path);
            ASSERT_TRUE(result.HasValue());
        }
        {
            auto result = Molten::FileFormat::Image::Bmp::ReadFile(path);
            ASSERT_TRUE(result.HasValue());
            const auto bmpFile = std::move(result.Value());

            EXPECT_EQ(bmpFile.header.signature[0], 'B');
            EXPECT_EQ(bmpFile.header.signature[1], 'M');
            EXPECT_EQ(bmpFile.header.fileSize, uint32_t{ 70 });
            EXPECT_EQ(bmpFile.header.reserved1, uint16_t{ 0 });
            EXPECT_EQ(bmpFile.header.reserved2, uint16_t{ 0 });
            EXPECT_EQ(bmpFile.header.dataOffset, uint32_t{ 54 });

            EXPECT_EQ(bmpFile.infoHeader.headerSize, uint32_t{ 40 });
            EXPECT_EQ(bmpFile.infoHeader.width, uint32_t{ 2 });
            EXPECT_EQ(bmpFile.infoHeader.height, uint32_t{ 2 });
            EXPECT_EQ(bmpFile.infoHeader.planes, uint16_t{ 1 });
            EXPECT_EQ(bmpFile.infoHeader.bitsPerPixel, uint16_t{ 24 });
            EXPECT_EQ(bmpFile.infoHeader.compression, uint32_t{ 0 });
            EXPECT_EQ(bmpFile.infoHeader.imageSize, uint32_t{ 0 });
            EXPECT_EQ(bmpFile.infoHeader.xPixelsPerM, uint32_t{ 123 });
            EXPECT_EQ(bmpFile.infoHeader.yPixelsPerM, uint32_t{ 234 });
            EXPECT_EQ(bmpFile.infoHeader.colorsUsed, uint32_t{ 0 });
            EXPECT_EQ(bmpFile.infoHeader.importantColorsUsed, uint32_t{ 0 });

            ASSERT_EQ(bmpFile.data.size(), size_t{ 12 });
            EXPECT_EQ(bmpFile.data[0], uint8_t{ 255 });
            EXPECT_EQ(bmpFile.data[1], uint8_t{ 1 });
            EXPECT_EQ(bmpFile.data[2], uint8_t{ 2 });
            EXPECT_EQ(bmpFile.data[3], uint8_t{ 255 });
            EXPECT_EQ(bmpFile.data[4], uint8_t{ 3 });
            EXPECT_EQ(bmpFile.data[5], uint8_t{ 255 });
            EXPECT_EQ(bmpFile.data[6], uint8_t{ 4 });
            EXPECT_EQ(bmpFile.data[7], uint8_t{ 5 });
            EXPECT_EQ(bmpFile.data[8], uint8_t{ 255 });
            EXPECT_EQ(bmpFile.data[9], uint8_t{ 6 });
            EXPECT_EQ(bmpFile.data[10], uint8_t{ 255 });
            EXPECT_EQ(bmpFile.data[11], uint8_t{ 7 });
        }
        
    }

}