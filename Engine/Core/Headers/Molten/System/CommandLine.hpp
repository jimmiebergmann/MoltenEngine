/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_SYSTEM_COMMANDLINE_HPP
#define MOLTEN_CORE_SYSTEM_COMMANDLINE_HPP

#include "Molten/Core.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <memory>
#include <optional>
#include <initializer_list>

namespace Molten
{

    /** Base class for command line arguments. */
    class MOLTEN_API CliArgument
    {

    public:

        enum class Type : uint8_t
        {
            Flag,
            Value,
            List
        };


        using SetValueFunction = std::function<bool(const std::string_view)>;
        using ClearFunction = std::function<void()>;

        CliArgument(
            const Type type,
            const bool isOptional,
            const SetValueFunction& setValue,
            const ClearFunction& clear,
            std::vector<std::string>&& names,
            const std::string& description = "");

        SetValueFunction SetValue;
        ClearFunction Clear;

        [[nodiscard]] Type GetType() const;

        [[nodiscard]] bool IsOptional() const;

        [[nodiscard]] const std::vector<std::string>& GetNames() const;

        [[nodiscard]] const std::string& GetDescription() const;

    private:

        const Type m_type;
        const bool m_isOptional;
        const std::vector<std::string> m_names;
        const std::string m_description;

    };

    /** Single argument value, optional or non-optional. */
    class CliValue : public CliArgument
    {

    public:

        template<typename TType>
        CliValue(
            std::vector<std::string>&& names,
            TType& value,
            const std::string& description = "");

        template<typename TType>
        CliValue(
            std::vector<std::string>&& names,
            std::optional<TType>& value,
            const std::string& description = "");

    private:

        template<typename TType>
        [[nodiscard]] static SetValueFunction CreateSetValueFunc(TType& value);

        template<typename TType>
        [[nodiscard]] static SetValueFunction CreateOptionalSetValueFunc(std::optional<TType>& value);

        template<typename TType>
        [[nodiscard]] static ClearFunction CreateClearFunc(TType& value);

        template<typename TType>
        [[nodiscard]] static ClearFunction CreateOptionalClearFunc(std::optional<TType>& value);

    };

    /** Single argument flag. */
    class MOLTEN_API CliFlag : public CliArgument
    {

    public:

        CliFlag(
            std::vector<char>&& names,
            bool& value,
            const std::string& description = "");

    private:

        [[nodiscard]] static SetValueFunction CreateSetValueFunc(bool& value);

        [[nodiscard]] static ClearFunction CreateClearFunc(bool& value);

        [[nodiscard]] static std::vector<std::string> CharToStringVector(const std::vector<char>& charVector);

    };

    /** List argument value, optional or non-optional. */
    class CliList : public CliArgument
    {

    public:

        template<typename TContainer>
        CliList(
            std::vector<std::string>&& names,
            TContainer& values,
            const std::string& description = "");

        template<typename TContainer>
        CliList(
            std::vector<std::string>&& names,
            std::optional<TContainer>& values,
            const std::string& description = "");

    private:

        template<typename TContainer>
        [[nodiscard]] static SetValueFunction CreateSetValuesFunc(TContainer& value);

        template<typename TContainer>
        [[nodiscard]] static SetValueFunction CreateOptionalSetValuesFunc(std::optional<TContainer>& value);

        template<typename TContainer>
        [[nodiscard]] static ClearFunction CreateClearFunc(TContainer& value);

        template<typename TContainer>
        [[nodiscard]] static ClearFunction CreateOptionalClearFunc(std::optional<TContainer>& value);

    };

    class MOLTEN_API CliParser
    {

    public:

        CliParser(std::initializer_list<CliArgument>&& args);

        bool Parse(const int argc, char** argv, const bool skip_first_argv = true) const;
        bool Parse(const int argc, const char** argv, const bool skip_first_argv = true) const;

        [[nodiscard]] std::string GetHelp() const;

    private:

        using ArgumentPointer = std::shared_ptr<CliArgument>;

        [[nodiscard]] bool ParseSingleArgument(std::string_view argument, ArgumentPointer& currentArgument) const;


        std::vector<CliArgument::ClearFunction*> m_clearFuncs;
        std::map<std::string, ArgumentPointer> m_mappedArgs;
        std::set<ArgumentPointer> m_requiredArgs;

    };



}

#include "Molten/System/CommandLine.inl"

#endif
