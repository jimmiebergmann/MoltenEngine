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

#include "Molten/EditorFramework/Project.hpp"
#include "Molten/EditorFramework/FileFormat/ProjectFile.hpp"

namespace Molten::EditorFramework
{

    /** Static functions. */
    using CreateProjectFolderResult = Expected<void, CreateProjectResult>;

    static CreateProjectFolderResult CreateProjectDirectory(
        const std::filesystem::path& directory)
    {
        if (directory.has_extension())
        {
            return Unexpected(CreateProjectResult::InvalidDirectory);
        }

        auto name = directory.filename().string();

        if (!Project::ValidateName(name))
        {
            return Unexpected(CreateProjectResult::InvalidName);
        }

        std::error_code ec{};
        if (std::filesystem::exists(directory, ec))
        {
            return Unexpected(CreateProjectResult::DirectoryAlreadyExists);
        }
        if (ec != std::error_code{})
        {
            return Unexpected(CreateProjectResult::CannotCreateDirectory);
        }

        if (!std::filesystem::create_directory(directory, ec))
        {
            return Unexpected(CreateProjectResult::CannotCreateDirectory);
        }

        return {};
    }

    static bool CreateProjectFile(const std::filesystem::path& directory)
    {
        std::random_device randomDevice;
        std::mt19937 randomEngine(randomDevice());

        auto projectFile = ProjectFile{ };
        projectFile.fileVersion = Version{ 0, 0, 1 };
        projectFile.engineVersion = Version{ 0, 0, 1 };
        projectFile.globalId = Uuid::GenerateVersion4(randomEngine);
        projectFile.description = "";

        const auto filename = directory.filename().replace_extension(".mproj");
        const auto filePath = directory / filename;
        return WriteProjectFile(projectFile, filePath);
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

    CreateProjectResult Project::Create(const std::filesystem::path& directory)
    {
        auto directoryCreationResult = CreateProjectDirectory(directory);
        if (!directoryCreationResult)
        {
            return directoryCreationResult.Error();
        }

        if (!CreateProjectFile(directory))
        {
            return CreateProjectResult::CannotCreateProjectFile;
        }

        if (const auto result = CreateProjectSubDirectories(directory); result != CreateProjectResult::Success)
        {
            return result;
        }

        return CreateProjectResult::Success;
    }

    OpenProjectResult Project::Open(const std::filesystem::path& filename)
    {
        Project project;

        auto absoluteFilename = std::filesystem::absolute(filename);
        if (!absoluteFilename.has_extension())
        {
            absoluteFilename /= absoluteFilename.filename().replace_extension(".mproj");
        }

        project.m_projectFile.open(absoluteFilename);
        if (!project.m_projectFile.is_open())
        {
            return Unexpected(OpenProjectResultCode::UnknownProjectFile);
        }

        project.m_projectDirectory = absoluteFilename.parent_path();

        return project;
    }

    std::filesystem::path Project::GetDirectoryPath() const
    {
        return m_projectDirectory;
    }

}
