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

#include "Molten/Logger.hpp"
#include <iostream>

namespace Molten
{

    // Static helper functions.
    static const std::string& GetSeverityString(const Logger::Severity severity)
    {
        static const std::string empty   = "";
        static const std::string info    = "[Info] - ";
        static const std::string debug   = "[Debug] - ";
        static const std::string warning = "[Warning] - ";
        static const std::string error   = "[Error] - ";

        switch (severity)
        {
            case Logger::Severity::Info:    return info;
            case Logger::Severity::Debug:   return debug;
            case Logger::Severity::Warning: return warning;
            case Logger::Severity::Error:   return error;
        }

        return empty;
    }

    static std::ios_base::openmode GetOpenModeFlag(FileLogger::OpenMode openMode, const std::ios_base::openmode baseMode)
    {
        std::ios_base::openmode mode = baseMode;
        switch (openMode)
        {
            case FileLogger::OpenMode::Append:
            {
                mode |= std::ios_base::app;
            } break;
            case FileLogger::OpenMode::Truncate:
            {
                mode |= std::ios_base::trunc;
            } break;
        }

        return mode;
    }

    // Logger implementations.
    const uint32_t Logger::SeverityAllFlags = 
        static_cast<uint32_t>(Logger::Severity::Info) |
        static_cast<uint32_t>(Logger::Severity::Debug) |
        static_cast<uint32_t>(Logger::Severity::Warning) |
        static_cast<uint32_t>(Logger::Severity::Error);

    Logger::Logger(const uint32_t severityFlags) :
        m_severityFlags(severityFlags),
        m_callback([](const Severity severity, const char * message)
        {
            std::cout << GetSeverityString(severity) << message << "\n";
        })
    {}

    Logger::Logger(Callback callback, const uint32_t severityFlags) :
        m_severityFlags(severityFlags),
        m_callback(callback)
    { }
    Logger::Logger(const uint32_t severityFlags, Logger* parent) :
        m_severityFlags(severityFlags),
        m_callback(parent->m_callback)
    { }

    Logger::~Logger()
    { }

    void Logger::SetSeverityFlags(const uint32_t severityFlags)
    {
        m_severityFlags = severityFlags;
    }

    uint32_t Logger::GetSeverityFlags() const
    {
        return m_severityFlags;
    }

    void Logger::Write(const Severity severity, const char* message)
    {
        if (m_severityFlags & static_cast<uint32_t>(severity))
        {
            m_callback(severity, message);
        }
    }

    void Logger::Write(const Severity severity, const std::string& message)
    {
        if (m_severityFlags & static_cast<uint32_t>(severity))
        {
            m_callback(severity, message.c_str());
        }
    }

    // File logger implementations.
    FileLogger::FileLogger(
        const std::string& filename,
        const OpenMode openMode,
        const uint32_t severityFlags
    ) :
        Logger([this](const Severity severity, const std::string& message)
        {
            m_file << GetSeverityString(severity) << message << "\n";
            m_file.flush();
        },
        severityFlags)
    {
        Open(filename, openMode);
    }

    FileLogger::FileLogger(
        const std::filesystem::path& path,
        const OpenMode openMode,
        const uint32_t severityFlags
    ) :
        Logger([this](const Severity severity, const std::string& message)
            {
                m_file << GetSeverityString(severity) << message << "\n";
                m_file.flush();
            },
            severityFlags)
    {
        Open(path, openMode);
    }

    FileLogger::~FileLogger()
    {
        Close();
    }

    bool FileLogger::Open(
        const std::string& filename,
        const OpenMode openMode,
        const uint32_t severityFlags)
    {
        if (filename.empty())
        {
            return false;
        }

        if (m_file.is_open())
        {
            m_file.close();
        }

        std::ios_base::openmode mode = GetOpenModeFlag(openMode, std::ios_base::out);
        m_file.open(filename, mode);
        if (!m_file.is_open())
        {
            return false;
        }

        m_severityFlags = severityFlags;

        return true;
    }

    bool FileLogger::Open(
        const std::filesystem::path& path,
        const OpenMode openMode,
        const uint32_t severityFlags)
    {
        return Open(path.string(), openMode, severityFlags);
    }

    void FileLogger::Close()
    {
        m_file.close();
    }

    bool FileLogger::IsOpen() const
    {
        return m_file.is_open();
    }


}
