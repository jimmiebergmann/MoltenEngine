/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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
#include "Molten/Utility/BufferedFileLineReader.hpp"
#include <sstream>


#include <fstream>
#include <thread>

namespace Molten
{
    TEST(Utility, BufferedFileLineReader_FitAll)
    {
        std::stringstream ss;
        ss << "Hello first world\nHello second world\r\nHello third world\r\n\r\nFoo\n\nBar\r\r\rEnding\r\n";     

        std::vector<std::string> lines;
        std::vector<BufferedFileLineReader::Buffer> buffers;
        auto addBuffer = [&](auto& buffer) { buffers.push_back(buffer); };

        {
            BufferedFileLineReader lineReader(ss, 25, 200);

            Molten::Test::Benchmarker bm("Reading lines");

            std::string_view line;
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
            lines.emplace_back(line);
            ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::EndOfFile);
        }

        ASSERT_EQ(buffers.size(), 1);
        EXPECT_NE(buffers[0], nullptr);

        ASSERT_EQ(lines.size(), 11);
        EXPECT_STREQ(lines[0].c_str(), "Hello first world");
        EXPECT_STREQ(lines[1].c_str(), "Hello second world");
        EXPECT_STREQ(lines[2].c_str(), "Hello third world");
        EXPECT_STREQ(lines[3].c_str(), "");
        EXPECT_STREQ(lines[4].c_str(), "Foo");
        EXPECT_STREQ(lines[5].c_str(), "");
        EXPECT_STREQ(lines[6].c_str(), "Bar");
        EXPECT_STREQ(lines[7].c_str(), "");
        EXPECT_STREQ(lines[8].c_str(), "");
        EXPECT_STREQ(lines[9].c_str(), "Ending");
        EXPECT_STREQ(lines[10].c_str(), "");
    }

    TEST(Utility, BufferedFileLineReader_FitExactly)
    {
        { // No newline at end.
            std::stringstream ss;
            ss << "Hello first world\r\nHello second world\nHello third world ";

            std::vector<std::string> lines;
            std::vector<BufferedFileLineReader::Buffer> buffers;
            auto addBuffer = [&](auto& buffer) { buffers.push_back(buffer); };

            {
                BufferedFileLineReader lineReader(ss, 19, 19);

                Molten::Test::Benchmarker bm("Reading lines");

                std::string_view line;
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::EndOfFile);
            }

            ASSERT_EQ(buffers.size(), 3);
            EXPECT_NE(buffers[0], nullptr);
            EXPECT_NE(buffers[1], nullptr);
            EXPECT_NE(buffers[2], nullptr);

            ASSERT_EQ(lines.size(), 3);
            EXPECT_STREQ(lines[0].c_str(), "Hello first world");
            EXPECT_STREQ(lines[1].c_str(), "Hello second world");
            EXPECT_STREQ(lines[2].c_str(), "Hello third world ");
        }
        { // Newline at end.
            std::stringstream ss;
            ss << "Hello first world\r\nHello second world\nHello third world\r";

            std::vector<std::string> lines;
            std::vector<BufferedFileLineReader::Buffer> buffers;
            auto addBuffer = [&](auto& buffer) { buffers.push_back(buffer); };

            {
                BufferedFileLineReader lineReader(ss, 19, 19);

                Molten::Test::Benchmarker bm("Reading lines");

                std::string_view line;
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::EndOfFile);
            }

            ASSERT_EQ(buffers.size(), 3);
            EXPECT_NE(buffers[0], nullptr);
            EXPECT_NE(buffers[1], nullptr);
            EXPECT_NE(buffers[2], nullptr);

            ASSERT_EQ(lines.size(), 4);
            EXPECT_STREQ(lines[0].c_str(), "Hello first world");
            EXPECT_STREQ(lines[1].c_str(), "Hello second world");
            EXPECT_STREQ(lines[2].c_str(), "Hello third world");
            EXPECT_STREQ(lines[3].c_str(), "");
        }
    }

    TEST(Utility, BufferedFileLineReader_FitSplit)
    {
        {
            std::stringstream ss;
            ss << "Hello first world - foo\nHello second world\r\nHello third world - bar";

            std::vector<std::string> lines;
            std::vector<BufferedFileLineReader::Buffer> buffers;
            auto addBuffer = [&](auto& buffer) { buffers.push_back(buffer); };

            {
                BufferedFileLineReader lineReader(ss, 30, 30);

                Molten::Test::Benchmarker bm("Reading lines");

                std::string_view line;
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::EndOfFile);
            }

            ASSERT_EQ(buffers.size(), 3);
            EXPECT_NE(buffers[0], nullptr);
            EXPECT_NE(buffers[1], nullptr);
            EXPECT_NE(buffers[2], nullptr);

            ASSERT_EQ(lines.size(), 3);
            EXPECT_STREQ(lines[0].c_str(), "Hello first world - foo");
            EXPECT_STREQ(lines[1].c_str(), "Hello second world");
            EXPECT_STREQ(lines[2].c_str(), "Hello third world - bar");
        }
        { // Ending with new line.
            std::stringstream ss;
            ss << "Hello first world - foo\nHello second world\r\nHello third world - bar\n";

            std::vector<std::string> lines;
            std::vector<BufferedFileLineReader::Buffer> buffers;
            auto addBuffer = [&](auto& buffer) { buffers.push_back(buffer); };

            {
                BufferedFileLineReader lineReader(ss, 30, 30);

                Molten::Test::Benchmarker bm("Reading lines");

                std::string_view line;
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::Successful);
                lines.emplace_back(line);
                ASSERT_EQ(lineReader.ReadLine(line, addBuffer), BufferedFileLineReader::LineReadResult::EndOfFile);
            }

            ASSERT_EQ(buffers.size(), 3);
            EXPECT_NE(buffers[0], nullptr);
            EXPECT_NE(buffers[1], nullptr);
            EXPECT_NE(buffers[2], nullptr);

            ASSERT_EQ(lines.size(), 4);
            EXPECT_STREQ(lines[0].c_str(), "Hello first world - foo");
            EXPECT_STREQ(lines[1].c_str(), "Hello second world");
            EXPECT_STREQ(lines[2].c_str(), "Hello third world - bar");
            EXPECT_STREQ(lines[3].c_str(), "");
        }
       
    }


    void ReadCompareFile(const std::string filename, const size_t minbufferSize, const size_t maxbufferSize)
    {
        { // Compare BufferedFileLineReader and std::getline.
            std::ifstream file(filename);
            ASSERT_TRUE(file.is_open());

            std::ifstream bufferFile(filename, std::ifstream::binary);
            ASSERT_TRUE(bufferFile.is_open());

            std::vector<BufferedFileLineReader::Buffer> buffers;
            auto addBuffer = [&](auto& buffer) { buffers.push_back(buffer); };

            BufferedFileLineReader lineReader(bufferFile, minbufferSize, maxbufferSize);

            BufferedFileLineReader::LineReadResult result = BufferedFileLineReader::LineReadResult::Successful;
            while (result == BufferedFileLineReader::LineReadResult::Successful)
            {
                std::string fileLine;
                std::string_view line;

                result = lineReader.ReadLine(line, addBuffer);
                if (result == BufferedFileLineReader::LineReadResult::EndOfFile)
                {
                    std::getline(file, fileLine);
                    ASSERT_STREQ(fileLine.c_str(), std::string(line).c_str());

                    EXPECT_TRUE(file.eof());
                    break;
                }
                if (result != BufferedFileLineReader::LineReadResult::Successful)
                {
                    ASSERT_TRUE(false);
                    return;
                }

                
                std::getline(file, fileLine);
                ASSERT_STREQ(fileLine.c_str(), std::string(line).c_str());         
            }

        }

        { // Benchmarking
            { // BufferedFileLineReader
                std::ifstream file(filename, std::ifstream::binary);
                ASSERT_TRUE(file.is_open());

                std::vector<BufferedFileLineReader::Buffer> buffers;
                auto addBuffer = [&](auto& buffer) { buffers.push_back(buffer); };

                BufferedFileLineReader lineReader(file, 2048, 1048576);

                BufferedFileLineReader::LineReadResult result = BufferedFileLineReader::LineReadResult::Successful;

                {
                    Molten::Test::Benchmarker bm(filename + " - BufferedFileLineReader");

                    while (result == BufferedFileLineReader::LineReadResult::Successful)
                    {
                        std::string_view line;
                        result = lineReader.ReadLine(line, addBuffer);
                        if (result == BufferedFileLineReader::LineReadResult::EndOfFile)
                        {
                            break;
                        }
                        if (result != BufferedFileLineReader::LineReadResult::Successful)
                        {
                            ASSERT_TRUE(false);
                        }
                    }
                }
            }
            { // std::getline
                std::ifstream file(filename);
                ASSERT_TRUE(file.is_open());

                {
                    Molten::Test::Benchmarker bm(filename + " - std::getline");

                    while (!file.eof())
                    {
                        std::string line;
                        std::getline(file, line);
                    }
                }
            }
        }
    }

    TEST(Utility, BufferedFileLineReader_ProjectFiles)
    {
        NESTED_TEST_FUNCTION(ReadCompareFile("../.gitignore", 1000, 1000))
        NESTED_TEST_FUNCTION(ReadCompareFile("../CONTRIBUTING.md", 1000, 1000))
        NESTED_TEST_FUNCTION(ReadCompareFile("../LICENSE", 1000, 1000))
        NESTED_TEST_FUNCTION(ReadCompareFile("../.appveyor.yml", 1000, 1000))
        NESTED_TEST_FUNCTION(ReadCompareFile("../.travis.yml", 1000, 1000))
        NESTED_TEST_FUNCTION(ReadCompareFile("../.codecov.yml", 1000, 1000))
    }

}