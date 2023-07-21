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

namespace Molten::EditorFramework::BinaryFile
{
    template<DataType VdataType>
    struct IsDataType
    {
        static constexpr auto dataType = VdataType;
    };

    // Property traits specializations.
    template<> struct PropertyTraits<std::monostate> : 
        IsDataType<DataType::Null>, IsArrayParsable<false>, IsScalarParsable<false>
    {};
    template<> struct PropertyTraits<BinaryFile::Block> : 
        IsDataType<DataType::Block>, IsArrayParsable<false>, IsScalarParsable<false>
    {};
    template<> struct PropertyTraits<BinaryFile::BlockView> : 
        IsDataType<DataType::Block>, IsArrayParsable<false>, IsScalarParsable<false>
    {};
    template<> struct PropertyTraits<BinaryFile::Array> : 
        IsDataType<DataType::Array>, IsArrayParsable<false>, IsScalarParsable<false>
    {};
    template<> struct PropertyTraits<BinaryFile::ArrayView> : 
        IsDataType<DataType::Array>, IsArrayParsable<false>, IsScalarParsable<false>
    {};
    template<> struct PropertyTraits<bool> : 
        IsDataType<DataType::Bool>, IsArrayParsable<false>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<int8_t> : 
        IsDataType<DataType::Int8>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<uint8_t> : 
        IsDataType<DataType::Uint8>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<int16_t> : 
        IsDataType<DataType::Int16>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<uint16_t> : 
        IsDataType<DataType::Uint16>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<int32_t> : 
        IsDataType<DataType::Int32>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<uint32_t> :
        IsDataType<DataType::Uint32>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<int64_t> : 
        IsDataType<DataType::Int64>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<uint64_t> : 
        IsDataType<DataType::Uint64>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<float> :
        IsDataType<DataType::Float32>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<double> : 
        IsDataType<DataType::Float64>, IsArrayParsable<true>, IsScalarParsable<true>
    {};
    template<> struct PropertyTraits<std::string_view> : 
        IsDataType<DataType::String>, IsArrayParsable<false>, IsScalarParsable<false>
    {};
    template<> struct PropertyTraits<std::string> : 
        IsDataType<DataType::String>, IsArrayParsable<true>, IsScalarParsable<true>
    {};

    template<> struct PropertyTraits<Vector2f32> :
        IsDataType<DataType::Vector2f32>, IsArrayParsable<false>, IsScalarParsable<false>
    {};
    template<> struct PropertyTraits<Vector3f32> :
        IsDataType<DataType::Vector3f32>, IsArrayParsable<false>, IsScalarParsable<false>
    {};
    template<> struct PropertyTraits<Vector4f32> :
        IsDataType<DataType::Vector3f32>, IsArrayParsable<false>, IsScalarParsable<false>
    {};
    template<> struct PropertyTraits<Matrix4x4f32> :
        IsDataType<DataType::Matrix4x4f32>, IsArrayParsable<false>, IsScalarParsable<false>
    {};


    // Parser stream wrapper implementations.
    template<typename T>
    Expected<T, ParserError> Parser::StreamWrapper::ReadIntegral()
    {
        if (Overflows(sizeof(T)))
        {
            return Unexpected(ParserError::BufferOverflow);
        }

        return ReadRawIntegral<T>();
    }

    template<typename T, class Enable = void>
    struct ScalarPropertyTraits
    {
        using type = T;
    };

    template<typename T>
    struct ScalarPropertyTraits<T, typename std::enable_if<std::is_enum_v<T>>::type>
    {
        using type = std::underlying_type_t<T>;
    };


    // Block parser implementations.
    

    template<typename ... T>
    Expected<std::tuple<T...>, ParserError> BlockParser::ReadProperties()
    {
        if (sizeof...(T) > propertyCount - m_propertyReadCount)
        {
            return Unexpected(ParserError::OutOfBounds);
        }

        auto result = std::tuple<T...>{};
        auto error = std::optional<ParserError>{};

        ForEachTemplateType<T...>([&](auto type)
        {
            if (error.has_value())
            {
                return;
            }

            using Param = decltype(type);
            using Type = typename Param::Type;

            if constexpr (std::is_same_v<Type, BlockParser> == true)
            {
                auto readResult = ReadBlockProperty();
                if (!readResult)
                {
                    error = readResult.Error();
                    return;
                }
                std::get<Param::index>(result) = *readResult;
            }
            else if constexpr (std::is_same_v<Type, ArrayParser> == true)
            {
                auto readResult = ReadArrayProperty();
                if (!readResult)
                {
                    error = readResult.Error();
                    return;
                }
                std::get<Param::index>(result) = *readResult;
            }
            else
            {
                auto readResult = ReadScalarProperty<Type>();
                if (!readResult)
                {
                    error = readResult.Error();
                    return;
                }
                std::get<Param::index>(result) = *readResult;
            }
        });

        if (error.has_value())
        {
            return Unexpected(error.value());
        }

        return result;
    }

    template<typename T>
    Expected<T, ParserError> BlockParser::ReadScalarProperty()
    {
        using underlyingType = ScalarPropertyTraits<T>::type;

        static_assert(PropertyTraits<underlyingType>::scalarParsable == true, "T is not parsable as scalar value.");

        if (m_propertyReadCount >= propertyCount)
        {
            return Unexpected(ParserError::OutOfBounds);
        }

        m_stream.SeekPosition(m_resetStreamPosition);

        auto dataTypeResult = m_stream.ReadIntegral<uint8_t>();
        if (!dataTypeResult)
        {
            return Unexpected(dataTypeResult.Error());
        }

        const auto dataType = static_cast<DataType>(*dataTypeResult);
        static_assert(sizeof(dataType) == Limits::dataTypeBytes);

        const auto expectedDataType = PropertyTraits<underlyingType>::dataType;
        if (expectedDataType != dataType)
        {
            return Unexpected(ParserError::BadDataType);
        }

        if constexpr (std::is_same_v<T, std::string> == true)
        {
            auto result = m_stream.ReadString();
            if (!result)
            {
                return result;
            }

            ++m_propertyReadCount;
            m_resetStreamPosition = m_stream.currentPosition;
            return result;
        }
        else
        {
            auto result = m_stream.ReadIntegral<underlyingType>();
            if (!result)
            {
                return Unexpected(result.Error());;
            }

            ++m_propertyReadCount;
            m_resetStreamPosition = m_stream.currentPosition;

            if constexpr (std::is_enum_v<T> == true)
            {
                return static_cast<T>(result.Value());
            }
            else
            {

                return result;
            }
        }
    }

    template<typename T>
    Expected<std::vector<T>, ParserError> ArrayParser::ReadScalarElements()
    {
        static_assert(PropertyTraits<T>::arrayParsable == true, "T is not parsable as array elements.");

        auto result = std::vector<T>();

        if constexpr (std::is_same_v<T, std::string> == true)
        {

        }
        else
        {
            const auto dataSize = static_cast<size_t>(elementCount) * sizeof(T);
            if (m_stream.Overflows(dataSize))
            {
                return Unexpected(ParserError::BufferOverflow);
            }

            result.resize(dataSize);
            m_stream.stream.read(reinterpret_cast<char*>(result.data()), dataSize);
            m_stream.currentPosition += dataSize;
        }

        return result;
    }

    template<typename T>
    T Parser::StreamWrapper::ReadRawIntegral()
    {
        static_assert(std::is_integral_v<T> == true || std::is_floating_point_v<T> == true, "Cannot read type T as scalar");

        auto value = T{};
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        currentPosition += sizeof(value);
        return value;
    }

}