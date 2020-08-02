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

#include "Test.hpp"
#include "Molten/System/FileSystem.hpp"
#include "Molten/Logger.hpp"
#include <memory>
#include <fstream>

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

namespace Molten
{
    TEST(Core, FileLogger)
    {
        const std::string logFilename = "test/log_test_1.txt";

        FileSystem::MakeDirectory("test");
        FileSystem::DeleteFile(logFilename);
        
        FileLogger logger(logFilename);
        logger.Write(Logger::Severity::Info, "Test info message.");
        logger.Write(Logger::Severity::Debug, "Test debug message.");
        logger.Write(Logger::Severity::Warning, "Test warning message.");
        logger.Write(Logger::Severity::Error, "Test error message.");
        
        std::ifstream file(logFilename);
        std::string line = "";
        
        std::getline(file, line);     
        EXPECT_STREQ(line.c_str(), "[Info] - Test info message.");
        std::getline(file, line);
        EXPECT_STREQ(line.c_str(), "[Debug] - Test debug message.");
        std::getline(file, line);
        EXPECT_STREQ(line.c_str(), "[Warning] - Test warning message.");
        std::getline(file, line);
        EXPECT_STREQ(line.c_str(), "[Error] - Test error message.");
    }

}

#endif