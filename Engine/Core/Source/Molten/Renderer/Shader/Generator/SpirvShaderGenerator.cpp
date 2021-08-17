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
#include "Molten/Logger.hpp"
#include <stack>

namespace Molten::Shader
{

    static const std::string g_emptyString = "";

    static const std::string g_arithmeticOperatorNameAdd = "add";
    static const std::string g_arithmeticOperatorNameDiv = "div";
    static const std::string g_arithmeticOperatorNameMul = "mul";
    static const std::string g_arithmeticOperatorNameSub = "sub";

    static const std::string g_functionNameCos = "cos";
    static const std::string g_functionNameSin = "sin";
    static const std::string g_functionNameTan = "tan";
    static const std::string g_functionNameMax = "max";
    static const std::string g_functionNameMin = "min";
    static const std::string g_functionNameCross = "cross";
    static const std::string g_functionNameDot = "dot";
    static const std::string g_functionNameTexture1D = "texture1D";
    static const std::string g_functionNameTexture2D = "texture2D";
    static const std::string g_functionNameTexture3D = "texture3D";

    // Global implementations.
    /*static size_t GetVariableByteOffset(const VariableDataType dataType)
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
    }*/

    static const std::string& GetOperatorName(const Visual::OperatorBase& operatorBase)
    {
        switch(operatorBase.GetOperatorType())
        {
            case Visual::OperatorType::Arithmetic:
            {
                const auto& arithmeticBase = static_cast<const Visual::ArithmeticOperatorBase&>(operatorBase);
                switch (arithmeticBase.GetArithmeticOperatorType())
                {
                    case Visual::ArithmeticOperatorType::Addition: return g_arithmeticOperatorNameAdd;
                    case Visual::ArithmeticOperatorType::Division: return g_arithmeticOperatorNameDiv;
                    case Visual::ArithmeticOperatorType::Multiplication: return g_arithmeticOperatorNameMul;
                    case Visual::ArithmeticOperatorType::Subtraction: return g_arithmeticOperatorNameSub;
                    default: break;
                }
            } break;
            default: break;
        }

        return g_emptyString;
    }

    static const std::string& GetFunctionName(const Visual::FunctionType functionType)
    {
        switch (functionType)
        {
            // Trigonometry
            case Visual::FunctionType::Cos: return g_functionNameCos;
            case Visual::FunctionType::Sin: return g_functionNameSin;
            case Visual::FunctionType::Tan: return g_functionNameTan;
            // Mathematics.
            case Visual::FunctionType::Max: return g_functionNameMax;
            case Visual::FunctionType::Min: return g_functionNameMin;
            // Vector.
            case Visual::FunctionType::Cross: return g_functionNameCross;
            case Visual::FunctionType::Dot: return g_functionNameDot;
            // Texture
            case Visual::FunctionType::Texture1D: return g_functionNameTexture1D;
            case Visual::FunctionType::Texture2D: return g_functionNameTexture2D;
            case Visual::FunctionType::Texture3D: return g_functionNameTexture3D;
            default: break;
        }

        return g_emptyString;
    }
    /*
    static bool GetGlslInstruction(Spirv::GlslInstruction& glslInstruction, const Visual::FunctionType functionType)
    {
        switch (functionType)
        {
            // Trigonometry
            case Visual::FunctionType::Cos: glslInstruction = Spirv::GlslInstruction::Cos; return true;
            case Visual::FunctionType::Sin: glslInstruction = Spirv::GlslInstruction::Sin; return true;
            case Visual::FunctionType::Tan: glslInstruction = Spirv::GlslInstruction::Tan; return true;
            // Mathematics.
            case Visual::FunctionType::Max: glslInstruction = Spirv::GlslInstruction::FMax; return true;
            case Visual::FunctionType::Min: glslInstruction = Spirv::GlslInstruction::FMin; return true;
            // Vector.
            case Visual::FunctionType::Cross: glslInstruction = Spirv::GlslInstruction::Cross; return true;
            default: break;
        }

        return false;
    }*/


    // Spirv function traits.
    /*template<Visual::FunctionType VFunction>
    struct SpirvFunctionTrait;

    template<>
    struct SpirvFunctionTrait<Visual::FunctionType::Sin>
    {
        static constexpr size_t inputPinCount = 1;
    };*/


    static std::optional<Spirv::Word> GetGlslFunction(const Visual::FunctionType functionType)
    {
        switch (functionType)
        {
            // Trigonometry
            case Visual::FunctionType::Cos: return std::make_optional(static_cast<Spirv::Word>(Spirv::GlslInstruction::Cos));
            case Visual::FunctionType::Sin: return std::make_optional(static_cast<Spirv::Word>(Spirv::GlslInstruction::Sin));
            case Visual::FunctionType::Tan: return std::make_optional(static_cast<Spirv::Word>(Spirv::GlslInstruction::Tan));
                // Mathematics.
            case Visual::FunctionType::Max: return std::make_optional(static_cast<Spirv::Word>(Spirv::GlslInstruction::FMax));
            case Visual::FunctionType::Min: return std::make_optional(static_cast<Spirv::Word>(Spirv::GlslInstruction::FMin));
                // Vector.
            case Visual::FunctionType::Cross: return std::make_optional(static_cast<Spirv::Word>(Spirv::GlslInstruction::Cross));
            default: break;
        }

        return {};
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
        m_logger(logger),
        m_script(nullptr),
        m_includeDebugSymbols(false),
        m_currentId(0),
        m_mainEntryPoint{},
        m_voidTypeId(0),
        m_mainFunctionTypeId(0),
        m_mainFunctionLabelId(0),
        m_opDebugNameStart(0)      
    {}

    Spirv::Words SpirvGenerator::Generate(
        const Visual::Script& script,
        const Template* /*spirvTemplate*/,
        const bool includeDebugSymbols)
    {
        InitGenerator(script, includeDebugSymbols);

        if(!BuildTree())
        {
            return {};
        }

        if(!WriteModule())
        {
            return {};
        }

        return std::move(m_module.words);
    }


    // Data type implementations.
    SpirvGenerator::DataType::DataType(const VariableDataType type) :
        id(0),
        type(type)
    {}


    // Data type pointer implementations.
    SpirvGenerator::DataTypePtr::DataTypePtr(
        DataTypePointer dataType,
        const Spirv::StorageClass storageClass
    ) :
        id(0),
        dataType(std::move(dataType)),
        storageClass(storageClass)
    {}


    // Output structure member implementations.
    SpirvGenerator::OutputStructure::Member::Member(
        GeneratorOutputPinPointer outputPin,
            DataTypePointer dataType,
            DataTypePtrPointer dataTypePointer
    ) :
        outputPin(std::move(outputPin)),
        dataType(std::move(dataType)),
        dataTypePointer(std::move(dataTypePointer))
    {}


    // Output structure member implementations.
    void SpirvGenerator::OutputStructure::Clear()
    {
        members.clear();
    }


    // Input structure member implementations.
    SpirvGenerator::InputStructure::Member::Member(
        GeneratorInputPinPointer inputPin,
        DataTypePointer dataType,
        DataTypePtrPointer dataTypePointer
    ) :
        id(0),
        inputPin(std::move(inputPin)),
        dataType(std::move(dataType)),
        dataTypePointer(std::move(dataTypePointer))
    {}


    // Input structure member implementations.
    SpirvGenerator::InputStructure::Member* SpirvGenerator::InputStructure::FindMember(GeneratorInputPinPointer& inputPin)
    {
        auto it = std::find_if(members.begin(), members.end(), [&](const auto& member)
        {
            return member.inputPin == inputPin;
        });

        return it != members.end() ? &(*it) : nullptr;
    }

    void SpirvGenerator::InputStructure::Clear()
    {
        members.clear();
    }


    /*SpirvGenerator::DataTypePointerDefinition::DataTypePointerDefinition(
        DataTypeDefinitionPointer dataTypeDefinition,
        const Spirv::Id pointerId
    ):
        dataTypeDefinition(std::move(dataTypeDefinition)),
        pointerId(pointerId)
    {}*/
    

    /*SpirvGenerator::ImageTypeStorage::ImageTypePointer SpirvGenerator::ImageTypeStorage::Find(const size_t dimensions)
    {
        auto it = std::find_if(dataTypePointers.begin(), dataTypePointers.end(), [&](const auto& imageDataType)
        {
            return imageDataType->dimensions == dimensions;
        });

        return it != dataTypePointers.end() ? *it : nullptr;
    }*/


    // Generator output pin implementations.
    SpirvGenerator::GeneratorInputPin::GeneratorInputPin(const Visual::Pin& pin) :
        pin(pin),
        connectedGeneratorOutputPin(nullptr)
    {}


    // Generator output pin implementations.
    SpirvGenerator::GeneratorOutputPin::GeneratorOutputPin(const Visual::Pin& pin) :
        pin(pin),
        id(0),
        storageClass(Spirv::StorageClass::Function),
        isVisited(false)
    {}


    // Generator node implementations.
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

    SpirvGenerator::GeneratorOutputPinPointer SpirvGenerator::GeneratorNode::FindOutputPin(const Visual::Pin& pin)
    {
        for(auto& outputPin : outputPins)
        {
            if(&outputPin->pin == &pin)
            {
                return outputPin;
            }
        }

        return nullptr;
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


    // Data type storage implementations.
    SpirvGenerator::DataTypePointer SpirvGenerator::DataTypeStorage::Get(const VariableDataType type)
    {
        auto it = std::find_if(m_dataTypes.begin(), m_dataTypes.end(), [&](const auto& dataTypePair)
        {
            return dataTypePair.type == type;
        });

        return it != m_dataTypes.end() ? it->dataType : nullptr;
    }

    SpirvGenerator::DataTypePointer SpirvGenerator::DataTypeStorage::GetOrCreate(const VariableDataType type)
    {
        if (auto dataType = Get(type); dataType != nullptr)
        {
            return dataType;
        }

        switch (type)
        {
            case VariableDataType::Vector2f32:
            case VariableDataType::Vector3f32:
            case VariableDataType::Vector4f32:
            {
                GetOrCreate(VariableDataType::Float32);
            } break;
            default: break;
        }

        auto dataType = std::make_shared<DataType>(type);
        m_dataTypes.push_back({ type, dataType });
        return dataType;
    }

    SpirvGenerator::DataTypePointers SpirvGenerator::DataTypeStorage::GetAllDependencySorted()
    {
        DataTypePointers result;

        // Scalar types.
        for (const auto& dataTypePair : m_dataTypes)
        {
            switch (dataTypePair.type)
            {
                case VariableDataType::Bool:
                case VariableDataType::Int32:
                case VariableDataType::Float32:
                {
                    result.push_back(dataTypePair.dataType);
                } break;
                default: break;
            }
        }

        // Composite types.
        for (const auto& dataTypePair : m_dataTypes)
        {
            switch (dataTypePair.type)
            {
                case VariableDataType::Vector2f32:
                case VariableDataType::Vector3f32:
                case VariableDataType::Vector4f32:
                case VariableDataType::Matrix4x4f32:
                {
                    result.push_back(dataTypePair.dataType);
                } break;
                default: break;
            }
        }

        return result;
    }

    void SpirvGenerator::DataTypeStorage::Clear()
    {
        m_dataTypes.clear();
    }


    // Data type pointer storage implementations.
    SpirvGenerator::DataTypePtrPointer SpirvGenerator::DataTypePointerStorage::Get(const Spirv::StorageClass storageClass, DataTypePointer& dataType)
    {
        const auto pointersIt = m_dataTypePointers.find(storageClass);
        if (pointersIt == m_dataTypePointers.end())
        {
            return nullptr;
        }

        const auto& pointers = pointersIt->second;
        auto it = std::find_if(pointers.begin(), pointers.end(), [&](const auto& pointer)
        {
            return pointer->dataType == dataType;
        });

        return it != pointers.end() ? *it : nullptr;
    }

    SpirvGenerator::DataTypePtrPointer SpirvGenerator::DataTypePointerStorage::GetOrCreate(const Spirv::StorageClass storageClass, DataTypePointer& dataType)
    {
        if (auto foundDataTypePointer = Get(storageClass, dataType); foundDataTypePointer)
        {
            return foundDataTypePointer;
        }

        auto pointersIt = m_dataTypePointers.find(storageClass);
        if (pointersIt == m_dataTypePointers.end())
        {
            pointersIt = m_dataTypePointers.insert({ storageClass, {} }).first;
        }

        auto& pointers = pointersIt->second;
        auto newDataTypePointer = std::make_shared<DataTypePtr>(dataType, storageClass);
        pointers.push_back(newDataTypePointer);

        return newDataTypePointer;
    }

    SpirvGenerator::DataTypePtrPointers SpirvGenerator::DataTypePointerStorage::GetAll(const Spirv::StorageClass storageClass)
    {
        const auto pointersIt = m_dataTypePointers.find(storageClass);
        if (pointersIt == m_dataTypePointers.end())
        {
            return {};
        }

        return pointersIt->second;
    }

    void SpirvGenerator::DataTypePointerStorage::Clear()
    {
        m_dataTypePointers.clear();
    }


    // Constant storage implementations.
    SpirvGenerator::ConstantPointer SpirvGenerator::ConstantStorage::Get(GeneratorInputPin& generatorInputPin)
    {
        const auto& pin = generatorInputPin.pin;
        if (pin.GetDirection() != Visual::PinDirection::In)
        {
            return nullptr;
        }

        switch (pin.GetDataType())
        {
            case VariableDataType::Bool: return Get(static_cast<const Visual::InputPin<bool>&>(pin).GetDefaultValue());
            case VariableDataType::Int32: return Get(static_cast<const Visual::InputPin<int32_t>&>(pin).GetDefaultValue());
            case VariableDataType::Float32: return Get(static_cast<const Visual::InputPin<float>&>(pin).GetDefaultValue());
            case VariableDataType::Vector2f32: return Get(static_cast<const Visual::InputPin<Vector2f32>&>(pin).GetDefaultValue());
            case VariableDataType::Vector3f32: return Get(static_cast<const Visual::InputPin<Vector3f32>&>(pin).GetDefaultValue());
            case VariableDataType::Vector4f32: return Get(static_cast<const Visual::InputPin<Vector4f32>&>(pin).GetDefaultValue());
            case VariableDataType::Matrix4x4f32: return Get(static_cast<const Visual::InputPin<Matrix4x4f32>&>(pin).GetDefaultValue());
            default: break;
        }

        return nullptr;
    }

    SpirvGenerator::ConstantPointer SpirvGenerator::ConstantStorage::GetOrCreate(DataTypeStorage& dataTypeStorage, GeneratorInputPin& generatorInputPin)
    {
        const auto& pin = generatorInputPin.pin;
        if (pin.GetDirection() != Visual::PinDirection::In)
        {
            return nullptr;
        }

        switch (pin.GetDataType())
        {
            case VariableDataType::Bool: return GetOrCreate(dataTypeStorage, static_cast<const Visual::InputPin<bool>&>(pin).GetDefaultValue());
            case VariableDataType::Int32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::InputPin<int32_t>&>(pin).GetDefaultValue());
            case VariableDataType::Float32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::InputPin<float>&>(pin).GetDefaultValue());
            case VariableDataType::Vector2f32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::InputPin<Vector2f32>&>(pin).GetDefaultValue());
            case VariableDataType::Vector3f32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::InputPin<Vector3f32>&>(pin).GetDefaultValue());
            case VariableDataType::Vector4f32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::InputPin<Vector4f32>&>(pin).GetDefaultValue());
            case VariableDataType::Matrix4x4f32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::InputPin<Matrix4x4f32>&>(pin).GetDefaultValue());
            default: break;
        }

        return nullptr;
    }

    SpirvGenerator::ConstantPointers SpirvGenerator::ConstantStorage::GetAllDependencySorted()
    {
        ConstantPointers result;

        for (const auto& constantsPair : m_constants)
        {
            switch (constantsPair.first)
            {
                case VariableDataType::Bool:
                case VariableDataType::Int32:
                case VariableDataType::Float32:
                {
                    result.insert(result.end(), constantsPair.second.begin(), constantsPair.second.end());
                } break;
                default: break;
            }
        }

        for (const auto& constantsPair : m_constants)
        {
            switch (constantsPair.first)
            {
                case VariableDataType::Vector2f32:
                case VariableDataType::Vector3f32:
                case VariableDataType::Vector4f32:
                case VariableDataType::Matrix4x4f32:
                {
                    result.insert(result.end(), constantsPair.second.begin(), constantsPair.second.end());
                } break;
                default: break;
            }
        }

        return result;
    }

    void SpirvGenerator::ConstantStorage::Clear()
    {
        m_constants.clear();
    }


    // Debug name storage implementations.
    SpirvGenerator::DebugNameStorage::NameEntry::NameEntry(
        GeneratorOutputPinPointer& generatorOutputPin,
        std::string name,
        const size_t opNameOffset
    ) :
        generatorOutputPin(generatorOutputPin),
        name(std::move(name)),
        opNameOffset(opNameOffset)
    {}

    SpirvGenerator::DebugNameStorage::DebugNameStorage() :
        m_currentOpNameOffset(0)
    {}

    void SpirvGenerator::DebugNameStorage::Add(GeneratorOutputPinPointer& generatorOutputPin, const std::string& name)
    {
        auto counterIt = m_debugNameCounters.find(name);
        if(counterIt == m_debugNameCounters.end())
        {
            counterIt = m_debugNameCounters.insert({ name, 0 }).first;
        }

        auto& counter = counterIt->second;
        std::string nameWithCounter = name + "_" + std::to_string(counter++);

        return AddWithoutCounter(generatorOutputPin, std::move(nameWithCounter));
    }

    void SpirvGenerator::DebugNameStorage::AddWithoutCounter(GeneratorOutputPinPointer& generatorOutputPin, const std::string& name)
    {
        m_entries.emplace_back(generatorOutputPin, name, m_currentOpNameOffset);
        m_currentOpNameOffset += static_cast<size_t>(Spirv::GetLiteralWordCount(name.size())) + 2;
    }

    const SpirvGenerator::DebugNameStorage::NameEntries& SpirvGenerator::DebugNameStorage::GetEntries() const
    {
        return m_entries;
    }

    void SpirvGenerator::DebugNameStorage::Clear()
    {
        m_entries.clear();
        m_debugNameCounters.clear();
        m_currentOpNameOffset = 0;
    }


    // Spirv generator private function implementations.
    void SpirvGenerator::InitGenerator(const Visual::Script& script, const bool includeDebugSymbols)
    {
        m_script = &script;

        m_includeDebugSymbols = includeDebugSymbols;
        m_module.words.clear();
        m_currentId = 0;
        m_capabilities = { Spirv::Capability::Shader };
        m_glslExtension = { GetNextId(), "GLSL.std.450" };
        m_mainEntryPoint = {
            m_script->GetType() == Shader::Type::Vertex ? Spirv::ExecutionModel::Vertex : Spirv::ExecutionModel::Fragment,
            GetNextId(),
            "main",
            {}
        };

        m_voidTypeId = GetNextId();
        m_mainFunctionTypeId = GetNextId();
        m_mainFunctionLabelId = GetNextId();

        m_rootNodes.clear();
        m_createdNodes.clear();
        m_visitedOutputPins.clear();

        m_dataTypeStorage.Clear();
        m_dataTypePointerStorage.Clear();
        m_constantStorage.Clear();   
        m_inputStructure.Clear();
        m_outputStructure.Clear();
        m_debugNameStorage.Clear();
        m_opDebugNameStart = 0;

        m_mainInstructions.clear();
    }

    void SpirvGenerator::InitInterfaces()
    {
        // Add inputs.
        /*const auto& inputInterface = m_script->GetInputInterface();
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
                generatorOutputPin->storageClass = Spirv::StorageClass::Input;
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
        }*/

    }

    bool SpirvGenerator::BuildTree()
    {
        // Initialize root notes from output interface.
        const auto& outputInterface = m_script->GetOutputInterface();
        if (const auto inputPins = outputInterface.GetInputPins(); !inputPins.empty())
        {
            auto generatorNode = std::make_shared<GeneratorNode>(outputInterface);
            
            for (auto& outputNodePin : generatorNode->inputPins)
            {
                const auto& pin = outputNodePin->pin;
                auto dataType = m_dataTypeStorage.GetOrCreate(pin.GetDataType());
                auto dataTypePointer = m_dataTypePointerStorage.GetOrCreate(Spirv::StorageClass::Output, dataType);
                m_outputStructure.members.emplace_back(outputNodePin, std::move(dataType), std::move(dataTypePointer));
            }

            m_rootNodes.push_back(std::move(generatorNode));
        }

        // Traverse and build tree.
        for (auto& outputNode : m_rootNodes)
        {
            std::stack<GeneratorNodePointer> nodeStack;
            nodeStack.push(outputNode); 

            while (!nodeStack.empty())
            {
                auto& currentNode = nodeStack.top();

                if (auto* currentInputPin = currentNode->GetNextInputPin(); currentInputPin)
                {
                    if (auto newNode = BuildVisitInputPin(currentNode, currentInputPin); newNode)
                    {
                        nodeStack.push(newNode);
                    }
                    continue;
                }

                BuildVisitNode(currentNode);

                nodeStack.pop();
            }
        }

        return true;
    }

    SpirvGenerator::GeneratorNodePointer SpirvGenerator::BuildVisitInputPin(GeneratorNodePointer& generatorNode, GeneratorInputPin* generatorInputPin)
    {
        auto* connectedOutputPin = generatorInputPin->pin.GetConnection();

        // No connection, create default value.
        if (!connectedOutputPin)
        {
            if(!m_constantStorage.GetOrCreate(m_dataTypeStorage, *generatorInputPin))
            {
                Logger::WriteError(m_logger, "Failed to create constant for input pin default value, data type: " +
                    std::to_string(static_cast<uint32_t>(generatorInputPin->pin.GetDataType())) + ".");
            }
            return nullptr;
        }

        // Check if connected pin has been created before
        if (const auto it = m_visitedOutputPins.find(connectedOutputPin); it != m_visitedOutputPins.end())
        {
            generatorInputPin->connectedGeneratorOutputPin = it->second.get();
            return nullptr;
        }

        // Get or create new generator node if not yet created.
        auto& connectedNode = connectedOutputPin->GetNode();
        GeneratorNodePointer connectedGeneratorNode;
        bool createdNewGeneratorNode = false;

        if (const auto it = m_createdNodes.find(&connectedNode); it != m_createdNodes.end())
        {
            connectedGeneratorNode = it->second;
        }
        else
        {
            connectedGeneratorNode = CreateGeneratorNode(connectedNode);
            createdNewGeneratorNode = true;
        }

        // Find generator output pin and connect it.
        auto connectedGeneratorOutputPin = connectedGeneratorNode->FindOutputPin(*connectedOutputPin);
        generatorInputPin->connectedGeneratorOutputPin = connectedGeneratorOutputPin.get();

        // Visit output pin.
        BuildVisitOutputPin(connectedGeneratorNode, connectedGeneratorOutputPin);

        // Finished, do not return already created node, causing the BuildTraversal to not handle this node twice.
        return createdNewGeneratorNode ? connectedGeneratorNode : nullptr;
    }

    void SpirvGenerator::BuildVisitOutputPin(GeneratorNodePointer& generatorNode, GeneratorOutputPinPointer& generatorOutputPin)
    {
        generatorOutputPin->isVisited = true;
        m_visitedOutputPins.insert({ &generatorOutputPin->pin, generatorOutputPin });

        const auto& node = generatorNode->node;
        const auto& pin = generatorOutputPin->pin;

        auto dataType = m_dataTypeStorage.GetOrCreate(pin.GetDataType());

        switch(node.GetType())
        {
            case Visual::NodeType::Input:
            {
                generatorOutputPin->storageClass = Spirv::StorageClass::Input;
                auto dataTypePointer = m_dataTypePointerStorage.GetOrCreate(Spirv::StorageClass::Input, dataType);
                m_inputStructure.members.emplace_back(generatorOutputPin, std::move(dataType), std::move(dataTypePointer));
            } break;
            default: break;
        }

        if(m_includeDebugSymbols)
        {
            if(!AddOutputPinDebugName(generatorNode, generatorOutputPin))
            {
                // Error checking here!
            }           
        }
    }

    void SpirvGenerator::BuildVisitNode(GeneratorNodePointer& generatorNode)
    {
        const auto& node = generatorNode->node;
        switch(node.GetType())
        {
            case Visual::NodeType::Function:
            case Visual::NodeType::Operator:
            case Visual::NodeType::Output: m_mainInstructions.push_back(generatorNode); break;
            default: break;
        }
    }

    bool SpirvGenerator::AddOutputPinDebugName(GeneratorNodePointer& generatorNode, GeneratorOutputPinPointer& generatorOutputPin)
    {
        switch (generatorNode->node.GetType())
        {
            case Visual::NodeType::Function: return AddFunctionDebugName(generatorNode, generatorOutputPin);
            case Visual::NodeType::Operator: return AddOperatorDebugName(generatorNode, generatorOutputPin);
            case Visual::NodeType::Input:m_debugNameStorage.Add(generatorOutputPin, "in"); return true;
            default: break;
        }

        return true;
    }

    bool SpirvGenerator::AddFunctionDebugName(GeneratorNodePointer& generatorNode, GeneratorOutputPinPointer& generatorOutputPin)
    {
        const auto& functionBase = static_cast<const Visual::FunctionBase&>(generatorNode->node);
        const auto& functioName = GetFunctionName(functionBase.GetFunctionType());
        if (functioName.empty())
        {
            return false;
        }

        m_debugNameStorage.Add(generatorOutputPin, functioName);
        return true;
    }

    bool SpirvGenerator::AddOperatorDebugName(GeneratorNodePointer& generatorNode, GeneratorOutputPinPointer& generatorOutputPin)
    {
        const auto& operatorBase = static_cast<const Visual::OperatorBase&>(generatorNode->node);
        const auto& operatorName = GetOperatorName(operatorBase);
        if (operatorName.empty())
        {
            return false;
        }

        m_debugNameStorage.Add(generatorOutputPin, operatorName);
        return true;
    }

    /*
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

        return true;
    }
    */

    SpirvGenerator::GeneratorNodePointer SpirvGenerator::CreateGeneratorNode(const Visual::Node& node)
    {
        auto generatorNode = std::make_shared<GeneratorNode>(node);
        m_createdNodes.insert({ &node, generatorNode });

        for(auto& generatorInputPin : generatorNode->inputPins)
        {
            m_dataTypeStorage.GetOrCreate(generatorInputPin->pin.GetDataType());
        }

        return generatorNode;
    }

    Spirv::Id SpirvGenerator::GetNextId()
    {
        return ++m_currentId;
    }

    bool SpirvGenerator::AccessNodeInputInMainFunction(GeneratorInputPin& generatorInputPin, std::vector<Spirv::Id>& inputIds)
    {
        if (auto* connectedOutputPin = generatorInputPin.connectedGeneratorOutputPin; connectedOutputPin)
        {
            const auto inputId = AccessOrTransformStorageClass(*connectedOutputPin, Spirv::StorageClass::Function);
            inputIds.push_back(inputId);
        }
        else
        {
            const auto constant = m_constantStorage.Get(generatorInputPin);
            if (!constant)
            {
                Logger::WriteError(m_logger, "Failed to find constant input value for function.");
                return false;
            }

            inputIds.push_back(constant->id);
        }

        return true;
    }

    Spirv::Id SpirvGenerator::AccessOrTransformStorageClass(GeneratorOutputPin& generatorOutputPin, const Spirv::StorageClass storageClass)
    {
        if(generatorOutputPin.storageClass != storageClass)
        {
            const auto outputDataType = m_dataTypeStorage.Get(generatorOutputPin.pin.GetDataType());
            const auto newId = GetNextId();

            m_module.AddOpLoad(outputDataType->id, newId, generatorOutputPin.id);

            generatorOutputPin.id = newId;
            generatorOutputPin.storageClass = storageClass;
        }

        return generatorOutputPin.id;
    }

    bool SpirvGenerator::WriteModule()
    {


        // Create debug names
        /*if(m_includeDebugSymbols)
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
*/

        UpdateDataTypeIds();
        UpdateInputPointerIds();
        UpdateInputIds();
        UpdateOutputPointerIds();
        UpdateOutputIds();

        m_module.words.clear();

        m_module.AddHeader(
            (Spirv::Word{ 1 } << 16),   // Version 1.0
            0x4d6f6c74,                 // Generator magic number: "Molt"
            0);                  // Max id bound - Update later.

        for (const auto& capability : m_capabilities)
        {
            m_module.AddOpCapability(capability);
        }

        m_module.AddOpExtInstImport(m_glslExtension);

        m_module.AddOpMemoryModel(Spirv::AddressingModel::Logical, Spirv::MemoryModel::Glsl450);
        
        for (const auto& member : m_inputStructure.members)
        {
            m_mainEntryPoint.interface.push_back(member.outputPin->id);
        }
        for (const auto& member : m_outputStructure.members)
        {
            m_mainEntryPoint.interface.push_back(member.id);
        }

        m_module.AddOpEntryPoint(m_mainEntryPoint);
        m_module.AddOpExecutionMode(m_mainEntryPoint.id, Spirv::ExecutionMode::OriginUpperLeft);

        if(m_includeDebugSymbols)
        {
            WriteDebugNames();
        }

        WriteDecorations();

        if (!WriteDataTypes())
        {
            return false;
        }

        WriteInputPointers();
        WriteInputs();
        WriteOutputPointers();
        WriteOutputs();
  
        if (!WriteConstants())
        {
            return false;
        }

        


        /*
        

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

        */

        // Create main functions
        m_module.AddOpFunction(m_mainEntryPoint.id, m_voidTypeId, Spirv::FunctionControl::None, m_mainFunctionTypeId);
        m_module.AddOpLabel(m_mainFunctionLabelId);

        if(!WriteMainInstructions())
        {
            return false;
        }

        m_module.AddOpReturn();
        m_module.AddOpFunctionEnd();

        m_module.UpdateIdBound(m_currentId + 1);

        if(m_includeDebugSymbols)
        {
            UpdateDebugNames();
        }

        return true;
    }

    void SpirvGenerator::UpdateDataTypeIds()
    {
        for (auto& dataType : m_dataTypeStorage.GetAllDependencySorted())
        {
            dataType->id = GetNextId();
        }
    }

    void SpirvGenerator::UpdateInputPointerIds()
    {
        for (auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::Input))
        {
            dataTypePointers->id = GetNextId();
        }
    }

    void SpirvGenerator::UpdateInputIds()
    {
        for (auto& member : m_inputStructure.members)
        {
            member.outputPin->id = GetNextId();
        }
    }

    void SpirvGenerator::UpdateOutputPointerIds()
    {
        for (auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::Output))
        {
            dataTypePointers->id = GetNextId();
        }
    }

    void SpirvGenerator::UpdateOutputIds()
    {
        for (auto& member : m_outputStructure.members)
        {
            member.id = GetNextId();
        }
    }

    void SpirvGenerator::WriteDebugNames()
    {
        m_module.AddOpName(m_mainEntryPoint.id, m_mainEntryPoint.name);

        m_opDebugNameStart = m_module.words.size();

        for(const auto& entry : m_debugNameStorage.GetEntries())
        {
            m_module.AddOpName(0, entry.name);
        }
    }

    void SpirvGenerator::WriteDecorations()
    {
        WriteInputDecorations();
        WriteOutputDecorations();
    }

    void SpirvGenerator::WriteInputDecorations()
    {
        Spirv::Word location = 0;
        for (const auto& member : m_inputStructure.members)
        {
            m_module.AddOpDecorateLocation(member.outputPin->id, location);
            ++location;
        }
    }

    void SpirvGenerator::WriteOutputDecorations()
    {
        Spirv::Word location = 0;
        for (const auto& member : m_outputStructure.members)
        {
            m_module.AddOpDecorateLocation(member.id, location);
            ++location;
        }
    }

    bool SpirvGenerator::WriteDataTypes()
    {
        m_module.AddOpTypeVoid(m_voidTypeId);
        m_module.AddOpTypeFunction(m_mainFunctionTypeId, m_voidTypeId);

        for (const auto& dataType : m_dataTypeStorage.GetAllDependencySorted())
        {
            if(dataType->id == 0)
            {
                Logger::WriteError(m_logger, "Failed to write data type due to id being 0.");
                return false;
            }

            switch (const auto type = dataType->type; type)
            {
                case VariableDataType::Bool:
                {
                    m_module.AddOpTypeBool(dataType->id);
                } break;
                case VariableDataType::Int32:
                {
                    m_module.AddOpTypeInt32(dataType->id, Spirv::Signedness::Signed);
                } break;
                case VariableDataType::Float32:
                {
                    m_module.AddOpTypeFloat32(dataType->id);
                } break;
                case VariableDataType::Vector2f32:
                {
                    const auto componentDataType = m_dataTypeStorage.Get(VariableDataType::Float32);
                    if (!componentDataType)
                    {
                        Logger::WriteError(m_logger, "Failed to find component data type for Vector2f32:" +
                            std::to_string(static_cast<int32_t>(VariableDataType::Float32)) + ".");
                        return false;
                    }
                    m_module.AddOpTypeVector(dataType->id, componentDataType->id, 2);
                } break;
                case VariableDataType::Vector3f32:
                {
                    const auto componentDataType = m_dataTypeStorage.Get(VariableDataType::Float32);
                    if (!componentDataType)
                    {
                        Logger::WriteError(m_logger, "Failed to find component data type for Vector3f32:" +
                            std::to_string(static_cast<int32_t>(VariableDataType::Float32)) + ".");
                        return false;
                    }
                    m_module.AddOpTypeVector(dataType->id, componentDataType->id, 3);
                } break;
                case VariableDataType::Vector4f32:
                {
                    const auto componentDataType = m_dataTypeStorage.Get(VariableDataType::Float32);
                    if (!componentDataType)
                    {
                        Logger::WriteError(m_logger, "Failed to find component data type for Vector4f32:" +
                            std::to_string(static_cast<int32_t>(VariableDataType::Float32)) + ".");
                        return false;
                    }
                    m_module.AddOpTypeVector(dataType->id, componentDataType->id, 4);
                } break;
                default:
                {
                    Logger::WriteError(m_logger, "Unsupported data type:" +
                        std::to_string(static_cast<int32_t>(type)) + ".");
                    return false;
                }
            }
        }
        return true;
    }

    void SpirvGenerator::WriteInputPointers()
    {
        for (const auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::Input))
        {
            m_module.AddOpTypePointer(dataTypePointers->id, Spirv::StorageClass::Input, dataTypePointers->dataType->id);
        }
    }

    void SpirvGenerator::WriteInputs()
    {
        for (const auto& member : m_inputStructure.members)
        {
            m_module.AddOpVariable(member.outputPin->id, member.dataTypePointer->id, Spirv::StorageClass::Input);
        }
    }

    void SpirvGenerator::WriteOutputPointers()
    {
        for (const auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::Output))
        {
            m_module.AddOpTypePointer(dataTypePointers->id, Spirv::StorageClass::Output, dataTypePointers->dataType->id);
        }
    }

    void SpirvGenerator::WriteOutputs()
    {
        for (const auto& member : m_outputStructure.members)
        {
            m_module.AddOpVariable(member.id, member.dataTypePointer->id, Spirv::StorageClass::Output);
        }
    }

    bool SpirvGenerator::WriteConstants()
    {
        auto getVectorValueIds = [&](const auto* value)
        {
            using VectorType = std::remove_pointer_t<decltype(value)>;
            using VectorIdType = Vector<VectorType::Dimensions, Spirv::Id>;

            if(!value)
            {
                Logger::WriteError(m_logger, "Stored constant vector value is not of type vector.");
                return std::optional<VectorIdType>{};
            }

            VectorIdType valueIds = {};
            for (size_t i = 0; i < VectorIdType::Dimensions; i++)
            {
                const auto scalarConstant = m_constantStorage.Get(value->c[i]);
                if (!scalarConstant)
                {
                    Logger::WriteError(m_logger, "Failed to find scalar constant value for vector constant.");
                    return std::optional<VectorIdType>{};
                }

                valueIds.c[i] = scalarConstant->id;
            }

            return std::optional<VectorIdType>{ valueIds };
        };

        for (const auto& constant : m_constantStorage.GetAllDependencySorted())
        {
            constant->id = GetNextId();
           
            switch(const auto type = constant->dataType->type; type)
            {
                case VariableDataType::Bool:
                {
                    const auto* value = std::get_if<bool>(&constant->value);
                    if (!value)
                    {
                        Logger::WriteError(m_logger, "Value of bool constant is not of type bool.");
                        return false;
                    }

                    m_module.AddOpConstantBool(constant->id, constant->dataType->id, *value);
                } break;
                case VariableDataType::Int32:
                {
                    const auto* value = std::get_if<int32_t>(&constant->value);
                    if (!value)
                    {
                        Logger::WriteError(m_logger, "Value of int32_t constant is not of type int32_t.");
                        return false;
                    }

                    m_module.AddOpConstantInt32(constant->id, constant->dataType->id, *value);
                } break;
                case VariableDataType::Float32:
                {
                    const auto* value = std::get_if<float>(&constant->value);
                    if (!value)
                    {
                        Logger::WriteError(m_logger, "Value of float constant is not of type float.");
                        return false;
                    }

                    m_module.AddOpConstantFloat32(constant->id, constant->dataType->id, *value);
                } break;
                case VariableDataType::Vector2f32:
                {
                    const auto valueIds = getVectorValueIds(std::get_if<Vector2f32>(&constant->value));
                    if (!valueIds.has_value())
                    {
                        return false;
                    }

                    m_module.AddOpConstantVector2(constant->id, constant->dataType->id, valueIds.value());
                } break;
                case VariableDataType::Vector3f32:
                {
                    const auto valueIds = getVectorValueIds(std::get_if<Vector3f32>(&constant->value));
                    if (!valueIds.has_value())
                    {
                        return false;
                    }

                    m_module.AddOpConstantVector3(constant->id, constant->dataType->id, valueIds.value());
                } break;
                case VariableDataType::Vector4f32:
                {
                    const auto valueIds = getVectorValueIds(std::get_if<Vector4f32>(&constant->value));
                    if(!valueIds.has_value())
                    {
                        return false;
                    }                  

                    m_module.AddOpConstantVector4(constant->id, constant->dataType->id, valueIds.value());
                } break;
                default:
                {
                    Logger::WriteError(m_logger, "Unsupported constant data type:" +
                        std::to_string(static_cast<int32_t>(type)) + ".");
                    return false;
                }
            }
        }

        return true;
    }

    bool SpirvGenerator::WriteMainInstructions()
    {
        for (const auto& mainInstruction : m_mainInstructions)
        {
            if(!WriteMainInstruction(mainInstruction))
            {
                return false;
            }
        }

        return true;
    }

    bool SpirvGenerator::WriteMainInstruction(const GeneratorNodePointer& generatorNode)
    {
        switch (generatorNode->node.GetType())
        {
            case Visual::NodeType::Function: return WriteFunction(generatorNode, static_cast<const Visual::FunctionBase&>(generatorNode->node));
            case Visual::NodeType::Operator: return WriteOperator(generatorNode, static_cast<const Visual::OperatorBase&>(generatorNode->node));
            case Visual::NodeType::Output: return WriteOutput(generatorNode, static_cast<const Visual::OutputInterface&>(generatorNode->node));
            default: break;
        }
    
        return true; // TODO: SWITCH TO FALSE!.
    }

    bool SpirvGenerator::WriteFunction(const GeneratorNodePointer& generatorNode, const Visual::FunctionBase& functionBase)
    {
        if(const auto outputPinCount = generatorNode->outputPins.size(); outputPinCount != 1)
        {
            Logger::WriteError(m_logger, "Function expects 1 output pin, found " + std::to_string(outputPinCount) + ".");
            return false;
        }

        const auto& outputPin = generatorNode->outputPins.front();
        const auto outputDataType = m_dataTypeStorage.Get(outputPin->pin.GetDataType());

        std::vector<Spirv::Id> inputIds;
        for(const auto& generatorInputPin : generatorNode->inputPins)
        {
            if(!AccessNodeInputInMainFunction(*generatorInputPin, inputIds))
            {
                return false;
            }
        }

        outputPin->id = GetNextId();

        const auto functionType = functionBase.GetFunctionType();
      
        if(const auto glslFunction = GetGlslFunction(functionType); glslFunction.has_value())
        {
            const auto instruction = glslFunction.value();
            m_module.AddOpExtInst(
                outputDataType->id,
                outputPin->id,
                m_glslExtension.resultId,
                instruction,
                inputIds);

            return true;
        }
        /*else
        {
            return false;
        }*/   

        return false;
    }

    bool SpirvGenerator::WriteOperator(const GeneratorNodePointer& generatorNode, const Visual::OperatorBase& operatorBase)
    {
        switch (operatorBase.GetOperatorType())
        {
            case Visual::OperatorType::Arithmetic: return WriteOperatorArithmetic(generatorNode, static_cast<const Visual::ArithmeticOperatorBase&>(operatorBase));
            default: break;
        }

        return false;
    }

    bool SpirvGenerator::WriteOperatorArithmetic(const GeneratorNodePointer& generatorNode, const Visual::ArithmeticOperatorBase& arithmeticOperatorBase)
    {
        if (const auto outputPinCount = generatorNode->outputPins.size(); outputPinCount != 1)
        {
            Logger::WriteError(m_logger, "Arithmetic operator expects 1 output pin, found " + std::to_string(outputPinCount) + ".");
            return false;
        }
        if (const auto inputPinCount = generatorNode->inputPins.size(); inputPinCount != 2)
        {
            Logger::WriteError(m_logger, "Arithmetic operator expects 2 input pins, found " + std::to_string(inputPinCount) + ".");
            return false;
        }
  
        const auto& outputPin = generatorNode->outputPins.front();
        const auto outputDataType = m_dataTypeStorage.Get(outputPin->pin.GetDataType());
 
        std::vector<Spirv::Id> inputIds;
        for (const auto& generatorInputPin : generatorNode->inputPins)
        {
            if (!AccessNodeInputInMainFunction(*generatorInputPin, inputIds))
            {
                return false;
            }
        }

        outputPin->id = GetNextId();

        switch(arithmeticOperatorBase.GetArithmeticOperatorType())
        {
            case Visual::ArithmeticOperatorType::Addition: m_module.AddOpFAdd(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            case Visual::ArithmeticOperatorType::Division: m_module.AddOpFDiv(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            case Visual::ArithmeticOperatorType::Multiplication: m_module.AddOpFMul(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            case Visual::ArithmeticOperatorType::Subtraction: m_module.AddOpFSub(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            default: return false;
        }
        
        return true;
    }

    bool SpirvGenerator::WriteOutput(const GeneratorNodePointer& generatorNode, const Visual::OutputInterface& outputInterface)
    {
        for (auto& generatorInputPin : generatorNode->inputPins)
        {
            std::vector<Spirv::Id> inputIds;
            if (!AccessNodeInputInMainFunction(*generatorInputPin, inputIds))
            {
                return false;
            }

            const auto* member = m_outputStructure.FindMember(generatorInputPin);
            if(!member)
            {
                Logger::WriteError(m_logger, "Failed to find output interface member.");
                return false;
            }

            m_module.AddOpStore(member->id, inputIds[0]);
        }
        return true;
    }

    void SpirvGenerator::UpdateDebugNames()
    {
        for (const auto& entry : m_debugNameStorage.GetEntries())
        {
            const auto offset = m_opDebugNameStart + entry.opNameOffset + 1;
            m_module.words[offset] = static_cast<Spirv::Word>(entry.generatorOutputPin->id);
        }
    }

}