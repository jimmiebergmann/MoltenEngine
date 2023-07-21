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

namespace Molten::EditorFramework
{

    // Materia builder node implementations.
    template<typename TInputDataTypesWrapper, typename TOutputDataTypesWrapper>
    MaterialAssetFileBuilderTypes::Node<TInputDataTypesWrapper, TOutputDataTypesWrapper>::Node(
        const uint64_t nodeIndex,
        const bool isOutputNode
    ) :
        m_nodeIndex(nodeIndex),
        m_isOutputNode(isOutputNode)
    {}

    template<typename TInputDataTypesWrapper, typename TOutputDataTypesWrapper>
    uint64_t MaterialAssetFileBuilderTypes::Node<TInputDataTypesWrapper, TOutputDataTypesWrapper>::GetIndex() const
    {
        return m_nodeIndex;
    }

    template<typename TInputDataTypesWrapper, typename TOutputDataTypesWrapper>
    bool MaterialAssetFileBuilderTypes::Node<TInputDataTypesWrapper, TOutputDataTypesWrapper>::IsOutputNode() const
    {
        return m_isOutputNode;
    }

    template<typename TInputDataTypesWrapper, typename TOutputDataTypesWrapper>
    MaterialAssetFileBuilderTypes::InputDataNode<TInputDataTypesWrapper>&
        MaterialAssetFileBuilderTypes::Node<TInputDataTypesWrapper, TOutputDataTypesWrapper>::GetInput()
    {
        return static_cast<InputDataNode<TInputDataTypesWrapper>&>(*this);
    }

    template<typename TInputDataTypesWrapper, typename TOutputDataTypesWrapper>
    const MaterialAssetFileBuilderTypes::InputDataNode<TInputDataTypesWrapper>&
        MaterialAssetFileBuilderTypes::Node<TInputDataTypesWrapper, TOutputDataTypesWrapper>::GetInput() const
    {
        return static_cast<const InputDataNode<TInputDataTypesWrapper>&>(*this);
    }

    template<typename TInputDataTypesWrapper, typename TOutputDataTypesWrapper>
    MaterialAssetFileBuilderTypes::OutputDataNode<TOutputDataTypesWrapper>&
        MaterialAssetFileBuilderTypes::Node<TInputDataTypesWrapper, TOutputDataTypesWrapper>::GetOutput()
    {
        return static_cast<OutputDataNode<TOutputDataTypesWrapper>&>(*this);
    }

    template<typename TInputDataTypesWrapper, typename TOutputDataTypesWrapper>
    const MaterialAssetFileBuilderTypes::OutputDataNode<TOutputDataTypesWrapper>&
        MaterialAssetFileBuilderTypes::Node<TInputDataTypesWrapper, TOutputDataTypesWrapper>::GetOutput() const
    {
        return static_cast<const OutputDataNode<TOutputDataTypesWrapper>&>(*this);
    }


    // Material builder exception
    MaterialAssetFileBuilderException::MaterialAssetFileBuilderException(const std::string& what) :
        std::logic_error(what)
    {}

    MaterialAssetFileBuilderException::MaterialAssetFileBuilderException(const char* what) :
        std::logic_error(what)
    {}


    // Material function builder implementations.
    template<typename T>
    MaterialAssetFileFunctionBuilder<T>::MaterialAssetFileFunctionBuilder(
        MaterialAssetFile& materialAssetFile,
        T& function
    ) :
        m_materialAssetFile(materialAssetFile),
        m_function(function)
    {}

    template<typename T>
    template<MaterialAssetFile::VertexInputType VInputType>
    auto MaterialAssetFileFunctionBuilder<T>::AddVertexInputNode(const uint8_t inputIndex)
    {
        using Type = typename Validator::VertexInputTypeTraits<VInputType>::Type;
        constexpr auto dataType = Validator::VertexInputTypeTraits<VInputType>::dataType;
        using ResultNodeType = Node<DataTypesWrapper<>, DataTypesWrapper<dataType>>;

        auto node = ResultNodeType{ static_cast<uint64_t>(m_function.nodes.size()) };

        m_function.nodes.emplace_back(MaterialAssetFile::VertexInputNode{
            .inputType = VInputType,
            .inputIndex = inputIndex
        });

        return node;
    }

    template<typename T>
    template<MaterialAssetFile::DataType VDataType>
    auto MaterialAssetFileFunctionBuilder<T>::AddParameterNode(const std::string name)
    {
        using ResultNodeType = Node<DataTypesWrapper<>, DataTypesWrapper<VDataType>>;

        auto node = ResultNodeType{ static_cast<uint64_t>(m_function.nodes.size()) };

        m_function.nodes.emplace_back(MaterialAssetFile::ParameterNode{
            .dataType = VDataType,
            .name = name
        });

        return node;
    }

    template<typename T>
    template<MaterialAssetFile::DataType VDataType>
    auto MaterialAssetFileFunctionBuilder<T>::AddConstantNode(const typename MaterialAssetFileValidator::DataTypeTraits<VDataType>::Type& value)
    {
        using ResultNodeType = Node<DataTypesWrapper<>, DataTypesWrapper<VDataType>>;

        auto node = ResultNodeType{ static_cast<uint64_t>(m_function.nodes.size()) };

        m_function.nodes.emplace_back(MaterialAssetFile::ConstantNode{
            .value = value
        });

        return node;
    }

    template<typename T>
    template<MaterialAssetFile::DataType VDataType, MaterialAssetFile::DataType ... VInputDataTypes>
    auto MaterialAssetFileFunctionBuilder<T>::AddCompositeNode()
    {
        static_assert(Validator::CompositeHasOverride<VDataType>({ VInputDataTypes... }), "Invalid override of composite node.");

        using ResultNodeType = Node<DataTypesWrapper<VInputDataTypes...>, DataTypesWrapper<VDataType>>;

        auto node = ResultNodeType{ static_cast<uint64_t>(m_function.nodes.size()) };

        m_function.nodes.emplace_back(MaterialAssetFile::CompositeNode{
            .dataType = VDataType,
            .parameters = { VInputDataTypes... },
            .arguments = CreateDefaultArguments<VInputDataTypes...>()
        });

        return node;
    }

    template<typename T>
    template<MaterialAssetFile::OperatorType VOperatorType, MaterialAssetFile::DataType VLhs, MaterialAssetFile::DataType VRhs>
    auto MaterialAssetFileFunctionBuilder<T>::AddOperatorNode()
    {
        constexpr auto operatorOverride = Validator::GetOperatorOverride<VOperatorType>({ VLhs, VRhs });

        static_assert(operatorOverride.has_value(), "Invalid override of operator node.");

        constexpr auto returnType = operatorOverride.value().returnType;
        using ReturnType = Validator::DataTypeTraits<returnType>::Type;

        using ResultNodeType = Node<DataTypesWrapper<VLhs, VRhs>, DataTypesWrapper<returnType>>;
        
        auto node = ResultNodeType{ static_cast<uint64_t>(m_function.nodes.size()) };

        m_function.nodes.emplace_back(MaterialAssetFile::OperatorNode{
            .operatorType = VOperatorType,
            .dataType = returnType,
            .parameters = { VLhs, VRhs },
            .arguments = CreateDefaultArguments<VLhs, VRhs>()
        });

        return node;
    }

    template<typename T>
    template<MaterialAssetFile::BuiltInFunctionType VFunctionType, MaterialAssetFile::DataType ... VInputDataTypes>
    auto MaterialAssetFileFunctionBuilder<T>::AddBuiltInFunctionNode()
    {
        constexpr auto functionOverride = Validator::GetFunctionOverride<VFunctionType>({ VInputDataTypes... });

        static_assert(functionOverride.has_value(), "Invalid override of built-in function node.");

        constexpr auto returnType = functionOverride.value().returnType;
        using ReturnType = Validator::DataTypeTraits<returnType>::Type;

        using ResultNodeType = Node<DataTypesWrapper<VInputDataTypes...>, DataTypesWrapper<returnType>>;

        auto node = ResultNodeType{ static_cast<uint64_t>(m_function.nodes.size()) };

        m_function.nodes.emplace_back(MaterialAssetFile::FunctionNode{
            .function = VFunctionType,
            .parameters = { VInputDataTypes... },
            .arguments = CreateDefaultArguments<VInputDataTypes...>()
        });

        return node;
    }

    template<typename T>
    template<MaterialAssetFile::EntryPointOutputType VOutputType>
    auto MaterialAssetFileFunctionBuilder<T>::AddOutputNode() requires std::is_same_v<T, MaterialAssetFile::EntryPoint>
    {
        using Type = typename Validator::EntryPointOutputTraits<VOutputType>::Type;
        constexpr auto dataType = Validator::EntryPointOutputTraits<VOutputType>::dataType;    
        using ResultNodeType = Node<DataTypesWrapper<dataType>, DataTypesWrapper<>>;
        
        auto node = ResultNodeType{ static_cast<uint64_t>(m_function.outputNodes.size()), true };

        m_function.outputNodes.emplace_back(MaterialAssetFile::EntryPointOutputNode{
            .outputType = VOutputType,
            .argument = Type{}
        });

        return node;
    }

    template<typename T>
    template<MaterialAssetFile::DataType VDataType>
    auto MaterialAssetFileFunctionBuilder<T>::AddOutputNode() requires std::is_same_v<T, MaterialAssetFile::Function>
    {
        using Type = typename Validator::DataTypeTraits<VDataType>::Type;
        using ResultNodeType = Node<DataTypesWrapper<VDataType>, DataTypesWrapper<>>;

        auto node = ResultNodeType{ static_cast<uint64_t>(m_function.outputNodes.size()), true };

        m_function.outputNodes.emplace_back(MaterialAssetFile::FunctionOutputNode{
            .dataType = VDataType,
            .argument = Type{}
        });

        return node;
    }

    template<typename T>
    template<uint64_t VSourceOutputIndex, uint64_t VTargetInputIndex, typename TSourceNodeType, typename TTargetNodeType>
    void MaterialAssetFileFunctionBuilder<T>::LinkNodes(const TSourceNodeType& sourceNode, const TTargetNodeType& targetNode)
    {
        static_assert(VSourceOutputIndex < TSourceNodeType::outputDataTypes.size(),
            "Pin index of source node is out of bounds.");

        static_assert(VTargetInputIndex < TTargetNodeType::inputDataTypes.size(),
            "Pin index of target node is out of bounds.");

        static_assert(TSourceNodeType::outputDataTypes[VSourceOutputIndex] == TTargetNodeType::inputDataTypes[VTargetInputIndex],
            "Mismatching data types of input and output pins.");


        if (targetNode.IsOutputNode())
        {
            auto& node = m_function.outputNodes.at(targetNode.GetIndex());
            node.argument = File::NodeLink{
                .nodeIndex = sourceNode.GetIndex(),
                .outputIndex = VSourceOutputIndex
            };
        }
        else
        {
            auto& node = m_function.nodes.at(targetNode.GetIndex());

            std::visit([sourceNodeIndex = sourceNode.GetIndex()](auto& node)
            {
                using Type = std::decay_t<decltype(node)>;
                if constexpr (
                    std::is_same_v<Type, File::CompositeNode> == true ||
                    std::is_same_v<Type, File::OperatorNode> == true ||
                    std::is_same_v<Type, File::FunctionNode> == true)
                {
                    auto& argument = node.arguments.at(VTargetInputIndex);
                    argument = File::NodeLink{
                        .nodeIndex = sourceNodeIndex,
                        .outputIndex = VSourceOutputIndex
                    };
                }
            }, node);
        }
    }

    template<typename T>
    template<uint64_t VTargetInputIndex, typename TSourceNodeType, typename TTargetNodeType>
    void MaterialAssetFileFunctionBuilder<T>::LinkNodes(const TSourceNodeType& sourceNode, const TTargetNodeType& targetNode)
    {
        LinkNodes<0, VTargetInputIndex, TSourceNodeType, TTargetNodeType>(sourceNode, targetNode);
    }

    template<typename T>
    template<typename TSourceNodeType, typename TTargetNodeType>
    void MaterialAssetFileFunctionBuilder<T>::LinkNodes(const TSourceNodeType& sourceNode, const TTargetNodeType& targetNode)
    {
        LinkNodes<0, 0, TSourceNodeType, TTargetNodeType>(sourceNode, targetNode);
    }

    template<typename T>
    template<uint64_t VTargetInputIndex, typename TValue, typename TTargetNodeType>
    void MaterialAssetFileFunctionBuilder<T>::SetNodeInput(TValue value, const TTargetNodeType& targetNode)
    {
        static_assert(VTargetInputIndex < TTargetNodeType::inputDataTypes.size(),
            "Pin index of target node is out of bounds.");

        static_assert(Validator::TypeTraits<TValue>::dataType == TTargetNodeType::inputDataTypes[VTargetInputIndex],
            "Mismatching data types of input pin and value.");

        if (targetNode.IsOutputNode())
        {
            auto& node = m_function.outputNodes.at(targetNode.GetIndex());
            node.argument = value;
        }
        else
        {
            auto& node = m_function.nodes.at(targetNode.GetIndex());

            std::visit([&value](auto& node)
            {
                using Type = std::decay_t<decltype(node)>;
                if constexpr (
                    std::is_same_v<Type, File::CompositeNode> == true ||
                    std::is_same_v<Type, File::OperatorNode> == true ||
                    std::is_same_v<Type, File::FunctionNode> == true)
                {
                    auto& argument = node.arguments.at(VTargetInputIndex);
                    argument = value;
                }
            }, node);
        }
    }

    template<typename T>
    template<typename TValue, typename TTargetNodeType>
    void MaterialAssetFileFunctionBuilder<T>::SetNodeInput(TValue value, const TTargetNodeType& targetNode)
    {
        SetNodeInput<0, TTargetNodeType, TValue>(value, targetNode);
    }


    template<typename T>
    template<MaterialAssetFile::DataType ... VInputDataTypes>
    MaterialAssetFile::NodeArguments MaterialAssetFileFunctionBuilder<T>::CreateDefaultArguments()
    {
        const auto argumentCount = sizeof...(VInputDataTypes);

        auto arguments = File::NodeArguments{};
        arguments.reserve(argumentCount);

        ForEachTemplateValue<File::DataType, VInputDataTypes...>([&arguments](auto value)
        {
            switch (value.value)
            {
                case File::DataType::Bool: arguments.emplace_back(File::NodeArgument{ bool{ } }); break;
                case File::DataType::Int32: arguments.emplace_back(File::NodeArgument{ int32_t{} }); break;
                case File::DataType::Float32: arguments.emplace_back(File::NodeArgument{ float{} }); break;
                case File::DataType::Vector2f32: arguments.emplace_back(File::NodeArgument{ Vector2f32{} }); break;
                case File::DataType::Vector3f32: arguments.emplace_back(File::NodeArgument{ Vector3f32{} }); break;
                case File::DataType::Vector4f32: arguments.emplace_back(File::NodeArgument{ Vector4f32{} }); break;
                case File::DataType::Matrix4x4f32: arguments.emplace_back(File::NodeArgument{ Matrix4x4f32{} }); break;
                case File::DataType::Sampler1D: arguments.emplace_back(File::NodeArgument{ File::Sampler1D{} }); break;
                case File::DataType::Sampler2D: arguments.emplace_back(File::NodeArgument{ File::Sampler2D{} }); break;
                case File::DataType::Sampler3D: arguments.emplace_back(File::NodeArgument{ File::Sampler3D{} }); break;
            }
        });

        return arguments;
    } 


    // Material builder implementations.
    MaterialAssetFileBuilder::MaterialAssetFileBuilder(MaterialAssetFile& materialAssetFile) :
        MaterialAssetFileFunctionBuilder(materialAssetFile, materialAssetFile.entryPoint)
    {}

    auto MaterialAssetFileBuilder::AddFunction(const std::string& name)
    {
        if (!Validator::ValidateFunctionName(name))
        {
            throw Exception(std::string{"Invalid function name: "} + name);
        }
        if (!Validator::ValidateFunctionDuplicate(m_materialAssetFile.functions, name))
        {
            throw Exception(std::string{"Function name already in use: "} + name);
        }

        auto& function = m_materialAssetFile.functions.emplace_back(File::Function{
            .name = name
        });

        return MaterialAssetFileFunctionBuilder<MaterialAssetFile::Function>
        {
            m_materialAssetFile, function
        };
    }

}