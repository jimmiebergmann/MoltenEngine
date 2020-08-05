/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_LOGGER_HPP
#define MOLTEN_CORE_LOGGER_HPP

#include "Molten/Types.hpp"
#include <functional>
#include <string>
#include <fstream>

namespace Molten
{

    /** Logger class. */
    class MOLTEN_API Logger
    {

    public:

        /** Enumerator of log types. */
        enum class Severity : uint32_t
        {
            Info    = 1 << 0,
            Debug   = 1 << 1,
            Warning = 1 << 2,
            Error   = 1 << 3
        };

        using Callback = std::function<void(const Severity, const char *)>;

        /** Constant, containing all severity flags. */
        static const uint32_t SeverityAllFlags;

        /**
         * Default constructor.
         *
         * @param severityFlags Flags of severity levels.
         *
         */
        explicit Logger(const uint32_t severityFlags = SeverityAllFlags);

        /** Constructor, by providing a callback function. */
        explicit Logger(Callback callback, const uint32_t severityFlags = SeverityAllFlags);

        /**
         * Constructor.
         *
         * @param severityFlags Flags of severity levels.
         * @param parent Parent logger, inheriting the callback, but with possibly different severity flags.
         *
         */
        Logger(const uint32_t severityFlags, Logger* parent);

        /** Virtual destructor. */
        virtual ~Logger();

        /** Set log severity flags. */
        virtual void SetSeverityFlags(const uint32_t severityFlags);

        /** Get log severity flags. */
        virtual uint32_t GetSeverityFlags() const;

        /** Write log message function. */
        virtual void Write(const Severity severity, const char* message);

        /** Write log message function. */
        virtual void Write(const Severity severity, const std::string& message);

        /** 
         * Helper funktion for writing log messages.
         * The funktion returns immediately if logger == nullptr.
         */
        /**@{*/
        static void WriteInfo(Logger * logger, const std::string& message);
        static void WriteDebug(Logger * logger, const std::string& message);
        static void WriteWarning(Logger * logger, const std::string& message);
        static void WriteError(Logger * logger, const std::string& message);
        /**@}*/
        
    protected:

        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;

        uint32_t m_severityFlags;
        Callback m_callback;

    };


    /** File logger class. */
    class MOLTEN_API FileLogger : public Logger
    {

    public:

        /** Enumerator of file open modes. */
        enum class OpenMode
        {
            Append,
            Truncate
        };

        /** Constructor. Opens log file, if filename is provided. */
        explicit FileLogger(
            const std::string& filename = "",
            const OpenMode openMode = OpenMode::Append,
            const uint32_t severityFlags = Logger::SeverityAllFlags);

        /** Destructor. Closing current open log file. */
        virtual ~FileLogger();

        /** Open log file for writing. */
        virtual bool Open(
            const std::string & filename,
            const OpenMode openMode = OpenMode::Append,
            const uint32_t severityFlags = Logger::SeverityAllFlags);

        /** Close the log file. */
        virtual void Close();

        /** Checks if the log file is open. */
        virtual bool IsOpen() const;

    private:

        FileLogger(const FileLogger&) = delete;
        FileLogger(FileLogger&&) = delete;
        FileLogger& operator=(const FileLogger&) = delete;
        FileLogger& operator=(FileLogger&&) = delete;

        std::ofstream m_file;

    };

}

#include "Molten/Logger.inl"

#endif
