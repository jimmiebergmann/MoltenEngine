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

#include "Molten/FileFormat/Project/ProjectFile.hpp"
#include "ThirdParty/rapidjson/include/rapidjson/document.h"
#include "ThirdParty/rapidjson/include/rapidjson/istreamwrapper.h"
#include "ThirdParty/rapidjson/include/rapidjson/schema.h"
#include <array>
#include <string>
#include <fstream>


namespace Molten
{

    ProjectFileReadResult ReadProjectFile(std::istream& istream)
    {
        rapidjson::IStreamWrapper fileWrapper(istream);

        rapidjson::Document jsonDocumentNonConst;
        rapidjson::ParseResult parseResult = jsonDocumentNonConst.ParseStream(fileWrapper);
        if (parseResult.IsError())
        {
            return ProjectFileReadResult::CreateError(JsonParseError{ parseResult.Offset(), parseResult.Code() });
        }

        // We could use a schema file to validate, but it is probably faster to just check the values manually...
        const auto& jsonDocument = jsonDocumentNonConst;

        std::vector<ProjectFileWarningCodes> warnings;

        // File version.
        auto getFileVersion = [&]() -> Result<Version, ProjectFileErrorCodes>
        {
            using ResultType = Result<Version, ProjectFileErrorCodes>;

            if (!jsonDocument.HasMember("file_version"))
            {
                warnings.push_back(ProjectFileWarningCodes::MissingFileVersion);
                return ResultType::CreateSuccess(Version{});
            }

            const auto& jsonFileVersion = jsonDocument["file_version"];
            if (!jsonFileVersion.IsString())
            {
                return ResultType::CreateError(ProjectFileErrorCodes::InvalidFileVersion);
            }
            const auto fileVersion = FromString<Version>(jsonFileVersion.GetString());
            if (!fileVersion.IsValid())
            {
                return ResultType::CreateError(ProjectFileErrorCodes::InvalidFileVersion);
            }

            return ResultType::CreateSuccess(fileVersion.Value());
        };

        auto fileVersion = getFileVersion();
        if (!fileVersion.IsValid())
        {
            return ProjectFileReadResult::CreateError(fileVersion.Error());
        }

        // Engine version.
        auto getEngineVersion = [&]() -> Result<Version, ProjectFileErrorCodes>
        {
            using ResultType = Result<Version, ProjectFileErrorCodes>;

            if (!jsonDocument.HasMember("engine_version"))
            {
                warnings.push_back(ProjectFileWarningCodes::MissingEngineVersion);
                return ResultType::CreateSuccess(Version{});
            }

            const auto& jsonEngineVersion = jsonDocument["engine_version"];
            if (!jsonEngineVersion.IsString())
            {
                return ResultType::CreateError(ProjectFileErrorCodes::InvalidEngineVersion);
            }
            const auto engineVersion = FromString<Version>(jsonEngineVersion.GetString());
            if (!engineVersion.IsValid())
            {
                return ResultType::CreateError(ProjectFileErrorCodes::InvalidEngineVersion);
            }

            return ResultType::CreateSuccess(engineVersion.Value());
        };

        auto engineVersion = getEngineVersion();
        if (!engineVersion.IsValid())
        {
            return ProjectFileReadResult::CreateError(engineVersion.Error());
        }

        // Global id.
        auto getGlobalId = [&]() -> Result<Uuid, ProjectFileErrorCodes>
        {
            using ResultType = Result<Uuid, ProjectFileErrorCodes>;

            if (!jsonDocument.HasMember("global_id"))
            {
                warnings.push_back(ProjectFileWarningCodes::MissingGlobalId);
                return ResultType::CreateSuccess(Uuid{});
            }

            const auto& jsonGlobalId = jsonDocument["global_id"];
            if (!jsonGlobalId.IsString())
            {
                return ResultType::CreateError(ProjectFileErrorCodes::InvalidGlobalId);
            }
            const auto globalId = FromString<Uuid>(jsonGlobalId.GetString());
            if (!globalId.IsValid())
            {
                return ResultType::CreateError(ProjectFileErrorCodes::InvalidGlobalId);
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
        return ReadProjectFile(file);
    }

}
