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

#include "Molten/EditorFramework/Project.hpp"
#include "Molten/EditorFramework/FileFormat/ProjectFile.hpp"

namespace Molten
{

    /** Static functions. */
    using CreateProjectFolderResult = Result<std::filesystem::path, CreateProjectResult>;

    static CreateProjectFolderResult CreateProjectDirectory(
        const std::filesystem::path& path,
        const std::string& name)
    {
        if (!Project::ValidateName(name))
        {
            return CreateProjectFolderResult::CreateError(CreateProjectResult::InvalidName);
        }

        std::error_code ec{};
        if (!std::filesystem::is_directory(path, ec))
        {
            return CreateProjectFolderResult::CreateError(CreateProjectResult::InvalidDirectory);
        }

        const auto projectDirectory = path / name;
        if (std::filesystem::exists(projectDirectory, ec))
        {
            return CreateProjectFolderResult::CreateError(CreateProjectResult::AlreadyExists);
        }
        if (ec != std::error_code{})
        {
            return CreateProjectFolderResult::CreateError(CreateProjectResult::CannotCreateDirectory);
        }

        if (!std::filesystem::create_directory(projectDirectory, ec))
        {
            return CreateProjectFolderResult::CreateError(CreateProjectResult::CannotCreateDirectory);
        }

        return CreateProjectFolderResult::CreateSuccess(projectDirectory);
    }

    static bool CreateProjectFile(
        const std::filesystem::path& directory,
        const std::string& name)
    {
        auto projectFile = ProjectFile{ };
        projectFile.fileVersion = Version{ 0, 0, 1 };
        projectFile.engineVersion = Version{ 0, 0, 1 };
        projectFile.globalId = Uuid{ };
        projectFile.description = "";

        const auto path = directory / (name + ".mproj");
        return WriteProjectFile(projectFile, path);
    }

    static CreateProjectResult CreateProjectSubDirectories(const std::filesystem::path& projectDirectory)
    {
        std::error_code ec{};
        if (!std::filesystem::create_directory(projectDirectory / "Assets", ec))
        {
            return CreateProjectResult::CannotCreateDirectory;
        }

        return CreateProjectResult::Success;
    }

    /** Project implementations. */
    Project::Project() :
        m_assetManager(*this)
    {}


    /** Free project functions. */
    bool Project::ValidateName(const std::string& name)
    {
        return !name.empty() && name.find_first_of("/\\.* \t\n\r") == std::string::npos;
    }

    CreateProjectResult Project::Create(
        const std::filesystem::path& directory,
        const std::string& name,
        const std::string& templateName)
    {
        auto directoryCreationResult = CreateProjectDirectory(directory, name);
        if (!directoryCreationResult)
        {
            return directoryCreationResult.Error();
        }

        const auto projectFolder = std::move(directoryCreationResult.Value());

        if (!CreateProjectFile(projectFolder, name))
        {
            return CreateProjectResult::CannotCreateProjectFile;
        }

        if (const auto result = CreateProjectSubDirectories(projectFolder); result != CreateProjectResult::Success)
        {
            return result;
        }

        return CreateProjectResult::Success;
    }

    OpenProjectResult Project::Open(const std::filesystem::path& filename)
    {
        Project project;

        const auto projectFilePath = std::filesystem::absolute(filename);

        project.m_projectFile.open(projectFilePath);
        if (!project.m_projectFile.is_open())
        {
            return OpenProjectResult::CreateError(OpenProjectResultCode::UnknownProjectFile);
        }

        project.m_projectDirectory = projectFilePath.parent_path();

        return OpenProjectResult::CreateSuccess(std::move(project));
    }

    std::filesystem::path Project::GetDirectoryPath() const
    {
        return m_projectDirectory;
    }

}
