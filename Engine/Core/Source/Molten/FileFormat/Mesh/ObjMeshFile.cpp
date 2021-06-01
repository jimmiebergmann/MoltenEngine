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

#include "Molten/FileFormat/Mesh/ObjMeshFile.hpp"
#include "Molten/System/ThreadPool.hpp"
#include "Molten/Utility/StringUtility.hpp"
#include <fstream>
#include <charconv>

namespace Molten
{

    // Global implementations.
    static bool IsWhitespace(const size_t index, const std::string& string)
    {
        return index < string.size() && (string[index] == ' ' || string[index] == '\t');
    }

    template<typename T>
    bool ParseVector(std::string_view lineView, T& value)
    {
        auto currentLineView = lineView;

        size_t index = 0;
        while (!currentLineView.empty())
        {
            if(index > T::Dimensions)
            {
                return false;
            }

            StringUtility::TrimFront(currentLineView);
            const auto end = currentLineView.find_first_of(" \t");
            auto element = currentLineView.substr(0, end);
            currentLineView = std::string_view{ currentLineView.data() + element.size(), currentLineView.size() - element.size() };

            if(std::from_chars(element.data(), element.data() + element.size(), value.c[index]).ec != std::errc())
            {
                return false;
            }

            ++index;
        }
        return true;
    }

    // Object mesh file implementations.
    TextFileFormatResult ObjMeshFile::ReadFromFile(
        const std::filesystem::path& filename,
        ThreadPool& threadPool)
    {
        ObjMeshFileReader reader;
        return reader.ReadFromFile(*this, filename, threadPool);
    }

    [[nodiscard]] TextFileFormatResult ObjMeshFile::ReadFromFile(
        const std::filesystem::path& filename,
        ObjMeshFileReader& reader)
    {
        return reader.ReadFromFile(*this, filename);
    }

    void ObjMeshFile::Clear()
    {
    }


    // Obj mesh file reader implementations.
    ObjMeshFileReader::ObjMeshFileReader() :
        m_threadPool(nullptr),
        m_objMeshFile(nullptr)
    {}

    TextFileFormatResult ObjMeshFileReader::ReadFromFile(
        ObjMeshFile& objMeshFile,
        const std::filesystem::path& filename)
    {
        m_threadPool = nullptr;

        auto result = InternalReadFromFile(objMeshFile, filename);

        for (auto& future : m_objectFutures)
        {
            future.wait();
        }

        return result;
    }

    TextFileFormatResult ObjMeshFileReader::ReadFromFile(
        ObjMeshFile& objMeshFile,
        const std::filesystem::path& filename,
        ThreadPool& threadPool)
    {
        m_threadPool = &threadPool;

        auto result = InternalReadFromFile(objMeshFile, filename);

        for (auto& future : m_objectFutures)
        {
            future.wait();
        }

        return result;
    }

    ObjMeshFileReader::MaterialCommand::MaterialCommand(
        const size_t lineNumber,
        std::string&& line
    ) :
        lineNumber(lineNumber),
        line(std::move(line))
    {}

    ObjMeshFileReader::ObjectCommand::ObjectCommand(
        const size_t lineNumber,
        ObjectCommandType type,
        std::string&& line
    ) :
        lineNumber(lineNumber),
        type(type),
        line(std::move(line))
    {}

    TextFileFormatResult ObjMeshFileReader::InternalReadFromFile(
        ObjMeshFile& objMeshFile,
        const std::filesystem::path& filename)
    {
        objMeshFile.Clear();

        std::ifstream file(filename);
        if (!file.is_open())
        {
            return { TextFileFormatResult::OpenFileError, "Failed to open file " + filename.string() };
        }

        Prepare(objMeshFile, filename);

        // Helper function for creating error values.
        size_t lineNumber = 0;

        auto createMissingCommandDataError = [&lineNumber]() -> TextFileFormatResult
        {
            return { TextFileFormatResult::ParseError, lineNumber, "Missing command data" };
        };

        auto createUnknownCommandError = [&lineNumber]() -> TextFileFormatResult
        {
            return { TextFileFormatResult::ParseError, lineNumber, "Unknown command" };
        };

        // Read lines from file and process them.
        while (!file.eof())
        {
            std::string line;
            std::getline(file, line);
            StringUtility::Trim(line);

            // Ignore empty line
            if (line.empty())
            {
                ++lineNumber;
                continue;
            }

            // Process first character in order to determine what command current line is.
            switch (line[0])
            {
                case '#': break; // Comment.
                case 'm': // Material import.
                {
                    if (line.size() < 7 || line.rfind("mtllib", 0) != 0 || !IsWhitespace(6, line))
                    {
                        return createUnknownCommandError();
                    }

                    if (auto result = ExecuteProcessMaterial({ lineNumber, std::move(line) }); !result)
                    {
                        return result;
                    }
                } break;
                case 'o': // Object
                {

                    if (!IsWhitespace(1, line))
                    {
                        return createMissingCommandDataError();
                    }

                    if (!m_objectCommands->empty())
                    {
                        // Send current line buffer to new thread and process object lines.
                        if (auto result = ExecuteProcessObject(std::move(m_objectCommands)); !result)
                        {
                            return result;
                        }

                        // Create new line buffer and object
                        m_objectCommands = std::make_shared<ObjectCommands>();
                    }
                    m_objectCommands->emplace_back(lineNumber, ObjectCommandType::Object, std::move(line));
                } break;
                case 'g': // Group
                {
                    if (!IsWhitespace(1, line))
                    {
                        return createMissingCommandDataError();
                    }
                    m_objectCommands->emplace_back(lineNumber, ObjectCommandType::Group, std::move(line));
                } break;
                case 's': // Shading group
                {
                    if (!IsWhitespace(1, line))
                    {
                        return createMissingCommandDataError();
                    }
                    m_objectCommands->emplace_back(lineNumber, ObjectCommandType::ShadingGroup, std::move(line));
                } break;
                case 'v': // Vertex / Normal / UV
                {
                    if (line.size() < 4)
                    {
                        return createUnknownCommandError();
                    }

                    switch (line[1])
                    {
                        case ' ': // Vertex
                        case '\t':
                        {
                            m_objectCommands->emplace_back(lineNumber, ObjectCommandType::Vertex, std::move(line));
                        } break;
                        case 'n': // Normal
                        {
                            if (!IsWhitespace(2, line))
                            {
                                return createUnknownCommandError();;
                            }
                            m_objectCommands->emplace_back(lineNumber, ObjectCommandType::Normal, std::move(line));
                        } break;
                        case 't': // UV
                        {
                            if (!IsWhitespace(2, line))
                            {
                                return createUnknownCommandError();
                            }
                            m_objectCommands->emplace_back(lineNumber, ObjectCommandType::Uv, std::move(line));
                        } break;
                        default: break;
                    }

                } break;
                case 'u':
                {
                    if (line.size() < 7 || line.rfind("usemtl", 0) != 0 || !IsWhitespace(6, line))
                    {
                        return createUnknownCommandError();
                    }
                    m_objectCommands->emplace_back(lineNumber, ObjectCommandType::UseMaterial, std::move(line));
                } break;
                case 'f':
                {
                    if (line.size() < 4)
                    {
                        return createUnknownCommandError();
                    }
                    m_objectCommands->emplace_back(lineNumber, ObjectCommandType::Face, std::move(line));
                } break;
                default: // Unknown command.
                {
                    return createUnknownCommandError();
                }
            }

            ++lineNumber;
        }

        // Process remaining object lines if any.
        if (!m_objectCommands->empty())
        {
            if (auto result = ExecuteProcessObject(std::move(m_objectCommands)); !result)
            {
                return result;
            }
        }

        // Wait for futures to complete.
        if(auto result = HandleFutures(); !result)
        {
            return result;
        }

        return {};
    }

    void ObjMeshFileReader::Prepare(
        ObjMeshFile& objMeshFile,
        const std::filesystem::path& filename)
    {
        m_objMeshFile = &objMeshFile;
        m_objMeshDirectory = filename.parent_path();

        // Clear line commands.
        if (m_objectCommands)
        {
            m_objectCommands->clear();
        }
        else
        {
            m_objectCommands = std::make_shared<ObjectCommands>();
        }

        m_materialFilenames.clear();
        m_materialFutures.clear();
        m_objectFutures.clear();
    }

    TextFileFormatResult ObjMeshFileReader::ExecuteProcessMaterial(MaterialCommand&& materialCommand)
    {
        // Handle all futures and check for errors.
        if (auto result = TryHandleFutures(); !result)
        {
            return result;
        }

        // Parse filenames.
        auto lineView = std::string_view{ materialCommand.line.data() + 7, materialCommand.line.size() - 7 };
        StringUtility::TrimFront(lineView);

        std::vector<std::string> filenames;
        while (!lineView.empty())
        {
            StringUtility::TrimFront(lineView);
            const auto end = lineView.find_first_of(" \t");
            auto filename = lineView.substr(0, end);
            lineView = std::string_view{ lineView.data() + filename.size(), lineView.size() - filename.size() };

            if (!filename.empty())
            {
                filenames.push_back((m_objMeshDirectory / std::string{ filename }).generic_string());
            }
        }

        if (filenames.empty())
        {
            return { TextFileFormatResult::ParseError, materialCommand.lineNumber, "Expecting one or more material file names" };
        }

        // Use thread pool.
        if (m_threadPool)
        {
            for(auto& filename : filenames)
            {
                auto future = ProcessMaterialAsync(std::move(filename));
                m_materialFutures.push_back(std::move(future));
            }
            
            return {};
        }

        // Execute material processing on this thread.
        for (auto& filename : filenames)
        {
            auto result = ProcessMaterial(std::move(filename));
            if (result.index() == 0)
            {
                auto& material = std::get<MaterialSharedPointer>(result);
                m_objMeshFile->materials.push_back(material);
            }
            else
            {
                // An error occurred while processing on this thread.
                auto& error = std::get<TextFileFormatResult::Error>(result);
                return { std::move(error) };
            }
        }

        return {};
    }
    ObjMeshFileReader::ProcessMaterialResult ObjMeshFileReader::ProcessMaterial(std::string&& filename)
    {
        return { nullptr };
    }

    ObjMeshFileReader::ProcessMaterialFuture ObjMeshFileReader::ProcessMaterialAsync(std::string&& filename)
    {
        return m_threadPool->Execute(
            [this, filename = std::move(filename)]() mutable
        {
            return ProcessMaterial(std::move(filename));
        });
    }

    TextFileFormatResult ObjMeshFileReader::ExecuteProcessObject(ObjectCommandsSharedPointer&& objectCommands)
    {
        // Handle all futures and check for errors.
        if(auto result = TryHandleFutures(); !result)
        {
            return result;
        }

        // Use thread pool.
        if(m_threadPool)
        {
            auto future = ProcessObjectAsync(std::move(objectCommands));
            m_objectFutures.push_back(std::move(future));
            return {};
        }

        // Execute object processing on this thread.
        auto result = ProcessObject(std::move(objectCommands));
        if(result.index() == 0)
        {
            auto& object = std::get<ObjectSharedPointer>(result);
            m_objMeshFile->objects.push_back(object);
            return {};
        }

        // An error occurred while processing on this thread.
        auto& error = std::get<TextFileFormatResult::Error>(result);
        return { std::move(error) };
    }

    ObjMeshFileReader::ProcessObjectResult ObjMeshFileReader::ProcessObject(ObjectCommandsSharedPointer&& objectCommands)
    {
        auto object = std::make_shared<Object>();

        auto createLineView = [](const ObjectCommand& command, const size_t offset)
        {
            return std::string_view{ command.line.data() + offset, command.line.size() - offset };
        };

        for(auto& command : *objectCommands)
        {
            switch(command.type)
            {
                case ObjectCommandType::Object: ///< o
                {
                    auto lineView = createLineView(command, 2);
                    StringUtility::TrimFront(lineView);
                    if(lineView.empty())
                    {
                        return TextFileFormatResult::Error{ TextFileFormatResult::ParseError, command.lineNumber, "Expecting an object name" };
                    }
                    object->name = std::string{ lineView };
                } break;
                case ObjectCommandType::Vertex: ///< v
                {
                    auto lineView = createLineView(command, 2);
                    StringUtility::TrimFront(lineView);

                    if (lineView.empty())
                    {
                        return TextFileFormatResult::Error{ TextFileFormatResult::ParseError, command.lineNumber, "Expecting vertex data" };
                    }

                    Vector3f32 vertex;
                    if(!ParseVector(lineView, vertex))
                    {
                        return TextFileFormatResult::Error{ TextFileFormatResult::ParseError, command.lineNumber, "Invalid vertex data" };
                    }
                    object->vertices.push_back(vertex);
                    
                } break;
                case ObjectCommandType::Normal: ///< vn
                {
                    auto lineView = createLineView(command, 3);
                    StringUtility::TrimFront(lineView);

                    if (lineView.empty())
                    {
                        return TextFileFormatResult::Error{ TextFileFormatResult::ParseError, command.lineNumber, "Expecting vertex normal data" };
                    }

                    Vector3f32 normal;
                    if (!ParseVector(lineView, normal))
                    {
                        return TextFileFormatResult::Error{ TextFileFormatResult::ParseError, command.lineNumber, "Invalid vertex normal data" };
                    }
                    object->normals.push_back(normal);
                } break;
                case ObjectCommandType::Uv: ///< vt
                {
                    auto lineView = createLineView(command, 3);
                    StringUtility::TrimFront(lineView);

                    if (lineView.empty())
                    {
                        return TextFileFormatResult::Error{ TextFileFormatResult::ParseError, command.lineNumber, "Expecting texture coordinate data" };
                    }

                    Vector2f32 textureCoord;
                    if (!ParseVector(lineView, textureCoord))
                    {
                        return TextFileFormatResult::Error{ TextFileFormatResult::ParseError, command.lineNumber, "Invalid texture coordinate data" };
                    }
                    object->textureCoords.push_back(textureCoord);
                } break;
                case ObjectCommandType::Group: ///< g
                {

                } break;
                case ObjectCommandType::ShadingGroup: ///< s
                {

                } break;
                case ObjectCommandType::Face: ///< f
                {

                } break;
                case ObjectCommandType::UseMaterial: ///< usemtl
                {

                } break;
            }
        }

        return { object };
    }

    ObjMeshFileReader::ProcessObjectFuture ObjMeshFileReader::ProcessObjectAsync(ObjectCommandsSharedPointer&& objectCommands)
    {
        return m_threadPool->Execute(
            [this, objectCommands = std::move(objectCommands)]() mutable
        {
            return ProcessObject(std::move(objectCommands));
        });
    }

    TextFileFormatResult ObjMeshFileReader::HandleFutures()
    {
        if (auto result = HandleMaterialFutures(); !result)
        {
            return result;
        }
        if (auto result = HandleObjectFutures(); !result)
        {
            return result;
        }

        return {};
    }

    TextFileFormatResult ObjMeshFileReader::HandleMaterialFutures()
    {
        // Check material futures.
        for (auto& future : m_materialFutures)
        {
            if (auto result = future.get(); result.index() == 0)
            {
                auto& material = std::get<MaterialSharedPointer>(result);
                m_objMeshFile->materials.push_back(material);
            }
            else
            {
                // An error occurred while processing via thread pool.
                auto& error = std::get<TextFileFormatResult::Error>(result);
                return { std::move(error) };
            }
        }

        m_materialFutures.clear();
        return {};
    }

    TextFileFormatResult ObjMeshFileReader::HandleObjectFutures()
    {
        // Check object futures.
        for (auto& future : m_objectFutures)
        {
            if (auto result = future.get(); result.index() == 0)
            {
                auto& object = std::get<ObjectSharedPointer>(result);
                m_objMeshFile->objects.push_back(object);
            }
            else
            {
                // An error occurred while processing via thread pool.
                auto& error = std::get<TextFileFormatResult::Error>(result);
                return { std::move(error) };
            }
        }

        m_objectFutures.clear();
        return {};
    }

    TextFileFormatResult ObjMeshFileReader::TryHandleFutures()
    {
        if (auto result = TryHandleMaterialFutures(); !result)
        {
            return result;
        }
        if (auto result = TryHandleObjectFutures(); !result)
        {
            return result;
        }

        return {};
    }

    TextFileFormatResult ObjMeshFileReader::TryHandleMaterialFutures()
    {
        for (auto it = m_materialFutures.begin(); it != m_materialFutures.end();)
        {
            if (auto& future = *it; future.wait_for(std::chrono::seconds{ 0 }) == std::future_status::ready)
            {
                if (auto result = future.get(); result.index() == 0)
                {
                    auto& material = std::get<MaterialSharedPointer>(result);
                    m_objMeshFile->materials.push_back(material);

                    it = m_materialFutures.erase(it);
                }
                else
                {
                    // An error occurred while processing via thread pool.
                    auto& error = std::get<TextFileFormatResult::Error>(result);
                    return { std::move(error) };
                }
            }
            else
            {
                ++it;
            }
        }

        return {};
    }

    TextFileFormatResult ObjMeshFileReader::TryHandleObjectFutures()
    {
        for (auto it = m_objectFutures.begin(); it != m_objectFutures.end();)
        {
            if (auto& future = *it; future.wait_for(std::chrono::seconds{ 0 }) == std::future_status::ready)
            {
                if (auto result = future.get(); result.index() == 0)
                {
                    auto& object = std::get<ObjectSharedPointer>(result);
                    m_objMeshFile->objects.push_back(object);

                    it = m_objectFutures.erase(it);
                }
                else
                {
                    // An error occurred while processing via thread pool.
                    auto& error = std::get<TextFileFormatResult::Error>(result);
                    return { std::move(error) };
                }
            }
            else
            {
                ++it;
            }
        }

        return {};
    }

}
