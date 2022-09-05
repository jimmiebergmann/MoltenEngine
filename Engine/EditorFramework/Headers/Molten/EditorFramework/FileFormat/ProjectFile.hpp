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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_PROJECTFILE_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_PROJECTFILE_HPP

#include "Molten/EditorFramework/Build.hpp"
#include "Molten/FileFormat/FileFormatResult.hpp"
#include "Molten/FileFormat/JsonFormatResult.hpp"
#include "Molten/Utility/Uuid.hpp"
#include "Molten/System/Version.hpp"
#include <istream>
#include <filesystem>
#include <vector>
#include <string>

namespace Molten
{

    struct MOLTEN_EDITOR_FRAMEWORK_API ProjectFile
    {
        Version fileVersion;
        Version engineVersion;
        Uuid globalId;
        std::string description;
    };

    enum class ProjectFileErrorCode
    {
        InvalidFileVersion,
        InvalidEngineVersion,
        InvalidGlobalId
    };

    enum class ProjectFileWarningCode
    {
        MissingFileVersion,
        MissingEngineVersion,
        MissingGlobalId
    };

    using ProjectFileReadErrorResult = std::variant<
        ProjectFileErrorCode,
        JsonParseError,
        OpenFileError>;

    using ProjectFileReadResult = FileFormatResult<
        ProjectFile,
        ProjectFileReadErrorResult,
        ProjectFileWarningCode>;

    MOLTEN_EDITOR_FRAMEWORK_API ProjectFileReadResult ReadProjectFile(std::istream& istream);
    MOLTEN_EDITOR_FRAMEWORK_API ProjectFileReadResult ReadProjectFile(std::filesystem::path path);

    MOLTEN_EDITOR_FRAMEWORK_API bool WriteProjectFile(
        const ProjectFile& projectFile,
        std::filesystem::path path);

}

#endif