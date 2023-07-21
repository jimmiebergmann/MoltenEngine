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

#ifndef MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_BUILDER_MATERIALASSETFILEBUILDER_HPP
#define MOLTEN_EDITOR_FRAMEWORK_FILEFORMAT_BUILDER_MATERIALASSETFILEBUILDER_HPP

#include "Molten/EditorFramework/FileFormat/Validator/MaterialAssetFileValidator.hpp"
#include <limits>
#include <stdexcept>

namespace Molten::EditorFramework
{

    struct MaterialAssetFileBuilderTypes
    {
        using File = MaterialAssetFile;

        template<File::DataType ... VDataTypes>
        struct DataTypesWrapper
        {
            static constexpr auto dataTypes = std::array<File::DataType, sizeof...(VDataTypes)>{ VDataTypes... };
        };

        struct NodeBase
        {
            virtual uint64_t GetIndex() const = 0;
            virtual bool IsOutputNode() const = 0;
        };

        template<typename TDataTypesWrapper>
        struct InputDataNode : NodeBase
        {
            static constexpr auto inputDataTypes = TDataTypesWrapper::dataTypes;

            virtual uint64_t GetIndex() const = 0;
            virtual bool IsOutputNode() const = 0;
        };

        template<typename TDataTypesWrapper>
        struct OutputDataNode : NodeBase
        {
            static constexpr auto outputDataTypes = TDataTypesWrapper::dataTypes;

            virtual uint64_t GetIndex() const = 0;
            virtual bool IsOutputNode() const = 0;
        };

        template<typename TInputDataTypesWrapper, typename TOutputDataTypesWrapper>
        class Node : public InputDataNode<TInputDataTypesWrapper>, public OutputDataNode<TOutputDataTypesWrapper>
        {

        public:

            Node(const uint64_t nodeIndex, const bool isOutputNode = false);

            uint64_t GetIndex() const override;
            bool IsOutputNode() const override;

            InputDataNode<TInputDataTypesWrapper>& GetInput();
            const InputDataNode<TInputDataTypesWrapper>& GetInput() const;

            OutputDataNode<TOutputDataTypesWrapper>& GetOutput();
            const OutputDataNode<TOutputDataTypesWrapper>& GetOutput() const;

        private:

            const uint64_t m_nodeIndex;
            const bool m_isOutputNode;
        };
    };


    class MaterialAssetFileBuilderException : public std::logic_error
    {

    public:

        MaterialAssetFileBuilderException(const std::string& what);
        MaterialAssetFileBuilderException(const char* what);

        virtual ~MaterialAssetFileBuilderException() = default;

    };


    /** @throws MaterialAssetFileBuilderException. */
    template<typename T>
    class MaterialAssetFileFunctionBuilder
    {

    public:

        using File = MaterialAssetFileBuilderTypes::File;

        template<File::DataType ... VDataTypes>
        using DataTypesWrapper = MaterialAssetFileBuilderTypes::DataTypesWrapper<VDataTypes...>;

        using NodeBase = MaterialAssetFileBuilderTypes::NodeBase;

        template<typename TDataTypesWrapper>
        using InputDataNode = MaterialAssetFileBuilderTypes::InputDataNode<TDataTypesWrapper>;

        template<typename TDataTypesWrapper>
        using OutputDataNode = MaterialAssetFileBuilderTypes::OutputDataNode<TDataTypesWrapper>;

        template<typename TInputDataTypesWrapper, typename TOutputDataTypesWrapper>
        using Node = MaterialAssetFileBuilderTypes::Node<TInputDataTypesWrapper, TOutputDataTypesWrapper>;

        using Validator = MaterialAssetFileValidator;
        using Exception = MaterialAssetFileBuilderException;

        MaterialAssetFileFunctionBuilder(
            MaterialAssetFile& materialAssetFile,
            T& function);

        template<File::VertexInputType VInputType>
        auto AddVertexInputNode(const uint8_t inputIndex);

        template<File::DataType VDataType>
        auto AddParameterNode(const std::string name);

        template<File::DataType VDataType>
        auto AddConstantNode(const typename MaterialAssetFileValidator::DataTypeTraits<VDataType>::Type& value);

        template<File::DataType VDataType, File::DataType ... VInputDataTypes>
        auto AddCompositeNode();

        template<File::OperatorType VOperatorType, File::DataType VLhs, File::DataType VRhs>
        auto AddOperatorNode();

        template<File::BuiltInFunctionType VFunctionType, File::DataType ... VInputDataTypes>
        auto AddBuiltInFunctionNode();

        template<File::EntryPointOutputType VOutputType>
        auto AddOutputNode() requires std::is_same_v<T, File::EntryPoint>;

        template<File::DataType VDataType>
        auto AddOutputNode() requires std::is_same_v<T, File::Function>;

        template<uint64_t VSourceOutputIndex, uint64_t VTargetInputIndex, typename TSourceNodeType, typename TTargetNodeType>
        void LinkNodes(const TSourceNodeType& sourceNode, const TTargetNodeType& targetNode);

        template<uint64_t VTargetInputIndex, typename TSourceNodeType, typename TTargetNodeType>
        void LinkNodes(const TSourceNodeType& sourceNode, const TTargetNodeType& targetNode);

        template<typename TSourceNodeType, typename TTargetNodeType>
        void LinkNodes(const TSourceNodeType& sourceNode, const TTargetNodeType& targetNode);

        template<uint64_t VTargetInputIndex, typename TValue, typename TTargetNodeType>
        void SetNodeInput(TValue value, const TTargetNodeType& targetNode);

        template<typename TValue, typename TTargetNodeType>
        void SetNodeInput(TValue value, const TTargetNodeType& targetNode);

        template<File::DataType ... VInputDataTypes>
        static File::NodeArguments CreateDefaultArguments();

    protected:

        MaterialAssetFile& m_materialAssetFile;
        T& m_function;

    };


    /** @throws MaterialAssetFileBuilderException. */
    class MaterialAssetFileBuilder : public MaterialAssetFileFunctionBuilder<MaterialAssetFile::EntryPoint>
    {

    public:
       
        MaterialAssetFileBuilder(MaterialAssetFile& materialAssetFile);

        auto AddFunction(const std::string& name);

    };




}

#include "Molten/EditorFramework/FileFormat/Builder/MaterialAssetFileBuilder.inl"

#endif