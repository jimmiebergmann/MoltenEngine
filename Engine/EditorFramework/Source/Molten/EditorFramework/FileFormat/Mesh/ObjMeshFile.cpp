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
#include "Molten/Utility/BufferedStreamReader.hpp"
#include <algorithm>
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
    static bool ParseValue(std::string_view& lineView, T& value)
    {
        const auto end = lineView.find_first_of(" \t");

        auto element = lineView.substr(0, end);
        if (element.empty())
        {
            return false;
        }
        if (std::from_chars(element.data(), element.data() + element.size(), value).ec != std::errc())
        {
            return false;
        }

        lineView = StringUtility::TrimFront(lineView.substr(element.size()));
        return true;
    }

    template<typename T>
    static bool ParseVector(std::string_view& lineView, T& value)
    {
        for(size_t index = 0; index < T::Dimensions; index++)
        {
            if (!ParseValue(lineView, value.c[index]))
            {
                return false;
            }
        }
        return true;
    }

    template<typename T>
    static bool TryParseVector(std::string_view& lineView, T& value)
    {
        for (size_t index = 0; index < T::Dimensions; index++)
        {
            if (lineView.empty())
            {
                return true;
            }
            if (!ParseValue(lineView, value.c[index]))
            {
                return false;
            }
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
        materialFiles.clear();
    }


    // Object mesh materialfile implementations.
    void ObjMaterialFile::Clear()
    {
        materials.clear();
    }


    // Obj mesh file reader implementations.
    ObjMeshFileReader::ObjMeshFileReader() :
        m_threadPool(nullptr)
    {}

    Expected<ObjMeshFile, TextFileFormatError> ObjMeshFileReader::Read(const std::filesystem::path& filename)
    {
        m_threadPool = nullptr;
        return InternalRead(filename);
    }

    Expected<ObjMeshFile, TextFileFormatError> ObjMeshFileReader::Read(
        const std::filesystem::path& filename,
        ThreadPool& threadPool)
    {
        m_threadPool = &threadPool;
        return InternalRead(filename);
    }

    Expected<ObjMeshFile, TextFileFormatError> ObjMeshFileReader::Read(std::istream& stream)
    {
        m_threadPool = nullptr;
        return InternalRead(stream);
    }

    Expected<ObjMeshFile, TextFileFormatError> ObjMeshFileReader::Read(
        std::istream& stream,
        ThreadPool& threadPool)
    {
        m_threadPool = &threadPool;
        return InternalRead(stream);
    }

    ObjMeshFileReader::ObjectCommand::ObjectCommand(
        const size_t lineNumber,
        const ObjectCommandType type,
        const std::string_view line
    ) :
        lineNumber(lineNumber),
        type(type),
        line(line)
    {}

    ObjMeshFileReaderResult ObjMeshFileReader::InternalRead(
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

        return InternalRead(file);
    }

    ObjMeshFileReaderResult ObjMeshFileReader::InternalRead(
        std::istream& stream)
    {
        m_objectFutures.clear();

        ObjMeshFile objMeshFile{};
        size_t lineNumber = 0;

        // Helper function for creating error values.
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
        BufferedStreamReader lineReader(stream, 2048, 1048576);
        BufferedStreamReader::LineReadResult readResult = BufferedStreamReader::LineReadResult::Successful;

        while(readResult == BufferedStreamReader::LineReadResult::Successful)
        {
            std::string_view line;
            readResult = lineReader.ReadLine(line, addNewBuffer);

            switch(readResult)
            {                
                case BufferedStreamReader::LineReadResult::BufferOverflow: return CreateParseError(lineNumber, "Row is too long for an obj file");
                case BufferedStreamReader::LineReadResult::AllocationError: return CreateParseError(lineNumber, "Failed to allocate required memory");
                case BufferedStreamReader::LineReadResult::Successful:
                case BufferedStreamReader::LineReadResult::EndOfFile: break;
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

                    line = StringUtility::TrimFront(line.substr(6));
                    while (!line.empty())
                    {
                        const size_t endPos = line.find_first_of(" \t");
                        auto materialFilename = line.substr(0, endPos);

                        if (auto it = std::find(objMeshFile.materialFiles.begin(), objMeshFile.materialFiles.end(), materialFilename); it == objMeshFile.materialFiles.end())
                        {
                            objMeshFile.materialFiles.emplace_back(materialFilename);
                        }
                        
                        line = StringUtility::TrimFront(line.substr(materialFilename.size()));
                    }      
                } break;
                case 'o': // Object
                {
                    if (!IsWhitespace(1, line))
                    {
                        return createMissingCommandDataError();
                    }

                    if (!currentObjectBuffer->commands.empty())
                    {
                        // Send current line buffer to new thread and process object lines.
                        if (auto result = ProcessObject(objMeshFile, currentObjectBuffer); !result)
                        {
                            return Unexpected(std::move(result.Error()));
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
                return Unexpected(std::move(result.Error()));
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
            return StringUtility::TrimFront(command.line.substr(offset));
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
                    lineView = lineView.substr(element.size());

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
                    if(!TryParseVector(lineView, vertex) || !lineView.empty())
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
                    if (!TryParseVector(lineView, normal) || !lineView.empty())
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
                    if (!TryParseVector(lineView, textureCoordinate) || !lineView.empty())
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


    // Global obj mesh file reader functions.
    ObjMeshFileReaderResult ReadObjMeshFile(
        const std::filesystem::path& filename)
    {
        return ObjMeshFileReader{}.Read(filename);
    }

    ObjMeshFileReaderResult ReadObjMeshFile(
        const std::filesystem::path& filename,
        ThreadPool& threadPool)
    {
        return ObjMeshFileReader{}.Read(filename, threadPool);
    }

    ObjMeshFileReaderResult ReadObjMeshFile(
        std::istream& stream)
    {
        return ObjMeshFileReader{}.Read(stream);
    }

    ObjMeshFileReaderResult ReadObjMeshFile(
        std::istream& stream,
        ThreadPool& threadPool)
    {
        return ObjMeshFileReader{}.Read(stream, threadPool);
    }

    ObjMaterialFileReaderResult ObjMaterialFileReader::Read(
        const std::filesystem::path& filename,
        const ObjMaterialFileReaderOptions& options)
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

        return Read(file, options);
    }

    // Obj mesh material file reader implementations.
    ObjMaterialFileReaderResult ObjMaterialFileReader::Read(
        std::istream& stream,
        const ObjMaterialFileReaderOptions& options)
    {
        // Storage for current object buffer.
        MaterialBufferSharedPointer currentMaterialBuffer = std::make_shared<MaterialBuffer>();
        auto addNewBuffer = [&currentMaterialBuffer](auto& buffer)
        {
            currentMaterialBuffer->buffers.push_back(buffer);
        };

        // Read lines from file and process them.
        BufferedStreamReader lineReader(stream, 2048, 1048576);
        BufferedStreamReader::LineReadResult readResult = BufferedStreamReader::LineReadResult::Successful;
        size_t lineNumber = 0;
        std::string_view line{};

        ObjMaterialReaderResult readerResult{};
        auto& objMaterialFile = readerResult.file;

        auto addUnkownCommandWarning = [&]() -> ProcessMaterialResult
        {
            if (options.ignoreUnknownCommands)
            {
                return {};
            }

            auto createMessage = [&]()
            {
                auto commandSlice = line.substr(0, std::min(line.size(), size_t{ 32 }));
                auto commandEnd = commandSlice.find_first_of(" \t");
                auto command = commandSlice.substr(0, commandEnd);
                auto isSliced = commandEnd == std::string_view::npos;
                return std::string{ "Unkown command '" } + std::string{ command } + (isSliced ? "..." : "") + std::string{ "'" };
            };

            if (options.warningsAsErrors)
            {
                return CreateParseError(lineNumber, createMessage());
            }
            if (!options.useWarnings)
            {
                return {};
            }

            readerResult.warnings.emplace_back(lineNumber, createMessage());
            return {};
        };

        auto parseTexture = [&](std::string_view mapLine) -> ProcessMaterialResult
        {
            switch (mapLine[0])
            {
                case 'K':
                {
                    if (!IsWhitespace(2, mapLine)) 
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }

                    auto data = StringUtility::TrimFront(mapLine.substr(2));

                    switch (mapLine[1])
                    {
                        case 'a':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::AmbientTexture, data);
                        } break;
                        case 'e':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::EmissiveTexture, data);
                        } break;
                        case 'd':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::DiffuseTexture, data);
                        } break;
                        case 's':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::SpecularTexture, data);
                        } break;
                        default: 
                        {
                            if (auto result = addUnkownCommandWarning(); !result)
                            {
                                return Unexpected(std::move(result.Error()));
                            }
                        } break;
                    }

                } break;
                case 'P':
                {
                    if (!IsWhitespace(2, mapLine))
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }

                    auto data = StringUtility::TrimFront(mapLine.substr(2));

                    switch (mapLine[1])
                    {
                        case 'r':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::RoughnessTexture, data);
                        } break;
                        case 'm':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::MetallicTexture, data);
                        } break;
                        default:
                        {
                            if (auto result = addUnkownCommandWarning(); !result)
                            {
                                return Unexpected(std::move(result.Error()));
                            }
                        } break;
                    }

                } break;
                case 'N':
                {
                    if (!IsWhitespace(2, mapLine))
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }

                    auto data = StringUtility::TrimFront(mapLine.substr(2));

                    switch (mapLine[1])
                    {
                        case 's':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::SpecularExponentTexture, data);
                        } break;
                        default:
                        {
                            if (auto result = addUnkownCommandWarning(); !result)
                            {
                                return Unexpected(std::move(result.Error()));
                            }
                        } break;
                    }
                } break;
                case 'd':
                {
                    if (IsWhitespace(1, mapLine))
                    {
                        auto data = StringUtility::TrimFront(mapLine.substr(1));
                        currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::DissolveTexture, data);
                        break;
                    }
                    if (line.rfind("disp", 0) == 0 && IsWhitespace(4, line))
                    {
                        auto data = StringUtility::TrimFront(mapLine.substr(4));
                        currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::DisplacementTexture, data);
                        break;
                    }
                    if (auto result = addUnkownCommandWarning(); !result)
                    {
                        return Unexpected(std::move(result.Error()));
                    }
                } break;
                case 'b': 
                {
                    if (!IsWhitespace(4, mapLine) || mapLine.rfind("bump", 0) != 0)
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }
                    auto data = StringUtility::TrimFront(mapLine.substr(4));
                    currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::BumpTexture, data);
                } break;
                default:
                {
                    if (auto result = addUnkownCommandWarning(); !result)
                    {
                        return Unexpected(std::move(result.Error()));
                    }
                } break;
            }

            return {};
        };

        while (readResult == BufferedStreamReader::LineReadResult::Successful)
        {
            readResult = lineReader.ReadLine(line, addNewBuffer);

            switch (readResult)
            {
                case BufferedStreamReader::LineReadResult::BufferOverflow: return CreateParseError(lineNumber, "Row is too long for an obj material file");
                case BufferedStreamReader::LineReadResult::AllocationError: return CreateParseError(lineNumber, "Failed to allocate required memory");
                case BufferedStreamReader::LineReadResult::Successful:
                case BufferedStreamReader::LineReadResult::EndOfFile: break;
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
                case 'n': // New material.
                {
                    if (!IsWhitespace(6, line) || line.rfind("newmtl", 0) != 0)
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }
                    auto data = StringUtility::TrimFront(line.substr(7));

                    if (!currentMaterialBuffer->commands.empty())
                    {
                        if (auto result = ProcessMaterial(readerResult, options, objMaterialFile, currentMaterialBuffer); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }

                        // Create new line buffer and object
                        auto newMaterialBuffer = std::make_shared<MaterialBuffer>();
                        if (!currentMaterialBuffer->buffers.empty())
                        {
                            newMaterialBuffer->buffers.push_back(currentMaterialBuffer->buffers.back());
                        }
                        currentMaterialBuffer = newMaterialBuffer;
                    }

                    currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::NewMaterial, data);

                } break;
                case 'i':
                {
                    if (!IsWhitespace(5, line) || line.rfind("illum", 0) != 0)
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }
                } break;
                case 'K':
                {
                    if (!IsWhitespace(2, line))
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }
                    auto data = StringUtility::TrimFront(line.substr(3));

                    switch (line[1])
                    {
                        case 'a':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::AmbientColor, data);
                        } break;
                        case 'e':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::EmissiveColor, data);
                        } break;
                        case 'd':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::DiffuseColor, data);
                        } break;
                        case 's':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::SpecularColor, data);
                        } break;
                        default:
                        {
                            if (auto result = addUnkownCommandWarning(); !result)
                            {
                                return Unexpected(std::move(result.Error()));
                            }
                        } break;
                    }
                } break;
                case 'N':
                {
                    if (!IsWhitespace(2, line))
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }
                    auto data = StringUtility::TrimFront(line.substr(3));
                    switch (line[1])
                    {
                        case 's':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::SpecularExponent, data);
                        } break;
                        case 'i':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::OpticalDensity, data);
                        } break;
                        default:
                        {
                            if (auto result = addUnkownCommandWarning(); !result)
                            {
                                return Unexpected(std::move(result.Error()));
                            }
                        } break;
                    }
                } break;
                case 'd':
                {
                    if (IsWhitespace(1, line))
                    {
                        auto data = StringUtility::TrimFront(line.substr(2));
                        currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::Dissolve, data);
                        break;
                    }

                    if (auto result = parseTexture(line); !result)
                    {
                        return Unexpected(std::move(result.Error()));
                    }
                } break;
                case 'b':
                {
                    if (IsWhitespace(1, line))
                    {
                        auto data = StringUtility::TrimFront(line.substr(2));
                        currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::Dissolve, data);
                        break;
                    }

                    if (auto result = parseTexture(line); !result)
                    {
                        return Unexpected(std::move(result.Error()));
                    }
                } break;
                case 'P':
                {
                    if (!IsWhitespace(2, line))
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }
                    auto data = StringUtility::TrimFront(line.substr(3));
                    switch (line[1])
                    {
                        case 'r':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::Roughness, data);
                        } break;
                        case 'm':
                        {
                            currentMaterialBuffer->commands.emplace_back(lineNumber, MaterialCommandType::Metallic, data);
                        } break;
                        default:
                        {
                            if (auto result = addUnkownCommandWarning(); !result)
                            {
                                return Unexpected(std::move(result.Error()));
                            }
                        } break;
                    }
                } break;
                case 'm':
                {
                    if (line.size() < 6 || line.rfind("map_", 0) != 0)
                    {
                        if (auto result = addUnkownCommandWarning(); !result)
                        {
                            return Unexpected(std::move(result.Error()));
                        }
                        break;
                    }
                    if (auto result = parseTexture(line.substr(4)); !result)
                    {
                        return Unexpected(std::move(result.Error()));
                    }
                } break;
                default:
                {
                    if (auto result = addUnkownCommandWarning(); !result)
                    {
                        return Unexpected(std::move(result.Error()));
                    }
                } break;
            }

            ++lineNumber;
        }

        // Process remaining object lines if any.
        if (!currentMaterialBuffer->commands.empty())
        {
            if (auto result = ProcessMaterial(readerResult, options, objMaterialFile, currentMaterialBuffer); !result)
            {
                return Unexpected(std::move(result.Error()));
            }
        }

        return readerResult;
    }

    ObjMaterialFileReader::MaterialCommand::MaterialCommand(
        const size_t lineNumber,
        const MaterialCommandType type,
        const std::string_view data
    ) :
        lineNumber(lineNumber),
        type(type),
        data(data)
    {}

    ObjMaterialFileReader::ProcessMaterialResult ObjMaterialFileReader::ProcessMaterial(
        ObjMaterialReaderResult& readerResult,
        const ObjMaterialFileReaderOptions& options,
        ObjMaterialFile& objMaterialFile,
        MaterialBufferSharedPointer materialBuffer)
    {
        auto material = std::make_shared<Material>();
        objMaterialFile.materials.push_back(material);

        auto duplicateCommandWarning = [&](const MaterialCommand& command, const char* member) -> ProcessMaterialResult
        {
            auto createMessage = [&]()
            {
                return std::string{"Duplicate command for '"} + member + std::string{ "'" };;
            };

            if (options.warningsAsErrors)
            {
                return CreateParseError(command.lineNumber, createMessage());
            }
            if (!options.useWarnings)
            {
                return {};
            }

            readerResult.warnings.emplace_back(command.lineNumber, createMessage());
            return {};
        };

        auto readFloat32 = [](const MaterialCommand& command) -> Expected<float, TextFileFormatError> 
        {
            if (command.data.empty())
            {
                return CreateParseError(command.lineNumber, "Expecting command data");
            }
            float value{};
            auto data = std::string_view{ command.data };
            if (!ParseValue(data, value) || !data.empty())
            {
                return CreateParseError(command.lineNumber, "Invalid command data");
            }
            return value;
        };

        auto readVector3f32 = [](const MaterialCommand& command) -> Expected<Vector3f32, TextFileFormatError> 
        {
            if (command.data.empty())
            {
                return CreateParseError(command.lineNumber, "Expecting command data");
            }
            Vector3f32 value{};
            auto data = std::string_view{ command.data };
            if (!ParseVector(data, value) || !data.empty())
            {
                return CreateParseError(command.lineNumber, "Invalid command data");
            }
            return value;
        };

        auto assignFloat32 = [&](const MaterialCommand& command, std::optional<float>& member, const char* memberName) -> ProcessMaterialResult
        {
            if (member.has_value())
            {
                if (options.ignoreDuplicateCommands)
                {
                    return {};
                }
                if (auto result = duplicateCommandWarning(command, memberName); !result)
                {
                    return Unexpected(std::move(result.Error()));
                }
            }

            auto result = readFloat32(command);
            if (!result)
            {
                return Unexpected(std::move(result.Error()));
            }
            member = result.Value();
            return {};
        };

        auto assignVector3f32 = [&](const MaterialCommand& command, std::optional<Vector3f32>& member, const char* memberName) -> ProcessMaterialResult
        {
            if (member.has_value()) 
            {
                if (options.ignoreDuplicateCommands)
                {
                    return {};
                }
                if (auto result = duplicateCommandWarning(command, memberName); !result)
                {
                    return Unexpected(std::move(result.Error()));
                }
            }

            auto result = readVector3f32(command);
            if (!result)
            {
                return Unexpected(std::move(result.Error()));
            }
            member = result.Value();
            return {};
        };

        auto assignTexture = [&](const MaterialCommand& command, std::optional<MaterialTexture>& member, const char* memberName) -> ProcessMaterialResult
        {
            if (member.has_value())
            {
                if (options.ignoreDuplicateCommands)
                {
                    return {};
                }
                if (auto result = duplicateCommandWarning(command, memberName); !result)
                {
                    return Unexpected(std::move(result.Error()));
                }
            }

            member = MaterialTexture{ .filename = std::string{ command.data } };
            return {};
        };

        auto processCommand = [&](const MaterialCommand& command) -> ProcessMaterialResult
        {
            switch (command.type)
            {
                case MaterialCommandType::NewMaterial:
                {
                    if (command.data.empty())
                    {
                        return CreateParseError(command.lineNumber, "Expecting a material name");
                    }
                    material->name = std::string{ command.data };
                } break;
                case MaterialCommandType::AmbientColor:
                {
                    return assignVector3f32(command, material->ambientColor, "ambient color");
                } break;
                case MaterialCommandType::DiffuseColor:
                {
                    return assignVector3f32(command, material->diffuseColor, "diffuse color");
                } break;
                case MaterialCommandType::SpecularColor:
                {
                    return assignVector3f32(command, material->specularColor, "specular color");
                } break;
                case MaterialCommandType::SpecularExponent:
                {
                    return assignFloat32(command, material->specularExponent, "specular exponent");
                } break;
                case MaterialCommandType::Dissolve:
                {
                    return assignFloat32(command, material->dissolve, "dissolve");
                } break;
                case MaterialCommandType::OpticalDensity:
                {
                    return assignFloat32(command, material->opticalDensity, "optical density");
                } break;
                case MaterialCommandType::AmbientTexture:
                {
                    return assignTexture(command, material->ambientTexture, "ambient texture");
                } break;
                case MaterialCommandType::DiffuseTexture:
                {
                    return assignTexture(command, material->diffuseTexture, "diffuse texture");
                } break;
                case MaterialCommandType::SpecularTexture:
                {
                    return assignTexture(command, material->specularTexture, "specular texture");
                } break;
                case MaterialCommandType::SpecularExponentTexture:
                {
                    return assignTexture(command, material->specularExponentTexture, "specular exponent texture");
                } break;
                case MaterialCommandType::DissolveTexture:
                {
                    return assignTexture(command, material->dissolveTexture, "dissolve texture");
                } break;
                case MaterialCommandType::BumpTexture:
                {
                    return assignTexture(command, material->bumpTexture, "bump texture");
                } break;
                case MaterialCommandType::DisplacementTexture:
                {
                    return assignTexture(command, material->displacementTexture, "displacement texture");
                } break;
                case MaterialCommandType::Roughness:
                {
                    return assignFloat32(command, material->roughness, "roughness");
                } break;
                case MaterialCommandType::Metallic:
                {
                    return assignFloat32(command, material->metallic, "metallic");
                } break;
                case MaterialCommandType::EmissiveColor:
                {
                    return assignVector3f32(command, material->emissiveColor, "emissive color");
                } break;
                case MaterialCommandType::RoughnessTexture:
                {
                    return assignTexture(command, material->roughnessTexture, "roughness texture");
                } break;
                case MaterialCommandType::MetallicTexture:
                {
                    return assignTexture(command, material->metallicTexture, "metallic texture");
                } break;
                case MaterialCommandType::EmissiveTexture:
                {
                    return assignTexture(command, material->emissiveTexture, "emissive texture");
                } break;
            }

            return {};
        };

        // Run all material commands.
        for (auto& command : materialBuffer->commands)
        {
            if (auto result = processCommand(command); !result)
            {
                return Unexpected(std::move(result.Error()));
            }
            
        }

        return {};
    }


    // Global obj material file implementations.
    ObjMaterialFileReaderResult ReadObjMaterialFile(
        const std::filesystem::path& filename,
        const ObjMaterialFileReaderOptions& options)
    {
        return ObjMaterialFileReader{}.Read(filename, options);
    }

    ObjMaterialFileReaderResult ReadObjMaterialFile(
        std::istream& stream,
        const ObjMaterialFileReaderOptions& options)
    {
        return ObjMaterialFileReader{}.Read(stream, options);
    }
}
