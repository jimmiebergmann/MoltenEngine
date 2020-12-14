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

#include <sstream>

namespace Molten
{

    // Command line value implementations.
    template<typename TType>
    CliValue::CliValue(
        std::vector<std::string>&& names,
        TType& value,
        const std::string& description
    ) :
        CliArgument(
            Type::Value,
            false,
            CreateSetValueFunc<TType>(value),
            CreateClearFunc<TType>(value),
            std::move(names),
            description)
    {}

    template<typename TType>
    CliValue::CliValue(
        std::vector<std::string>&& names,
        std::optional<TType>& value,
        const std::string& description
    ) :
        CliArgument(
            Type::Value,
            true,
            CreateOptionalSetValueFunc<TType>(value),
            CreateOptionalClearFunc<TType>(value),
            std::move(names),
            description)
    {}


    template<typename TType>
    [[nodiscard]] CliArgument::SetValueFunction CliValue::CreateSetValueFunc(TType& value)
    {
        return [&value](const std::string_view strValue) -> bool
        {
            std::stringstream ss;
            ss << strValue;
            ss >> value;
            return ss.eof();
        };
    }

    template<typename TType>
    [[nodiscard]] CliArgument::SetValueFunction CliValue::CreateOptionalSetValueFunc(std::optional<TType>& value)
    {
        return [&value](const std::string_view strValue) -> bool
        {
            TType tmpValue{};
            std::stringstream ss;
            ss << strValue;
            ss >> tmpValue;
            if (!ss.eof())
            {
                return false;
            }
            if (value.has_value())
            {
                value = tmpValue;
            }
            else
            {
                value = std::make_optional<TType>(tmpValue);
            }

            return true;
        };
    }

    template<typename TType>
    [[nodiscard]] CliArgument::ClearFunction CliValue::CreateClearFunc(TType& value)
    {
        return [&value]()
        {
            value = {};
        };
    }

    template<typename TType>
    [[nodiscard]] CliArgument::ClearFunction CliValue::CreateOptionalClearFunc(std::optional<TType>& value)
    {
        return [&value]()
        {
            value.reset();
        };
    }



    // Command line list implementations.
    template<typename TContainer>
    CliList::CliList(
        std::vector<std::string>&& names,
        TContainer& values,
        const std::string& description
    ) :
        CliArgument(
            Type::List,
            false,
            CreateSetValuesFunc(values),
            CreateClearFunc(values),
            std::move(names),
            description)
    { }

    template<typename TContainer>
    CliList::CliList(
        std::vector<std::string>&& names,
        std::optional<TContainer>& values,
        const std::string& description
    ) :
        CliArgument(
            Type::List,
            true,
            CreateOptionalSetValuesFunc(values),
            CreateOptionalClearFunc(values),
            std::move(names),
            description)
    { }

    template<typename TContainer>
    [[nodiscard]] CliArgument::SetValueFunction CliList::CreateSetValuesFunc(TContainer& value)
    {
        return [&value](const std::string_view strValue) -> bool
        {
            typename TContainer::value_type tmpValue{};
            std::stringstream ss;
            ss << strValue;
            ss >> tmpValue;

            if (!ss.eof())
            {
                return false;
            }
            value.push_back(tmpValue);
            return true;
        };
    }

    template<typename TContainer>
    [[nodiscard]] CliArgument::SetValueFunction CliList::CreateOptionalSetValuesFunc(std::optional<TContainer>& value)
    {
        return [&value](const std::string_view strValue) -> bool
        {
            typename TContainer::value_type tmpValue{};
            std::stringstream ss;
            ss << strValue;
            ss >> tmpValue;

            if (!ss.eof())
            {
                return false;
            }

            if (!value.has_value())
            {
                value = std::make_optional<TContainer>({});
            }

            value->push_back(tmpValue);
            return true;
        };
    }

    template<typename TContainer>
    [[nodiscard]] CliArgument::ClearFunction CliList::CreateClearFunc(TContainer& value)
    {
        return [&value]()
        {
            value.clear();
        };
    }

    template<typename TContainer>
    [[nodiscard]] CliArgument::ClearFunction CliList::CreateOptionalClearFunc(std::optional<TContainer>& value)
    {
        return [&value]()
        {
            value.reset();
        };
    }


}