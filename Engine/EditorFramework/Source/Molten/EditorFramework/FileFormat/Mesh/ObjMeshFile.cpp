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

#include "Molten/EditorFramework/FileFormat/Mesh/ObjMeshFile.hpp"
#include "Molten/System/ThreadPool.hpp"
#include "Molten/Utility/StringUtility.hpp"
#include "Molten/Utility/BufferedFileLineReader.hpp"
#include <fstream>
#include <charconv>

namespace Molten::EditorFramework
{

    // Global implementations.
    static bool IsWhitespace(const size_t index, const std::string_view& stringView)
    {
        return index < stringView.size() && (stringView[index] == ' ' || stringView[index] == '\t');
    }

    template<typename T>
    static bool ParseVector(std::string_view lineView, T& value)
    {
        size_t index = 0;
        while (!lineView.empty())
        {
            if(index > T::Dimensions)
            {
                return false;
            }

            StringUtility::TrimFront(lineView);
            const auto end = lineView.find_first_of(" \t");
            auto element = lineView.substr(0, end);
            lineView = lineView.substr(element.size());

            if(std::from_chars(element.data(), element.data() + element.size(), value.c[index]).ec != std::errc())
            {
                return false;
            }

            ++index;
        }
        return true;
    }

    template<typename TCommand>
    static Unexpected<TextFileFormatError> CreateParseError(const TCommand& command, std::string&& message)
    {
        return Unexpected(TextFileFormatError{
            .code = TextFileFormatErrorCode::ParseError,
            .message = std::move(message),
            .line = command.lineNumber,
            .column = 0
        });
    }

    static Unexpected<TextFileFormatError> CreateParseError(std::string&& message)
    {
        return Unexpected(TextFileFormatError{
            .code = TextFileFormatErrorCode::ParseError,
            .message = std::move(message),
            .line = 0,
            .column = 0
        });
    }

    static Unexpected<TextFileFormatError> CreateParseError(const size_t lineNumber, std::string&& message)
    {
        return Unexpected(TextFileFormatError{
            .code = TextFileFormatErrorCode::ParseError,
            .message = std::move(message),
            .line = lineNumber,
            .column = 0
        });
    }


    // Triangle implementations.
    ObjMeshFile::Triangle::Triangle() :
        vertexIndices{ std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max() },
        textureCoordinateIndices{ std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max() },
        normalIndices{ std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max() }
    {}


    // Smoothing group implementations.
    ObjMeshFile::SmoothingGroup::SmoothingGroup() :
        id(0)
    {}

    bool ObjMeshFile::SmoothingGroup::IsEmpty() const
    {
        return triangles.empty();
    }


    // Group implementations.
    bool ObjMeshFile::Group::IsEmpty() const
    {
        if(smoothingGroups.empty())
        {
            return true;
        }

        for(auto& smoothingGroup : smoothingGroups)
        {
            if(!smoothingGroup->IsEmpty())
            {
                return false;
            }
        }

        return true;
    }


    // Object mesh file implementations.
    void ObjMeshFile::Clear()
    {
        objects.clear();
    }


    // Obj mesh file reader implementations.
    ObjMeshFileReader::ObjMeshFileReader() :
        m_threadPool(nullptr)
    {}

    Expected<ObjMeshFile, TextFileFormatError> ObjMeshFileReader::ReadFromFile(const std::filesystem::path& filename)
    {
        m_threadPool = nullptr;
        return InternalReadFromFile(filename);
    }

    Expected<ObjMeshFile, TextFileFormatError> ObjMeshFileReader::ReadFromFile(
        const std::filesystem::path& filename,
        ThreadPool& threadPool)
    {
        m_threadPool = &threadPool;
        return InternalReadFromFile(filename);
    }

    /*ObjMeshFileReader::MaterialCommand::MaterialCommand(
        const size_t lineNumber,
        std::string line
    ) :
        lineNumber(lineNumber),
        line(std::move(line))
    {}*/

    ObjMeshFileReader::ObjectCommand::ObjectCommand(
        const size_t lineNumber,
        const ObjectCommandType type,
        const std::string_view line
    ) :
        lineNumber(lineNumber),
        type(type),
        line(line)
    {}

    ObjMeshFileReaderResult ObjMeshFileReader::InternalReadFromFile(
        const std::filesystem::path& filename)
    {

        // Open file and prepare obj mesh file reader.
        std::ifstream file(filename, std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(TextFileFormatError{
                .code = TextFileFormatErrorCode::OpenFileError,
                .message = "Missing command data"
            });
        }

        m_objectFutures.clear();
        ObjMeshFile objMeshFile{};

        // Helper function for creating error values.
        size_t lineNumber = 0;

        auto createMissingCommandDataError = [&lineNumber]()
        {
            return CreateParseError(lineNumber, "Missing command data");
        };

        auto createUnknownCommandError = [&lineNumber]()
        {
            return CreateParseError(lineNumber, "Unknown command");
        };

        // Storage for current object buffer.
        ObjectBufferSharedPointer currentObjectBuffer = std::make_shared<ObjectBuffer>();
        auto addNewBuffer = [&currentObjectBuffer](auto& buffer)
        {
            currentObjectBuffer->buffers.push_back(buffer);
        };

        // Read lines from file and process them.
        BufferedFileLineReader lineReader(file, 2048, 1048576);
        BufferedFileLineReader::LineReadResult readResult = BufferedFileLineReader::LineReadResult::Successful;

        onProgress(0.0);

        while(readResult == BufferedFileLineReader::LineReadResult::Successful)
        {
            std::string_view line;
            readResult = lineReader.ReadLine(line, addNewBuffer);

            switch(readResult)
            {                
                case BufferedFileLineReader::LineReadResult::BufferOverflow: return CreateParseError(lineNumber, "Row is too long for an obj file");
                case BufferedFileLineReader::LineReadResult::AllocationError: return CreateParseError(lineNumber, "Failed to allocate required memory");
                case BufferedFileLineReader::LineReadResult::Successful:
                case BufferedFileLineReader::LineReadResult::EndOfFile: break;
            }

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

                    // Add material names to obj mesh file.
                    // ...

                    /*if (auto result = ExecuteProcessMaterial({ lineNumber, std::string{ line } }); !result)
                    {
                        return result;
                    }*/
                } break;
                case 'o': // Object
                {
                    if (!IsWhitespace(1, line))
                    {
                        return createMissingCommandDataError();
                    }

                    if (!currentObjectBuffer->commands.empty())
                    {
                        const auto progress = (1.0 - (static_cast<double>(lineReader.GetSizeLeft()) / static_cast<double>(lineReader.GetStreamSize()))) * 100.0;
                        onProgress(progress);


                        // Send current line buffer to new thread and process object lines.
                        if (auto result = ProcessObject(objMeshFile, currentObjectBuffer); !result)
                        {
                            return Unexpected(result.Error());
                        }

                        // Create new line buffer and object
                        auto newObjectBuffer = std::make_shared<ObjectBuffer>();
                        if(!currentObjectBuffer->buffers.empty())
                        {
                            newObjectBuffer->buffers.push_back(currentObjectBuffer->buffers.back());
                        }
                        currentObjectBuffer = newObjectBuffer;
                    }

                    currentObjectBuffer->commands.emplace_back(lineNumber, ObjectCommandType::Object, line);
                } break;
                case 'g': // Group
                {
                    if (!IsWhitespace(1, line))
                    {
                        return createMissingCommandDataError();
                    }
                    currentObjectBuffer->commands.emplace_back(lineNumber, ObjectCommandType::Group, line);
                } break;
                case 's': // Smoothing group
                {
                    if (!IsWhitespace(1, line))
                    {
                        return createMissingCommandDataError();
                    }
                    currentObjectBuffer->commands.emplace_back(lineNumber, ObjectCommandType::SmoothingGroup, line);
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
                            currentObjectBuffer->commands.emplace_back(lineNumber, ObjectCommandType::Vertex, line);
                        } break;
                        case 'n': // Normal
                        {
                            if (!IsWhitespace(2, line))
                            {
                                return createUnknownCommandError();;
                            }
                            currentObjectBuffer->commands.emplace_back(lineNumber, ObjectCommandType::Normal, line);
                        } break;
                        case 't': // UV
                        {
                            if (!IsWhitespace(2, line))
                            {
                                return createUnknownCommandError();
                            }
                            currentObjectBuffer->commands.emplace_back(lineNumber, ObjectCommandType::Uv, line);
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
                    currentObjectBuffer->commands.emplace_back(lineNumber, ObjectCommandType::UseMaterial, line);
                } break;
                case 'f':
                {
                    if (line.size() < 4)
                    {
                        return createUnknownCommandError();
                    }
                    currentObjectBuffer->commands.emplace_back(lineNumber, ObjectCommandType::Face, line);
                } break;
                default: // Unknown command.
                {
                    return createUnknownCommandError();
                }
            }

            ++lineNumber;
        }

        // Process remaining object lines if any.
        if (!currentObjectBuffer->commands.empty())
        {
            if (auto result = ProcessObject(objMeshFile, currentObjectBuffer); !result)
            {
                return Unexpected(result.Error());
            }
        }

        // Wait for futures to complete.
        if(auto result = HandleObjectFutures(objMeshFile); !result)
        {
            return Unexpected(std::move(result.Error()));
        }

        return objMeshFile;
    }

    ObjMeshFileReader::ProcessObjectResult ObjMeshFileReader::ProcessObject(
        ObjMeshFile& objMeshFile,
        ObjectBufferSharedPointer objectBuffer)
    {
        // Handle all futures and check for errors.
        if(auto result = TryHandleObjectFutures(objMeshFile); !result)
        {
            return result;
        }

        // Use thread pool.
        if(m_threadPool)
        {
            auto future = ParseObjectCommandsAsync(std::move(objectBuffer));
            m_objectFutures.push_back(std::move(future));
            return {};
        }

        // Execute object processing on this thread.
        auto result = ParseObjectCommands(std::move(objectBuffer));
        if (!result)
        {
            // An error occurred while processing on this thread.
            return Unexpected(std::move(result.Error()));
        }

        auto& object = result.Value();
        objMeshFile.objects.push_back(object);

        return {};
    }
    
    ObjMeshFileReader::ParseObjectResult ObjMeshFileReader::ParseObjectCommands(ObjectBufferSharedPointer objectBuffer)
    {
        auto object = std::make_shared<Object>();

        auto currentGroup = std::make_shared<Group>();
        object->groups.push_back(currentGroup);

        auto currentSmoothingGroup = std::make_shared<SmoothingGroup>();
        currentGroup->smoothingGroups.push_back(currentSmoothingGroup);
        
        // Create new smooth group lambda.
        auto createNewSmoothingGroup = [&]()
        {
            currentSmoothingGroup = std::make_shared<SmoothingGroup>();
            currentGroup->smoothingGroups.push_back(currentSmoothingGroup);
        };

        // Create new group lambda.
        auto createNewGroup = [&]()
        {
            const auto prevGroup = object->groups.back();
            currentGroup = std::make_shared<Group>();
            object->groups.push_back(currentGroup);

            createNewSmoothingGroup();

            if (!prevGroup->material.empty())
            {
                currentGroup->material = prevGroup->material;
            }
        };

        // Helper lambda for extracting data from string view.
        auto createDataView = [](const ObjectCommand& command, const size_t offset)
        {
            auto dataView = std::string_view{ command.line.data() + offset, command.line.size() - offset };
            StringUtility::TrimFront(dataView);
            return dataView;
        };

        // Lambda for reading face data.
        auto readFace = [&](std::string_view lineView) -> ProcessObjectResult
        {
            enum class FaceFlags : uint8_t
            {
                Vertex = 1,
                Uv = 2,
                Normal = 4
            };

            static const uint8_t indexedFlags[3] = {
                static_cast<uint8_t>(FaceFlags::Vertex),
                static_cast<uint8_t>(FaceFlags::Uv),
                static_cast<uint8_t>(FaceFlags::Normal) };

            static std::array<uint32_t, 3> ObjMeshFile::Triangle::* const triangleMembers[3] = {
                &ObjMeshFile::Triangle::vertexIndices,
                &ObjMeshFile::Triangle::textureCoordinateIndices,
                &ObjMeshFile::Triangle::normalIndices
            };

            auto readElement = [&](ObjMeshFile::Triangle& triangle, const size_t index) -> uint8_t
            {
                uint8_t faceFlags = 0;

                for (size_t i = 0; i < 3; i++)
                {
                    StringUtility::TrimFront(lineView);

                    const size_t endPos = lineView.find_first_of(" /\t");

                    auto element = lineView.substr(0, endPos);
                    lineView = std::string_view{ lineView.data() + element.size(), lineView.size() - element.size() };

                    if (!element.empty())
                    {
                        uint32_t value = 0;
                        if (std::from_chars(element.data(), element.data() + element.size(), value).ec == std::errc())
                        {
                            faceFlags |= indexedFlags[i];
                            (triangle.*triangleMembers[i])[index] = value;
                        }
                    }

                    StringUtility::TrimFront(lineView);
                    if (lineView.empty() || lineView.front() != '/')
                    {
                        break;
                    }
                    lineView = lineView.substr(1);
                }

                return faceFlags;
            };

            auto& triangle = currentSmoothingGroup->triangles.emplace_back();

            const auto elementLayout = readElement(triangle, 0);
            if (elementLayout == 0)
            {
                return CreateParseError("Invalid face layout");
            }

            for (size_t i = 1; i < 3; i++)
            {
                if (auto flags = readElement(triangle, i); flags != elementLayout)
                {
                    if (flags == 0)
                    {
                        return CreateParseError("Invalid face layout");
                    }

                    return CreateParseError("Mismatching face layout");
                }
            }

            // Current face is a quad, add extra triangle.
            if (!lineView.empty())
            {
                auto& newTriangle = currentSmoothingGroup->triangles.emplace_back();
                auto& prevTriangle = currentSmoothingGroup->triangles[currentSmoothingGroup->triangles.size() - 2];
                if (auto flags = readElement(newTriangle, 0); flags != elementLayout)
                {
                    if (flags == 0)
                    {
                        return CreateParseError("Invalid face layout");
                    }

                    return CreateParseError("Mismatching face layout");
                }

                newTriangle.vertexIndices[1] = prevTriangle.vertexIndices[0];
                newTriangle.textureCoordinateIndices[1] = prevTriangle.textureCoordinateIndices[0];
                newTriangle.normalIndices[1] = prevTriangle.normalIndices[0];

                newTriangle.vertexIndices[2] = prevTriangle.vertexIndices[2];
                newTriangle.textureCoordinateIndices[2] = prevTriangle.textureCoordinateIndices[2];
                newTriangle.normalIndices[2] = prevTriangle.normalIndices[2];
            }

            if (!lineView.empty())
            {
                return CreateParseError("Unsupported face layout");
            }

            return {};
        };


        // Run all object commands.
        for(auto& command : objectBuffer->commands)
        {
            switch(command.type)
            {
                case ObjectCommandType::Object: ///< o
                {
                    auto lineView = createDataView(command, 2);
                    if(lineView.empty())
                    {
                        return CreateParseError(command.lineNumber, "Expecting an object name");
                    }
                    object->name = std::string{ lineView };
                } break;
                case ObjectCommandType::Vertex: ///< v
                {
                    auto lineView = createDataView(command, 2);
                    if (lineView.empty())
                    {
                        return CreateParseError(command.lineNumber, "Expecting vertex data");
                    }

                    Vector3f32 vertex;
                    if(!ParseVector(lineView, vertex))
                    {
                        return CreateParseError(command.lineNumber, "Invalid vertex data");
                    }
                    object->vertices.push_back(vertex);
                    
                } break;
                case ObjectCommandType::Normal: ///< vn
                {
                    auto lineView = createDataView(command, 3);
                    if (lineView.empty())
                    {
                        return CreateParseError(command.lineNumber, "Expecting vertex normal data");
                    }

                    Vector3f32 normal;
                    if (!ParseVector(lineView, normal))
                    {
                        return CreateParseError(command.lineNumber, "Invalid vertex normal data");
                    }
                    object->normals.push_back(normal);
                } break;
                case ObjectCommandType::Uv: ///< vt
                {
                    auto lineView = createDataView(command, 3);
                    if (lineView.empty())
                    {
                        return CreateParseError(command.lineNumber, "Expecting texture coordinate data");
                    }

                    Vector2f32 textureCoordinate;
                    if (!ParseVector(lineView, textureCoordinate))
                    {
                        return CreateParseError(command.lineNumber, "Invalid texture coordinate data");
                    }
                    object->textureCoordinates.push_back(textureCoordinate);
                } break;
                case ObjectCommandType::Group: ///< g
                {
                    auto lineView = createDataView(command, 2);
                    if (lineView.empty())
                    {
                        return CreateParseError(command.lineNumber, "Expecting group name");
                    }

                    if (!currentGroup->IsEmpty())
                    {
                        createNewGroup();
                    }

                    currentGroup->name = lineView;
                } break;
                case ObjectCommandType::SmoothingGroup: ///< s
                {
                    auto lineView = createDataView(command, 2);
                    if (lineView.empty())
                    {
                        return CreateParseError(command.lineNumber, "Expecting smoothing group id");
                    }

                    if (!currentSmoothingGroup->IsEmpty())
                    {
                        createNewSmoothingGroup();
                    }

                    if (lineView == "off")
                    {
                        currentSmoothingGroup->id = 0;
                    }
                    else
                    {
                        if (std::from_chars(lineView.data(), lineView.data() + lineView.size(), currentSmoothingGroup->id).ec != std::errc())
                        {
                            return CreateParseError(command.lineNumber, "Invalid smoothing group id");
                        }
                    }
                } break;
                case ObjectCommandType::Face: ///< f
                {
                    auto lineView = createDataView(command, 2);
                    if (lineView.empty())
                    {
                        return CreateParseError(command.lineNumber, "Expecting face data");
                    }

                    if (auto result = readFace(lineView); !result)
                    {
                        auto& error = result.Error();
                        error.line = command.lineNumber;
                        return Unexpected(std::move(error));
                    }

                } break;
                case ObjectCommandType::UseMaterial: ///< usemtl
                {
                    auto lineView = createDataView(command, 7);
                    if (lineView.empty())
                    {
                        return CreateParseError(command.lineNumber, "Expecting material name");
                    }

                    if (!currentGroup->IsEmpty())
                    {
                        createNewGroup();
                    }
                    currentGroup->material = lineView;
                } break;
            }
        }

        return object;
    }

    ObjMeshFileReader::ParseObjectFuture ObjMeshFileReader::ParseObjectCommandsAsync(ObjectBufferSharedPointer objectBuffer)
    {
        return m_threadPool->Execute(
            [this, objectBuffer = std::move(objectBuffer)]() mutable
        {
            return ParseObjectCommands(std::move(objectBuffer));
        });
    }

    ObjMeshFileReader::ProcessObjectResult ObjMeshFileReader::HandleObjectFutures(ObjMeshFile& objMeshFile)
    {
        for (auto it = m_objectFutures.begin(); it != m_objectFutures.end();)
        {
            auto& future = *it;

            if (auto result = future.get(); !result)
            {  
                m_objectFutures.erase(it);
                return Unexpected(std::move(result.Error()));
            }
            else
            {
                objMeshFile.objects.emplace_back(std::move(result.Value()));
                it = m_objectFutures.erase(it);
            }
        }

        return {};
    }

    ObjMeshFileReader::ProcessObjectResult ObjMeshFileReader::TryHandleObjectFutures(ObjMeshFile& objMeshFile)
    {
        for (auto it = m_objectFutures.begin(); it != m_objectFutures.end();)
        {
            if (auto& future = *it; future.wait_for(std::chrono::seconds{ 0 }) == std::future_status::ready)
            {
                if (auto result = future.get(); !result)
                {
                    m_objectFutures.erase(it);
                    return Unexpected(std::move(result.Error()));
                }
                else
                {
                    objMeshFile.objects.emplace_back(std::move(result.Value()));
                    it = m_objectFutures.erase(it);
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
