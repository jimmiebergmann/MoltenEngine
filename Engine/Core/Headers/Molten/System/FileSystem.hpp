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

#ifndef MOLTEN_CORE_SYSTEM_FILESYSTEM_HPP
#define MOLTEN_CORE_SYSTEM_FILESYSTEM_HPP

#include "Molten/Types.hpp"
#include <vector>
#include <string>

namespace Molten
{

    /**
    * @brief Filesystem class.
    */
    class MOLTEN_API FileSystem
    {

    public:

        /**
        * @brief Read file of given filename.
        *
        * @param filename Full path and name of file to read.
        *
        * @throw Exception if unable to open file.
        *
        * @return Raw pointer of data. User is responsible of deleting the pointer.
        *         nullptr is returned if file is empty.
        */
        static std::vector<uint8_t> ReadFile(const std::string& filename);
        
        /**
        * @brief Make directory, from current directory.
        *        Recursive directory creation is not yet supported.
        *        
        */
        static bool MakeDirectory(const std::string& directory);

        /**
        * @brief Delete file, from current directory.
        *
        */
        static bool DeleteFile(const std::string& filename);


    };

}

#endif
