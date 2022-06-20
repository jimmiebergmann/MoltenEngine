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

#ifndef MOLTEN_CORE_FILEFORMAT_TEXTFILEFORMATRESULT_HPP
#define MOLTEN_CORE_FILEFORMAT_TEXTFILEFORMATRESULT_HPP

#include "Molten/Core.hpp"
#include <string>
#include <optional>

namespace Molten
{

    class MOLTEN_API TextFileFormatResult
    {

    public:

        enum ErrorCode
        {
            OpenFileError,
            ParseError
        };

        /** Error struct, returned via GetError(). */
        struct Error
        {
            Error(
                const ErrorCode code,
                const size_t lineNumber,
                const std::string& message);

            Error(
                const ErrorCode code,
                const size_t lineNumber,
                std::string&& message);

            ~Error() = default;

            Error(const Error&) = default;
            Error(Error&&) = default;
            Error& operator = (const Error&) = default;
            Error& operator = (Error&&) = default;

            ErrorCode code;
            size_t lineNumber;
            std::string message;
        };

        /** Default constructing a successful result. */
        TextFileFormatResult() = default;

        /** Constructors for errors. */
        /**@{*/
        TextFileFormatResult(
            const ErrorCode code,
            const std::string& message);

        TextFileFormatResult(
            const ErrorCode code,
            std::string&& message);

        TextFileFormatResult(
            const ErrorCode code,
            const size_t line = 0,
            const std::string& message = "");

        TextFileFormatResult(
            const ErrorCode code,
            const size_t line,
            std::string&& message);

        TextFileFormatResult(const Error& error);
        TextFileFormatResult(Error&& error);
        /**@}*/

        /** Default destructor. */
        ~TextFileFormatResult() = default;

        /** Copy/move construction/assignment operators. */
        /**@{*/
        TextFileFormatResult(const TextFileFormatResult& other) = default;
        TextFileFormatResult(TextFileFormatResult&& other) noexcept = default;
        TextFileFormatResult& operator = (const TextFileFormatResult& other) = default;
        TextFileFormatResult& operator = (TextFileFormatResult&& other) noexcept = default;
        /**@}*/

        /** Bool operator, returns true if successful, else false. */
        [[nodiscard]] operator bool() const;

        /** Check if result is successful. */
        [[nodiscard]] bool IsSuccessful() const;

        /** Get error. It is undefined behaviour to call GetError() if IsSuccessful() == true. */
        [[nodiscard]] Error& GetError();
        [[nodiscard]] const Error& GetError() const;

    private:

        std::optional<Error> m_error;

    };

}

#endif