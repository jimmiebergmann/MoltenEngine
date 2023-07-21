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

#include "Molten/Utility/BufferedStreamReader.hpp"

namespace Molten
{

    // Buffered file line reader implementations.
    BufferedStreamReader::BufferedStreamReader(std::istream& inStream, const size_t minBufferSize, const size_t maxBufferSize) :
        m_inStream(inStream),
        m_minBufferSize(std::max(size_t{ 16 }, minBufferSize)),
        m_maxBufferSize(std::max(m_minBufferSize, maxBufferSize)),
        m_fileSize(0),
        m_currentReadPosition(0),
        m_currentFilePosition(0),
        m_currentBuffer(nullptr),
        m_currentBufferSize(0),
        m_currentBufferPosition(0)
    {
        const auto startPos = m_inStream.tellg();
        m_inStream.seekg(0, std::istream::end);
        m_fileSize = static_cast<size_t>(m_inStream.tellg());
        m_inStream.seekg(startPos, std::istream::beg);
    }

    size_t BufferedStreamReader::GetStreamSize() const
    {
        return m_fileSize;
    }

    size_t BufferedStreamReader::GetSizeLeft() const
    {
        return m_fileSize - m_currentReadPosition + (m_currentBufferSize - m_currentBufferPosition);
    }

    BufferedStreamReader::LineReadResult BufferedStreamReader::ReadEndOfFile(std::string_view& line, std::string_view view)
    {
        m_currentBufferPosition = m_currentBufferSize + 1;
        m_currentFilePosition = m_fileSize + 1;

        line = view;
        return LineReadResult::Successful;
    }


    size_t BufferedStreamReader::FindNextNewline(const std::string_view& line)
    {
        const auto newline = line.find_first_of("\r\n");
        if (newline == std::string_view::npos)
        {
            return newline;
        }

        if (line[newline] == '\r')
        {
            if (const auto nextCharacter = newline + 1; nextCharacter < line.size())
            {
                if (line[nextCharacter] == '\n')
                {
                    ++m_currentBufferPosition;
                    ++m_currentFilePosition;
                }
            }
            else if (m_currentReadPosition < m_fileSize)
            {
                return std::string_view::npos;
            }
        }

        m_currentBufferPosition += newline + 1;
        m_currentFilePosition += newline + 1;

        return newline;
    }

}