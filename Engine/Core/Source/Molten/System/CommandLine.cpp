/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#include "Molten/System/CommandLine.hpp"
#include "Molten/System/Exception.hpp"
namespace Molten
{

    // Command line argument implementations.
    CliArgument::CliArgument(
        const Type type,
        const bool isOptional,
        const SetValueFunction& setValue,
        const ClearFunction& clear,
        std::vector<std::string>&& names,
        const std::string& description
    ) :
        SetValue(setValue),
        Clear(clear),    
        m_type(type),
        m_isOptional(isOptional),
        m_names(std::move(names)),
        m_description(description)
    {}

    [[nodiscard]] CliArgument::Type CliArgument::GetType() const
    {
        return m_type;
    }

    [[nodiscard]] bool CliArgument::IsOptional() const
    {
        return m_isOptional;
    }

    [[nodiscard]] const std::vector<std::string>& CliArgument::GetNames() const
    {
        return m_names;
    }

    [[nodiscard]] const std::string& CliArgument::GetDescription() const
    {
        return m_description;
    }


    // Command line flag argument implementations.
    CliFlag::CliFlag(
        std::vector<char>&& names,
        bool& value,
        const std::string& description
    ) :
        CliArgument(
            Type::Flag,
            true,
            CreateSetValueFunc(value),
            CreateClearFunc(value),
            CharToStringVector(names),
            description)
    {}

    [[nodiscard]] CliArgument::SetValueFunction CliFlag::CreateSetValueFunc(bool& value)
    {
        return [&value](const std::string_view) -> bool
        {
            value = true;
            return true;
        };
    }

    [[nodiscard]] CliArgument::ClearFunction CliFlag::CreateClearFunc(bool& value)
    {
        return [&value]()
        {
            value = false;
        };
    }

    [[nodiscard]] std::vector<std::string> CliFlag::CharToStringVector(const std::vector<char>& charVector)
    {
        std::vector<std::string> newVector;
        for (const auto& charValue : charVector)
        {
            newVector.push_back({ charValue });
        }
        return newVector;
    }


    CliParser::CliParser(std::initializer_list<CliArgument>&& args)
    {
        m_clearFuncs.reserve(args.size());

        for (const auto& arg : args)
        {
            auto new_arg = std::make_shared<CliArgument>(arg);
            m_clearFuncs.push_back(&new_arg->Clear);
            if (!arg.IsOptional())
            {
                m_requiredArgs.insert(new_arg);
            }

            for (const auto& name : new_arg->GetNames())
            {
                if (m_mappedArgs.find(name) != m_mappedArgs.end())
                {
                    throw Exception("Argument name \"" + name + "\" has already been mapped.");
                }

                m_mappedArgs.insert({ name, new_arg });
            }
        }
    }

    bool CliParser::Parse(int argc, char** argv, const bool skip_first_argv) const
    {
        for(auto& clearFunc : m_clearFuncs)
        {
            (*clearFunc)();
        }

        std::string_view emptyView;

        ArgumentPointer currentArgument = nullptr;
        for(int i = (skip_first_argv ? 1 : 0); i < argc; i++)
        {
            auto argument = std::string_view{ argv[i], strlen(argv[i]) };

            if (argument.size() >= 2 && argument[0] == '-')
            {
                if(argument[1] == '-')
                {
                    // New value/list key
                    argument = std::string_view{ argument.data() + 2, argument.size() - 2 };
                    auto it = m_mappedArgs.find(std::string{ argument });
                    if (it == m_mappedArgs.end())
                    {
                        return false;
                    }

                    currentArgument = it->second;
                    continue;
                }
                else
                {
                    // Flags
                    argument = std::string_view{ argument.data() + 1, argument.size() - 1 };
                    for(const auto& flag : argument)
                    {
                        auto it = m_mappedArgs.find(std::string{ flag });
                        if(it == m_mappedArgs.end())
                        {
                            return false;
                        }
                        if(!it->second->SetValue(emptyView))
                        {
                            return false;
                        }
                    }

                    currentArgument = nullptr;
                    continue;
                }
            }

            if(!currentArgument)
            {
                return false;
            }

            // Value/list
            if(!currentArgument->SetValue(argument))
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] std::string CliParser::GetHelp() const
    {
        return "";
    }




}