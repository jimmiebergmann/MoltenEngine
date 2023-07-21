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

#include "Molten/EditorFramework/FileFormat/BinaryFile.hpp"
#include <map>
#include <fstream>
#include <type_traits>
#include <algorithm>

namespace Molten::EditorFramework
{

    template<typename T>
    struct IsReferenceWrapper : std::false_type
    {};

    template<typename T>
    struct IsReferenceWrapper<std::reference_wrapper<T>> : std::true_type 
    {};

    template<typename T>
    struct ArrayDataTraits
    {
        static const T& GetData(const T& data) { return data;  }
    };

    template<typename T>
    struct ArrayDataTraits<std::reference_wrapper<T>>
    {
        static const T& GetData(const std::reference_wrapper<T>& data) { return data.get(); }
    };

    template<typename T>
    struct ArrayTraits {};

    template<> struct ArrayTraits<std::vector<BinaryFile::Block>> {
        static constexpr auto dataType = BinaryFile::DataType::Block;
    };
    template<> struct ArrayTraits<std::vector<BinaryFile::BlockView>> {
        static constexpr auto dataType = BinaryFile::DataType::Block;
    };
    template<> struct ArrayTraits<std::vector<BinaryFile::Array>> {
        static constexpr auto dataType = BinaryFile::DataType::Array;
    };
    template<> struct ArrayTraits<std::vector<BinaryFile::ArrayView>> {
        static constexpr auto dataType = BinaryFile::DataType::Array;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<int8_t>>> {
        static constexpr auto dataType = BinaryFile::DataType::Int8;
    };
    template<> struct ArrayTraits<std::vector<int8_t>> {
        static constexpr auto dataType = BinaryFile::DataType::Int8;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<uint8_t>>> {
        static constexpr auto dataType = BinaryFile::DataType::Uint8;
    };
    template<> struct ArrayTraits<std::vector<uint8_t>> {
        static constexpr auto dataType = BinaryFile::DataType::Uint8;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<int16_t>>> {
        static constexpr auto dataType = BinaryFile::DataType::Int16;
    };
    template<> struct ArrayTraits<std::vector<int16_t>> {
        static constexpr auto dataType = BinaryFile::DataType::Int16;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<uint16_t>>> {
        static constexpr auto dataType = BinaryFile::DataType::Uint16;
    };
    template<> struct ArrayTraits<std::vector<uint16_t>> {
        static constexpr auto dataType = BinaryFile::DataType::Uint16;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<int32_t>>> {
        static constexpr auto dataType = BinaryFile::DataType::Int32;
    };
    template<> struct ArrayTraits<std::vector<int32_t>> {
        static constexpr auto dataType = BinaryFile::DataType::Int32;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<uint32_t>>> {
        static constexpr auto dataType = BinaryFile::DataType::Uint32;
    };
    template<> struct ArrayTraits<std::vector<uint32_t>> {
        static constexpr auto dataType = BinaryFile::DataType::Uint32;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<int64_t>>> {
        static constexpr auto dataType = BinaryFile::DataType::Int64;
    };
    template<> struct ArrayTraits<std::vector<int64_t>> {
        static constexpr auto dataType = BinaryFile::DataType::Int64;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<uint64_t>>> {
        static constexpr auto dataType = BinaryFile::DataType::Uint64;
    };
    template<> struct ArrayTraits<std::vector<uint64_t>> {
        static constexpr auto dataType = BinaryFile::DataType::Uint64;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<float>>> {
        static constexpr auto dataType = BinaryFile::DataType::Float32;
    };
    template<> struct ArrayTraits<std::vector<float>> {
        static constexpr auto dataType = BinaryFile::DataType::Float32;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<double>>> {
        static constexpr auto dataType = BinaryFile::DataType::Float64;
    };
    template<> struct ArrayTraits<std::vector<double>> {
        static constexpr auto dataType = BinaryFile::DataType::Float64;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<std::string>>> {
        static constexpr auto dataType = BinaryFile::DataType::String;
    };
    template<> struct ArrayTraits<std::vector<std::string>> {
        static constexpr auto dataType = BinaryFile::DataType::String;
    };
    template<> struct ArrayTraits<std::reference_wrapper<const std::vector<std::string_view>>> {
        static constexpr auto dataType = BinaryFile::DataType::String;
    };
    template<> struct ArrayTraits<std::vector<std::string_view>> {
        static constexpr auto dataType = BinaryFile::DataType::String;
    };


    using SizeMap = std::map<const void*, size_t>;


    template<bool VIsView>
    static size_t CalculateBlockSize(
        SizeMap& sizeMap,
        const BinaryFile::BasicBlock<VIsView>& block);

    template<bool VIsView>
    static size_t CalculateArraySize(
        SizeMap& sizeMap,
        const BinaryFile::BasicArray<VIsView>& array);

    static void WriteDataType(
        std::ostream& stream,
        BinaryFile::DataType dataType);

    template<bool VIsView>
    static Expected<void, WriteBinaryFileError> WriteProperty(
        std::ostream& stream,
        const SizeMap& sizeMap,
        const BinaryFile::BasicProperty<VIsView>& property);

    template<bool VIsView>
    static Expected<void, WriteBinaryFileError> WriteBlock(
        std::ostream& stream,
        const SizeMap& sizeMap,
        const BinaryFile::BasicBlock<VIsView>& block);

    template<bool VIsView>
    static Expected<void, WriteBinaryFileError> WriteArray(
        std::ostream& stream,
        const SizeMap& sizeMap,
        const BinaryFile::BasicArray<VIsView>& array);

    
    template<bool VIsView>
    static size_t CalculateBlockSize(
        SizeMap& sizeMap, 
        const BinaryFile::BasicBlock<VIsView>& block)
    {
        size_t size = 
            static_cast<size_t>(sizeof(BinaryFile::DataType)) +
            sizeof(uint64_t) +
            sizeof(uint64_t) +
            block.name.size() +
            sizeof(uint64_t);

        for (const auto& property : block.properties)
        {
            size += std::visit([&](const auto& value)
            {
                using type = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<type, std::monostate> == true)
                {
                    return static_cast<size_t>(sizeof(BinaryFile::DataType));
                }
                else if constexpr (
                    std::is_same_v<type, BinaryFile::Block> == true ||
                    std::is_same_v<type, BinaryFile::BlockView> == true)
                {
                    return CalculateBlockSize(sizeMap, value);
                }
                else if constexpr (
                    std::is_same_v<type, BinaryFile::Array> == true ||
                    std::is_same_v<type, BinaryFile::ArrayView> == true)
                {
                    return CalculateArraySize(sizeMap, value);
                }
                else if constexpr (
                    std::is_same_v<type, bool> == true ||
                    std::is_same_v<type, int8_t> == true ||
                    std::is_same_v<type, uint8_t> == true ||
                    std::is_same_v<type, int16_t> == true ||
                    std::is_same_v<type, uint16_t> == true ||
                    std::is_same_v<type, int32_t> == true ||
                    std::is_same_v<type, uint32_t> == true ||
                    std::is_same_v<type, int64_t> == true ||
                    std::is_same_v<type, uint64_t> == true ||
                    std::is_same_v<type, float> == true ||
                    std::is_same_v<type, double> == true ||
                    std::is_same_v<type, Vector2f32> == true ||
                    std::is_same_v<type, Vector3f32> == true ||
                    std::is_same_v<type, Vector4f32> == true ||
                    std::is_same_v<type, Matrix4x4f32> == true)
                {
                    return 
                        static_cast<size_t>(sizeof(BinaryFile::DataType)) + 
                        static_cast<size_t>(sizeof(type));
                }
                else if constexpr (
                    std::is_same_v<type, std::string> == true ||
                    std::is_same_v<type, std::string_view> == true)
                {
                    return 
                        static_cast<size_t>(sizeof(BinaryFile::DataType)) + 
                        static_cast<size_t>(sizeof(uint64_t)) +
                        value.size();
                }
                else
                {
                    static_assert(AlwaysFalse<type>, "Missing data type case in WriteBinaryFile::CalculateBlockSize.");
                }
            }, property);
        }

        sizeMap.insert({ reinterpret_cast<const void*>(&block), size });
        return size;
    }

    template<bool VIsView>
    static size_t CalculateArraySize(
        SizeMap& sizeMap,
        const BinaryFile::BasicArray<VIsView>& array)
    {
        size_t size =
            static_cast<size_t>(sizeof(BinaryFile::DataType)) +
            sizeof(uint64_t) +
            static_cast<size_t>(sizeof(BinaryFile::DataType)) + 
            sizeof(uint64_t);

        size += std::visit([&](const auto& value)
        {
            using type = std::decay_t<decltype(value)>;
            const auto& data = ArrayDataTraits<type>::GetData(value);
            using elementType = typename std::decay_t<decltype(data)>::value_type;

            if constexpr (
                std::is_same_v<elementType, BinaryFile::Block> == true ||
                std::is_same_v<elementType, BinaryFile::BlockView> == true)
            {
                auto totalSize = size_t{ 0 };
                for (const auto& element : data)
                {
                    totalSize += CalculateBlockSize(sizeMap, element);
                }
                return totalSize;
            }
            else if constexpr (
                std::is_same_v<elementType, BinaryFile::Array> == true ||
                std::is_same_v<elementType, BinaryFile::ArrayView> == true)
            {
                auto totalSize = size_t{ 0 };
                for (const auto& element : data)
                {
                    totalSize += CalculateArraySize(sizeMap, element);
                }
                return totalSize;
            }
            else if constexpr (std::is_same_v<elementType, bool> == true)
            {
                return data.size() * sizeof(elementType);
            }
            else if constexpr (
                std::is_same_v<elementType, int8_t> == true ||
                std::is_same_v<elementType, uint8_t> == true ||
                std::is_same_v<elementType, int16_t> == true ||
                std::is_same_v<elementType, uint16_t> == true ||
                std::is_same_v<elementType, int32_t> == true ||
                std::is_same_v<elementType, uint32_t> == true ||
                std::is_same_v<elementType, int64_t> == true ||
                std::is_same_v<elementType, uint64_t> == true ||
                std::is_same_v<elementType, float> == true ||
                std::is_same_v<elementType, double> == true)
            {
                return data.size() * sizeof(elementType);
            }
            else if constexpr (
                std::is_same_v<elementType, std::string> == true ||
                std::is_same_v<elementType, std::string_view> == true)
            {
                auto totalSize = size_t{ 0 };
                for (const auto& element : data)
                {
                    totalSize += static_cast<size_t>(sizeof(uint64_t)) + element.size();
                }
                return totalSize;
            }
            else
            {
                static_assert(AlwaysFalse<type>, "Missing data type case in WriteBinaryFile::CalculateArraySize.");
            }

        }, array.data);

        sizeMap.insert({ reinterpret_cast<const void*>(&array), size });
        return size;
    }

    static void WriteDataType(
        std::ostream& stream,
        BinaryFile::DataType dataType)
    {
        static_assert(sizeof(dataType) == BinaryFile::Limits::dataTypeBytes);
        stream.write(reinterpret_cast<const char*>(&dataType), sizeof(dataType));
    }

    template<bool VIsView>
    static Expected<void, WriteBinaryFileError> WriteProperty(
        std::ostream& stream,
        const SizeMap& sizeMap,
        const BinaryFile::BasicProperty<VIsView>& property)
    {
        return std::visit([&](const auto& value) -> Expected<void, WriteBinaryFileError>
        {
            using type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<type, std::monostate> == true) 
            {
                WriteDataType(stream, BinaryFile::PropertyTraits<type>::dataType);
                return {};
            }
            else if constexpr (
                std::is_same_v<type, BinaryFile::Block> == true ||
                std::is_same_v<type, BinaryFile::BlockView> == true)
            {
                return WriteBlock(stream, sizeMap, value);
            }
            else if constexpr (
                std::is_same_v<type, BinaryFile::Array> == true ||
                std::is_same_v<type, BinaryFile::ArrayView> == true)
            {
                return WriteArray(stream, sizeMap, value);
            }
            else if constexpr (
                std::is_same_v<type, bool> == true ||
                std::is_same_v<type, int8_t> == true ||
                std::is_same_v<type, uint8_t> == true ||
                std::is_same_v<type, int16_t> == true ||
                std::is_same_v<type, uint16_t> == true ||
                std::is_same_v<type, int32_t> == true ||
                std::is_same_v<type, uint32_t> == true ||
                std::is_same_v<type, int64_t> == true ||
                std::is_same_v<type, uint64_t> == true ||
                std::is_same_v<type, float> == true ||
                std::is_same_v<type, double> == true ||
                std::is_same_v<type, Vector2f32> == true ||
                std::is_same_v<type, Vector3f32> == true ||
                std::is_same_v<type, Vector4f32> == true ||
                std::is_same_v<type, Matrix4x4f32> == true)
            {
                WriteDataType(stream, BinaryFile::PropertyTraits<type>::dataType);
                stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
                return {};
            }
            else if constexpr (
                std::is_same_v<type, std::string> == true ||
                std::is_same_v<type, std::string_view> == true)
            {
                WriteDataType(stream, BinaryFile::PropertyTraits<type>::dataType);
                const auto stringLength = static_cast<uint64_t>(value.size());
                static_assert(sizeof(stringLength) == BinaryFile::Limits::stringLengthBytes);
                stream.write(reinterpret_cast<const char*>(&stringLength), sizeof(stringLength));
                stream.write(reinterpret_cast<const char*>(value.data()), value.size());
                return {};
            }
            else
            {
                static_assert(AlwaysFalse<type>, "Missing data type case in WriteBinaryFile::WriteProperty.");
            }
        }, property);
    }

    template<bool VIsView>
    static Expected<void, WriteBinaryFileError> WriteBlock(
        std::ostream& stream,
        const SizeMap& sizeMap,
        const BinaryFile::BasicBlock<VIsView>& block)
    {
        WriteDataType(stream, BinaryFile::DataType::Block);

        const auto blockSizeIt = sizeMap.find(&block);
        if (blockSizeIt == sizeMap.end())
        {
            return Unexpected(WriteBinaryFileError::InternalError);
        }

        const auto blockSize = static_cast<uint64_t>(blockSizeIt->second);
        static_assert(sizeof(blockSize) == BinaryFile::Limits::blockSizeBytes);
        stream.write(reinterpret_cast<const char*>(&blockSize), sizeof(blockSize));

        const auto blockNameLength = static_cast<uint64_t>(block.name.size());
        static_assert(sizeof(blockNameLength) == BinaryFile::Limits::stringLengthBytes);
        stream.write(reinterpret_cast<const char*>(&blockNameLength), sizeof(blockNameLength));
        stream.write(reinterpret_cast<const char*>(block.name.data()), block.name.size());

        const auto propertyCount = static_cast<uint64_t>(block.properties.size());
        static_assert(sizeof(propertyCount) == BinaryFile::Limits::propertyCountBytes);
        stream.write(reinterpret_cast<const char*>(&propertyCount), sizeof(propertyCount));

        for (const auto& property : block.properties)
        {
            if (auto result = WriteProperty(stream, sizeMap, property); !result)
            {
                return Unexpected(result.Error());
            }
        }

        return {};
    }

    template<bool VIsView>
    static Expected<void, WriteBinaryFileError> WriteArray(
        std::ostream& stream,
        const SizeMap& sizeMap,
        const BinaryFile::BasicArray<VIsView>& array)
    {
        WriteDataType(stream, BinaryFile::DataType::Array);

        const auto blockSizeIt = sizeMap.find(&array);
        if (blockSizeIt == sizeMap.end())
        {
            return Unexpected(WriteBinaryFileError::InternalError);
        }

        const auto blockSize = static_cast<uint64_t>(blockSizeIt->second);
        static_assert(sizeof(blockSize) == BinaryFile::Limits::blockSizeBytes);
        stream.write(reinterpret_cast<const char*>(&blockSize), sizeof(blockSize));

        return std::visit([&](const auto& value) -> Expected<void, WriteBinaryFileError>
        {
            using type = std::decay_t<decltype(value)>;

            WriteDataType(stream, ArrayTraits<type>::dataType);

            const auto& data = ArrayDataTraits<type>::GetData(value);
            using elementType = typename std::decay_t<decltype(data)>::value_type;

            const auto elementCount = static_cast<uint64_t>(data.size());
            static_assert(sizeof(elementCount) == BinaryFile::Limits::elementCountBytes);
            stream.write(reinterpret_cast<const char*>(&elementCount), sizeof(elementCount));

            if constexpr (
                std::is_same_v<elementType, BinaryFile::Block> == true ||
                std::is_same_v<elementType, BinaryFile::BlockView> == true)
            {
                for (const auto& element : data)
                {
                    if (auto result = WriteBlock(stream, sizeMap, element); !result)
                    {
                        return Unexpected(result.Error());
                    }
                }
            }
            else if constexpr (
                std::is_same_v<elementType, BinaryFile::Array> == true ||
                std::is_same_v<elementType, BinaryFile::ArrayView> == true)
            {
                for (const auto& element : data)
                {
                    if (auto result = WriteArray(stream, sizeMap, element); !result)
                    {
                        return Unexpected(result.Error());
                    }
                }
            }
            else if constexpr (std::is_same_v<elementType, bool> == true)
            {
                for (const auto element : data)
                {
                    stream.write(reinterpret_cast<const char*>(element), sizeof(element));
                }
            }
            else if constexpr (
                std::is_same_v<elementType, int8_t> == true ||
                std::is_same_v<elementType, uint8_t> == true ||
                std::is_same_v<elementType, int16_t> == true ||
                std::is_same_v<elementType, uint16_t> == true ||
                std::is_same_v<elementType, int32_t> == true ||
                std::is_same_v<elementType, uint32_t> == true ||
                std::is_same_v<elementType, int64_t> == true ||
                std::is_same_v<elementType, uint64_t> == true ||
                std::is_same_v<elementType, float> == true ||
                std::is_same_v<elementType, double> == true)
            {
                stream.write(reinterpret_cast<const char*>(data.data()), data.size());
            }
            else if constexpr (
                std::is_same_v<elementType, std::string> == true ||
                std::is_same_v<elementType, std::string_view> == true)
            {
                for (const auto& element : data)
                {
                    const auto stringLength = static_cast<uint64_t>(element.size());
                    static_assert(sizeof(stringLength) == BinaryFile::Limits::stringLengthBytes);
                    stream.write(reinterpret_cast<const char*>(&stringLength), sizeof(stringLength));
                    stream.write(reinterpret_cast<const char*>(element.data()), element.size());
                }
            }
            else
            {
                static_assert(AlwaysFalse<type>, "Missing data type case in WriteBinaryFile::WriteArray.");
            }

            return {};

        }, array.data);
    }
 
    Expected<void, WriteBinaryFileError> WriteBinaryFile(
        std::ostream& stream,
        const BinaryFile::BlockView& block)
    {
        auto sizeMap = SizeMap{};
        CalculateBlockSize(sizeMap, block);   
        return WriteBlock(stream, sizeMap, block);;
    }

    Expected<void, WriteBinaryFileError> WriteBinaryFile(
        std::filesystem::path path,
        const BinaryFile::BlockView& block)
    {
        std::ofstream file(path.c_str(), std::ifstream::binary);
        if (!file.is_open())
        {
            return Unexpected(WriteBinaryFileError::OpenFileError);
        }

        return WriteBinaryFile(file, block);
    }

}

// Binary file namespace
namespace Molten::EditorFramework::BinaryFile
{

    struct BlockHeader
    {
        uint64_t size = 0;
        std::string name = {};
        uint64_t propertyCount = 0;
    };

    struct ArrayHeader
    {
        uint64_t size = 0;
        DataType elementDataType = DataType::Null;
        uint64_t elementCount = 0;
    };

    static Expected<BlockHeader, ParserError> ReadBlockHeader(Parser::StreamWrapper& stream)
    {
        if (stream.Overflows(Limits::dataTypeBytes + Limits::blockSizeBytes))
        {
            return Unexpected(ParserError::BufferOverflow);
        }

        const auto blockHeaderPosition = stream.currentPosition;

        const auto dataTypeByte = stream.ReadRawIntegral<uint8_t>();
        static_assert(sizeof(dataTypeByte) == Limits::dataTypeBytes);
        if (static_cast<DataType>(dataTypeByte) != DataType::Block)
        {
            return Unexpected(ParserError::BadDataType);
        }

        const auto blockSize = static_cast<size_t>(stream.ReadRawIntegral<uint64_t>());
        static_assert(sizeof(blockSize) == Limits::blockSizeBytes);
        if (blockSize < Limits::minBlockSizeBytes || stream.OverflowsFrom(blockHeaderPosition, blockSize))
        {
            return Unexpected(ParserError::InvalidBlockSize);
        }

        auto blockName = stream.ReadString();
        if (!blockName)
        {
            return Unexpected(blockName.Error());
        }

        if (stream.Overflows(Limits::propertyCountBytes))
        {
            return Unexpected(ParserError::BufferOverflow);
        }
        const auto propertyCount = stream.ReadRawIntegral<uint64_t>();
        static_assert(sizeof(propertyCount) == Limits::propertyCountBytes);
        if (stream.Overflows(propertyCount))
        {
            return Unexpected(ParserError::BufferOverflow);
        }

        return BlockHeader{
            .size = blockSize,
            .name = std::move(*blockName),
            .propertyCount = propertyCount
        };
    }

    static Expected<ArrayHeader, ParserError> ReadArrayHeader(Parser::StreamWrapper& stream)
    {
        if (stream.Overflows(Limits::dataTypeBytes + Limits::blockSizeBytes))
        {
            return Unexpected(ParserError::BufferOverflow);
        }

        const auto arrayHeaderPosition = stream.currentPosition;

        const auto dataTypeByte = stream.ReadRawIntegral<uint8_t>();
        static_assert(sizeof(dataTypeByte) == Limits::dataTypeBytes);
        if (static_cast<DataType>(dataTypeByte) != DataType::Array)
        {
            return Unexpected(ParserError::BadDataType);
        }

        const auto blockSize = static_cast<size_t>(stream.ReadRawIntegral<uint64_t>());
        static_assert(sizeof(blockSize) == Limits::blockSizeBytes);
        if (blockSize < Limits::minBlockSizeBytes || stream.OverflowsFrom(arrayHeaderPosition, blockSize))
        {
            return Unexpected(ParserError::InvalidBlockSize);
        }

        if (stream.Overflows(Limits::dataTypeBytes + Limits::elementCountBytes))
        {
            return Unexpected(ParserError::BufferOverflow);
        }

        const auto elementDataTypeByte = stream.ReadRawIntegral<uint8_t>();
        static_assert(sizeof(elementDataTypeByte) == Limits::dataTypeBytes);
        const auto elementDataType = static_cast<DataType>(elementDataTypeByte);

        const auto elementCount = stream.ReadRawIntegral<uint64_t>();
        static_assert(sizeof(elementCount) == Limits::elementCountBytes);
        if (stream.Overflows(elementCount))
        {
            return Unexpected(ParserError::BufferOverflow);
        }

        return ArrayHeader{
            .size = blockSize,
            .elementDataType = elementDataType,
            .elementCount = elementCount
        };
    }


    // Stream wrapper implementations.
    bool Parser::StreamWrapper::Overflows(const size_t byteCount) const
    {
        return currentPosition + byteCount > endPosition;
    }

    bool Parser::StreamWrapper::OverflowsFrom(const size_t position, const size_t byteCount) const
    {
        return position + byteCount > endPosition;
    }

    Expected<std::string, ParserError> Parser::StreamWrapper::ReadString()
    {
        static_assert(sizeof(uint64_t) == Limits::stringLengthBytes);

        auto stringLengthResult = ReadIntegral<uint64_t>();
        if (!stringLengthResult)
        {
            return Unexpected(stringLengthResult.Error());
        }

        const auto stringLength = static_cast<size_t>(*stringLengthResult);
        if (Overflows(stringLength))
        {
            return Unexpected(ParserError::BufferOverflow);
        }

        auto result = std::string{};
        result.resize(stringLength);
        stream.read(result.data(), stringLength);
        currentPosition += stringLength;
        return result;
    }

    void Parser::StreamWrapper::SeekPosition(size_t position)
    {
        if (currentPosition == position)
        {
            return;
        }

        position = std::min(position, endPosition);

        stream.seekg(position, std::istream::beg);
        currentPosition = position;
    }


    // Parser implementations.
    Expected<BlockParser, ParserError> Parser::ReadBlock(std::istream& stream)
    {
        const auto streamCurrentPosition = static_cast<size_t>(stream.tellg());
        stream.seekg(0, std::istream::end);
        const auto streamEndPosition = static_cast<size_t>(stream.tellg());
        stream.seekg(streamCurrentPosition, std::istream::beg);

        auto streamWrapper = StreamWrapper{
            .stream = stream,
            .endPosition = streamEndPosition,
            .currentPosition = streamCurrentPosition
        };

        auto blockHeader = ReadBlockHeader(streamWrapper);
        if (!blockHeader)
        {
            return Unexpected(blockHeader.Error());
        }

        return BlockParser{
            .m_stream = streamWrapper,
            .m_resetStreamPosition = streamWrapper.currentPosition,
            .m_propertyReadCount = 0,
            .size = blockHeader->size,
            .name = blockHeader->name,
            .propertyCount = blockHeader->propertyCount
        };
    }


    // Block parser implementations.
    Expected<BlockParser, ParserError> BlockParser::ReadBlockProperty()
    {
        if (m_propertyReadCount >= propertyCount)
        {
            return Unexpected(ParserError::OutOfBounds);
        }

        m_stream.SeekPosition(m_resetStreamPosition);

        auto blockHeader = ReadBlockHeader(m_stream);
        if (!blockHeader)
        {
            return Unexpected(blockHeader.Error());
        }

        ++m_propertyReadCount;
        m_resetStreamPosition = m_resetStreamPosition + blockHeader->size;

        return BlockParser{
            .m_stream = m_stream,
            .m_resetStreamPosition = m_stream.currentPosition,
            .m_propertyReadCount = 0,
            .size = blockHeader->size,
            .name = blockHeader->name,
            .propertyCount = blockHeader->propertyCount
        };
    }

    Expected<ArrayParser, ParserError> BlockParser::ReadArrayProperty()
    {
        if (m_propertyReadCount >= propertyCount)
        {
            return Unexpected(ParserError::OutOfBounds);
        }

        m_stream.SeekPosition(m_resetStreamPosition);

        auto arrayHeader = ReadArrayHeader(m_stream);
        if (!arrayHeader)
        {
            return Unexpected(arrayHeader.Error());
        }

        ++m_propertyReadCount;
        m_resetStreamPosition = m_resetStreamPosition + arrayHeader->size;

        return ArrayParser{
            .m_stream = m_stream,
            .m_resetStreamPosition = m_stream.currentPosition,
            .m_elementReadCount = 0,
            .size = arrayHeader->size,
            .elementDataType = arrayHeader->elementDataType,
            .elementCount = arrayHeader->elementCount
        };
    }


    // Array block implementations.
    Expected<BlockParser, ParserError> ArrayParser::ReadBlockElement()
    {
        if (elementDataType != DataType::Block)
        {
            return Unexpected(ParserError::BadElementDataType);
        }

        if (m_elementReadCount >= elementCount)
        {
            return Unexpected(ParserError::OutOfBounds);
        }

        m_stream.SeekPosition(m_resetStreamPosition);

        auto blockHeader = ReadBlockHeader(m_stream);
        if (!blockHeader)
        {
            return Unexpected(blockHeader.Error());
        }

        ++m_elementReadCount;
        m_resetStreamPosition = m_resetStreamPosition + blockHeader->size;

        return BlockParser{
            .m_stream = m_stream,
            .m_resetStreamPosition = m_stream.currentPosition,
            .m_propertyReadCount = 0,
            .size = blockHeader->size,
            .name = blockHeader->name,
            .propertyCount = blockHeader->propertyCount
        };
    }
}
