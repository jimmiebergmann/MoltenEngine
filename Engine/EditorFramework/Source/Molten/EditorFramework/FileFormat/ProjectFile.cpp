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

#include "Molten/EditorFramework/FileFormat/ProjectFile.hpp"
#include "Molten/FileFormat/RapidJsonFormatResult.hpp"
#include "ThirdParty/rapidjson/include/rapidjson/document.h"
#include "ThirdParty/rapidjson/include/rapidjson/istreamwrapper.h"
#include "ThirdParty/rapidjson/include/rapidjson/schema.h"
#include "ThirdParty/rapidjson/include/rapidjson/prettywriter.h"
#include <array>
#include <string>
#include <fstream>

namespace Molten
{

    ProjectFileReadResult ReadProjectFile(std::istream& stream)
    {
        rapidjson::IStreamWrapper fileWrapper(stream);

        rapidjson::Document jsonDocumentNonConst;
        rapidjson::ParseResult parseResult = jsonDocumentNonConst.ParseStream(fileWrapper);
        if (parseResult.IsError())
        {
            return ProjectFileReadResult::CreateError(CreateJsonErrorCode(parseResult.Offset(), parseResult.Code()));
        }

        // We could use a schema file to validate, but it is probably faster to just check the values manually...
        const auto& jsonDocument = jsonDocumentNonConst;

        std::vector<ProjectFileWarningCode> warnings;

        // File version.
        auto getFileVersion = [&]() -> Result<Version, ProjectFileErrorCode>
        {
            using ResultType = Result<Version, ProjectFileErrorCode>;

            if (!jsonDocument.HasMember("file_version"))
            {
                warnings.push_back(ProjectFileWarningCode::MissingFileVersion);
                return ResultType::CreateSuccess(Version{});
            }

            const auto& jsonFileVersion = jsonDocument["file_version"];
            if (!jsonFileVersion.IsString())
            {
                return ResultType::CreateError(ProjectFileErrorCode::InvalidFileVersion);
            }
            const auto fileVersion = FromString<Version>(jsonFileVersion.GetString());
            if (!fileVersion.IsValid())
            {
                return ResultType::CreateError(ProjectFileErrorCode::InvalidFileVersion);
            }

            return ResultType::CreateSuccess(fileVersion.Value());
        };

        auto fileVersion = getFileVersion();
        if (!fileVersion.IsValid())
        {
            return ProjectFileReadResult::CreateError(fileVersion.Error());
        }

        // Engine version.
        auto getEngineVersion = [&]() -> Result<Version, ProjectFileErrorCode>
        {
            using ResultType = Result<Version, ProjectFileErrorCode>;

            if (!jsonDocument.HasMember("engine_version"))
            {
                warnings.push_back(ProjectFileWarningCode::MissingEngineVersion);
                return ResultType::CreateSuccess(Version{});
            }

            const auto& jsonEngineVersion = jsonDocument["engine_version"];
            if (!jsonEngineVersion.IsString())
            {
                return ResultType::CreateError(ProjectFileErrorCode::InvalidEngineVersion);
            }
            const auto engineVersion = FromString<Version>(jsonEngineVersion.GetString());
            if (!engineVersion.IsValid())
            {
                return ResultType::CreateError(ProjectFileErrorCode::InvalidEngineVersion);
            }

            return ResultType::CreateSuccess(engineVersion.Value());
        };

        auto engineVersion = getEngineVersion();
        if (!engineVersion.IsValid())
        {
            return ProjectFileReadResult::CreateError(engineVersion.Error());
        }

        // Global id.
        auto getGlobalId = [&]() -> Result<Uuid, ProjectFileErrorCode>
        {
            using ResultType = Result<Uuid, ProjectFileErrorCode>;

            if (!jsonDocument.HasMember("global_id"))
            {
                warnings.push_back(ProjectFileWarningCode::MissingGlobalId);
                return ResultType::CreateSuccess(Uuid{});
            }

            const auto& jsonGlobalId = jsonDocument["global_id"];
            if (!jsonGlobalId.IsString())
            {
                return ResultType::CreateError(ProjectFileErrorCode::InvalidGlobalId);
            }
            const auto globalId = FromString<Uuid>(jsonGlobalId.GetString());
            if (!globalId.IsValid())
            {
                return ResultType::CreateError(ProjectFileErrorCode::InvalidGlobalId);
            }

            return ResultType::CreateSuccess(globalId.Value());
        };

        auto globalId = getGlobalId();
        if (!globalId.IsValid())
        {
            return ProjectFileReadResult::CreateError(globalId.Error());
        }

        // Description
        auto getDescription = [&]() -> std::string
        {
            if (!jsonDocument.HasMember("description"))
            {
                return "";
            }

            const auto& jsonDescription = jsonDocument["description"];
            return std::string{ jsonDescription.GetString(), jsonDescription.GetStringLength() };
        };

        const auto description = getDescription();

        return ProjectFileReadResult::CreateSuccess({
            ProjectFileReadResult::Type{
                ProjectFile{
                    fileVersion.Value(),
                    engineVersion.Value(),
                    globalId.Value(),
                    description
                },
                std::move(warnings)
            }
        });
    }

    ProjectFileReadResult ReadProjectFile(std::filesystem::path path)
    {
        std::ifstream file(path.c_str());
        if (!file.is_open())
        {
            return ProjectFileReadResult::CreateError(OpenFileError{});
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
            const auto fileVersion = ToString(projectFile.fileVersion);

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
            const auto engineVersion = ToString(projectFile.engineVersion);

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
