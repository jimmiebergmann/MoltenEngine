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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_BINARYFILE_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_BINARYFILE_HPP

#include "Molten/EditorFramework/Build.hpp"
#include "Molten/Utility/Expected.hpp"
#include "Molten/Utility/Template.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Matrix.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <tuple>
#include <optional>
#include <ostream>
#include <filesystem>

namespace Molten::EditorFramework::BinaryFile
{

    enum class DataType : uint8_t
    {
        Null = 0,
        Block,
        Array,
        Bool,
        Int8,
        Uint8,
        Int16,
        Uint16,
        Int32,
        Uint32,
        Int64,
        Uint64,
        Float32,
        Float64,
        String,
        Vector2f32,
        Vector3f32,
        Vector4f32,
        Matrix4x4f32
    };

    namespace Limits
    {
        static constexpr auto dataTypeBytes = static_cast<size_t>(sizeof(DataType));
        static constexpr auto blockSizeBytes = static_cast<size_t>(sizeof(uint64_t));
        static constexpr auto stringLengthBytes = static_cast<size_t>(sizeof(uint64_t));
        static constexpr auto propertyCountBytes = static_cast<size_t>(sizeof(uint64_t));
        static constexpr auto elementCountBytes = static_cast<size_t>(sizeof(uint64_t));
        static constexpr auto minBlockSizeBytes = 
            dataTypeBytes +
            blockSizeBytes +
            stringLengthBytes +
            propertyCountBytes;
    }

    template<bool Vvalue>
    struct IsScalarParsable
    {
        static constexpr auto scalarParsable = Vvalue;
    };

    template<bool Vvalue>
    struct IsArrayParsable
    {
        static constexpr auto arrayParsable = Vvalue;
    };

    template<typename T>
    struct PropertyTraits : IsScalarParsable<false>, IsArrayParsable<false> {};

    template<bool VIsView>
    struct BasicBlock;

    template<bool VIsView>
    struct BasicArray;

    using Block = BasicBlock<false>;
    using BlockView = BasicBlock<true>;

    using Array = BasicArray<false>;
    using ArrayView = BasicArray<true>;

    template<bool VIsView>
    using BasicProperty = std::variant<
        std::monostate,
        BasicBlock<VIsView>,
        BasicArray<VIsView>,
        bool,
        int8_t,
        uint8_t,
        int16_t,
        uint16_t,
        int32_t,
        uint32_t,
        int64_t,
        uint64_t,
        float,
        double,
        std::string_view,
        std::string,
        Vector2f32,
        Vector3f32,
        Vector4f32,
        Matrix4x4f32
    >;

    template<bool VIsView>
    using BasicProperties = std::vector<BasicProperty<VIsView>>;

    using Property = BasicProperty<false>;
    using PropertyView = BasicProperty<true>;
    using Properties = BasicProperties<false>;
    using PropertiesView = BasicProperties<true>;

    template<bool VIsView>
    struct BasicBlock
    {
        std::string name = {};
        BasicProperties<VIsView> properties = {};
    };

    template<bool VIsView>
    using BasicArrayData = std::variant<
        std::vector<BasicBlock<VIsView>>,
        std::vector<BasicArray<VIsView>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<int8_t>>, std::vector<int8_t>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<uint8_t>>, std::vector<uint8_t>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<int16_t>>, std::vector<int16_t>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<uint16_t>>, std::vector<uint16_t>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<int32_t>>, std::vector<int32_t>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<uint32_t>>, std::vector<uint32_t>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<int64_t>>, std::vector<int64_t>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<uint64_t>>, std::vector<uint64_t>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<float>>, std::vector<float>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<double>>, std::vector<double>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<std::string>>, std::vector<std::string>>,
        std::conditional_t<VIsView, std::reference_wrapper<const std::vector<std::string_view>>, std::vector<std::string_view>>
    >;

    using ArrayData = BasicArrayData<false>;
    using ArrayViewData = BasicArrayData<true>;

    template<bool VIsView>
    struct BasicArray
    {
        BasicArrayData<VIsView> data;
    };

    enum class ParserError
    {
        BufferOverflow,     ///< Requested read operation resulted in buffer overflow.
        OutOfBounds,        ///< Requested read operation of property or array element resulted in out of bounds error.
        BadDataType,        ///< Requested read operation doesn't match with actual data type in mesh file.
        BadElementDataType, ///< Requested array read operation doesn't match with actual data type in mesh file.
        InvalidBlockSize    ///< Encountered invalid block size in file.
    };

    struct BlockParser;
    struct ArrayParser;

    struct Parser
    {
        struct StreamWrapper
        {
            bool Overflows(const size_t byteCount) const;
            bool OverflowsFrom(const size_t position, const size_t byteCount) const;

            template<typename T>
            Expected<T, ParserError> ReadIntegral();
            Expected<std::string, ParserError> ReadString();
            template<typename T>
            T ReadRawIntegral();

            void SeekPosition(size_t position);

            std::istream& stream;
            const size_t endPosition;
            size_t currentPosition;
        };

        static Expected<BlockParser, ParserError> ReadBlock(std::istream& stream);
    };

    struct BlockParser
    {
        template<typename ... T>
        Expected<std::tuple<T...>, ParserError> ReadProperties();
        template<typename T>
        Expected<T, ParserError> ReadScalarProperty();
        Expected<BlockParser, ParserError> ReadBlockProperty();
        Expected<ArrayParser, ParserError> ReadArrayProperty();

        Parser::StreamWrapper m_stream;
        size_t m_resetStreamPosition = 0;
        uint64_t m_propertyReadCount = 0;

        const uint64_t size = 0;
        const std::string name = {};
        const uint64_t propertyCount = 0;
    };

    struct ArrayParser
    {
        Parser::StreamWrapper m_stream;
        size_t m_resetStreamPosition = 0;
        uint64_t m_elementReadCount = 0;

        template<typename T>
        Expected<std::vector<T>, ParserError> ReadScalarElements();
        Expected<BlockParser, ParserError> ReadBlockElement();



        const uint64_t size = 0;
        const DataType elementDataType = DataType::Null;
        const uint64_t elementCount = 0;
    };

}

namespace Molten::EditorFramework
{

    enum class WriteBinaryFileError
    {
        OpenFileError,
        InternalError
    };

    enum class ReadBinaryFileError
    {
        OpenFileError,
    };

    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteBinaryFileError> WriteBinaryFile(
        std::ostream& stream,
        const BinaryFile::BlockView& block);

    MOLTEN_EDITOR_FRAMEWORK_API Expected<void, WriteBinaryFileError> WriteBinaryFile(
        std::filesystem::path path,
        const BinaryFile::BlockView& block);
}

#include "Molten/EditorFramework/FileFormat/BinaryFile.inl"

#endif