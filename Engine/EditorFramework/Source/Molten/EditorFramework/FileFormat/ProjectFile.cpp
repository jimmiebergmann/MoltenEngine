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

#include "Molten/EditorFramework/FileFormat/ProjectFile.hpp"
#include "Molten/FileFormat/RapidJsonFormatResult.hpp"
#include "ThirdParty/rapidjson/include/rapidjson/document.h"
#include "ThirdParty/rapidjson/include/rapidjson/istreamwrapper.h"
#include "ThirdParty/rapidjson/include/rapidjson/schema.h"
#include "ThirdParty/rapidjson/include/rapidjson/prettywriter.h"
#include <array>
#include <string>
#include <fstream>

namespace Molten::EditorFramework
{

    ReadProjectFileResult ReadProjectFile(std::istream& stream)
    {
        rapidjson::IStreamWrapper fileWrapper(stream);

        rapidjson::Document jsonDocumentNonConst;
        rapidjson::ParseResult parseResult = jsonDocumentNonConst.ParseStream(fileWrapper);
        if (parseResult.IsError())
        {
            return Unexpected(CreateJsonErrorCode(parseResult.Offset(), parseResult.Code()));
        }

        // We could use a schema file to validate, but it is probably faster to just check the values manually...
        const auto& jsonDocument = jsonDocumentNonConst;

        //std::vector<ProjectFileWarningCode> warnings;

        auto parseVersion = [&](const std::string& member) -> std::optional<Version>
        {
            if (!jsonDocument.HasMember(member.c_str()))
            {
                //warnings.push_back(ProjectFileWarningCode::MissingFileVersion);
                //warnings.push_back(ProjectFileWarningCode::MissingEngineVersion);
                return Version{ 0, 0, 0 };
            }

            const auto& jsonVersion = jsonDocument[member.c_str()];
            if (!jsonVersion.IsString())
            {
                return std::nullopt;
            }
            const auto version = FromString<Version>(jsonVersion.GetString());
            if (!version.HasValue())
            {
                return std::nullopt;
            }

            return version.Value();
        };

        auto parseUuid = [&](const std::string& member) -> std::optional<Uuid>
        {
            if (!jsonDocument.HasMember(member.c_str()))
            {
                //warnings.push_back(ProjectFileWarningCode::MissingGlobalId);
                return Uuid{};
            }

            const auto& jsonUuid = jsonDocument[member.c_str()];
            if (!jsonUuid.IsString())
            {
                return std::nullopt;
            }
            const auto uuid = FromString<Uuid>(jsonUuid.GetString());
            if (!uuid.HasValue())
            {
                return std::nullopt;
            }

            return uuid.Value();
        };

        auto parseString = [&](const std::string& member) -> std::string
        {
            if (!jsonDocument.HasMember(member.c_str()))
            {
                return "";
            }

            const auto& jsonString = jsonDocument[member.c_str()];
            return std::string{ jsonString.GetString(), jsonString.GetStringLength() };
        };

        auto fileVersion = parseVersion("file_version");
        if (!fileVersion.has_value())
        {
            return Unexpected(ProjectFileErrorCode::InvalidFileVersion);
        }

        auto engineVersion = parseVersion("engine_version");
        if (!engineVersion.has_value())
        {
            return Unexpected(ProjectFileErrorCode::InvalidEngineVersion);
        }

        auto globalId = parseUuid("global_id");
        if (!globalId.has_value())
        {
            return Unexpected(ProjectFileErrorCode::InvalidGlobalId);
        }

        const auto description = parseString("description");

        return ProjectFile{
            .fileVersion = fileVersion.value(),
            .engineVersion = engineVersion.value(),
            .globalId = globalId.value(),
            .description = description
        };
    }

    ReadProjectFileResult ReadProjectFile(std::filesystem::path path)
    {
        std::ifstream file(path.c_str());
        if (!file.is_open())
        {
            return Unexpected(ProjectFileReadErrorResult{ OpenFileError{} });
        }

        return ReadProjectFile(file);
    }

    bool WriteProjectFile(
        const ProjectFile& projectFile,
        std::filesystem::path path)
    {
        std::ofstream file(path);
        if (!file.is_open())
        {
            return false;
        }

        rapidjson::Document jsonDocument;
        jsonDocument.SetObject();

        {
            const auto fileVersion = ToString(projectFile.fileVersion, false);

            jsonDocument.AddMember(
                "file_version", 
                rapidjson::Value{
                     fileVersion.c_str(),
                    static_cast<rapidjson::SizeType>(fileVersion.size()),
                    jsonDocument.GetAllocator()
                },
                jsonDocument.GetAllocator());
        } 
        {
            const auto engineVersion = ToString(projectFile.engineVersion, false);

            jsonDocument.AddMember(
                "engine_version",
                rapidjson::Value{
                    engineVersion.c_str(),
                    static_cast<rapidjson::SizeType>(engineVersion.size()),
                    jsonDocument.GetAllocator()
                },
                jsonDocument.GetAllocator());
        }
        {
            const auto globalId = ToString(projectFile.globalId);

            jsonDocument.AddMember(
                "global_id",
                rapidjson::Value{
                    globalId.c_str(),
                    static_cast<rapidjson::SizeType>(globalId.size()),
                    jsonDocument.GetAllocator()
                },
                jsonDocument.GetAllocator());
        }
        {
            jsonDocument.AddMember(
                "description",
                rapidjson::Value{
                    projectFile.description.c_str(),
                    static_cast<rapidjson::SizeType>(projectFile.description.size()),
                    jsonDocument.GetAllocator()
                },
                jsonDocument.GetAllocator());
        }
        
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter writer(buffer);

        jsonDocument.Accept(writer);
        file.write(buffer.GetString(), buffer.GetSize());

        return true;
    }

}
