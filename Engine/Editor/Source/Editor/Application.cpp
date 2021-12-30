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


#include "Editor/Application.hpp"
#include "Molten/System/CommandLine.hpp"
#include <exception>

static int g_error_load = -1;
static int g_error_exception = -2;

namespace Molten::Editor
{

    Application::Application() :
        m_logger(std::make_shared<Logger>())
    {}

    int Application::Run(
        int argc,
        char** argv)
    {
        EditorDescriptor editorDescriptor;
        if (!Load(argc, argv, editorDescriptor))
        {
            return g_error_load;
        }

        return Start(editorDescriptor);
    }

    bool Application::Load(
        int argc,
        char** argv,
        EditorDescriptor& editorDescriptor)
    {
        try
        {
            return LoadSettings(argc, argv, editorDescriptor);
        }
        catch (std::exception& e)
        {
            m_logger->Write(Logger::Severity::Error, std::string{ "Uncaught exception while loading editor: " } + e.what());
            exit(g_error_exception);
        }
        catch (...)
        {
            m_logger->Write(Logger::Severity::Error, "Uncaught exception while loading editor.");
            exit(g_error_exception);
        }
    }

    bool Application::LoadSettings(
        int argc,
        char** argv,
        EditorDescriptor& editorDescriptor)
    {
        std::optional<std::string> loggerFilename;
        std::optional<bool> enableGpuLogging;
        std::optional<std::string> backendRendererApiName;
        std::optional<std::string> backendRendererApiVersionString;

        const CliParser parser{
            CliValue{ { "logger.filename" }, loggerFilename },
            CliValue{ { "logger.gpu" }, enableGpuLogging },
            CliValue{ { "backend.renderer.api.name" }, backendRendererApiName },
            CliValue{ { "backend.renderer.api.version" }, backendRendererApiVersionString },
            CliValue{ { "fps.max" }, editorDescriptor.fpsLimit }
        };

        if(!parser.Parse(argc, argv))
        {
            return false;
        }

        // Logger settings.
        LoadLogger(loggerFilename);
        editorDescriptor.logger = m_logger;

        // Gpu logging
        if (enableGpuLogging.has_value())
        {
            editorDescriptor.enableGpuLogging = enableGpuLogging.value();
        }

        // Renderer settings.
        if(backendRendererApiName.has_value())
        {
            const auto& backendRendererApiNameString = *backendRendererApiName;
            if (backendRendererApiNameString == "vulkan")
            {
                editorDescriptor.backendRendererApi = std::make_optional<Renderer::BackendApi>(Renderer::BackendApi::Vulkan);
            }
            else if (backendRendererApiNameString == "opengl")
            {
                editorDescriptor.backendRendererApi = std::make_optional<Renderer::BackendApi>(Renderer::BackendApi::OpenGL);
            }
            else
            {
                m_logger->Write(Logger::Severity::Error, "Invalid cli parameter \"backend.renderer.api.name\" :" + *backendRendererApiName);
                return false;
            }
        }

        if (backendRendererApiVersionString.has_value())
        {
            Version backendRendererVersion;
            if(!backendRendererVersion.FromString(*backendRendererApiVersionString))
            {
                m_logger->Write(Logger::Severity::Error, "Invalid cli parameter \"backend.renderer.api.version\" :" + *backendRendererApiVersionString);
                return false;
            }
            editorDescriptor.backendRendererApiVersion = std::make_optional<Version>(backendRendererVersion);
        }

        return true;
    }

    void Application::LoadLogger(const std::optional<std::string>& filename)
    {

#if MOLTEN_BUILD == MOLTEN_BUILD_RELEASE
        const uint32_t loggerSeverityFlags = 
            static_cast<uint32_t>(Logger::Severity::Error) |
            static_cast<uint32_t>(Logger::Severity::Warning) |
            static_cast<uint32_t>(Logger::Severity::Info);
#else
        const uint32_t loggerSeverityFlags = Logger::SeverityAllFlags;
#endif
        
        auto createDefaultLogger = [&]()
        {
            return std::make_shared<Logger>(loggerSeverityFlags);
        };

        auto createFileLogger = [&]() -> std::shared_ptr<FileLogger>
        {
            auto fileLogger = std::make_shared<FileLogger>(*filename, FileLogger::OpenMode::Append, loggerSeverityFlags);
            if (!fileLogger->IsOpen())
            {
                return nullptr;
            }

            return fileLogger;
        };

        if(filename.has_value() && !filename->empty())
        {
            m_logger = createFileLogger();
            if(!m_logger)
            {
                m_logger = createDefaultLogger();
                m_logger->Write(Logger::Severity::Error, "Failed to open log file \"" + *filename + "\"");
                return;
            }
        }

        m_logger = createDefaultLogger();    
    }

    int Application::Start(const EditorDescriptor& editorDescriptor)
    {
        try
        {
            Semaphore cancellationSemaphore;
            m_editor = std::make_unique<Editor>(cancellationSemaphore);
            if (!m_editor->Open(editorDescriptor))
            {
                m_logger->Write(Logger::Severity::Error, "Failed to open editor.");
                return g_error_load;
            }
            cancellationSemaphore.Wait();
            return true;
        }
        catch (std::exception& e)
        {
            m_logger->Write(Logger::Severity::Error, std::string{ "Uncaught exception while starting editor: " } + e.what());
            exit(g_error_exception);
        }
        catch (...)
        {
            m_logger->Write(Logger::Severity::Error, "Uncaught exception while starting editor.");
            exit(g_error_exception);
        }
    }

}
