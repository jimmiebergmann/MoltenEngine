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

#include "Molten/EditorFramework/Manager/AssetManager.hpp"
#include "Molten/EditorFramework/Project.hpp"

namespace Molten
{

    AssetManager::AssetManager(Project& project) :
        m_project(project)
    {}

    std::filesystem::path AssetManager::GetDirectoryPath() const
    {
        return m_project.GetDirectoryPath() / "Asset";
    }

    bool AssetManager::DeleteFile(const std::filesystem::path& path)
    {
        auto assetPath = GetAssetFilePath(path);
        if (!assetPath.has_value())
        {
            return false;
        }

        auto ec = std::error_code{};
        auto entry = std::filesystem::directory_entry{ assetPath.value() };
        if (!entry.is_regular_file(ec) || ec != std::error_code{})
        {
            return false;
        }

        if (!std::filesystem::remove(entry, ec) || ec != std::error_code{})
        {
            return false;
        }

        return true;
    }

    bool AssetManager::RenameFile(const std::filesystem::path& fromPath, const std::filesystem::path& toPath)
    {
        auto fromAssetPath = GetAssetFilePath(fromPath);
        if (!fromAssetPath.has_value())
        {
            return false;
        }

        auto ec = std::error_code{};
        auto fromEntry = std::filesystem::directory_entry{ fromAssetPath.value() };    
        if (!fromEntry.is_regular_file(ec) || ec != std::error_code{})
        {
            return false;
        }

        auto toAssetPath = GetAssetFilePath(toPath);
        if (!toAssetPath.has_value())
        {
            return false;
        }

        if (std::filesystem::exists(toAssetPath.value(), ec) || ec != std::error_code{})
        {
            return false;
        }

        
        std::filesystem::rename(fromEntry, toAssetPath.value(), ec);
        if (ec != std::error_code{})
        {
            return false;
        }

        return true;
    }

    bool AssetManager::MakeDirectory(const std::filesystem::path& path)
    {
        return false;
    }

    bool AssetManager::DeleteDirectory(const std::filesystem::path& path)
    {
        return false;
    }

    bool AssetManager::RenameDirectory(const std::filesystem::path& from, const std::filesystem::path& to)
    {
        return false;
    }

    std::filesystem::directory_iterator AssetManager::GetDirectoryIterator() const
    {
        return std::filesystem::directory_iterator{ GetDirectoryPath() };
    }

    std::filesystem::recursive_directory_iterator AssetManager::GetRecursiveDirectoryIterator() const
    {
        return std::filesystem::recursive_directory_iterator{ GetDirectoryPath() };
    }

    std::optional<std::filesystem::path> AssetManager::GetAssetPath(const std::filesystem::path& relativePath)
    {
        auto rootDirectory = GetDirectoryPath();
        auto path = std::filesystem::weakly_canonical(rootDirectory / relativePath);

        if (path.string().find(rootDirectory.string()) != 0)
        {
            return {};
        }

        return path;    
    }

    std::optional<std::filesystem::path> AssetManager::AssetManager::GetAssetFilePath(
        const std::filesystem::path& relativePath)
    {
        auto optionalPath = GetAssetPath(relativePath);
        if (!optionalPath.has_value())
        {
            return {};
        }

        auto path = optionalPath.value();

        if (path.has_extension())
        {
            if (path.extension() != ".asset")
            {
                return {};
            }
        }
        else
        {
            path += ".asset";
        }

        return path;
    }
    
}