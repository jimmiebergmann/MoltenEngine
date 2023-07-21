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

#ifndef MOLTEN_CORE_UTILITY_BUFFEREDSTREAMREADER_HPP
#define MOLTEN_CORE_UTILITY_BUFFEREDSTREAMREADER_HPP

#include "Molten/Core.hpp"
#include <cstring>
#include <memory>
#include <istream>
#include <string_view>

namespace Molten
{

    /** Optimized stream reader.
     *  Streams are read in chunks, but parsed line by line. Resulting data type is a string_view, pointing to an
     */
    class MOLTEN_CORE_API BufferedStreamReader
    {

    public:

        using Buffer = std::shared_ptr<char[]>; ///< Buffer type.

        /** Line read result enumerator. */
        enum class LineReadResult
        {
            Successful, ///< Successful read, more that might be available.
            EndOfFile, ///< Reached end of file. No more data is available.
            BufferOverflow, ///< Found no line less than or equal to allocated buffer.
            AllocationError ///< Failed to allocate a buffer larger or equal to minBufferSize.
        };

        /** Constructing a buffered reader, by providing a std::istream and min/max buffer size.
         *  Streaming is not supported because size of file is determined at construction.
         */
        BufferedStreamReader(
            std::istream& inStream,
            const size_t minBufferSize,
            const size_t maxBufferSize);

        /** Deleted copy/move constructors/operators. */
        /**@{*/
        BufferedStreamReader(const BufferedStreamReader&) = delete;
        BufferedStreamReader(BufferedStreamReader&&) = delete;
        BufferedStreamReader& operator = (const BufferedStreamReader&) = delete;
        BufferedStreamReader& operator = (BufferedStreamReader&&) = delete;
        /**@}*/

        /** Default destructor.*/
        ~BufferedStreamReader() = default;

        /** Read next line from stream.
         *  Underlying implementation reads chunks of data, but provides an output string_view parameter which will contain the next line if result is Successful.
         *  Provided parameter "bufferCreationCallback" is invoked with a new data chunk of data type Buffer, when new chunks are allocated.
         *  it is undefined behaviour to access "line" after it's data chunk is destroyed, so received chunk through bufferCreationCallback should be stored in some way by the caller.
         */

        template<typename TBufferCreationCallback>
        [[nodiscard]] LineReadResult ReadLine(std::string_view& line, TBufferCreationCallback&& bufferCreationCallback);

        [[nodiscard]] size_t GetStreamSize() const;

        [[nodiscard]] size_t GetSizeLeft() const;

    private:

        template<typename TBufferCreationCallback>
        [[nodiscard]] LineReadResult CreateNewBuffer(TBufferCreationCallback&& bufferCreationCallback);

        template<typename TBufferCreationCallback>
        [[nodiscard]] LineReadResult ReadNextLine(std::string_view& line, TBufferCreationCallback&& bufferCreationCallback);

        [[nodiscard]] LineReadResult ReadEndOfFile(std::string_view& line, std::string_view view);

        [[nodiscard]] size_t FindNextNewline(const std::string_view& line);

        std::istream& m_inStream;
        size_t m_minBufferSize;
        size_t m_maxBufferSize;

        size_t m_fileSize;
        size_t m_currentReadPosition;
        size_t m_currentFilePosition;

        Buffer m_currentBuffer;
        size_t m_currentBufferSize;
        size_t m_currentBufferPosition;

    };

}

#include "Molten/Utility/BufferedStreamReader.inl"

#endif