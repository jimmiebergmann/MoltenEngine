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

#include "Curse/System/FileSystem.hpp"
#include "Curse/System/Exception.hpp"
#include <fstream>

#if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
#include "Curse/Platform/Win32Headers.hpp"
#endif

namespace Curse
{

    std::vector<uint8_t> FileSystem::ReadFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            throw Exception("Unable to open file.");
        }

        file.seekg(0, std::ios::end);
        const size_t dataSize = static_cast<size_t>(file.tellg());
        if (!dataSize)
        {
            return {};
        }
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> data(dataSize);
        file.read(reinterpret_cast<char*>(data.data()), dataSize);
        return std::move(data);
    }

    bool FileSystem::MakeDirectory(const std::string& directory)
    {
    #if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
        const size_t currDirSize = 256;
        char currDir[256];
        if (!GetCurrentDirectory(currDirSize, currDir))
        {
            return false;
        }

        const std::string fullDir = std::string(currDir) + "\\" + directory;

        return CreateDirectory(fullDir.c_str(), NULL) ? true : false;
    #else
        return false;
    #endif  
    }


    bool FileSystem::DeleteFile(const std::string& filename)
    {
    #if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
        const size_t currDirSize = 256;
        char currDir[256];
        if (!GetCurrentDirectory(currDirSize, currDir))
        {
            return false;
        }

        const std::string fullFilename = std::string(currDir) + "\\" + filename;

        return ::DeleteFileA(fullFilename.c_str()) ? true : false;
    #else
        return false;
    #endif  
    }

}