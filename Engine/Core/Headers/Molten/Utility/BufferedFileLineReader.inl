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

namespace Molten
{

    // Buffered file line reader implementations.
    template<typename TBufferCreationCallback>
    BufferedFileLineReader::LineReadResult BufferedFileLineReader::ReadLine(std::string_view& line, TBufferCreationCallback&& bufferCreationCallback)
    {
        if (m_currentFilePosition > m_fileSize)
        {
            line = std::string_view{ m_currentBuffer.get() + m_currentBufferPosition, 0 };
            return LineReadResult::EndOfFile;
        }

        if (m_currentBufferPosition >= m_currentBufferSize)
        {
            if (auto result = CreateNewBuffer(bufferCreationCallback); result != LineReadResult::Successful)
            {
                return result;
            }
        }

        return ReadNextLine(line, bufferCreationCallback);
    }

    template<typename TBufferCreationCallback>
    BufferedFileLineReader::LineReadResult BufferedFileLineReader::CreateNewBuffer(TBufferCreationCallback&& bufferCreationCallback)
    {
        if (m_currentReadPosition >= m_fileSize)
        {
            return LineReadResult::Successful;
        }

        const auto sizeLeft = GetSizeLeft();
        const auto minBufferSize = std::min(sizeLeft, m_minBufferSize);

        // Find buffer size and allocate memory.
        size_t currentBufferSize = std::min(sizeLeft, m_maxBufferSize);
        while (currentBufferSize >= minBufferSize)
        {
            auto* bufferPointer = new (std::nothrow) char[currentBufferSize];
            if (!bufferPointer)
            {
                currentBufferSize /= 2;
                continue;
            }

            auto newBuffer = std::shared_ptr<char[]>(bufferPointer);

            // Check if we have any bytes left in previous buffer.
            size_t diff = 0;
            if (m_currentBufferPosition < m_currentBufferSize)
            {
                diff = m_currentBufferSize - m_currentBufferPosition;
                if (diff >= currentBufferSize)
                {
                    return LineReadResult::BufferOverflow;
                }

                std::memcpy(newBuffer.get(), m_currentBuffer.get() + m_currentBufferPosition, diff);
            }

            // Use the new buffer.
            m_currentBuffer = newBuffer;

            m_currentBufferSize = currentBufferSize;
            m_currentBufferPosition = 0;

            const size_t readSize = m_currentBufferSize - diff;
            m_inStream.read(m_currentBuffer.get() + diff, readSize);
            m_currentReadPosition += readSize;

            bufferCreationCallback(m_currentBuffer);
            return LineReadResult::Successful;
        }

        return LineReadResult::AllocationError;
    }

    template<typename TBufferCreationCallback>
    BufferedFileLineReader::LineReadResult BufferedFileLineReader::ReadNextLine(std::string_view& line, TBufferCreationCallback&& bufferCreationCallback)
    {
        auto view = std::string_view{ m_currentBuffer.get() + m_currentBufferPosition, m_currentBufferSize - m_currentBufferPosition };
        auto newline = FindNextNewline(view);

        if (newline == std::string_view::npos)
        {
            // Some data is left, but reached end of line.
            if (m_currentReadPosition >= m_fileSize)
            {
                return ReadEndOfFile(line, view);
            }

            // Some data is left, but has not reached end of line yet.
            if (auto result = CreateNewBuffer(bufferCreationCallback); result != LineReadResult::Successful)
            {
                return result;
            }
            view = std::string_view{ m_currentBuffer.get() + m_currentBufferPosition, m_currentBufferSize - m_currentBufferPosition };

            newline = FindNextNewline(view);
            if (newline == std::string_view::npos)
            {
                if (m_currentReadPosition >= m_fileSize)
                {
                    return ReadEndOfFile(line, view);
                }

                return LineReadResult::BufferOverflow;
            }
        }

        line = std::string_view{ view.data(), newline };

        return LineReadResult::Successful;
    }

}