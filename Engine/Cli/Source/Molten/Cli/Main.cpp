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
#include "ThirdParty/cppli/include/cppli/cppli.hpp"
#include <functional>
#include <filesystem>
#include <iostream>

static int CreateProject(cppli::context& context)
{
    auto name = std::string{};
    auto templateName = std::optional<std::string>{};

    const auto options
        = cppli::option<std::string>{ name, { "name" } }
        | cppli::option<std::optional<std::string>>{ templateName, { "--template" } };

    if (const auto result = options.parse(context); result == cppli::parse_codes::successful_help)
    {
        return 0;
    }
    else if (result != cppli::parse_codes::successful)
    {
        return result;
    }

    switch (const auto result = Molten::Project::Create(std::filesystem::current_path(), name, templateName.value_or("")); result)
    {
    case Molten::CreateProjectResult::Success: return 0;
    case Molten::CreateProjectResult::InvalidName: std::cerr << "Project name is invalid." << std::endl; break;
    case Molten::CreateProjectResult::InvalidDirectory: std::cerr << "Project directory is invalid." << std::endl; break;
    case Molten::CreateProjectResult::CannotCreateDirectory: std::cerr << "Cannot create project directory." << std::endl; break;
    case Molten::CreateProjectResult::AlreadyExists: std::cerr << "Project name already exsits in directory." << std::endl; break;
    case Molten::CreateProjectResult::UnknownTemplate: std::cerr << "Invalid template name." << std::endl; break;
    case Molten::CreateProjectResult::InvalidTemplate: std::cerr << "Corrupt template." << std::endl; break;
    case Molten::CreateProjectResult::CannotCreateProjectFile: std::cerr << "Cannot create project file." << std::endl; break;
    }

    return 0;
}

static int OpenProject(cppli::context&/*int argc, char** argv*/)
{
    return 0;

    /*auto openProject = [&]()
    {
        if (argc < 1)
        {
            auto current_path = std::filesystem::current_path();
            auto path = (current_path / current_path.stem()).replace_extension("uproj");
            return  Molten::Project::Open(path);
        }

        return  Molten::Project::Open(argv[0]);
    };

    auto result = openProject();
    if (!result)
    {
        return 1;
    }

    return 0;*/
}

int main(int argc, char** argv)
{
    const auto commands
        = cppli::command{ { "new" }, "", &CreateProject }
        | cppli::command{ { "open" }, "", &OpenProject };

    auto context 
        = cppli::context{ argc, argv }
        | cppli::default_error{}
        | cppli::default_help{};

   return commands.parse(context);
}