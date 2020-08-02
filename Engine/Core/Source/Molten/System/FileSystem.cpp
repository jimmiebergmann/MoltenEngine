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

#include "Molten/System/FileSystem.hpp"
#include "Molten/System/Exception.hpp"
#include <fstream>

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
#include "Molten/Platform/Win32Headers.hpp"
#endif

namespace Molten
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

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

    bool FileSystem::MakeDirectory(const std::string& directory)
    {
        const size_t currDirSize = 256;
        char currDir[256];
        if (!GetCurrentDirectory(currDirSize, currDir))
        {
            return false;
        }

        const std::string fullDir = std::string(currDir) + "\\" + directory;
        return CreateDirectory(fullDir.c_str(), NULL) ? true : false;
    }

    bool FileSystem::DeleteFile(const std::string& filename)
    {
        const size_t currDirSize = 256;
        char currDir[256];
        if (!GetCurrentDirectory(currDirSize, currDir))
        {
            return false;
        }

        const std::string fullFilename = std::string(currDir) + "\\" + filename;
        return ::DeleteFileA(fullFilename.c_str()) ? true : false; 
    }

#else

    bool FileSystem::MakeDirectory(const std::string&)
    {
        return false;
    }

    bool FileSystem::DeleteFile(const std::string&)
    {
        return false;
    }
    
#endif  

}