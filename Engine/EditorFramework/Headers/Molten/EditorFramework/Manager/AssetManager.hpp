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

#ifndef MOLTEN_EDITOR_FRAMEWORK_MANAGER_ASSETMANAGER_HPP
#define MOLTEN_EDITOR_FRAMEWORK_MANAGER_ASSETMANAGER_HPP

#include "Molten/EditorFramework/Build.hpp"
#include <filesystem>
#include <optional>

namespace Molten
{

    class Project;

    /** Manager class responsible for managing a project's asset folder.
    *   All paths are relative to the project's asset folder. 
    */
    class MOLTEN_EDITOR_FRAMEWORK_API AssetManager
    {
        
    public:

        AssetManager(Project& project);

        std::filesystem::path GetDirectoryPath() const;

        bool DeleteFile(const std::filesystem::path& path);
        bool RenameFile(const std::filesystem::path& from, const std::filesystem::path& to);

        bool MakeDirectory(const std::filesystem::path& path);
        bool DeleteDirectory(const std::filesystem::path& path);
        bool RenameDirectory(const std::filesystem::path& from, const std::filesystem::path& to);

        std::filesystem::directory_iterator GetDirectoryIterator() const;
        std::filesystem::recursive_directory_iterator GetRecursiveDirectoryIterator() const;

    private:

        /** Returns path without any extension. */
        std::optional<std::filesystem::path> GetAssetPath(
            const std::filesystem::path& relativePath);

        /** Returns path with ".asset" extention. */
        std::optional<std::filesystem::path> GetAssetFilePath(
            const std::filesystem::path& relativePath);

        Project& m_project;

    };

}

#endif