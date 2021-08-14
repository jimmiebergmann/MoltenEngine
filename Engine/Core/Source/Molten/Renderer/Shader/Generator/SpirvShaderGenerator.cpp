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

#include "Molten/Renderer/Shader/Generator/SpirvShaderGenerator.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Logger.hpp"
#include <stack>

namespace Molten::Shader
{

    // Global implementations.
    static size_t GetVariableByteOffset(const VariableDataType dataType)
    {
        switch (dataType)
        {
            case VariableDataType::Bool:         return 4;
            case VariableDataType::Int32:        return 4;
            case VariableDataType::Float32:      return 4;
            case VariableDataType::Vector2f32:   return 8;
            case VariableDataType::Vector3f32:   return 12;
            case VariableDataType::Vector4f32:   return 16;
            case VariableDataType::Matrix4x4f32: return 64;
            case VariableDataType::Sampler1D:
            case VariableDataType::Sampler2D:
            case VariableDataType::Sampler3D: break;
        }
        return 0;
    }

    static const std::string g_emptyString = "";
    static const std::string g_arithmeticOperatorNameAdd = "add";
    static const std::string g_arithmeticOperatorNameDiv = "div";
    static const std::string g_arithmeticOperatorNameMul = "mul";
    static const std::string g_arithmeticOperatorNameSub = "sub";

    static const std::string& GetArithmeticOperatorName(const Visual::ArithmeticOperatorType operatorType)
    {
        switch (operatorType)
        {
            case Visual::ArithmeticOperatorType::Addition: return g_arithmeticOperatorNameAdd;
            case Visual::ArithmeticOperatorType::Division: return g_arithmeticOperatorNameDiv;
            case Visual::ArithmeticOperatorType::Multiplication: return g_arithmeticOperatorNameMul;
            case Visual::ArithmeticOperatorType::Subtraction: return g_arithmeticOperatorNameSub;
            default: break;           
        }

        return g_emptyString;
    }


    // Spirv generator implementations.
    bool SpirvGenerator::CreateTemplate(
        Template& /*glslTemplate*/,
        const std::vector<Visual::Script*>& /*scripts*/,
        Logger* /*logger*/)
    {
        return true;
    }

    SpirvGenerator::SpirvGenerator(Logger* logger) :
        m_script(nullptr),
        m_logger(logger)
    {}

    Spirv::Words SpirvGenerator::Generate(
        const Visual::Script& script,
        const Template* /*spirvTemplate*/,
        const bool includeDebugSymbols)
    {
        InitGenerator(script, includeDebugSymbols);
        InitInterfaces();

        if(!TraverseScriptNodes())
        {
            return {};
        }

        return WriteModule();
    }

    SpirvGenerator::DataTypeDefinition::DataTypeDefinition(
        const VariableDataType dataType,
        const Spirv::Id typeId
    ) :
        dataType(dataType),
        typeId(typeId)
    {}

    SpirvGenerator::DataTypePointerDefinition::DataTypePointerDefinition(
        DataTypeDefinitionPointer dataTypeDefinition,
        const Spirv::Id pointerId
    ):
        dataTypeDefinition(std::move(dataTypeDefinition)),
        pointerId(pointerId)
    {}

    SpirvGenerator::DataTypeDefinitionPointer SpirvGenerator::DataTypeStorage::Find(const VariableDataType dataType)
    {
        auto it = std::find_if(dataTypes.begin(), dataTypes.end(), [&](const auto& dataTypePair)
        {
            return dataTypePair.dataType == dataType;
        });

        return it != dataTypes.end() ? it->dataTypeDefintion : nullptr;
    }

    SpirvGenerator::ImageTypeStorage::ImageTypePointer SpirvGenerator::ImageTypeStorage::Find(const size_t dimensions)
    {
        auto it = std::find_if(dataTypePointers.begin(), dataTypePointers.end(), [&](const auto& imageDataType)
        {
            return imageDataType->dimensions == dimensions;
        });

        return it != dataTypePointers.end() ? *it : nullptr;
    }

    SpirvGenerator::GeneratorInputPin::GeneratorInputPin(const Visual::Pin& pin) :
        pin(pin),
        connectedGeneratorOutputPin(nullptr),
        connectedId(0),
        loadId(0)
    {}

    SpirvGenerator::GeneratorOutputPin::GeneratorOutputPin(const Visual::Pin& pin) :
        pin(pin),
        id(0),
        storeId(0)
    {}


    SpirvGenerator::GeneratorNode::GeneratorNode(const Visual::Node& node) :
        node(node),
        inputPins(CreateInputPins(this, node)),
        inputPinIterator(inputPins.begin()),
        outputPins(CreateOutputPins(this, node))
    {}

    SpirvGenerator::GeneratorInputPin* SpirvGenerator::GeneratorNode::GetNextInputPin()
    {
        if (inputPinIterator == inputPins.end())
        {
            return nullptr;
        }

        auto* inputPin = inputPinIterator->get();
        ++inputPinIterator;
        return inputPin;
    }

    SpirvGenerator::GeneratorNode::GeneratorInputPinPointers SpirvGenerator::GeneratorNode::CreateInputPins(GeneratorNode* generatorNode, const Visual::Node& parentNode)
    {
        auto inputPins = parentNode.GetInputPins();

        GeneratorInputPinPointers generatorInputPins;
        generatorInputPins.reserve(inputPins.size());

        for (auto* inputPin : inputPins)
        {
            generatorInputPins.push_back(std::make_shared<GeneratorInputPin>(*inputPin));
        }
        return generatorInputPins;
    }

    SpirvGenerator::GeneratorNode::GeneratorOutputPinPointers SpirvGenerator::GeneratorNode::CreateOutputPins(GeneratorNode* generatorNode, const Visual::Node& parentNode)
    {
        auto outputPins = parentNode.GetOutputPins();

        GeneratorOutputPinPointers generatorOutputPins;
        generatorOutputPins.reserve(outputPins.size());

        for (auto* outputPin : outputPins)
        {
            generatorOutputPins.push_back(std::make_shared<GeneratorOutputPin>(*outputPin));
        }
        return generatorOutputPins;
    }

    SpirvGenerator::GeneratorInputOutputInterfacePin::GeneratorInputOutputInterfacePin(
        const Visual::Pin& pin,
        DataTypePointerDefinitionPointer dataTypePointerDefinition,
        const Spirv::Id id,
        const Spirv::Word location
    ) :
        pin(pin),
        dataTypePointerDefinition(std::move(dataTypePointerDefinition)),
        id(id),
        location(location)
    {}

    SpirvGenerator::GeneratorInputOutputInterfacePinPointer SpirvGenerator::GeneratorInputOutputInterface::Find(const Visual::Pin& pin)
    {
        const auto it = pinMap.find(&pin);
        return it != pinMap.end() ? it->second : nullptr;
    }

    void SpirvGenerator::InitGenerator(const Visual::Script& script, const bool includeDebugSymbols)
    {
        // TODO: Clean up all variables!

        m_script = &script;

        m_includeDebugSymbols = includeDebugSymbols;
        m_currentId = 0;
        m_capabilities.emplace_back(Spirv::Capability::Shader);
        //m_glslExtension = { GetNextId(), "GLSL.std.450" };
        m_mainEntryPoint = {
            m_script->GetType() == Shader::Type::Vertex ? Spirv::ExecutionModel::Vertex : Spirv::ExecutionModel::Fragment,
            GetNextId(),
            "main",
            {} // TODO: Need inputs here.
        };
        m_debugNames.insert({ m_mainEntryPoint.id, m_mainEntryPoint.name });

        m_voidTypeId = GetNextId();
        m_mainFunctionTypeId = GetNextId();
        m_mainFunctionLabelId = GetNextId();

        m_pushConstantStruct.reset();
        //m_inputStruct.reset();
        //m_outputStruct.reset();

        m_dataTypes.dataTypes.clear();
        m_inputDataTypePointers.dataTypePointers.clear();
        m_outputDataTypePointers.dataTypePointers.clear();
        m_uniformBuffers.clear();
        //m_uniformBufferStorage.uniformBuffers.clear();

        std::get<ConstantScalarStorage<int32_t>>(m_constantScalarStorages).constants.clear();
        std::get<ConstantScalarStorage<float>>(m_constantScalarStorages).constants.clear();

        std::get<ConstantVectorStorage<2, float>>(m_constantVectorStorages).constants.clear();
        std::get<ConstantVectorStorage<3, float>>(m_constantVectorStorages).constants.clear();
        std::get<ConstantVectorStorage<4, float>>(m_constantVectorStorages).constants.clear();

        m_outputNodes.clear();
        m_nodes.clear();
        m_outputPins.clear();
    }

    void SpirvGenerator::InitInterfaces()
    {
        // Add inputs.
        const auto& inputInterface = m_script->GetInputInterface();
        if (const auto outputPins = inputInterface.GetOutputPins(); !outputPins.empty())
        {
            auto generatorNode = CreateGeneratorNode(inputInterface);

            Spirv::Word location = 0;
            for (auto& generatorOutputPin : generatorNode->outputPins)
            {
                const auto& outputPin = generatorOutputPin->pin;

                const auto pinDataType = outputPin.GetDataType();
                auto dataTypePointerDefinition = GetOrCreateDataTypePointer(m_inputDataTypePointers, pinDataType);

                const auto outputId = GetNextId();
                generatorOutputPin->id = outputId;
                CreateOutputInterfacePin(m_inputInterface, outputPin, std::move(dataTypePointerDefinition), outputId, location++);

                if (m_includeDebugSymbols)
                {
                    m_debugNames.insert({ outputId, GetNextDebugName("in") });
                }
            }
        }

        // Add outputs
        const auto& outputInterface = m_script->GetOutputInterface();
        if (const auto inputPins = outputInterface.GetInputPins(); !inputPins.empty())
        {
            auto generatorNode = std::make_shared<GeneratorNode>(outputInterface);
            m_outputNodes.push_back(generatorNode);

            Spirv::Word location = 0;
            for(auto& generatorInputPin : generatorNode->inputPins)
            {
                const auto& inputPin = generatorInputPin->pin;

                const auto pinDataType = inputPin.GetDataType();
                auto dataTypePointerDefinition = GetOrCreateDataTypePointer(m_outputDataTypePointers, pinDataType);

                const auto outputId = GetNextId();
                CreateOutputInterfacePin(m_outputInterface, inputPin, std::move(dataTypePointerDefinition), outputId, location++);

                if (m_includeDebugSymbols)
                {  
                    m_debugNames.insert({ outputId, GetNextDebugName("out") });
                }
            }
        }

        // Add push constants.
        const auto& pushConstants = m_script->GetPushConstantsBase();
        if (auto pushConstantsOutputPins = pushConstants.GetOutputPins(); !pushConstantsOutputPins.empty())
        {
            CreateGeneratorNode(pushConstants);

            m_pushConstantStruct = std::make_shared<PushConstantStruct>();

            auto& pushConstantMembers = m_pushConstantStruct->members;
            pushConstantMembers.reserve(pushConstantsOutputPins.size());

            Spirv::Word index = 0;
            Spirv::Word offset = 0;
            for (const auto* pin : pushConstantsOutputPins)
            {
                GetOrCreateConstantScalar(static_cast<int32_t>(index));

                const auto pinDataType = pin->GetDataType();

                pushConstantMembers.push_back({ index, offset, GetOrCreateDataType(pinDataType) });

                ++index;
                offset += std::max(Spirv::Word{ 16 }, static_cast<Spirv::Word>(GetVariableByteOffset(pinDataType)));
            }

            m_pushConstantStruct->structTypeId = GetNextId();
            m_pushConstantStruct->structTypePointerId = GetNextId();
            m_pushConstantStruct->id = GetNextId();
        }

        // Add descriptor sets.
        auto& descriptorSetsBase = m_script->GetDescriptorSetsBase();
        for(size_t setIndex = 0; setIndex < descriptorSetsBase.GetSetCount(); setIndex++)
        {
            auto* setBase = descriptorSetsBase.GetSetBase(setIndex);
            for (size_t bindingIndex = 0; bindingIndex < setBase->GetBindingCount(); bindingIndex++)
            {
                const auto* bindingBase = setBase->GetBindingBase(bindingIndex);

                auto generatorNode = CreateGeneratorNode(*bindingBase);

                // Uniform buffer.
                const auto bindingType = bindingBase->GetBindingType();
                if(bindingType == DescriptorBindingType::UniformBuffer)
                {
                    auto uniformBuffer = std::make_shared<UniformBuffer>();

                    Spirv::Word memberIndex = 0;
                    Spirv::Word memberOffset = 0;
 
                    for(auto& generatorOutputPin : generatorNode->outputPins)
                    {
                        const auto& outputPin = generatorOutputPin->pin;
                        const auto pinDataType = outputPin.GetDataType();

                        auto uniformBufferMember = std::make_shared<UniformBufferMember>();
                        uniformBufferMember->index = memberIndex;
                        uniformBufferMember->offset = memberOffset;
                        uniformBufferMember->dataTypeDefinition = GetOrCreateDataType(pinDataType);
                        uniformBufferMember->pointerId = GetNextId();
                        uniformBufferMember->pin = &outputPin;

                        uniformBuffer->members.push_back(uniformBufferMember);
                        uniformBuffer->memberMap.insert({&outputPin, uniformBufferMember});

                        ++memberIndex;
                        memberOffset += std::max(Spirv::Word{ 16 }, static_cast<Spirv::Word>(GetVariableByteOffset(pinDataType)));
                    }

                    uniformBuffer->structTypeId = GetNextId();                  
                    uniformBuffer->structTypePointerId = GetNextId();
                    uniformBuffer->id = GetNextId();

                    uniformBuffer->setIndex = setBase->GetId();
                    uniformBuffer->bindingIndex = bindingBase->GetId();

                    m_uniformBuffers.push_back(uniformBuffer);
                }
                else // Samplers.
                {
                    GetOrCreateDataType(VariableDataType::Float32);

                    size_t dimensions = 0;
                    switch(bindingType)
                    {
                        case DescriptorBindingType::Sampler1D: dimensions = 1; break;
                        case DescriptorBindingType::Sampler2D: dimensions = 2; break;
                        case DescriptorBindingType::Sampler3D: dimensions = 3; break;
                        default: break;
                    }

                    if(m_imageDataTypes.Find(dimensions) == nullptr)
                    {
                        auto imageDataType = std::make_shared < ImageTypeStorage::ImageType>();
                        imageDataType->dimensions = dimensions;
                        imageDataType->typeId = GetNextId();
                        m_imageDataTypes.dataTypePointers.push_back(std::move(imageDataType));
                    }

                    switch (bindingType)
                    {
                        case DescriptorBindingType::Sampler1D: GetOrCreateDataType(VariableDataType::Sampler1D); break;
                        case DescriptorBindingType::Sampler2D: GetOrCreateDataType(VariableDataType::Sampler2D); break;
                        case DescriptorBindingType::Sampler3D: GetOrCreateDataType(VariableDataType::Sampler3D); break;
                        default: break;
                    }

                   // generatorNode->outputPins[0]->id =
                    
                }
            }
        }

    }

    bool SpirvGenerator::TraverseScriptNodes()
    {
        for (auto& outputNode : m_outputNodes)
        {
            std::stack<GeneratorNodePointer> nodeStack;
            nodeStack.push(outputNode);

            while (!nodeStack.empty())
            {
                auto& currentNode = nodeStack.top();

                if(auto* currentInputPin = currentNode->GetNextInputPin(); currentInputPin)
                {                
                    if (auto newNode = ProcessScriptInputPin(currentInputPin); newNode)
                    {
                        nodeStack.push(newNode);
                    }
                    continue;
                }

                if(!ProcessScriptNode(currentNode))
                {
                    return false;
                }

                nodeStack.pop();
            }
        }

        return true;
    }

    SpirvGenerator::GeneratorNodePointer SpirvGenerator::ProcessScriptInputPin(GeneratorInputPin* generatorInputPin)
    {
        auto* pinConnection = generatorInputPin->pin.GetConnection();

        // No connection, use default value later.
        if (!pinConnection)
        {
            return nullptr;
        }

        // Check if connected pin has been created before
        if (auto it = m_outputPins.find(pinConnection); it != m_outputPins.end())
        {
            generatorInputPin->connectedGeneratorOutputPin = it->second.get();
            return nullptr;
        }

        const auto& node = pinConnection->GetNode();
        GeneratorNodePointer generatorNode = CreateGeneratorNode(node);

        auto generatorPinConnection = m_outputPins.find(pinConnection)->second; 
        generatorInputPin->connectedGeneratorOutputPin = generatorPinConnection.get();

        return generatorNode;
    }

    bool SpirvGenerator::ProcessScriptNode(GeneratorNodePointer& generatorNode)
    {
        switch (generatorNode->node.GetType())
        {
            case Visual::NodeType::Function: return ProcessScriptFunctionNode(generatorNode);;
            case Visual::NodeType::Operator: return ProcessScriptOperatorNode(generatorNode);
            /*case Visual::NodeType::PushConstants: return ProcessScriptPushConstantNode(generatorNode);          
            case Visual::NodeType::Constant: return ProcessScriptConstantNode(generatorNode);
            case Visual::NodeType::DescriptorBinding: return ProcessScriptDescriptorBindingNode(generatorNode);
            case Visual::NodeType::Input: return ProcessScriptInputNode(generatorNode);*/
            
            case Visual::NodeType::Output: return ProcessScriptOutputNode(generatorNode);
            default: break;
        }

        Logger::WriteError(m_logger, "Unhandled operator node type: " + 
            std::to_string(static_cast<uint32_t>(generatorNode->node.GetType())) + ".");
        return false;
    }

    bool SpirvGenerator::ProcessScriptFunctionNode(GeneratorNodePointer& generatorNode)
    {
        return true;
    }

    bool SpirvGenerator::ProcessScriptOperatorNode(GeneratorNodePointer& generatorNode)
    {
        auto& operatorBase = static_cast<const Visual::OperatorBase&>(generatorNode->node);

        switch (operatorBase.GetOperatorType())
        {
            case Visual::OperatorType::Arithmetic:
            {
                if (const auto inputPinCount = generatorNode->inputPins.size(); inputPinCount != 2)
                {
                    Logger::WriteError(m_logger, "Operator node expects 2 input pins, found " + std::to_string(inputPinCount) + ".");
                    return false;
                }

                if (const auto outputPinCount = generatorNode->outputPins.size(); outputPinCount != 1)
                {
                    Logger::WriteError(m_logger, "Operator node expects 1 output pin, found " + std::to_string(outputPinCount) + ".");
                    return false;
                }

                auto& arithmeticOperatorBase = static_cast<const Visual::ArithmeticOperatorBase&>(operatorBase);

                for(auto& generatorInputPin : generatorNode->inputPins)
                {
                    if(const auto* connectedGeneratorOutputPin = generatorInputPin->connectedGeneratorOutputPin; connectedGeneratorOutputPin)
                    {
                        generatorInputPin->connectedId = connectedGeneratorOutputPin->id;
                        generatorInputPin->loadId = GetNextId();
                    }
                    else
                    {
                        const auto& inputPin = generatorInputPin->pin;
                        const auto constantId = GetOrCreateInputPinDefaultConstant(inputPin);
                        if(constantId == 0)
                        {
                            Logger::WriteError(m_logger, "Failed to create default value constant.");
                            return false;
                        }

                        generatorInputPin->connectedId = constantId;
                        generatorInputPin->loadId = constantId;
                    }
                }

                auto& generatorOutputPin = generatorNode->outputPins[0];
                GetOrCreateDataTypePointer(m_functionDataTypePointers, generatorOutputPin->pin.GetDataType());

                generatorOutputPin->id = GetNextId();
                generatorOutputPin->storeId = GetNextId();
                m_mainInstructions.push_back(generatorNode);

                if(m_includeDebugSymbols)
                {
                    auto name = GetNextDebugName(GetArithmeticOperatorName(arithmeticOperatorBase.GetArithmeticOperatorType()));
                    m_debugNames.insert({ generatorOutputPin->id, std::move(name) });
                }

            } break;
            default: break;
        }

        return true;
    }

    bool SpirvGenerator::ProcessScriptPushConstantNode(GeneratorNode& generatorNode)
    {
       

        return true;
    }

    bool SpirvGenerator::ProcessScriptConstantNode(GeneratorNode& generatorNode)
    {
        const auto& constantBase = static_cast<const Visual::ConstantBase&>(generatorNode.node);

        switch(constantBase.GetDataType())
        {
            case VariableDataType::Int32: GetOrCreateConstantScalar(static_cast<const Visual::Constant<int32_t>&>(constantBase).GetValue()); return true;
            case VariableDataType::Float32: GetOrCreateConstantScalar(static_cast<const Visual::Constant<float>&>(constantBase).GetValue()); return true;
            case VariableDataType::Vector2f32: GetOrCreateConstantVector(static_cast<const Visual::Constant<Vector2f32>&>(constantBase).GetValue()); return true;
            case VariableDataType::Vector3f32: GetOrCreateConstantVector(static_cast<const Visual::Constant<Vector3f32>&>(constantBase).GetValue()); return true;
            case VariableDataType::Vector4f32: GetOrCreateConstantVector(static_cast<const Visual::Constant<Vector4f32>&>(constantBase).GetValue()); return true;
            default: break;
        }

        Logger::WriteError(m_logger, "Data type of constant node is not support, data type: " + 
            std::to_string(static_cast<size_t>(constantBase.GetDataType())) + ".");

        return false;
    }

    bool SpirvGenerator::ProcessScriptDescriptorBindingNode(GeneratorNode& generatorNode)
    {
        return true;
    }

    bool SpirvGenerator::ProcessScriptInputNode(GeneratorNode& generatorNode)
    {
        return true;
    }

    bool SpirvGenerator::ProcessScriptOutputNode(GeneratorNodePointer& generatorNode)
    {
        for (const auto& generatorInputPin : generatorNode->inputPins)
        {
            if (const auto* connectedGeneratorOutputPin = generatorInputPin->connectedGeneratorOutputPin; connectedGeneratorOutputPin)
            {
                generatorInputPin->connectedId = connectedGeneratorOutputPin->id;
                generatorInputPin->loadId = GetNextId();
            }
            else
            {
                const auto& inputPin = generatorInputPin->pin;
                const auto constantId = GetOrCreateInputPinDefaultConstant(inputPin);
                if (constantId == 0)
                {
                    Logger::WriteError(m_logger, "Failed to create default value constant.");
                    return false;
                }

                generatorInputPin->connectedId = constantId;
                generatorInputPin->loadId = constantId;
            }

            m_mainInstructions.push_back(generatorNode);
        }
        return true;
    }

    SpirvGenerator::DataTypeDefinitionPointer SpirvGenerator::GetOrCreateDataType(const VariableDataType dataType)
    {
        if(auto dataTypeDefinition = m_dataTypes.Find(dataType); dataTypeDefinition != nullptr)
        {
            return dataTypeDefinition;
        }

        switch (dataType)
        {
            case VariableDataType::Vector2f32:
            case VariableDataType::Vector3f32:
            case VariableDataType::Vector4f32:
            {
                GetOrCreateDataType(VariableDataType::Float32);
            } break;
            default: break;
        }

        auto dataTypeDefinition = std::make_shared<DataTypeDefinition>(dataType, GetNextId());
        m_dataTypes.dataTypes.push_back({ dataType, dataTypeDefinition });
        return dataTypeDefinition;
    }

    SpirvGenerator::DataTypePointerDefinitionPointer SpirvGenerator::GetOrCreateDataTypePointer(DataTypePointerStorage& storage, const VariableDataType dataType)
    {
        if(auto it = storage.dataTypePointers.find(dataType); it != storage.dataTypePointers.end())
        {
            return it->second;
        }

        auto dataTypeDefinition = GetOrCreateDataType(dataType);

        auto dataTypePointerDefinition = std::make_shared<DataTypePointerDefinition>(dataTypeDefinition, GetNextId());
        storage.dataTypePointers.insert({ dataType, dataTypePointerDefinition });
        return dataTypePointerDefinition;
    }

    Spirv::Id SpirvGenerator::GetOrCreateInputPinDefaultConstant(const Visual::Pin& pin)
    {
        if(pin.GetDirection() != Visual::PinDirection::In)
        {
            return 0;
        }

        switch(pin.GetDataType())
        {
            case VariableDataType::Int32: return GetOrCreateConstantScalar(static_cast<const Visual::InputPin<int32_t>&>(pin).GetDefaultValue())->id;
            case VariableDataType::Float32: return GetOrCreateConstantScalar(static_cast<const Visual::InputPin<float>&>(pin).GetDefaultValue())->id;
            case VariableDataType::Vector2f32: return GetOrCreateConstantVector(static_cast<const Visual::InputPin<Vector2f32>&>(pin).GetDefaultValue())->id;
            case VariableDataType::Vector3f32: return GetOrCreateConstantVector(static_cast<const Visual::InputPin<Vector3f32>&>(pin).GetDefaultValue())->id;
            case VariableDataType::Vector4f32: return GetOrCreateConstantVector(static_cast<const Visual::InputPin<Vector4f32>&>(pin).GetDefaultValue())->id;
            default: break;
        }

        return 0;
    }

    void SpirvGenerator::CreateOutputInterfacePin(
        GeneratorInputOutputInterface& inputOutputInterface,
        const Visual::Pin& pin,
        DataTypePointerDefinitionPointer dataTypePointerDefinition,
        const Spirv::Id id,
        const Spirv::Word location)
    {
        auto generatorOutputInterfacePin = std::make_shared<GeneratorInputOutputInterfacePin>(pin, std::move(dataTypePointerDefinition), id, location);
        inputOutputInterface.pins.push_back(generatorOutputInterfacePin);
        inputOutputInterface.pinMap.insert({ &pin, generatorOutputInterfacePin });
    }

    Spirv::Id SpirvGenerator::GetNextId()
    {
        return ++m_currentId;
    }

    SpirvGenerator::GeneratorNodePointer SpirvGenerator::CreateGeneratorNode(const Visual::Node& node)
    {
        auto generatorNode = std::make_shared<GeneratorNode>(node);
        for (auto& generatorOutputPin : generatorNode->outputPins)
        {
            m_outputPins.insert({ &generatorOutputPin->pin, generatorOutputPin });
        }
        m_nodes.insert({ &node, generatorNode });

        return generatorNode;
    }

    std::string SpirvGenerator::GetNextDebugName(const std::string& namePrefix)
    {
        auto it = m_debugNameCounters.find(namePrefix);
        if (it != m_debugNameCounters.end())
        {
            return namePrefix + "_" + std::to_string(++it->second);
        }
        
        it = m_debugNameCounters.insert({ namePrefix, 0 }).first;

        return namePrefix + "_" + std::to_string(it->second);
    }

    Spirv::Words SpirvGenerator::WriteModule()
    {
        m_module.words.clear();

        m_module.AddHeader(
            (Spirv::Word{ 1 } << 16),   // Version 1.0
            0x4d6f6c74,                 // Generator magic number: "Molt"
            m_currentId + 1);    // Max id bound.

        for (const auto& capability : m_capabilities)
        {
            m_module.AddOpCapability(capability);
        }

        if(m_glslExtension.has_value())
        {
            m_module.AddOpExtInstImport(m_glslExtension.value());
        }

        m_module.AddOpMemoryModel(Spirv::AddressingModel::Logical, Spirv::MemoryModel::Glsl450);

        for (const auto& pin : m_inputInterface.pins)
        {
            m_mainEntryPoint.interface.push_back(pin->id);
        }
        for(const auto& pin : m_outputInterface.pins)
        {
            m_mainEntryPoint.interface.push_back(pin->id);
        }

        m_module.AddOpEntryPoint(m_mainEntryPoint);
        m_module.AddOpExecutionMode(m_mainEntryPoint.id, Spirv::ExecutionMode::OriginUpperLeft);

        // Create debug names
        if(m_includeDebugSymbols)
        {
            for(const auto& debugNamePair : m_debugNames)
            {
                m_module.AddOpName(debugNamePair.first, debugNamePair.second);
            }

            // TODO: Get rid of this...      
            if (m_pushConstantStruct)
            {
                m_module.AddOpName(m_pushConstantStruct->structTypeId, "s_pc");
                m_module.AddOpName(m_pushConstantStruct->id, "pc");
            }

            if (!m_uniformBuffers.empty())
            {
                size_t index = 0;
                for (const auto& uniformBuffer : m_uniformBuffers)
                {
                    m_module.AddOpName(uniformBuffer->id, "ubo_" + std::to_string(index));
                    m_module.AddOpName(uniformBuffer->structTypeId, "s_ubo_" + std::to_string(index));
                    ++index;
                }
            }
        }
        
        // Create decorations.
        if(m_pushConstantStruct)
        {
            const auto structId = m_pushConstantStruct->structTypeId;
            for(const auto& member : m_pushConstantStruct->members)
            {
                m_module.AddOpMemberDecorateOffset(structId, member.index, member.offset);
            }

            m_module.AddOpDecorateBlock(structId);
        }

        for (const auto& pin : m_inputInterface.pins)
        {
            m_module.AddOpDecorateLocation(pin->id, pin->location);
        }

        for (const auto& pin : m_outputInterface.pins)
        {
            m_module.AddOpDecorateLocation(pin->id, pin->location);
        }

        for(const auto& uniformBuffer : m_uniformBuffers)
        {
            for (const auto& member : uniformBuffer->members)
            {
                m_module.AddOpMemberDecorateOffset(uniformBuffer->structTypeId, member->index, member->offset);
            }

            m_module.AddOpDecorateBlock(uniformBuffer->structTypeId);
            m_module.AddOpDecorateDescriptorSet(uniformBuffer->id, uniformBuffer->setIndex);
            m_module.AddOpDecorateBinding(uniformBuffer->id, uniformBuffer->bindingIndex);          
        }

        // Create types + type pointers + global variables.
        m_module.AddOpTypeVoid(m_voidTypeId);
        m_module.AddOpTypeFunction(m_mainFunctionTypeId, m_voidTypeId);

        for(const auto& dataTypePair : m_dataTypes.dataTypes)
        {
            switch(auto& dataTypeDefinition = dataTypePair.dataTypeDefintion; dataTypeDefinition->dataType)
            {
                case VariableDataType::Int32: m_module.AddOpTypeInt32(dataTypeDefinition->typeId, Spirv::Signedness::Signed); break;
                case VariableDataType::Float32: m_module.AddOpTypeFloat32(dataTypeDefinition->typeId); break;
                case VariableDataType::Vector2f32:
                {
                    const auto componentDataTypeDefinition = m_dataTypes.Find(VariableDataType::Float32);
                    const auto componentTypeId = componentDataTypeDefinition->typeId;
                    m_module.AddOpTypeVector(dataTypeDefinition->typeId, componentTypeId, 2);
                } break;
                case VariableDataType::Vector3f32:
                {
                    const auto componentDataTypeDefinition = m_dataTypes.Find(VariableDataType::Float32);
                    const auto componentTypeId = componentDataTypeDefinition->typeId;
                    m_module.AddOpTypeVector(dataTypeDefinition->typeId, componentTypeId, 3);
                } break;
                case VariableDataType::Vector4f32:
                {
                    const auto componentDataTypeDefinition = m_dataTypes.Find(VariableDataType::Float32);
                    const auto componentTypeId = componentDataTypeDefinition->typeId;
                    m_module.AddOpTypeVector(dataTypeDefinition->typeId, componentTypeId, 4);
                } break;
                default: break;
            }
        }

        if (m_pushConstantStruct)
        {
            std::vector<Spirv::Id> pushConstantMemberTypeIds;
            for (const auto& member : m_pushConstantStruct->members)
            {
                pushConstantMemberTypeIds.push_back(member.dataTypeDefinition->typeId);
            }
            
            m_module.AddOpTypeStruct(m_pushConstantStruct->structTypeId, pushConstantMemberTypeIds);
            m_module.AddOpTypePointer(m_pushConstantStruct->structTypePointerId, Spirv::StorageClass::PushConstant, m_pushConstantStruct->structTypeId);
            m_module.AddOpVariable(m_pushConstantStruct->id, m_pushConstantStruct->structTypePointerId, Spirv::StorageClass::PushConstant);
        }        

        if (!m_inputInterface.pins.empty())
        {
            for (const auto& dataTypePointerPair : m_inputDataTypePointers.dataTypePointers)
            {
                const auto& dataTypePointer = dataTypePointerPair.second;
                m_module.AddOpTypePointer(dataTypePointer->pointerId, Spirv::StorageClass::Input, dataTypePointer->dataTypeDefinition->typeId);
            }

            for (const auto& pin : m_inputInterface.pins)
            {
                const auto& dataTypePointer = pin->dataTypePointerDefinition;
                m_module.AddOpVariable(pin->id, dataTypePointer->pointerId, Spirv::StorageClass::Input);
            }
        }

        if (!m_outputInterface.pins.empty())
        {
            for (const auto& dataTypePointerPair : m_outputDataTypePointers.dataTypePointers)
            {
                const auto& dataTypePointer = dataTypePointerPair.second;
                m_module.AddOpTypePointer(dataTypePointer->pointerId, Spirv::StorageClass::Output, dataTypePointer->dataTypeDefinition->typeId);
            }

            for (const auto& pin : m_outputInterface.pins)
            {
                const auto& dataTypePointer = pin->dataTypePointerDefinition;
                m_module.AddOpVariable(pin->id, dataTypePointer->pointerId, Spirv::StorageClass::Output);
            }
        }

        for (const auto& uniformBuffer : m_uniformBuffers)
        {
            std::vector<Spirv::Id> structMemberTypeIds;
            for (const auto& member : uniformBuffer->members)
            {
                structMemberTypeIds.push_back(member->dataTypeDefinition->typeId);
            }
            
            m_module.AddOpTypeStruct(uniformBuffer->structTypeId, structMemberTypeIds);
            m_module.AddOpTypePointer(uniformBuffer->structTypePointerId, Spirv::StorageClass::Uniform, uniformBuffer->structTypeId);
            m_module.AddOpVariable(uniformBuffer->id, uniformBuffer->structTypePointerId, Spirv::StorageClass::Uniform);

            for (const auto& member : uniformBuffer->members)
            {
                m_module.AddOpTypePointer(member->pointerId, Spirv::StorageClass::Uniform, member->dataTypeDefinition->typeId);
            }
        }

        for (auto& dataTypePointerPair : m_functionDataTypePointers.dataTypePointers)
        {
            const auto& dataTypePointer = dataTypePointerPair.second;
            m_module.AddOpTypePointer(dataTypePointer->pointerId, Spirv::StorageClass::Function, dataTypePointer->dataTypeDefinition->typeId);
        }

        for(const auto& imageDataType : m_imageDataTypes.dataTypePointers)
        {
            const auto dataType = GetOrCreateDataType(VariableDataType::Float32);

            switch (imageDataType->dimensions)
            {
                case 1: m_module.AddOpTypeImage(imageDataType->typeId, dataType->typeId, Spirv::Dimensionality::Image1D); break;
                case 2: m_module.AddOpTypeImage(imageDataType->typeId, dataType->typeId, Spirv::Dimensionality::Image2D); break;
                case 3: m_module.AddOpTypeImage(imageDataType->typeId, dataType->typeId, Spirv::Dimensionality::Image3D); break;
                default: break;
            }
        }
        for (const auto& dataTypePair : m_dataTypes.dataTypes)
        {
            switch (auto& dataTypeDefinition = dataTypePair.dataTypeDefintion; dataTypeDefinition->dataType)
            {
                case VariableDataType::Sampler1D: m_module.AddOpTypeSampledImage(dataTypeDefinition->typeId, m_imageDataTypes.Find(1)->typeId); break;
                case VariableDataType::Sampler2D: m_module.AddOpTypeSampledImage(dataTypeDefinition->typeId, m_imageDataTypes.Find(2)->typeId); break;
                case VariableDataType::Sampler3D: m_module.AddOpTypeSampledImage(dataTypeDefinition->typeId, m_imageDataTypes.Find(3)->typeId); break;
                default: break;
            }
        }


        // Create constants.
        for (auto& constantPair : std::get<ConstantScalarStorage<int32_t>>(m_constantScalarStorages).constants)
        {
            const auto& constant = constantPair.second;
            m_module.AddOpConstantInt32(constant->id, constant->dataTypeDefinition->typeId, constant->value);
        }
        for (auto& constantPair : std::get<ConstantScalarStorage<float>>(m_constantScalarStorages).constants)
        {
            const auto& constant = constantPair.second;
            m_module.AddOpConstantFloat32(constant->id, constant->dataTypeDefinition->typeId, constant->value);
        }
        for (auto& constantPair : std::get<ConstantVectorStorage<2, float>>(m_constantVectorStorages).constants)
        {
            const auto& constant = constantPair.second;
            m_module.AddOpConstantVector2(constant.constant->id, constant.constant->dataTypeDefinition->typeId, constant.components);
        }
        for (auto& constantPair : std::get<ConstantVectorStorage<3, float>>(m_constantVectorStorages).constants)
        {
            const auto& constant = constantPair.second;
            m_module.AddOpConstantVector3(constant.constant->id, constant.constant->dataTypeDefinition->typeId, constant.components);
        }
        for (auto& constantPair : std::get<ConstantVectorStorage<4, float>>(m_constantVectorStorages).constants)
        {
            const auto& constant = constantPair.second;
            m_module.AddOpConstantVector4(constant.constant->id, constant.constant->dataTypeDefinition->typeId, constant.components);
        }

        // Create global variables.
        // ...

        // Create main functions
        m_module.AddOpFunction(m_mainEntryPoint.id, m_voidTypeId, Spirv::FunctionControl::None, m_mainFunctionTypeId);
        m_module.AddOpLabel(m_mainFunctionLabelId);

        // Write main variable.
        for (const auto& mainInstructions : m_mainInstructions)
        {
            for(const auto& outputPin : mainInstructions->outputPins)
            {
                if(outputPin->id == 0)
                {
                    continue;
                }

                // %float_1 = OpVariable %_ptr_Function_float Function
                const auto dataTypePointer = GetOrCreateDataTypePointer(m_functionDataTypePointers, outputPin->pin.GetDataType());
                m_module.AddOpVariable(outputPin->id, dataTypePointer->pointerId, Spirv::StorageClass::Function);

            }
        }

        // Write main instructions.
        for(const auto& mainInstructions : m_mainInstructions)
        {
            switch(mainInstructions->node.GetType())
            {
                case Visual::NodeType::Operator: WriteOperator(mainInstructions, static_cast<const Visual::OperatorBase&>(mainInstructions->node)); break;
                case Visual::NodeType::Output: WriteOutput(mainInstructions, static_cast<const Visual::OutputInterface&>(mainInstructions->node)); break;
                default: break;
            }
        }

        m_module.AddOpReturn();
        m_module.AddOpFunctionEnd();

        // Return words.
        return std::move(m_module.words);
    }

    void SpirvGenerator::WriteOperator(const GeneratorNodePointer& generatorNode, const Visual::OperatorBase& operatorBase)
    {
        switch(operatorBase.GetOperatorType())
        {
            case Visual::OperatorType::Arithmetic: WriteOperatorArithmetic(generatorNode, static_cast<const Visual::ArithmeticOperatorBase&>(operatorBase)); break;
            default: break;
        }
    }

    void SpirvGenerator::WriteOperatorArithmetic(const GeneratorNodePointer& generatorNode, const Visual::ArithmeticOperatorBase& arithmeticOperatorBase)
    {
        const auto& outputPin = generatorNode->outputPins[0];
        const auto dataType = GetOrCreateDataType(outputPin->pin.GetDataType());

        std::array<Spirv::Id, 2> inputIds = { 0, 0 };

        for (size_t i = 0; i < inputIds.size(); i++)
        {
            const auto& generatorInputPin = generatorNode->inputPins[i];

            if (generatorInputPin->connectedId != generatorInputPin->loadId)
            {
                m_module.AddOpLoad(dataType->typeId, generatorInputPin->loadId, generatorInputPin->connectedId);
                inputIds[i] = generatorInputPin->loadId;
            }
            else
            {
                inputIds[i] = generatorInputPin->connectedId;
            }
        }

        switch(arithmeticOperatorBase.GetArithmeticOperatorType())
        {
            case Visual::ArithmeticOperatorType::Addition: m_module.AddOpFAdd(dataType->typeId, outputPin->storeId, inputIds[0], inputIds[1]); break;
            case Visual::ArithmeticOperatorType::Division: m_module.AddOpFDiv(dataType->typeId, outputPin->storeId, inputIds[0], inputIds[1]); break;
            case Visual::ArithmeticOperatorType::Multiplication: m_module.AddOpFMul(dataType->typeId, outputPin->storeId, inputIds[0], inputIds[1]); break;
            case Visual::ArithmeticOperatorType::Subtraction: m_module.AddOpFSub(dataType->typeId, outputPin->storeId, inputIds[0], inputIds[1]); break;
            default: break;
        }
        
        m_module.AddOpStore(outputPin->id, outputPin->storeId);
    }

    void SpirvGenerator::WriteOutput(const GeneratorNodePointer& generatorNode, const Visual::OutputInterface& outputInterface)
    {
        for(auto& generatorInputPin : generatorNode->inputPins)
        {
            auto outputInterfacePin = m_outputInterface.Find(generatorInputPin->pin);
            if(outputInterfacePin == nullptr)
            {
                continue;
            }

            const auto inputId = WriteOutputLoadOp(generatorInputPin);
            m_module.AddOpStore(outputInterfacePin->id, inputId);
        }
    }

    Spirv::Id SpirvGenerator::WriteOutputLoadOp(const GeneratorInputPinPointer& generatorInputPin)
    {
        const auto dataType = GetOrCreateDataType(generatorInputPin->pin.GetDataType());

        if (generatorInputPin->connectedId != generatorInputPin->loadId)
        {
            m_module.AddOpLoad(dataType->typeId, generatorInputPin->loadId, generatorInputPin->connectedId);
            return generatorInputPin->loadId;
        }
        
        return generatorInputPin->connectedId;
    }

}