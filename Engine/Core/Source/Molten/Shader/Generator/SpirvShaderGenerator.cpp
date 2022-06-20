/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#include "Molten/Shader/Generator/SpirvShaderGenerator.hpp"
#include "Molten/Logger.hpp"
#include <stack>
#include <algorithm>

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

    static size_t GetVariableComponentCount(const VariableDataType dataType)
    {
        switch (dataType)
        {
            case VariableDataType::Bool:         return 1;
            case VariableDataType::Int32:        return 1;
            case VariableDataType::Float32:      return 1;
            case VariableDataType::Vector2f32:   return 2;
            case VariableDataType::Vector3f32:   return 3;
            case VariableDataType::Vector4f32:   return 4;
            case VariableDataType::Matrix4x4f32:
            case VariableDataType::Sampler1D:
            case VariableDataType::Sampler2D:
            case VariableDataType::Sampler3D: break;
        }
        return 0;
    }

    static VariableDataType GetVariableComponentType(const VariableDataType dataType)
    {
        switch (dataType)
        {
            case VariableDataType::Vector2f32:
            case VariableDataType::Vector3f32:
            case VariableDataType::Vector4f32:
            case VariableDataType::Matrix4x4f32: return VariableDataType::Float32;
            default: break;
        }
        return dataType;
    }

    static const std::string& GetArithmeticOperatorName(const Visual::ArithmeticOperatorType arithmeticOperatorType)
    {
        switch (arithmeticOperatorType)
        {
            case Visual::ArithmeticOperatorType::Addition: return g_arithmeticOperatorNameAdd;
            case Visual::ArithmeticOperatorType::Division: return g_arithmeticOperatorNameDiv;
            case Visual::ArithmeticOperatorType::Multiplication: return g_arithmeticOperatorNameMul;
            case Visual::ArithmeticOperatorType::Subtraction: return g_arithmeticOperatorNameSub;
            default: break;
        }      

        return g_emptyString;
    }

    static Spirv::OpCode GetArithmeticOperatorOpCode(const Visual::ArithmeticOperatorType arithmeticOperatorType, const VariableDataType leftType, const VariableDataType rightType)
    {
        switch (arithmeticOperatorType)
        {
            case Visual::ArithmeticOperatorType::Addition: return Spirv::OpCode::FAdd;
            case Visual::ArithmeticOperatorType::Division: return Spirv::OpCode::FDiv;
            case Visual::ArithmeticOperatorType::Multiplication:
            {
                if (leftType == VariableDataType::Matrix4x4f32 && rightType == VariableDataType::Vector4f32)
                {
                    return Spirv::OpCode::MatrixTimesVector;
                }
                if (leftType == VariableDataType::Vector4f32 && rightType == VariableDataType::Matrix4x4f32)
                {
                    return Spirv::OpCode::VectorTimesMatrix;
                }
                return Spirv::OpCode::FMul;
            }
            case Visual::ArithmeticOperatorType::Subtraction: return Spirv::OpCode::FSub;
            default: break;
        }

        return Spirv::OpCode::Nop;
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
    SpirvGenerator::SpirvGenerator(Logger* logger) :
        m_logger(logger),
        m_descriptor(nullptr),
        m_currentId(0),
        m_glslExtension{},
        m_mainEntryPoint{},
        m_voidTypeId(0),
        m_mainFunctionTypeId(0),
        m_mainFunctionLabelId(0),
        m_pushConstantStructure(Spirv::StorageClass::PushConstant)
    {}

    bool SpirvGenerator::CreateCombinedShaderTemplate(
        SpirvCombinedShadersTemplate& combinedShadersTemplate,
        std::vector<const Visual::Script*> scripts,
        Logger* logger)
    {
        auto& locations = combinedShadersTemplate.pushConstantLocations;
        locations.clear();

        // Loop scripts and add location data types to output.
        // Data types of of same push constant id's are evaluated and error checked.
        size_t scriptIndex = 0;
        for (const auto* script : scripts)
        {
            auto& pushConstants = script->GetPushConstantsBase();
            const size_t pushConstantCount = pushConstants.GetMemberCount();

            for (size_t i = 0; i < pushConstantCount; i++)
            {
                auto* pushConstant = pushConstants.GetMemberBase(i);
                if (pushConstant == nullptr)
                {
                    Logger::WriteError(logger, "Push constant at id " + std::to_string(i) +
                        " of script no " + std::to_string(scriptIndex) + " is nullptr.");
                    return false;
                }

                const auto id = pushConstant->GetId();
                const auto dataType = pushConstant->GetPin().GetDataType();

                auto it = locations.find(id);
                if (it == locations.end())
                {
                    locations.insert({ id, { id, 0, dataType } });
                }
                else
                {
                    auto& pushConstantLocation = it->second;
                    if (pushConstantLocation.dataType != dataType)
                    {
                        Logger::WriteError(logger, "Push constant at id " + std::to_string(i) +
                            " already exists with different data type: Exists: " +
                            std::to_string(static_cast<int32_t>(pushConstantLocation.dataType)) +
                            ", New: " + std::to_string(static_cast<int32_t>(dataType)) + ".");
                        return false;
                    }
                }
            }

            // Calculate offsets(location).
            size_t offset = 0;
            for (auto& locationPair : locations)
            {
                auto& location = locationPair.second;
                location.location = static_cast<uint32_t>(offset);
                offset += std::max(size_t{ 16 }, GetVariableByteOffset(location.dataType));
            }
            combinedShadersTemplate.pushConstantBlockByteCount = static_cast<uint32_t>(offset);

            ++scriptIndex;
        }

        // Map descriptor sets and their bindings.
        auto& mappedDescriptorSets = combinedShadersTemplate.mappedDescriptorSets;
        mappedDescriptorSets.clear();

        for (const auto* script : scripts)
        {
            const auto& sets = script->GetDescriptorSetsBase();
            const size_t setCount = sets.GetSetCount();
            for (size_t i = 0; i < setCount; i++)
            {
                const auto* set = sets.GetSetBase(i);
                const size_t bindingCount = set->GetBindingCount();

                if (bindingCount == 0)
                {
                    continue;
                }
                auto& mappedSet = mappedDescriptorSets.insert({ set->GetId(), {} }).first->second;

                for (size_t j = 0; j < bindingCount; j++)
                {
                    const auto* binding = set->GetBindingBase(j);
                    mappedSet.bindings.insert({ binding->GetId(), { 0, binding->GetBindingType() } });
                }
            }
        }

        uint32_t setIndex = 0;
        for (auto& mappedDescriptorSetPair : mappedDescriptorSets)
        {
            auto& mappedDescriptorSet = mappedDescriptorSetPair.second;
            mappedDescriptorSet.index = setIndex++;

            uint32_t bindingIndex = 0;
            for (auto& mappedBindingPair : mappedDescriptorSet.bindings)
            {
                auto& mappedBinding = mappedBindingPair.second;
                mappedBinding.index = bindingIndex++;
            }
        }

        return true;
    }

    SpirvGeneratoResult SpirvGenerator::Generate(const SpirvGeneratorDescriptor& descriptor)
    {
        if(!InitGenerator(descriptor))
        {
            return {};
        }

        if(!BuildTree())
        {
            return {};
        }

        if(!WriteModule())
        {
            return {};
        }


        return { std::move(m_module.words), m_inputStructure.GetUnusedMemberIndices() };
    }


    // Generator output pin implementations.
    SpirvGenerator::GeneratorInputPin::GeneratorInputPin(const Visual::Pin& pin) :
        pin(pin),
        connectedGeneratorOutputPin(nullptr)
    {}


    // Generator output pin implementations.
    SpirvGenerator::GeneratorOutputPin::GeneratorOutputPin(const Visual::Pin& pin) :
        pin(pin),
        id(0),
        storageClass(Spirv::StorageClass::Function)
    {}


    // Generator node implementations.
    SpirvGenerator::GeneratorNode::GeneratorNode(const Visual::Node& node) :
        node(node),
        inputPins(CreateInputPins(node)),
        inputPinIterator(inputPins.begin()),
        outputPins(CreateOutputPins(node))
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
        for (auto& outputPin : outputPins)
        {
            if (&outputPin->pin == &pin)
            {
                return outputPin;
            }
        }

        return nullptr;
    }

    SpirvGenerator::GeneratorNode::GeneratorInputPinPointers SpirvGenerator::GeneratorNode::CreateInputPins(const Visual::Node& parentNode)
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

    SpirvGenerator::GeneratorNode::GeneratorOutputPinPointers SpirvGenerator::GeneratorNode::CreateOutputPins(const Visual::Node& parentNode)
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


    // Input structure implementations.
    SpirvGenerator::InputStructure::Member::Member(const Visual::Pin* pin) :
        pin(pin),
        outputPin{},
        dataType{},
        dataTypePointer{}
    {}

    void SpirvGenerator::InputStructure::Initialize(const Visual::InputInterface& inputInterface)
    {
        for(const auto& input : inputInterface)
        {
            m_members.emplace_back(&input->GetPin());
        }
    }

    void SpirvGenerator::InputStructure::AddMember(
        DataTypeStorage& dataTypeStorage,
        DataTypePointerStorage& dataTypePointerStorage,
        GeneratorOutputPinPointer& generatorOutputPin)
    {
        const auto& pin = generatorOutputPin->pin;

        const auto it = std::find_if(m_members.begin(), m_members.end(), [&](const auto& member)
        {
            return member.pin == &pin;
        });

        if(it == m_members.end())
        {
            return;
        }

        auto& member = *it;
        if(member.outputPin)
        {
            return;
        }

        member.dataType = dataTypeStorage.GetOrCreate(pin.GetDataType());
        member.dataTypePointer = dataTypePointerStorage.GetOrCreate(Spirv::StorageClass::Input, member.dataType);
        member.outputPin = generatorOutputPin;

        generatorOutputPin->storageClass = Spirv::StorageClass::Input;
    }

    std::vector<size_t> SpirvGenerator::InputStructure::GetUnusedMemberIndices()
    {
        std::vector<size_t> indices;

        for (size_t i = 0; i < m_members.size(); i++)
        {
            if (m_members[i].outputPin == nullptr)
            {
                indices.push_back(i);
            }
        }

        return indices;
    }

    SpirvGenerator::InputStructure::Members SpirvGenerator::InputStructure::GetMembers()
    {
        Members result;

        for(auto& member : m_members)
        {
            if(member.outputPin)
            {
                result.push_back(member);
            }
        }

        return result;
    }

    void SpirvGenerator::InputStructure::Clear()
    {
        m_members.clear();
    }


    // Output structure implementations.
    SpirvGenerator::OutputStructure::Member::Member(
        GeneratorInputPinPointer inputPin,
        DataTypePointer dataType,
        DataTypePtrPointer dataTypePointer
    ) :
        id(0),
        inputPin(std::move(inputPin)),
        dataType(std::move(dataType)),
        dataTypePointer(std::move(dataTypePointer))
    {}

    SpirvGenerator::OutputStructure::Member* SpirvGenerator::OutputStructure::FindMember(GeneratorInputPinPointer& inputPin)
    {
        const auto it = std::find_if(members.begin(), members.end(), [&](const auto& member)
        {
            return member.inputPin == inputPin;
        });

        return it != members.end() ? &(*it) : nullptr;
    }

    void SpirvGenerator::OutputStructure::Clear()
    {
        members.clear();
    }


    // structure implementations.
    SpirvGenerator::Structure::Member::Member(
        Structure* structure,
        GeneratorOutputPinPointer outputPin,
        DataTypePointer dataType,
        DataTypePtrPointer dataTypePointer
    ) :
        structure(structure),
        id(0),
        index(0),
        offset(0),
        outputPin(std::move(outputPin)),
        dataType(std::move(dataType)),
        dataTypePointer(std::move(dataTypePointer))
    {}

    SpirvGenerator::Structure::Structure(const Spirv::StorageClass storageClass) :
        id(0),
        typeId(0),
        typePointerId(0),
        isEmpty(true),
        storageClass(storageClass)
    {}

    void SpirvGenerator::Structure::AddMember(
        DataTypeStorage& dataTypeStorage,
        DataTypePointerStorage& dataTypePointerStorage,
        GeneratorOutputPinPointer& generatorOutputPin)
    {
        const auto& pin = generatorOutputPin->pin;
        auto dataType = dataTypeStorage.GetOrCreate(pin.GetDataType());
        auto dataTypePointer = dataTypePointerStorage.GetOrCreate(storageClass, dataType);

        members.emplace_back(this, generatorOutputPin, std::move(dataType), std::move(dataTypePointer));
        isEmpty = false;

        generatorOutputPin->storageClass = storageClass;
    }

    SpirvGenerator::Structure::Member* SpirvGenerator::Structure::FindMember(const Visual::Pin* pin)
    {
        const auto it = std::find_if(members.begin(), members.end(), [&](const auto& member)
        {
            return &member.outputPin->pin == pin;
        });

        return it != members.end() ? &(*it) : nullptr;
    }

    void SpirvGenerator::Structure::Clear()
    {
        id = 0;
        typeId = 0;
        typePointerId = 0;
        members.clear();
        isEmpty = true;
    }


    // Vertex output struct implementations.
    SpirvGenerator::VertexOutputStructure::VertexOutputStructure() :
        id(0),
        typeId(0),
        typePointerId(0),
        isEmpty(true)
    {}

    void SpirvGenerator::VertexOutputStructure::Clear()
    {
        id = 0;
        typeId = 0;
        typePointerId = 0;
        position.Clear();
        isEmpty = true;
    }

    void SpirvGenerator::VertexOutputStructure::Position::Clear()
    {
        dataType = nullptr;
        dataTypePointer = nullptr;
        indexConstantPointer = nullptr;
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
            case VariableDataType::Sampler1D:
            case VariableDataType::Sampler2D:
            case VariableDataType::Sampler3D: GetOrCreate(VariableDataType::Float32); break;
            case VariableDataType::Matrix4x4f32: GetOrCreate(VariableDataType::Vector4f32); break;
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
                {
                    result.push_back(dataTypePair.dataType);
                } break;
                default: break;
            }
        }

        // Matrix types:
        for (const auto& dataTypePair : m_dataTypes)
        {
            switch (dataTypePair.type)
            {
                case VariableDataType::Matrix4x4f32:
                {
                    result.push_back(dataTypePair.dataType);
                } break;
                default: break;
            }
        }

        // Sampler types.
        for (const auto& dataTypePair : m_dataTypes)
        {
            switch (dataTypePair.type)
            {
                case VariableDataType::Sampler1D:
                case VariableDataType::Sampler2D:
                case VariableDataType::Sampler3D:
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
    SpirvGenerator::ConstantPointer SpirvGenerator::ConstantStorage::Get(const GeneratorInputPin& generatorInputPin)
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

    SpirvGenerator::ConstantPointer SpirvGenerator::ConstantStorage::Get(const Visual::ConstantBase& constantBase)
    {
        switch (constantBase.GetDataType())
        {
            case VariableDataType::Bool: return Get(static_cast<const Visual::Constant<bool>&>(constantBase).GetValue());
            case VariableDataType::Int32: return Get(static_cast<const Visual::Constant<int32_t>&>(constantBase).GetValue());
            case VariableDataType::Float32: return Get(static_cast<const Visual::Constant<float>&>(constantBase).GetValue());
            case VariableDataType::Vector2f32: return Get(static_cast<const Visual::Constant<Vector2f32>&>(constantBase).GetValue());
            case VariableDataType::Vector3f32: return Get(static_cast<const Visual::Constant<Vector3f32>&>(constantBase).GetValue());
            case VariableDataType::Vector4f32: return Get(static_cast<const Visual::Constant<Vector4f32>&>(constantBase).GetValue());
            case VariableDataType::Matrix4x4f32: return Get(static_cast<const Visual::Constant<Matrix4x4f32>&>(constantBase).GetValue());
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

    SpirvGenerator::ConstantPointer SpirvGenerator::ConstantStorage::GetOrCreate(DataTypeStorage& dataTypeStorage, const Visual::ConstantBase& constantBase)
    {
        switch (constantBase.GetDataType())
        {
            case VariableDataType::Bool: return GetOrCreate(dataTypeStorage, static_cast<const Visual::Constant<bool>&>(constantBase).GetValue());
            case VariableDataType::Int32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::Constant<int32_t>&>(constantBase).GetValue());
            case VariableDataType::Float32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::Constant<float>&>(constantBase).GetValue());
            case VariableDataType::Vector2f32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::Constant<Vector2f32>&>(constantBase).GetValue());
            case VariableDataType::Vector3f32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::Constant<Vector3f32>&>(constantBase).GetValue());
            case VariableDataType::Vector4f32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::Constant<Vector4f32>&>(constantBase).GetValue());
            case VariableDataType::Matrix4x4f32: return GetOrCreate(dataTypeStorage, static_cast<const Visual::Constant<Matrix4x4f32>&>(constantBase).GetValue());
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


    // Uniform storage implementations.
    SpirvGenerator::SamplerStorage::Sampler::Sampler(
        const Spirv::Id setId,
        const Spirv::Id bindingId,
        GeneratorOutputPinPointer outputPin,
        DataTypePointer dataType,
        DataTypePtrPointer dataTypePointer
    ) :
        setId(setId),
        bindingId(bindingId),
        outputPin(std::move(outputPin)),
        dataType(std::move(dataType)),
        dataTypePointer(std::move(dataTypePointer))
    {}

    void SpirvGenerator::SamplerStorage::Add(
        DataTypeStorage& dataTypeStorage,
        DataTypePointerStorage& dataTypePointerStorage,
        GeneratorOutputPinPointer& generatorOutputPin,
        const Visual::DescriptorBindingBase& descriptorBindingBase)
    {
        const auto setId = descriptorBindingBase.GetSet().GetId();
        const auto bindingId = descriptorBindingBase.GetId();

        const auto& pin = generatorOutputPin->pin;
        auto dataType = dataTypeStorage.GetOrCreate(pin.GetDataType());
        auto dataTypePointer = dataTypePointerStorage.GetOrCreate(Spirv::StorageClass::UniformConstant, dataType);

        m_samplers.emplace_back(setId, bindingId, generatorOutputPin, std::move(dataType), std::move(dataTypePointer));

        generatorOutputPin->storageClass = Spirv::StorageClass::UniformConstant;
    }

    SpirvGenerator::SamplerStorage::Samplers& SpirvGenerator::SamplerStorage::GetSamplers()
    {
        return m_samplers;
    }

    void SpirvGenerator::SamplerStorage::Clear()
    {
        m_samplers.clear();
    }


    // Uniform buffer storage.
    void SpirvGenerator::UniformBufferStorage::Add(
        DataTypeStorage& dataTypeStorage,
        DataTypePointerStorage& dataTypePointerStorage,
        GeneratorOutputPinPointer& generatorOutputPin,
        const Visual::DescriptorBindingBase& descriptorBindingBase)
    {
        const auto& set = descriptorBindingBase.GetSet();

        const auto idPair = SetAndBindingPair{ set.GetId(), descriptorBindingBase.GetId() };
        auto it = m_uniformBufferMap.find(idPair);
        if(it == m_uniformBufferMap.end())
        {
            it = m_uniformBufferMap.insert({ idPair, std::make_shared<Structure>(Spirv::StorageClass::Uniform) }).first;
        }

        auto& structure = it->second;
        structure->AddMember(dataTypeStorage, dataTypePointerStorage, generatorOutputPin);
    }

    SpirvGenerator::Structure::Member* SpirvGenerator::UniformBufferStorage::FindMember(const Visual::Pin* pin)
    {
        for(auto& uniformBuffer : m_uniformBufferMap)
        {      
            if(auto* member = uniformBuffer.second->FindMember(pin); member)
            {
                return member;
            }
        }

        return nullptr;
    }

    SpirvGenerator::UniformBufferStorage::UniforBufferMap& SpirvGenerator::UniformBufferStorage::GetBuffers()
    {
        return m_uniformBufferMap;
    }

    void SpirvGenerator::UniformBufferStorage::Clear()
    {
        m_uniformBufferMap.clear();
    }


    // Composite extract storage implementations.
    SpirvGenerator::CompositeExtractStorage::CompositeExtract::CompositeExtract(const size_t index) :
        index(index),
        id(0)
    {}

    SpirvGenerator::CompositeExtractStorage::CompositeExtract* SpirvGenerator::CompositeExtractStorage::GetOrCreate(
        GeneratorOutputPin* generatorOutputPin,
        const size_t index)
    {
        auto it = m_compositeExtractMap.find(generatorOutputPin);
        if(it == m_compositeExtractMap.end())
        {
            it = m_compositeExtractMap.insert({ generatorOutputPin, {}}).first;
        }

        auto& compositeExtracts = it->second;

        auto it2 = std::find_if(compositeExtracts.begin(), compositeExtracts.end(), [&](const auto& compositeExtract)
        {
            return compositeExtract.index == index;
        });
        if(it2 != compositeExtracts.end())
        {
            return &(*it2);
        }

        compositeExtracts.emplace_back(index);
        return &compositeExtracts.back();
    }

    void SpirvGenerator::CompositeExtractStorage::Clear()
    {
        m_compositeExtractMap.clear();
    }


    // Debug name storage implementations.
    SpirvGenerator::DebugNameStorage::DebugNameStorage() :
        m_debugNamesOffset(0)
    {}

    void SpirvGenerator::DebugNameStorage::SetDebugNamesOffset(const size_t offset)
    {
        m_debugNamesOffset = offset;
    }

    size_t SpirvGenerator::DebugNameStorage::GetDebugNamesOffset() const
    {
        return m_debugNamesOffset;
    }

    void SpirvGenerator::DebugNameStorage::Add(const Spirv::Id targetId, const std::string& name)
    {
        auto counterIt = m_debugNameCounters.find(name);
        if(counterIt == m_debugNameCounters.end())
        {
            counterIt = m_debugNameCounters.insert({ name, 0 }).first;
        }

        auto& counter = counterIt->second;
        const std::string nameWithCounter = name + "_" + std::to_string(counter++);

        return AddWithoutCounter(targetId, nameWithCounter);
    }

    void SpirvGenerator::DebugNameStorage::AddWithoutCounter(const Spirv::Id targetId, const std::string& name)
    {
        m_buffer.AddOpName(targetId, name);
    }

    Spirv::ModuleBuffer& SpirvGenerator::DebugNameStorage::GetBuffer()
    {
        return m_buffer;
    }

    void SpirvGenerator::DebugNameStorage::Clear()
    {
        m_debugNameCounters.clear();
        m_buffer.words.clear();
        m_debugNamesOffset = 0;
    }


    // Spirv generator private function implementations.
    bool SpirvGenerator::InitGenerator(const SpirvGeneratorDescriptor& descriptor)
    {
        if(!descriptor.script)
        {
            Logger::WriteError(m_logger, "Provided script for Spir-V generator is nullptr.");
            return false;
        }

        m_descriptor = &descriptor;
        m_module.words.clear();
        m_currentId = 0;
        m_capabilities = { Spirv::Capability::Shader };
        m_glslExtension = { GetNextId(), "GLSL.std.450" };
        m_mainEntryPoint = {
            m_descriptor->script->GetType() == Shader::Type::Vertex ? Spirv::ExecutionModel::Vertex : Spirv::ExecutionModel::Fragment,
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
        m_vertexOutputStructure.Clear();
        m_pushConstantStructure.Clear();
        m_samplerStorage.Clear();
        m_uniformBufferStorage.Clear();
        m_compositeExtractStorage.Clear();
        m_debugNameStorage.Clear();

        m_mainInstructions.clear();

        m_inputStructure.Initialize(m_descriptor->script->GetInputInterface());

        return true;
    }

    bool SpirvGenerator::BuildTree()
    {
        // Initialize root notes from output interface.
        const auto& outputInterface = m_descriptor->script->GetOutputInterface();
        if (const auto inputPins = outputInterface.GetInputPins(); !inputPins.empty())
        {
            auto generatorNode = std::make_shared<GeneratorNode>(outputInterface);

            size_t index = 0;
            for (auto& outputNodePin : generatorNode->inputPins)
            {
                const auto it = std::find(m_descriptor->ignoredOutputIndices.begin(), m_descriptor->ignoredOutputIndices.end(), index);
                if(it == m_descriptor->ignoredOutputIndices.end())
                {
                    const auto& pin = outputNodePin->pin;
                    auto dataType = m_dataTypeStorage.GetOrCreate(pin.GetDataType());
                    auto dataTypePointer = m_dataTypePointerStorage.GetOrCreate(Spirv::StorageClass::Output, dataType);
                    m_outputStructure.members.emplace_back(outputNodePin, std::move(dataType), std::move(dataTypePointer));
                } 

                ++index;
            }

            m_rootNodes.push_back(std::move(generatorNode));
        }

        // Initialize input pins
        if(!m_descriptor->ignoreUnusedInputs)
        {
            const auto& inputInterface = m_descriptor->script->GetInputInterface();
            auto generatorInputNode = CreateGeneratorNode(inputInterface);

            for(auto& generatorOutputPin : generatorInputNode->outputPins)
            {
                BuildVisitOutputPin(generatorInputNode, generatorOutputPin);
            }
        }

        // Add vertex output if current script is a vertex script.
        if(const auto* vertexOutput = m_descriptor->script->GetVertexOutput(); vertexOutput)
        {
            auto generatorNode = std::make_shared<GeneratorNode>(*vertexOutput);

            for (auto& outputNodePin : generatorNode->inputPins)
            {
                const auto& pin = outputNodePin->pin;
                auto dataType = m_dataTypeStorage.GetOrCreate(pin.GetDataType());
                auto dataTypePointer = m_dataTypePointerStorage.GetOrCreate(Spirv::StorageClass::Output, dataType);
                auto positionIndexConstant = m_constantStorage.GetOrCreate(m_dataTypeStorage, int32_t{ 0 });

                m_vertexOutputStructure.position.dataType = dataType;
                m_vertexOutputStructure.position.dataTypePointer = dataTypePointer;
                m_vertexOutputStructure.position.indexConstantPointer = positionIndexConstant;
            }

            m_vertexOutputStructure.isEmpty = false;

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
                    if (auto newNode = BuildVisitInputPin(currentInputPin); newNode)
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

    SpirvGenerator::GeneratorNodePointer SpirvGenerator::BuildVisitInputPin(GeneratorInputPin* generatorInputPin)
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
        m_visitedOutputPins.insert({ &generatorOutputPin->pin, generatorOutputPin });

        switch(const auto& node = generatorNode->node; generatorNode->node.GetType())
        {
            case Visual::NodeType::Constant:
            {
                const auto& constantBase = static_cast<const Visual::ConstantBase&>(node);
                m_constantStorage.GetOrCreate(m_dataTypeStorage, constantBase);
            } break;
            case Visual::NodeType::PushConstants:
            {
                m_pushConstantStructure.AddMember(m_dataTypeStorage, m_dataTypePointerStorage, generatorOutputPin);
            } break;
            case Visual::NodeType::DescriptorBinding:
            {
                const auto& bindingBase = static_cast<const Visual::DescriptorBindingBase&>(node);
                switch (const auto bindingType = bindingBase.GetBindingType(); bindingType)
                {
                    case DescriptorBindingType::Sampler1D:
                    case DescriptorBindingType::Sampler2D:
                    case DescriptorBindingType::Sampler3D:
                    {
                        m_samplerStorage.Add(m_dataTypeStorage, m_dataTypePointerStorage, generatorOutputPin, bindingBase);
                    } break;
                    case DescriptorBindingType::UniformBuffer:
                    {
                        m_uniformBufferStorage.Add(m_dataTypeStorage, m_dataTypePointerStorage, generatorOutputPin, bindingBase);
                    } break;
                    default: break;
                }
            } break;
            case Visual::NodeType::Input:
            {
                m_inputStructure.AddMember(m_dataTypeStorage, m_dataTypePointerStorage, generatorOutputPin);
            } break;
            default: break;
        }
    }

    void SpirvGenerator::BuildVisitNode(GeneratorNodePointer& generatorNode)
    {
        const auto& node = generatorNode->node;
        switch(node.GetType())
        {
            case Visual::NodeType::Constant:
            case Visual::NodeType::Composite:
            case Visual::NodeType::Function:
            case Visual::NodeType::Operator:
            case Visual::NodeType::VertexOutput:
            case Visual::NodeType::Output: m_mainInstructions.push_back(generatorNode); break;
            default: break;
        }
    }

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

    Spirv::Id SpirvGenerator::GetNextId(const Spirv::Id incrementor)
    {
        return (m_currentId += incrementor);
    }

    Spirv::Id SpirvGenerator::AccessNodeInputInMain(GeneratorInputPin& generatorInputPin)
    {
        if (auto* connectedOutputPin = generatorInputPin.connectedGeneratorOutputPin; connectedOutputPin)
        {
            const auto inputId = AccessOrTransformStorageClassInMain(*connectedOutputPin);
            if (inputId == 0)
            {
                Logger::WriteError(m_logger, "Id is 0 of connected pin or storage class transformation failed.");
                return 0;
            }

            return inputId;
        }
   
        const auto constant = m_constantStorage.Get(generatorInputPin);
        if (!constant)
        {
            Logger::WriteError(m_logger, "Failed to find constant input value for node access.");
            return 0;
        }

        return constant->id;
    }

    bool SpirvGenerator::AccessNodeInputInMain(GeneratorInputPin& generatorInputPin, std::vector<Spirv::Id>& inputIds)
    {
        const auto id = AccessNodeInputInMain(generatorInputPin);
        if(id == 0)
        {
            return false;
        }

        inputIds.push_back(id);
        return true;
    }

    Spirv::Id SpirvGenerator::AccessOrTransformStorageClassInMain(GeneratorOutputPin& generatorOutputPin)
    {
        switch(generatorOutputPin.storageClass)
        {
            case Spirv::StorageClass::Function: return generatorOutputPin.id;
            case Spirv::StorageClass::UniformConstant:
            case Spirv::StorageClass::Input:
            {
                const auto dataType = generatorOutputPin.pin.GetDataType();
                const auto outputDataType = m_dataTypeStorage.Get(dataType);
                if (!outputDataType)
                {
                    Logger::WriteError(m_logger, "Failed to get data type for output pin: " + std::to_string(static_cast<int>(dataType)) + ".");
                    return 0;
                }

                const auto newId = GetNextId();

                m_module.AddOpLoad(outputDataType->id, newId, generatorOutputPin.id);

                generatorOutputPin.id = newId;
                generatorOutputPin.storageClass = Spirv::StorageClass::Function;

                return generatorOutputPin.id;
            }
            case Spirv::StorageClass::PushConstant:
            {
                auto* member = m_pushConstantStructure.FindMember(&generatorOutputPin.pin);
                if (!member)
                {
                    return 0;
                }

                const auto accessChainId = GetNextId();
                const auto newId = GetNextId();
                m_module.AddOpAccessChain(member->dataTypePointer->id, accessChainId, m_pushConstantStructure.id, member->indexConstant->id);
                m_module.AddOpLoad(member->dataType->id, newId, accessChainId);

                generatorOutputPin.id = newId;
                generatorOutputPin.storageClass = Spirv::StorageClass::Function;

                return generatorOutputPin.id;
            }
            case Spirv::StorageClass::Uniform:
            {
                auto* member = m_uniformBufferStorage.FindMember(&generatorOutputPin.pin);
                if (!member)
                {
                    return 0;
                }

                const auto* structure = member->structure;

                const auto accessChainId = GetNextId();
                const auto newId = GetNextId();
                m_module.AddOpAccessChain(member->dataTypePointer->id, accessChainId, structure->id, member->indexConstant->id);
                m_module.AddOpLoad(member->dataType->id, newId, accessChainId);
                
                generatorOutputPin.id = newId;
                generatorOutputPin.storageClass = Spirv::StorageClass::Function;

                return generatorOutputPin.id;
            }
            default: break;
        }

        return 0;
    }

    bool SpirvGenerator::WriteModule()
    {     
        if(!UpdatePushConstantMembers())
        {
            return false;
        }
        UpdateUniformBuffersMembers();

        UpdateDataTypeIds();
        UpdateInputPointerIds();
        UpdateInputIds();
        UpdateOutputPointerIds();
        UpdateOutputIds();
        UpdateVertexOutputs();
        UpdatePushConstantPointerIds();
        UpdatePushConstantStruct();
        UpdateUniformConstantPointers();
        UpdateSamplerIds();
        UpdateUniformPointerIds();
        UpdateUniformBufferStructs();

        if (m_descriptor->includeDebugSymbols)
        {
            AddGlobalDebugNames();
        }

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
        
        for (const auto& member : m_inputStructure.GetMembers())
        {
            m_mainEntryPoint.interfaceIds.push_back(member.outputPin->id);
        }
        for (const auto& member : m_outputStructure.members)
        {
            m_mainEntryPoint.interfaceIds.push_back(member.id);
        }
        if(!m_vertexOutputStructure.isEmpty)
        {
            m_mainEntryPoint.interfaceIds.push_back(m_vertexOutputStructure.id);
        }

        m_module.AddOpEntryPoint(m_mainEntryPoint);

        if(m_descriptor->script->GetType() == Shader::Type::Fragment)
        {
            m_module.AddOpExecutionMode(m_mainEntryPoint.id, Spirv::ExecutionMode::OriginUpperLeft);
        }     

        m_debugNameStorage.SetDebugNamesOffset(m_module.words.size());

        WriteDecorations();

        if (!WriteDataTypes())
        {
            return false;
        }

        WriteInputPointers();
        WriteInputs();
        WriteOutputPointers();
        WriteOutputs();
        WriteVertexOutputs();
        WritePushConstantStruct();
        WritePushConstantPointers();
        WriteUniformConstantPointers();
        WriteSamplers();
        WriteUniformBufferStructs();
        WriteUniformPointerIds();

        if (!WriteConstants())
        {
            return false;
        }

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

        if(m_descriptor->includeDebugSymbols)
        {
            InsertDebugNames();
        }

        return true;
    }

    bool SpirvGenerator::UpdatePushConstantMembers()
    {
        if (m_pushConstantStructure.isEmpty)
        {
            return true;
        }

        // Using combined shaders template.
        if(const auto* combinedShadersTemplate = m_descriptor->combinedShadersTemplate; combinedShadersTemplate)
        {
            const auto& pushConstantLocations = combinedShadersTemplate->pushConstantLocations;

            const auto& pushConstants = m_descriptor->script->GetPushConstantsBase();
            const size_t pushConstantCount = pushConstants.GetMemberCount();

            Spirv::Word index = 0;
            for(size_t i = 0; i < pushConstantCount; i++)
            {
                const auto* pushConstant = pushConstants.GetMemberBase(i);
                const auto& pin = pushConstant->GetPin();

                auto* member = m_pushConstantStructure.FindMember(&pin);
                if (!member)
                {
                    continue;
                }

                const auto id = pushConstant->GetId();

                const auto it = pushConstantLocations.find(id);
                if(it == pushConstantLocations.end())
                {
                    Logger::WriteError(m_logger, "Push constant is missing from template, id: " + std::to_string(id) + ".");
                    return false;
                }

                member->index = index++;
                member->offset = it->second.location;
                member->indexConstant = m_constantStorage.GetOrCreate(m_dataTypeStorage, static_cast<int32_t>(member->index));
            }
        }
        else
        {
            // Standalone script.
            const auto& pushConstants = m_descriptor->script->GetPushConstantsBase();
            if (auto pushConstantsOutputPins = pushConstants.GetOutputPins(); !pushConstantsOutputPins.empty())
            {
                Spirv::Word index = 0;
                Spirv::Word offset = 0;
                for (const auto* pin : pushConstantsOutputPins)
                {
                    const auto pinDataType = pin->GetDataType();

                    const auto prevOffset = offset;
                    offset += std::max(Spirv::Word{ 16 }, static_cast<Spirv::Word>(GetVariableByteOffset(pinDataType)));

                    auto* member = m_pushConstantStructure.FindMember(pin);
                    if (!member)
                    {
                        continue;
                    }

                    member->index = index++;
                    member->offset = prevOffset;
                    member->indexConstant = m_constantStorage.GetOrCreate(m_dataTypeStorage, static_cast<int32_t>(member->index));
                }
            }
        }

        return true;
    }

    void SpirvGenerator::UpdateUniformBuffersMembers()
    {
        const auto& descriptorSets = m_descriptor->script->GetDescriptorSetsBase();
        
        for(size_t i = 0; i < descriptorSets.GetSetCount(); i++)
        {
            const auto* set = descriptorSets.GetSetBase(i);

            for(size_t j = 0; j < set->GetBindingCount(); j++)
            {
                const auto* binding = set->GetBindingBase(j);

                if(binding->GetBindingType() != DescriptorBindingType::UniformBuffer)
                {
                    continue;
                }

                Spirv::Word index = 0;
                Spirv::Word offset = 0;

                for (const auto* pin : binding->GetOutputPins())
                {
                    const auto pinDataType = pin->GetDataType();

                    const auto prevOffset = offset;
                    offset += std::max(Spirv::Word{ 16 }, static_cast<Spirv::Word>(GetVariableByteOffset(pinDataType)));

                    auto* member = m_uniformBufferStorage.FindMember(pin);
                    if (!member)
                    {
                        continue;
                    }

                    member->index = index++;
                    member->offset = prevOffset;
                    member->indexConstant = m_constantStorage.GetOrCreate(m_dataTypeStorage, static_cast<int32_t>(member->index));
                }
            }
        }
    }

    void SpirvGenerator::UpdateDataTypeIds()
    {
        for (auto& dataType : m_dataTypeStorage.GetAllDependencySorted())
        {
            switch(dataType->type)
            {
                case VariableDataType::Sampler1D:
                case VariableDataType::Sampler2D:
                case VariableDataType::Sampler3D: dataType->id = GetNextId(2); break;
                default: dataType->id = GetNextId(); break;
            }
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
        for (auto& member : m_inputStructure.GetMembers())
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

    void SpirvGenerator::UpdateVertexOutputs()
    {
        if(m_vertexOutputStructure.isEmpty)
        {
            return;
        }

        m_vertexOutputStructure.typeId = GetNextId();
        m_vertexOutputStructure.typePointerId = GetNextId();
        m_vertexOutputStructure.id = GetNextId();

        if(m_descriptor->includeDebugSymbols)
        {
            m_debugNameStorage.AddWithoutCounter(m_vertexOutputStructure.typeId, "gl_PerVertex");
            m_debugNameStorage.AddWithoutCounter(m_vertexOutputStructure.id, "vout");
        }
    }

    void SpirvGenerator::UpdatePushConstantPointerIds()
    {
        for (auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::PushConstant))
        {
            dataTypePointers->id = GetNextId();
        }
    }

    void SpirvGenerator::UpdatePushConstantStruct()
    {
        if (m_pushConstantStructure.isEmpty)
        {
            return;
        }

        m_pushConstantStructure.typeId = GetNextId();
        m_pushConstantStructure.typePointerId = GetNextId();
        m_pushConstantStructure.id = GetNextId();
    }

    void SpirvGenerator::UpdateUniformConstantPointers()
    {
        for (auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::UniformConstant))
        {
            dataTypePointers->id = GetNextId();
        }
    }

    void SpirvGenerator::UpdateSamplerIds()
    {
        for (auto& sampler : m_samplerStorage.GetSamplers())
        {
            sampler.outputPin->id = GetNextId();
        }
    }

    void SpirvGenerator::UpdateUniformPointerIds()
    {
        for (auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::Uniform))
        {
            dataTypePointers->id = GetNextId();
        }
    }

    void SpirvGenerator::UpdateUniformBufferStructs()
    {
        for(auto& uniformBufferPair : m_uniformBufferStorage.GetBuffers())
        {
            auto& uniformBuffer = uniformBufferPair.second;
            uniformBuffer->typeId = GetNextId();
            uniformBuffer->typePointerId = GetNextId();
            uniformBuffer->id = GetNextId();
        }

    }

    void SpirvGenerator::AddGlobalDebugNames()
    {
        m_debugNameStorage.AddWithoutCounter(m_mainEntryPoint.id, m_mainEntryPoint.name);

        for (auto& member : m_inputStructure.GetMembers())
        {
            m_debugNameStorage.Add(member.outputPin->id, "in");
        }

        for (auto& member : m_outputStructure.members)
        {
            m_debugNameStorage.Add(member.id, "out");
        }

        if (!m_pushConstantStructure.isEmpty)
        {
            m_debugNameStorage.AddWithoutCounter(m_pushConstantStructure.typeId, "s_pc");
            m_debugNameStorage.AddWithoutCounter(m_pushConstantStructure.id, "pc");
        }

        for (const auto& sampler : m_samplerStorage.GetSamplers())
        {
            m_debugNameStorage.Add(sampler.outputPin->id, "sampler");
        }

        for (const auto& uniformBufferPair : m_uniformBufferStorage.GetBuffers())
        {
            m_debugNameStorage.Add(uniformBufferPair.second->typeId, "s_ubo");
            m_debugNameStorage.Add(uniformBufferPair.second->id, "ubo");        
        }
    }

    void SpirvGenerator::WriteDecorations()
    {
        WriteInputDecorations();
        WriteOutputDecorations();
        WriteVertexOutputDecorations();
        WritePushConstantDecorations();
        WriteSamplerDecorations();
        WriteUniformBufferDecorations();
    }

    void SpirvGenerator::WriteInputDecorations()
    {
        Spirv::Word location = 0;
        for (const auto& member : m_inputStructure.GetMembers())
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

    void SpirvGenerator::WriteVertexOutputDecorations()
    {
        if(m_vertexOutputStructure.isEmpty)
        {
            return;
        }

        m_module.AddOpMemberDecorateBuiltIn(m_vertexOutputStructure.typeId, 0, Spirv::BuiltIn::Position);
        m_module.AddOpDecorateBlock(m_vertexOutputStructure.typeId);
    }

    void SpirvGenerator::WritePushConstantDecorations()
    {
        if(m_pushConstantStructure.isEmpty)
        {
            return;
        }

        Structure::Members membersCopy = m_pushConstantStructure.members;

        std::sort(membersCopy.begin(), membersCopy.end(), [&](const auto& lhs, const auto& rhs)
        {
            return lhs.index < rhs.index;
        });

        const auto structTypeId = m_pushConstantStructure.typeId;

        for (const auto& member : membersCopy)
        {
            m_module.AddOpMemberDecorateOffset(structTypeId, member.index, member.offset);

            if(member.dataType->type == VariableDataType::Matrix4x4f32)
            {
                m_module.AddOpMemberDecorateColMajor(structTypeId, member.index);
                m_module.AddOpMemberDecorateMatrixStride(structTypeId, member.index, 16);
            }
        }

        m_module.AddOpDecorateBlock(structTypeId);
    }

    void SpirvGenerator::WriteSamplerDecorations()
    {
        for (const auto& sampler : m_samplerStorage.GetSamplers())
        {
            m_module.AddOpDecorateDescriptorSet(sampler.outputPin->id, sampler.setId);
            m_module.AddOpDecorateBinding(sampler.outputPin->id, sampler.bindingId);
        }
    }

    void SpirvGenerator::WriteUniformBufferDecorations()
    {
        for(const auto& uniformBufferPair : m_uniformBufferStorage.GetBuffers())
        {
            const auto& uniformBuffer = uniformBufferPair.second;
            const auto& idPairs = uniformBufferPair.first;

            Structure::Members membersCopy = uniformBuffer->members;

            std::sort(membersCopy.begin(), membersCopy.end(), [&](const auto& lhs, const auto& rhs)
            {
                return lhs.index < rhs.index;
            });

            const auto structTypeId = uniformBuffer->typeId;

            for (const auto& member : membersCopy)
            {
                m_module.AddOpMemberDecorateOffset(structTypeId, member.index, member.offset);
            }

            m_module.AddOpDecorateBlock(uniformBuffer->typeId);
            m_module.AddOpDecorateDescriptorSet(uniformBuffer->id, static_cast<Spirv::Id>(idPairs.first));
            m_module.AddOpDecorateBinding(uniformBuffer->id, static_cast<Spirv::Id>(idPairs.second));
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
                        Logger::WriteError(m_logger, "Failed to find component data type for Vector2f32: " +
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
                        Logger::WriteError(m_logger, "Failed to find component data type for Vector3f32: " +
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
                        Logger::WriteError(m_logger, "Failed to find component data type for Vector4f32: " +
                            std::to_string(static_cast<int32_t>(VariableDataType::Float32)) + ".");
                        return false;
                    }
                    m_module.AddOpTypeVector(dataType->id, componentDataType->id, 4);
                } break;
                case VariableDataType::Matrix4x4f32:
                {
                    const auto componentDataType = m_dataTypeStorage.Get(VariableDataType::Vector4f32);
                    if (!componentDataType)
                    {
                        Logger::WriteError(m_logger, "Failed to find component data type for Matrix4x4f32: " +
                            std::to_string(static_cast<int32_t>(VariableDataType::Vector4f32)) + ".");
                        return false;
                    }
                    m_module.AddOpTypeMatrix(dataType->id, componentDataType->id, 4);
                } break;
                case VariableDataType::Sampler1D:
                {
                    const auto componentDataType = m_dataTypeStorage.Get(VariableDataType::Float32);
                    if (!componentDataType)
                    {
                        Logger::WriteError(m_logger, "Failed to find float data type for Image1D type: " +
                            std::to_string(static_cast<int32_t>(VariableDataType::Float32)) + ".");
                        return false;
                    }

                    const auto imageTypeId = dataType->id - 1;
                    m_module.AddOpTypeImage(imageTypeId, componentDataType->id, Spirv::Dimensionality::Image1D);
                    m_module.AddOpTypeSampledImage(dataType->id, imageTypeId);
                } break;
                case VariableDataType::Sampler2D:
                {
                    const auto componentDataType = m_dataTypeStorage.Get(VariableDataType::Float32);
                    if (!componentDataType)
                    {
                        Logger::WriteError(m_logger, "Failed to find float data type for Image2D type: " +
                            std::to_string(static_cast<int32_t>(VariableDataType::Float32)) + ".");
                        return false;
                    }

                    const auto imageTypeId = dataType->id - 1;
                    m_module.AddOpTypeImage(imageTypeId, componentDataType->id, Spirv::Dimensionality::Image2D);
                    m_module.AddOpTypeSampledImage(dataType->id, imageTypeId);
                } break;
                case VariableDataType::Sampler3D:
                {
                    const auto componentDataType = m_dataTypeStorage.Get(VariableDataType::Float32);
                    if (!componentDataType)
                    {
                        Logger::WriteError(m_logger, "Failed to find float data type for Image3D type: " +
                            std::to_string(static_cast<int32_t>(VariableDataType::Float32)) + ".");
                        return false;
                    }

                    const auto imageTypeId = dataType->id - 1;
                    m_module.AddOpTypeImage(imageTypeId, componentDataType->id, Spirv::Dimensionality::Image3D);
                    m_module.AddOpTypeSampledImage(dataType->id, imageTypeId);
                } break;
                default:
                {
                    Logger::WriteError(m_logger, "Unsupported data type: " +
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
        for (const auto& member : m_inputStructure.GetMembers())
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

    void SpirvGenerator::WriteVertexOutputs()
    {
        if(m_vertexOutputStructure.isEmpty)
        {
            return;
        }

        const auto& position = m_vertexOutputStructure.position;

        const auto memberTypeIds = std::vector{ position.dataType->id };

        m_module.AddOpTypeStruct(m_vertexOutputStructure.typeId, memberTypeIds);
        m_module.AddOpTypePointer(m_vertexOutputStructure.typePointerId, Spirv::StorageClass::Output, m_vertexOutputStructure.typeId);
        m_module.AddOpVariable(m_vertexOutputStructure.id, m_vertexOutputStructure.typePointerId, Spirv::StorageClass::Output);
    }

    void SpirvGenerator::WritePushConstantStruct()
    {
        if(m_pushConstantStructure.isEmpty)
        {
            return;
        }

        Spirv::Words memberTypeids;
        for (const auto& member : m_pushConstantStructure.members)
        {
            memberTypeids.push_back(member.dataType->id);
        }

        m_module.AddOpTypeStruct(m_pushConstantStructure.typeId, memberTypeids);
        m_module.AddOpTypePointer(m_pushConstantStructure.typePointerId, Spirv::StorageClass::PushConstant, m_pushConstantStructure.typeId);
        m_module.AddOpVariable(m_pushConstantStructure.id, m_pushConstantStructure.typePointerId, Spirv::StorageClass::PushConstant);
    }

    void SpirvGenerator::WritePushConstantPointers()
    {
        for (const auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::PushConstant))
        {
            m_module.AddOpTypePointer(dataTypePointers->id, Spirv::StorageClass::PushConstant, dataTypePointers->dataType->id);
        }
    }

    void SpirvGenerator::WriteUniformConstantPointers()
    {
        for (const auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::UniformConstant))
        {
            m_module.AddOpTypePointer(dataTypePointers->id, Spirv::StorageClass::UniformConstant, dataTypePointers->dataType->id);
        }
    }

    void SpirvGenerator::WriteSamplers()
    {
        for (const auto& sampler : m_samplerStorage.GetSamplers())
        {
            m_module.AddOpVariable(sampler.outputPin->id, sampler.dataTypePointer->id, Spirv::StorageClass::UniformConstant);
        }
    }

    void SpirvGenerator::WriteUniformBufferStructs()
    {
        for(const auto& uniformBufferPair : m_uniformBufferStorage.GetBuffers())
        {
            const auto& uniformBuffer = uniformBufferPair.second;

            Spirv::Words memberTypeids;
            for (const auto& member : uniformBuffer->members)
            {
                memberTypeids.push_back(member.dataType->id);
            }

            m_module.AddOpTypeStruct(uniformBuffer->typeId, memberTypeids);
            m_module.AddOpTypePointer(uniformBuffer->typePointerId, Spirv::StorageClass::Uniform, uniformBuffer->typeId);
            m_module.AddOpVariable(uniformBuffer->id, uniformBuffer->typePointerId, Spirv::StorageClass::Uniform);
        }
    }

    void SpirvGenerator::WriteUniformPointerIds()
    {
        for (const auto& dataTypePointers : m_dataTypePointerStorage.GetAll(Spirv::StorageClass::Uniform))
        {
            m_module.AddOpTypePointer(dataTypePointers->id, Spirv::StorageClass::Uniform, dataTypePointers->dataType->id);
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
            case Visual::NodeType::Constant: return WriteConstant(generatorNode, static_cast<const Visual::ConstantBase&>(generatorNode->node));
            case Visual::NodeType::Composite: return WriteComposite(generatorNode);
            case Visual::NodeType::Function: return WriteFunction(generatorNode, static_cast<const Visual::FunctionBase&>(generatorNode->node));
            case Visual::NodeType::Operator: return WriteOperator(generatorNode, static_cast<const Visual::OperatorBase&>(generatorNode->node));
            case Visual::NodeType::VertexOutput: return WriteVertexOutput(generatorNode);
            case Visual::NodeType::Output: return WriteOutput(generatorNode);
            default: break;
        }
    
        return false;
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
            if(!AccessNodeInputInMain(*generatorInputPin, inputIds))
            {
                return false;
            }
        }

        outputPin->id = GetNextId();

        const auto functionType = functionBase.GetFunctionType();

        if(m_descriptor->includeDebugSymbols)
        {
            if(const auto& debugName = GetFunctionName(functionType); !debugName.empty())
            {
                m_debugNameStorage.Add(outputPin->id, debugName);
            }   
        }

        // GLSL extension functions.
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

        // Build-in functions.
        switch(functionType)
        {
            case Visual::FunctionType::Texture1D:
            case Visual::FunctionType::Texture2D:
            case Visual::FunctionType::Texture3D:
            {
                if(inputIds.size() != 2)
                {
                    Logger::WriteError(m_logger, "Texture sample function requires 2 input pins, found " + std::to_string(inputIds.size()) + ".");
                    return false;
                }

                m_module.AddOpImageSampleImplicitLod(
                    outputDataType->id,
                    outputPin->id,
                    inputIds[0],
                    inputIds[1]);
                return true;
            }
            default: break;
        }
        
        // Unknown function.
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
        std::vector<VariableDataType> inputDataTypes;
        for (const auto& generatorInputPin : generatorNode->inputPins)
        {
            if (!AccessNodeInputInMain(*generatorInputPin, inputIds))
            {
                return false;
            }

            inputDataTypes.push_back(generatorInputPin->pin.GetDataType());
        }

        outputPin->id = GetNextId();

        const auto arithmeticOperatorType = arithmeticOperatorBase.GetArithmeticOperatorType();

        if (m_descriptor->includeDebugSymbols)
        {
            if (const auto& debugName = GetArithmeticOperatorName(arithmeticOperatorType); !debugName.empty())
            {
                m_debugNameStorage.Add(outputPin->id, debugName);
            }
        }

        const auto opCode = GetArithmeticOperatorOpCode(arithmeticOperatorType, inputDataTypes[0], inputDataTypes[1]);

        switch(opCode)
        {
            case Spirv::OpCode::FAdd: m_module.AddOpFAdd(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            case Spirv::OpCode::FDiv: m_module.AddOpFDiv(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            case Spirv::OpCode::FMul: m_module.AddOpFMul(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            case Spirv::OpCode::FSub: m_module.AddOpFSub(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            case Spirv::OpCode::MatrixTimesVector: m_module.AddOpMatrixTimesVector(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            case Spirv::OpCode::VectorTimesMatrix: m_module.AddOpVectorTimesMatrix(outputDataType->id, outputPin->id, inputIds[0], inputIds[1]); break;
            default:
            {
                Logger::WriteError(m_logger, "Failed to find op code for arithmetic operator of input types: " 
                    + std::to_string(static_cast<size_t>(inputDataTypes[0])) + " and " + std::to_string(static_cast<size_t>(inputDataTypes[1])) + ".");
                return false;
            } break;
        }
        
        return true;
    }

    bool SpirvGenerator::WriteVertexOutput(const GeneratorNodePointer& generatorNode)
    {
        for (const auto& generatorInputPin : generatorNode->inputPins)
        {
            const auto inputId = AccessNodeInputInMain(*generatorInputPin);
            if(inputId == 0)
            {
                return false;
            }

            auto inputPinId = GetNextId();

            const auto& position = m_vertexOutputStructure.position;

            m_module.AddOpAccessChain(
                position.dataTypePointer->id,
                inputPinId,
                m_vertexOutputStructure.id,
                position.indexConstantPointer->id);

            m_module.AddOpStore(inputPinId, inputId);
        }
 
        return true;
    }

    bool SpirvGenerator::WriteOutput(const GeneratorNodePointer& generatorNode)
    {
        size_t index = 0;
        for (auto& generatorInputPin : generatorNode->inputPins)
        {
            const auto it = std::find(m_descriptor->ignoredOutputIndices.begin(), m_descriptor->ignoredOutputIndices.end(), index);
            if (it == m_descriptor->ignoredOutputIndices.end())
            {
                std::vector<Spirv::Id> inputIds;
                if (!AccessNodeInputInMain(*generatorInputPin, inputIds))
                {
                    return false;
                }

                const auto* member = m_outputStructure.FindMember(generatorInputPin);
                if (!member)
                {
                    Logger::WriteError(m_logger, "Failed to find output interface member.");
                    return false;
                }

                m_module.AddOpStore(member->id, inputIds[0]);
            }
            ++index;
        }
        return true;
    }

    bool SpirvGenerator::WriteConstant(const GeneratorNodePointer& generatorNode, const Visual::ConstantBase& constantBase)
    {
        if (const auto outputPinCount = generatorNode->outputPins.size(); outputPinCount != 1)
        {
            Logger::WriteError(m_logger, "Constant expects 1 output pin, found " + std::to_string(outputPinCount) + ".");
            return false;
        }
        if(auto& outputPin = generatorNode->outputPins.front(); outputPin->id == 0)
        {
            const auto constant = m_constantStorage.Get(constantBase);
            if(!constant)
            {
                Logger::WriteError(m_logger, "Failed to find value for constant node.");
                return false;
            }
            outputPin->id = constant->id;
        }

        return true;
    }

    bool SpirvGenerator::WriteComposite(const GeneratorNodePointer& generatorNode)
    {
        if (const auto outputPinCount = generatorNode->outputPins.size(); outputPinCount != 1)
        {
            Logger::WriteError(m_logger, "Composite expects 1 output pin, found " + std::to_string(outputPinCount) + ".");
            return false;
        }

        const auto& generatorOutputPin = generatorNode->outputPins.front();
        const auto outputComponentCount = GetVariableComponentCount(generatorOutputPin->pin.GetDataType());
        if(outputComponentCount == 0)
        {
            Logger::WriteError(m_logger, "Unsupported output data type for composite: " + 
                std::to_string(static_cast<size_t>(generatorOutputPin->pin.GetDataType())) + ".");
            return false;
        }

        size_t inputsComponentCount = 0;
        for(const auto& inputPin : generatorNode->inputPins)
        {
            const auto inputDataType = inputPin->pin.GetDataType();
            const auto componentCount = GetVariableComponentCount(inputDataType);
            if(componentCount == 0)
            {
                Logger::WriteError(m_logger, "Unsupported input data type for composite: " +
                    std::to_string(static_cast<size_t>(inputDataType)) + ".");
            }

            inputsComponentCount += componentCount;
        }

        if(outputComponentCount != inputsComponentCount)
        {
            Logger::WriteError(m_logger, "Composite expects " + std::to_string(outputComponentCount) + 
                " components, found " + std::to_string(inputsComponentCount) + ".");
            return false;
        }

        std::vector<Spirv::Id> inputIds;

        auto addConstantInputs = [&](const auto& origin) -> bool
        {
            auto constant = m_constantStorage.Get(origin);
            if (!constant)
            {
                Logger::WriteError(m_logger, "Failed to get default constant value for composite type.");
                return false;
            }

            if (!constant->componentConstants.empty())
            {
                for (const auto& componentConstant : constant->componentConstants)
                {
                    inputIds.push_back(componentConstant->id);
                }
            }
            else
            {
                inputIds.push_back(constant->id);
            }

            return true;
        };

        for (const auto& generatorInputPin : generatorNode->inputPins)
        {
            if (auto* connectedPin = generatorInputPin->connectedGeneratorOutputPin; connectedPin)
            {
                // Is connection a constant node. Use constant values directly instead of extracting each component.
                if(const auto& connectedNode = connectedPin->pin.GetNode(); connectedNode.GetType() == Visual::NodeType::Constant)
                {
                    if (!addConstantInputs(static_cast<const Visual::ConstantBase&>(connectedNode)))
                    {
                        return false;
                    }
                }
                else
                {
                    // Connected to a regual node.
                    const auto connectedDataType = connectedPin->pin.GetDataType();
                    const auto componentCount = GetVariableComponentCount(connectedDataType);

                    if(componentCount == 0)
                    {
                        Logger::WriteError(m_logger, "Failed to find component count for composite of type: " +
                            std::to_string(static_cast<size_t>(connectedDataType)) + ".");
                        return false;
                    }

                    if(componentCount == 1)
                    {
                        // Single component input variable, access it directly and use it.
                        const auto connectedId = AccessNodeInputInMain(*generatorInputPin);
                        if (connectedId == 0)
                        {
                            return false;
                        }

                        inputIds.push_back(connectedId);
                    }
                    else
                    {
                        // Multiple components of input variable, do composite extract if needed.
                        const auto connectedId = AccessNodeInputInMain(*generatorInputPin);
                        if (connectedId == 0)
                        {
                            return false;
                        }

                        const auto componentDataType = m_dataTypeStorage.Get(GetVariableComponentType(connectedDataType));
                        if(!componentDataType)
                        {
                            Logger::WriteError(m_logger, "Failed to find component data type for composite of type: " +
                                std::to_string(static_cast<size_t>(connectedDataType)) + ".");
                            return false;
                        }

                        for (size_t i = 0; i < componentCount; i++)
                        {
                            auto* componentExtract = m_compositeExtractStorage.GetOrCreate(connectedPin, i);
                            if (componentExtract->id == 0)
                            {
                                componentExtract->id = GetNextId();

                                m_module.AddOpCompositeExtract(
                                    componentDataType->id,
                                    componentExtract->id,
                                    connectedId,
                                    static_cast<uint32_t>(i));
                            }

                            inputIds.push_back(componentExtract->id);
                        }
                    }
                }
            }
            else
            {
                // Default value(constant).
                if(!addConstantInputs(*generatorInputPin))
                {
                    return false;
                }
            }
        }

        generatorOutputPin->id = GetNextId();
        const auto outputPinDataType = generatorOutputPin->pin.GetDataType();
        const auto dataType = m_dataTypeStorage.Get(outputPinDataType);

        m_module.AddOpCompositeConstruct(dataType->id, generatorOutputPin->id, inputIds);

        return true;
    }

    void SpirvGenerator::InsertDebugNames()
    {
        const auto& debugNames = m_debugNameStorage.GetBuffer().words;
        const auto offset = m_debugNameStorage.GetDebugNamesOffset();
        m_module.words.insert(m_module.words.begin() + offset, debugNames.begin(), debugNames.end());
    }

}