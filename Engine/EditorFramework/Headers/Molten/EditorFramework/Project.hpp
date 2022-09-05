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

#ifndef MOLTEN_EDITOR_FRAMEWORK_PROJECT_HPP
#define MOLTEN_EDITOR_FRAMEWORK_PROJECT_HPP

#include "Molten/EditorFramework/Build.hpp"
#include "Molten/EditorFramework/Manager/AssetManager.hpp"
#include "Molten/System/Result.hpp"
#include <string>
#include <filesystem>
#include <fstream>


namespace Molten
{

    class Project;

    enum class CreateProjectResult
    {
        Success,
        InvalidName, ///< Project name contains valid characters.
        InvalidDirectory, ///< Directory does not exists or is not a directory.
        CannotCreateDirectory, ///< Failed to create project directory.
        AlreadyExists, ///< Project name already exists in path folder.
        UnknownTemplate, ///< Cannot find template by name.
        InvalidTemplate, ///< Template is corrupt.
        CannotCreateProjectFile ///< Failed to create project file in directory.
    };

    enum class OpenProjectResultCode
    {
        Success,
        UnknownProjectFile, ///< Project file path does not exists.
        InvalidProjectFile, ///< Project file is corrupt.
    };

    using OpenProjectResult = Result<Project, OpenProjectResultCode>;

    class MOLTEN_EDITOR_FRAMEWORK_API Project
    {

    public:

        Project();

        static bool ValidateName(const std::string& name);

        static CreateProjectResult Create(
            const std::filesystem::path& directory,
            const std::string& name,
            const std::string& templateName = "");

        static OpenProjectResult Open(const std::filesystem::path& filename);

        std::filesystem::path GetDirectoryPath() const;

    private:

        std::filesystem::path m_projectDirectory;
        std::fstream m_projectFile;
        AssetManager m_assetManager;

    };

}

#endif