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

#include "Molten/FileFormat/TextFileFormatResult.hpp"

namespace Molten
{
    // Text file format result error implementations.
    TextFileFormatResult::Error::Error(
        const ErrorCode code,
        const size_t lineNumber,
        const std::string& message
    ) :
        code(code),
        lineNumber(lineNumber),
        message(message)
    {}

    TextFileFormatResult::Error::Error(
        const ErrorCode code,
        const size_t lineNumber,
        std::string&& message
    ) :
        code(code),
        lineNumber(lineNumber),
        message(std::move(message))
    {}


    // Text file format result implementations.
    TextFileFormatResult::TextFileFormatResult(
        const ErrorCode code,
        const std::string& message
    ) :
        m_error(std::make_optional<Error>(code, 0, message))
    {}

    TextFileFormatResult::TextFileFormatResult(
        const ErrorCode code,
        std::string&& message
    ) :
        m_error(std::make_optional<Error>(code, 0, std::move(message)))
    {}

    TextFileFormatResult::TextFileFormatResult(
        const ErrorCode code,
        const size_t line,
        const std::string& message
    ) :
        m_error(std::make_optional<Error>(code, line, message))
    {}

    TextFileFormatResult::TextFileFormatResult(
        const ErrorCode code,
        const size_t line,
        std::string&& message
    ) :
        m_error(std::make_optional<Error>(code, line, std::move(message)))
    {}

    TextFileFormatResult::TextFileFormatResult(const Error& error) :
        m_error(error)
    {}

    TextFileFormatResult::TextFileFormatResult(Error&& error) :
        m_error(std::move(error))
    {}

    TextFileFormatResult::operator bool() const
    {
        return !m_error.has_value();
    }

    bool TextFileFormatResult::IsSuccessful() const
    {
        return !m_error.has_value();
    }

    TextFileFormatResult::Error& TextFileFormatResult::GetError()
    {
        return m_error.value();
    }
    const TextFileFormatResult::Error& TextFileFormatResult::GetError() const
    {
        return m_error.value();
    }

}
