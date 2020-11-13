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

#include "Molten/Renderer/Shader/Generator/VulkanShaderGenerator.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Logger.hpp"
#include <memory>
#include <map>
#include <stack>

#if defined(MOLTEN_ENABLE_GLSLANG)
#include "ThirdParty/glslang/glslang/Public/ShaderLang.h"
#include "ThirdParty/glslang/SPIRV/GlslangToSpv.h"
#endif


namespace Molten::Shader
{

    struct OutputVariable;
    using OutputVariablePointer = std::shared_ptr<OutputVariable>;

    struct InputVariable;

    class NodeObject;
    using NodeObjectPointer = std::shared_ptr<NodeObject>;

    struct OutputVariable
    {
        OutputVariable(const Visual::Pin* pin, std::string&& name = "") :
            pin(pin), name(name)
        {}

        const Visual::Pin* pin;
        std::string name;
    };

    struct InputVariable
    {
        InputVariable(const Visual::Pin& pin) :
            pin(pin),
            connection(nullptr)
        {}

        const Visual::Pin& pin;
        OutputVariable* connection;
    };

    class NodeObject
    {

    public:

        using InputVariableContainer = std::vector<InputVariable>;
        using OutputVariablePointerContainer = std::vector<OutputVariablePointer>;
        using InputVariableIterator = typename InputVariableContainer::iterator;

        explicit NodeObject(const Visual::Node& node) :
            m_node(node),
            m_inputVariables(CreateInputVariables(node)),
            m_inputVariableIterator{ m_inputVariables.begin() },
            m_outputVariables(m_node.GetOutputPinCount(), nullptr)
        {}

        OutputVariablePointer CreateOutputVariable(const Visual::Pin* pin, std::string&& name = "")
        {
            auto outputPins = m_node.GetOutputPins();
            for (size_t i = 0; i < outputPins.size(); i++)
            {
                if (pin == outputPins[i])
                {
                    auto outputVariable = std::make_shared<OutputVariable>(pin, std::move(name));
                    m_outputVariables[i] = outputVariable;
                    return outputVariable;
                }
            }

            MOLTEN_DEBUG_ASSERT(false, "Passed pin is not contained by this node.");
            return nullptr;
        }

        const Visual::Node& GetNode() const
        {
            return m_node;
        }

        const InputVariableContainer& GetInputVariables() const
        {
            return m_inputVariables;
        }

        const OutputVariablePointerContainer& GetOutputVariables() const
        {
            return m_outputVariables;
        }

        InputVariable* GetNextInputVariable()
        {
            if (m_inputVariableIterator != m_inputVariables.end())
            {
                InputVariable* inputVariable = &(*m_inputVariableIterator);
                ++m_inputVariableIterator;
                return inputVariable;
            }
            return nullptr;
        }

        void AddOutputPin(const Visual::Pin* pin, const std::string& name)
        {
            auto outputVariable = std::make_shared<OutputVariable>(pin, std::string(std::move(name)));
            m_outputVariables.push_back(outputVariable);
        }

    private:

        InputVariableContainer CreateInputVariables(const Visual::Node& node)
        {
            auto inputPins = node.GetInputPins();
            InputVariableContainer inputVariables;
            inputVariables.reserve(inputPins.size());
            for (size_t i = 0; i < inputPins.size(); i++)
            {
                inputVariables.push_back({ *inputPins[i] });
            }
            return inputVariables;
        }

        const Visual::Node& m_node;
        InputVariableContainer m_inputVariables;
        InputVariableIterator m_inputVariableIterator;
        OutputVariablePointerContainer m_outputVariables;

    };



    static const std::string g_emptyString = "";

    // Data type names
    static const std::string g_glslDataTypeBool = "bool";
    static const std::string g_glslDataTypeInt = "int";
    static const std::string g_glslDataTypeFloat = "float";
    static const std::string g_glslDataTypeVec2 = "vec2";
    static const std::string g_glslDataTypeVec3 = "vec3";
    static const std::string g_glslDataTypeVec4 = "vec4";
    static const std::string g_glslDataTypeMat4 = "mat4";
    static const std::string g_glslDataTypeSample1D = "sampler1D";
    static const std::string g_glslDataTypeSample2D = "sampler2D";
    static const std::string g_glslDataTypeSample3D = "sampler3D";

    // Function names.
    static const std::string g_glslFunctionCos = "cos";
    static const std::string g_glslFunctionSin = "sin";
    static const std::string g_glslFunctionTan = "tan";
    static const std::string g_glslFunctionMax = "max";
    static const std::string g_glslFunctionMin = "min";
    static const std::string g_glslFunctionCross = "cross";
    static const std::string g_glslFunctionDot = "dot";

    // Arithmetic operator signs.
    static const std::string g_glslArithOpAdd = " + ";
    static const std::string g_glslArithOpDiv = " / ";
    static const std::string g_glslArithOpMul = " * ";
    static const std::string g_glslArithOpSub = " - ";

    // Arithmetic operator names.
    static const std::string g_glslArithOpNameAdd = "add";
    static const std::string g_glslArithOpNameDiv = "div";
    static const std::string g_glslArithOpNameMul = "mul";
    static const std::string g_glslArithOpNameSub = "sub";

    
    static const std::string& GetGlslVariableDataType(const VariableDataType dataType)
    {
        switch (dataType)
        {
            case VariableDataType::Bool:         return g_glslDataTypeBool;
            case VariableDataType::Int32:        return g_glslDataTypeInt;
            case VariableDataType::Float32:      return g_glslDataTypeFloat;
            case VariableDataType::Vector2f32:   return g_glslDataTypeVec2;
            case VariableDataType::Vector3f32:   return g_glslDataTypeVec3;
            case VariableDataType::Vector4f32:   return g_glslDataTypeVec4;
            case VariableDataType::Matrix4x4f32: return g_glslDataTypeMat4;
            case VariableDataType::Sampler1D:    return g_glslDataTypeSample1D;
            case VariableDataType::Sampler2D:    return g_glslDataTypeSample2D;
            case VariableDataType::Sampler3D:    return g_glslDataTypeSample3D;
        }
        throw Exception("GetGlslVariableDataType is missing return value for dataType = " + std::to_string(static_cast<size_t>(dataType)) + ".");
    }

    static size_t GetGlslVariableOffset(const VariableDataType dataType)
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
        throw Exception("GetGlslVariableOffset is missing return value for dataType = " + std::to_string(static_cast<size_t>(dataType)) + ".");
    }
   
    static const std::string& GetGlslArithmeticOperator(const Visual::ArithmeticOperatorType op)
    {
        switch (op)
        {
            case Visual::ArithmeticOperatorType::Addition:          return g_glslArithOpAdd;
            case Visual::ArithmeticOperatorType::Division:          return g_glslArithOpDiv;
            case Visual::ArithmeticOperatorType::Multiplication:    return g_glslArithOpMul;
            case Visual::ArithmeticOperatorType::Subtraction:       return g_glslArithOpSub;
        }
        throw Exception("GetGlslArithmeticOperator is missing return value for op = " + std::to_string(static_cast<size_t>(op)) + ".");
    }

    static const std::string& GetGlslArithmeticOperatorName(const Visual::ArithmeticOperatorType op)
    {
        switch (op)
        {
            case Visual::ArithmeticOperatorType::Addition:          return g_glslArithOpNameAdd;
            case Visual::ArithmeticOperatorType::Division:          return g_glslArithOpNameDiv;
            case Visual::ArithmeticOperatorType::Multiplication:    return g_glslArithOpNameMul;
            case Visual::ArithmeticOperatorType::Subtraction:       return g_glslArithOpNameSub;
        }
        throw Exception("GetGlslArithmeticOperatorName is missing return value for op = " + std::to_string(static_cast<size_t>(op)) + ".");
    }
    
    static std::string GetGlslFloatString(const float value)
    {
        std::string str = std::to_string(value);
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        str.erase(str.find_last_not_of('.') + 1, std::string::npos);
        return str;
    }
    
    static std::string GetGlslDefaultValue(const Visual::Pin& pin)
    {
        if (pin.GetDirection() != Visual::PinDirection::In)
        {
            throw Exception("Direction of pin passed to GetGlslDefaultValue is " + std::to_string(static_cast<size_t>(pin.GetDirection())) + ", expecting an input pin.");
        }

        switch (pin.GetDataType())
        {
            case VariableDataType::Bool:
                return static_cast<const Visual::InputPin<bool>&>(pin).GetDefaultValue() ? "true" : "false";
            case VariableDataType::Int32:
                return std::to_string(static_cast<const Visual::InputPin<int32_t>&>(pin).GetDefaultValue());
            case VariableDataType::Float32:
                return GetGlslFloatString(static_cast<const Visual::InputPin<float>&>(pin).GetDefaultValue());
            case VariableDataType::Vector2f32:
            {
                const auto& vec = static_cast<const Visual::InputPin<Vector2f32>&>(pin).GetDefaultValue();
                return "vec2(" + GetGlslFloatString(vec.x) + ", " + GetGlslFloatString(vec.y) + ")";
            }
            case VariableDataType::Vector3f32:
            {
                const auto& vec = static_cast<const Visual::InputPin<Vector3f32>&>(pin).GetDefaultValue();
                return "vec3(" + GetGlslFloatString(vec.x) + ", " + GetGlslFloatString(vec.y) + ", " + GetGlslFloatString(vec.z) + ")";
            }
            case VariableDataType::Vector4f32:
            {
                const auto& vec = static_cast<const Visual::InputPin<Vector4f32>&>(pin).GetDefaultValue();
                return "vec4(" + GetGlslFloatString(vec.x) + ", " + GetGlslFloatString(vec.y) + ", " + GetGlslFloatString(vec.z) + ", " + GetGlslFloatString(vec.w) + ")";
            }
            case VariableDataType::Matrix4x4f32:
            {
                const auto& mat = static_cast<const Visual::InputPin<Matrix4x4f32>&>(pin).GetDefaultValue();
                return "mat4(" + GetGlslFloatString(mat.e[0]) + ", " +  GetGlslFloatString(mat.e[1]) + ", " +  GetGlslFloatString(mat.e[2]) + ", " +  GetGlslFloatString(mat.e[3]) + ", " +
                                    GetGlslFloatString(mat.e[4]) + ", " +  GetGlslFloatString(mat.e[5]) + ", " +  GetGlslFloatString(mat.e[6]) + ", " +  GetGlslFloatString(mat.e[7]) + ", " +
                                    GetGlslFloatString(mat.e[8]) + ", " +  GetGlslFloatString(mat.e[9]) + ", " +  GetGlslFloatString(mat.e[10]) + ", " + GetGlslFloatString(mat.e[11]) + ", " +
                                    GetGlslFloatString(mat.e[12]) + ", " + GetGlslFloatString(mat.e[13]) + ", " + GetGlslFloatString(mat.e[14]) + ", " + GetGlslFloatString(mat.e[15]) + ")";
            }
            case VariableDataType::Sampler1D: 
            case VariableDataType::Sampler2D: 
            case VariableDataType::Sampler3D: break;
        }

        throw Exception("GetGlslDefaultValue is missing return value for pin.GetDataType() = " + std::to_string(static_cast<size_t>(pin.GetDataType())) + ".");
    }
    
    static std::string GetGlslConstantValue(const Visual::ConstantBase& constant)
    {
        switch (constant.GetDataType())
        {
            case VariableDataType::Bool:
                return static_cast<const Visual::Constant<bool>&>(constant).GetValue() ? "true" : "false";
            case VariableDataType::Int32:
                return std::to_string(static_cast<const Visual::Constant<int32_t>&>(constant).GetValue());
            case VariableDataType::Float32:
                return GetGlslFloatString(static_cast<const Visual::Constant<float>&>(constant).GetValue());
            case VariableDataType::Vector2f32:
            {
                const auto& vec = static_cast<const Visual::Constant<Vector2f32>&>(constant).GetValue();
                return "vec2(" + GetGlslFloatString(vec.x) + ", " + GetGlslFloatString(vec.y) + ")";
            }
            case VariableDataType::Vector3f32:
            {
                const auto& vec = static_cast<const Visual::Constant<Vector3f32>&>(constant).GetValue();
                return "vec3(" + GetGlslFloatString(vec.x) + ", " + GetGlslFloatString(vec.y) + ", " + GetGlslFloatString(vec.z) + ")";
            }
            case VariableDataType::Vector4f32:
            {
                const auto& vec = static_cast<const Visual::Constant<Vector4f32>&>(constant).GetValue();
                return "vec4(" + GetGlslFloatString(vec.x) + ", " + GetGlslFloatString(vec.y) + ", " + GetGlslFloatString(vec.z) + ", " + GetGlslFloatString(vec.w) + ")";
            }
            case VariableDataType::Matrix4x4f32:
            {
                const auto& mat = static_cast<const Visual::Constant<Matrix4x4f32>&>(constant).GetValue();
                return "mat4(" + GetGlslFloatString(mat.e[0]) + ", " +  GetGlslFloatString(mat.e[1]) + ", " +  GetGlslFloatString(mat.e[2]) + ", " +  GetGlslFloatString(mat.e[3]) + ", " +
                                    GetGlslFloatString(mat.e[4]) + ", " +  GetGlslFloatString(mat.e[5]) + ", " +  GetGlslFloatString(mat.e[6]) + ", " +  GetGlslFloatString(mat.e[7]) + ", " +
                                    GetGlslFloatString(mat.e[8]) + ", " +  GetGlslFloatString(mat.e[9]) + ", " +  GetGlslFloatString(mat.e[10]) + ", " + GetGlslFloatString(mat.e[11]) + ", " +
                                    GetGlslFloatString(mat.e[12]) + ", " + GetGlslFloatString(mat.e[13]) + ", " + GetGlslFloatString(mat.e[14]) + ", " + GetGlslFloatString(mat.e[15]) + ")";
            }
            case VariableDataType::Sampler1D: 
            case VariableDataType::Sampler2D: 
            case VariableDataType::Sampler3D: break;
        }
        throw Exception("GetGlslConstantValue is missing return value for constant.GetDataType() = " + std::to_string(static_cast<size_t>(constant.GetDataType())) + ".");
    }
    
    static const std::string& GetGlslFunctionName(const Visual::FunctionType functionType)
    {
        switch (functionType)
        {
            // Constructors
            case Visual::FunctionType::CreateVec2: return g_glslDataTypeVec2;
            case Visual::FunctionType::CreateVec3: return g_glslDataTypeVec3;
            case Visual::FunctionType::CreateVec4: return g_glslDataTypeVec4;
            // Trigonometry
            case Visual::FunctionType::Cos: return g_glslFunctionCos;
            case Visual::FunctionType::Sin: return g_glslFunctionSin;
            case Visual::FunctionType::Tan: return g_glslFunctionTan;
            // Mathematics.
            case Visual::FunctionType::Max: return g_glslFunctionMax;
            case Visual::FunctionType::Min: return g_glslFunctionMin;
            // Vector.
            case Visual::FunctionType::Cross: return g_glslFunctionCross;
            case Visual::FunctionType::Dot:   return g_glslFunctionDot;
        }

        throw Exception("GetGlslFunctionName is missing return value for functionType = " + std::to_string(static_cast<size_t>(functionType)) + ".");
    }

    static bool CreateConstantVariableSource(std::string& source, NodeObjectPointer nodeObject)
    {
        auto& outputVariables = nodeObject->GetOutputVariables();
        if (outputVariables.size() != 1)
        {
            return false;
        }

        auto& outputVariable = outputVariables.front();
        if (outputVariable->name.size())
        {
            return true;
        }

        auto& constantBase = static_cast<const Visual::ConstantBase&>(nodeObject->GetNode());
        auto dataType = constantBase.GetDataType();
        auto& dataTypeName = GetGlslVariableDataType(dataType);

        static size_t index = 0;
        outputVariable->name =  dataTypeName + "_" + std::to_string(index++);
 
        source = dataTypeName + " " + outputVariable->name + " = " + GetGlslConstantValue(constantBase) + ";\n";

        return true;
    }

    static void GetInputNameOrDefault(std::string& source, const InputVariable& inputVariable)
    {
        auto* connection = inputVariable.connection;
        if (!connection)
        {
            source += GetGlslDefaultValue(inputVariable.pin);
        }
        else
        {
            source += connection->name;
        }
    }

    static bool CreateFunctionVariableSource(std::string& source, NodeObjectPointer nodeObject)
    {
        auto& outputVariables = nodeObject->GetOutputVariables();
        if (outputVariables.size() != 1)
        {
            return false;
        }

        auto& outputVariable = outputVariables.front();
        if (outputVariable->name.size())
        {
            return true;
        }
        
        auto& functionBase = static_cast<const Visual::FunctionBase&>(nodeObject->GetNode());
        const std::string& functionName = GetGlslFunctionName(functionBase.GetFunctionType());

        static size_t index = 0;
        outputVariable->name = functionName + "_" + std::to_string(index++);

        auto* outputPin = outputVariable->pin;
        source = GetGlslVariableDataType(outputPin->GetDataType()) + " " + outputVariable->name + " = " +
            GetGlslFunctionName(functionBase.GetFunctionType()) + "(";

        auto& inputVariables = nodeObject->GetInputVariables();
        for (size_t i = 0; i < inputVariables.size() - 1; i++)
        {
            GetInputNameOrDefault(source, inputVariables[i]);
            source += ", ";
        }
        GetInputNameOrDefault(source, inputVariables.back());

        source += ");\n";

        return true;
    }

    static bool CreateOperatorVariableSource(std::string& source, NodeObjectPointer nodeObject)
    {
        auto& outputVariables = nodeObject->GetOutputVariables();
        if (outputVariables.size() != 1)
        {
            return false;
        }

        auto& outputVariable = outputVariables.front();
        if (outputVariable->name.size())
        {
            return true;
        }

        auto& operatorBase = static_cast<const Visual::OperatorBase&>(nodeObject->GetNode());

        switch (operatorBase.GetOperatorType())
        {
            case Visual::OperatorType::Arithmetic:
            {
                auto& inputVariables = nodeObject->GetInputVariables();
                if (inputVariables.size() != 2)
                {
                    return false;
                }

                static size_t index = 0;

                auto& arithmeticOperatorBase = static_cast<const Visual::ArithmeticOperatorBase&>(operatorBase);
                auto arithmeticOperatorType = arithmeticOperatorBase.GetArithmeticOperatorType();
                outputVariable->name = GetGlslArithmeticOperatorName(arithmeticOperatorType) + "_" + std::to_string(index++);
          
                auto* outputPin = outputVariable->pin;
                source = GetGlslVariableDataType(outputPin->GetDataType()) + " " + outputVariable->name + " = ";
                GetInputNameOrDefault(source, inputVariables[0]);
                source += GetGlslArithmeticOperator(arithmeticOperatorType);
                GetInputNameOrDefault(source, inputVariables[1]);
                source += +";\n";
            } break;
            default: break;
        }

        return true;
    }

    static bool CreateOutputAssignmentSource(std::string& source, NodeObjectPointer nodeObject)
    {
        auto& outputVariables = nodeObject->GetOutputVariables();
        auto& inputVariables = nodeObject->GetInputVariables();

        if (outputVariables.size() != inputVariables.size())
        {
            return false;
        }

        source = "";
        for (size_t i = 0; i < outputVariables.size(); i++)
        {
            source += outputVariables[i]->name + " = ";
            GetInputNameOrDefault(source, inputVariables[i]);
            source += ";\n";
        }

        return true;
    }

    static const size_t CalculateEstimatedScriptSize(const Visual::Script& script)
    {
        // Calculate estimated code length.
        constexpr size_t estPreMainLength = 70;
        constexpr size_t estMainLength = 20;
        constexpr size_t estInputInterfaceLength = 40;
        constexpr size_t estOutputInterfaceLength = 40;
        constexpr size_t estVertOutputLength = 35;
        constexpr size_t estPushConstantLength = 50;
        constexpr size_t estLocalVariableLength = 35;
        constexpr size_t estSamplerLength = 55;
        constexpr size_t estUniformBufferBlockLength = 60;
        constexpr size_t estUniformBufferVariableLength = 15;

        auto& descriptorSets = script.GetDescriptorSetsBase();
        auto& inputInterface = script.GetInputInterface();
        auto& outputInterface = script.GetOutputInterface();
        auto& pushConstants = script.GetPushConstantsBase();

        size_t estimatedDescriptorLength = descriptorSets.GetSetCount() * estUniformBufferBlockLength;
        for (size_t i = 0; i < descriptorSets.GetSetCount(); i++)
        {
            auto* set = descriptorSets.GetSetBase(i);
            estimatedDescriptorLength += set->GetBindingCount() * estUniformBufferVariableLength;
        }

        const Visual::VertexOutput* vertexOutput =
            (script.GetType() == Type::Vertex) ? static_cast<const Visual::VertexScript&>(script).GetVertexOutput() : nullptr;

        size_t estimatedSourceLength = estMainLength + estPreMainLength + estimatedDescriptorLength +
            (script.GetNodeCount() * estLocalVariableLength) +
            (inputInterface.GetMemberCount() * estInputInterfaceLength) +
            (outputInterface.GetMemberCount() * estOutputInterfaceLength) +
            (pushConstants.GetMemberCount() * estPushConstantLength) +
            (vertexOutput ? estVertOutputLength : 0);

        return estimatedSourceLength;
    }

    /*bool VulkanGenerator::GenerateGlslTemplate(
        VulkanGenerator::GlslTemplates & glslTemplates,
        const std::vector<Visual::Script*>& scripts,
        Logger* logger)
    {

        return true;
    }*/



    std::vector<uint8_t> VulkanGenerator::GenerateGlsl(
        const Visual::Script& script, 
        //const GlslStageTemplates* templateData,
        Logger* logger)
    {
        const size_t estimatedSourceLength = CalculateEstimatedScriptSize(script);
        std::vector<uint8_t> source;
        source.reserve(estimatedSourceLength);

        auto AppendSource = [&source](const std::string& input)
        {
            std::copy(input.begin(), input.end(), std::back_inserter(source));
        };

        AppendSource("#version 450\n");
        AppendSource("#extension GL_ARB_separate_shader_objects : enable\n");

        std::map<const Visual::Pin*, OutputVariablePointer> visitedOutputVariables;
        std::map<const Visual::Node*, NodeObjectPointer> visitedNodeObjects;
        
        size_t index = 0;

        // Input interface.
        auto& inputInterface = script.GetInputInterface();
        auto inputInterfaceOutputPins = inputInterface.GetOutputPins();
        if (inputInterfaceOutputPins.size())
        {
            auto nodeObject = std::make_shared<NodeObject>(inputInterface);
            visitedNodeObjects.insert({ &inputInterface, nodeObject });
            
            for (auto* pin : inputInterfaceOutputPins)
            {
                std::string name = "in_" + std::to_string(index);
                auto outputVariable = nodeObject->CreateOutputVariable(pin, std::move(name));
                visitedOutputVariables.insert({ pin, outputVariable });

                AppendSource(
                    "layout(location = " + std::to_string(index) + ") in " +
                    GetGlslVariableDataType(pin->GetDataType()) + " " + name + ";\n");

                index++;
            }
        }

        // Push constants
        auto& pushConstants = script.GetPushConstantsBase();
        auto pushConstantsOutputPins = pushConstants.GetOutputPins();
        if (inputInterfaceOutputPins.size())
        {
            auto nodeObject = std::make_shared<NodeObject>(pushConstants);
            visitedNodeObjects.insert({ &pushConstants, nodeObject });

            AppendSource("layout(std140, push_constant) uniform s_pc\n{\n");
            size_t offset = 0;
            for (auto* pin : pushConstantsOutputPins)
            {
                std::string currentOffset = std::to_string(offset);
                std::string name = "mem" + currentOffset;
                std::string pushConstantName = "pc." + name;
                auto outputVariable = nodeObject->CreateOutputVariable(pin, std::move(pushConstantName));
                visitedOutputVariables.insert({ pin, outputVariable });

                AppendSource("layout(offset = " + currentOffset +") " + GetGlslVariableDataType(pin->GetDataType()) +" " + name +";\n");
                offset += GetGlslVariableOffset(pin->GetDataType());
            }
            AppendSource("} pc;\n");
        }

        // Descriptor sets.
        size_t samplerIndex = 0;
        size_t uboIndex = 0;
        auto& descriptorSets = script.GetDescriptorSetsBase();
        for (size_t i = 0; i < descriptorSets.GetSetCount(); i++)
        {
            auto* set = descriptorSets.GetSetBase(i);
            const uint32_t setId = set->GetId();
            for (size_t j = 0; j < set->GetBindingCount(); j++)
            {
                auto* binding = set->GetBindingBase(j);
                auto nodeObject = std::make_shared<NodeObject>(*binding);
                visitedNodeObjects.insert({ binding, nodeObject });

                const uint32_t bindingId = binding->GetId();
                switch (binding->GetBindingType())
                {
                    case Visual::DescriptorBindingType::Sampler1D:
                    case Visual::DescriptorBindingType::Sampler2D: 
                    case Visual::DescriptorBindingType::Sampler3D: 
                    {
                        auto* pin = binding->GetOutputPin();
                        std::string name = "sampler_" + std::to_string(samplerIndex);
                        auto outputVariable = nodeObject->CreateOutputVariable(pin, std::move(name));
                        visitedOutputVariables.insert({ pin, outputVariable });

                        AppendSource("layout(set = " + std::to_string(setId) + ", binding = " + std::to_string(bindingId) +
                            ") uniform " + GetGlslVariableDataType(pin->GetDataType()) + " " + name + ";\n");
 
                        ++samplerIndex;
                    } break;
                    case Visual::DescriptorBindingType::UniformBuffer: 
                    {                  
                        const std::string blockName = "ubo_" + std::to_string(uboIndex);
                        AppendSource("layout(std140, set = " + std::to_string(setId) + ", binding=" + std::to_string(bindingId) +
                            ") uniform s_" + blockName + "\n{\n");

                        size_t uboMemberIndex = 0;
                        for (auto* pin : binding->GetOutputPins())
                        {
                            std::string memberName = "val_" + std::to_string(uboMemberIndex);
                            std::string name = blockName + "." + memberName;
                            auto outputVariable = nodeObject->CreateOutputVariable(pin, std::move(name));
                            visitedOutputVariables.insert({ pin, outputVariable });

                            AppendSource(GetGlslVariableDataType(pin->GetDataType()) + " " + memberName + ";\n");
                            ++uboMemberIndex;
                        }

                        AppendSource("} " + blockName +";\n");

                        ++uboIndex;
                    } break;
                }
            }
        }
        
        // Output interface.
        auto& outputInterface = script.GetOutputInterface();
        std::vector<NodeObjectPointer> outputNodes;

        auto outputInterfaceInputPins = outputInterface.GetInputPins();
        if (outputInterfaceInputPins.size())
        {
            auto nodeObject = std::make_shared<NodeObject>(outputInterface);
            outputNodes.push_back(nodeObject);

            index = 0;
            for (auto* pin : outputInterfaceInputPins)
            {
                const std::string name = "out_" + std::to_string(index);

                nodeObject->AddOutputPin(pin, name);

                AppendSource(
                    "layout(location = " + std::to_string(index) + ") out " +
                    GetGlslVariableDataType(pin->GetDataType()) + " " + name + ";\n");

                index++;
            }
        }

        const Visual::VertexOutput* vertexOutput = (script.GetType() == Type::Vertex) ? 
            static_cast<const Visual::VertexScript&>(script).GetVertexOutput() : nullptr;
        if (vertexOutput)
        {
            auto nodeObject = std::make_shared<NodeObject>(*vertexOutput);
            outputNodes.push_back(nodeObject);
            nodeObject->AddOutputPin(vertexOutput->GetInputPin(), "gl_Position");    
        }

        AppendSource("void main()\n{\n");
        
        size_t localVariableIndex = 0;
        auto GetNextLocalVariablePostfix = [&localVariableIndex]() -> std::string
        {
            return "_" + std::to_string(localVariableIndex++);
        };

        for (auto& outputNode : outputNodes)
        {
            std::stack<NodeObjectPointer> nodeObjectStack;
            nodeObjectStack.push(outputNode);

            while (nodeObjectStack.size())
            {
                auto& nodeObject = nodeObjectStack.top();

                // Get next input variable of node.
                auto* inputVariable = nodeObject->GetNextInputVariable();

                // Is null if no more inputs are available and if we should skip to source code creation.
                if (inputVariable)
                {
                    auto* pinConnection = inputVariable->pin.GetConnection();
                    
                    // No connection(default value).
                    if (!pinConnection)
                    {
                        continue;
                    }

                    // Already created output variable, reuse it.
                    auto vovIt = visitedOutputVariables.find(pinConnection);
                    if (vovIt != visitedOutputVariables.end())
                    {
                        inputVariable->connection = vovIt->second.get();
                        continue;
                    }

                    // Get or create new node object if needed.
                    NodeObjectPointer nodeObjectPointer;

                    auto& node = pinConnection->GetNode();
                    auto vnoIt = visitedNodeObjects.find(&node);
                    if (vnoIt != visitedNodeObjects.end())
                    {
                        nodeObjectPointer = vnoIt->second;
                    }
                    else
                    {
                        nodeObjectPointer = std::make_shared<NodeObject>(node);
                        visitedNodeObjects.insert({ &node, nodeObjectPointer });
                    }

                    // Create new output variable and connect it.
                    auto outputVariable = nodeObjectPointer->CreateOutputVariable(pinConnection);
                    inputVariable->connection = outputVariable.get();
                    visitedOutputVariables.insert({ pinConnection, outputVariable });

                    // Continue to new output object.
                    nodeObjectStack.push(nodeObjectPointer);
                    continue;
                }

                // All input variables of current node is travered, let's build the source code.
                auto& node = nodeObject->GetNode();
                switch (node.GetType())
                {
                    case Visual::NodeType::Constant:
                    {
                        std::string declaration;
                        if (!CreateConstantVariableSource(declaration, nodeObject))
                        {
                            Logger::WriteError(logger, "Failed to create write constant.");
                            return {};
                        }
                        AppendSource(declaration);
                    } break;
                    case Visual::NodeType::Function:
                    {
                        std::string declaration;
                        if (!CreateFunctionVariableSource(declaration, nodeObject))
                        {
                            Logger::WriteError(logger, "Failed to create write function.");
                            return {};
                        }
                        AppendSource(declaration);
                    } break;
                    case Visual::NodeType::Operator:
                    {
                        std::string declaration;
                        if (!CreateOperatorVariableSource(declaration, nodeObject))
                        {
                            Logger::WriteError(logger, "Failed to create write operator.");
                            return {};
                        }
                        AppendSource(declaration);
                    } break;
                    case Visual::NodeType::VertexOutput:
                    case Visual::NodeType::Output:
                    {
                        std::string assignment;
                        if (!CreateOutputAssignmentSource(assignment, nodeObject))
                        {
                            Logger::WriteError(logger, "Failed to create write output.");
                            return {};
                        }
                        AppendSource(assignment);
                    } break;
                    default: break;
                }

                nodeObjectStack.pop();
            }
        }

        AppendSource("}\n");

        return source;
    }

#if defined(MOLTEN_ENABLE_GLSLANG)
    std::vector<uint8_t> VulkanGenerator::ConvertGlslToSpriV(const std::vector<uint8_t>& code, Type shaderType, Logger* logger)
    {
        // Helper function for getting the shader type.
        static auto GetEShShaderType = [](const Shader::Type type) -> EShLanguage
        {
            switch (type)
            {
                case Type::Vertex:   return EShLanguage::EShLangVertex;
                case Type::Fragment: return EShLanguage::EShLangFragment;
                default: break;
            }

            throw Exception("Provided shader type is not supported by the shader compiler.");
        };

        // Helper function for building the default resources.
        static auto GetDefaultResources = []()->TBuiltInResource
        {
            TBuiltInResource resource = {};

            resource.limits.doWhileLoops = true;
            resource.limits.generalAttributeMatrixVectorIndexing = true;
            resource.limits.generalConstantMatrixVectorIndexing = true;
            resource.limits.generalSamplerIndexing = true;
            resource.limits.generalUniformIndexing = true;
            resource.limits.generalVariableIndexing = true;
            resource.limits.generalVaryingIndexing = true;
            resource.limits.nonInductiveForLoops = true;
            resource.limits.whileLoops = true;
            resource.maxAtomicCounterBindings = 1;
            resource.maxAtomicCounterBufferSize = 16384;
            resource.maxClipDistances = 8;
            resource.maxClipPlanes = 6;
            resource.maxCombinedAtomicCounterBuffers = 1;
            resource.maxCombinedAtomicCounters = 8;
            resource.maxCombinedClipAndCullDistances = 8;
            resource.maxCombinedImageUniforms = 8;
            resource.maxCombinedImageUnitsAndFragmentOutputs = 8;
            resource.maxCombinedShaderOutputResources = 8;
            resource.maxCombinedTextureImageUnits = 80;
            resource.maxComputeAtomicCounterBuffers = 1;
            resource.maxComputeAtomicCounters = 8;
            resource.maxComputeImageUniforms = 8;
            resource.maxComputeTextureImageUnits = 16;
            resource.maxComputeUniformComponents = 1024;
            resource.maxComputeWorkGroupCountX = 65535;
            resource.maxComputeWorkGroupCountY = 65535;
            resource.maxComputeWorkGroupCountZ = 65535;
            resource.maxComputeWorkGroupSizeX = 1024;
            resource.maxComputeWorkGroupSizeY = 1024;
            resource.maxComputeWorkGroupSizeZ = 64;
            resource.maxCullDistances = 8;
            resource.maxDrawBuffers = 32;
            resource.maxFragmentAtomicCounterBuffers = 1;
            resource.maxFragmentAtomicCounters = 8;
            resource.maxFragmentImageUniforms = 8;
            resource.maxFragmentInputComponents = 128;
            resource.maxFragmentInputVectors = 15;
            resource.maxFragmentUniformComponents = 4096;
            resource.maxFragmentUniformVectors = 16;
            resource.maxGeometryAtomicCounterBuffers = 0;
            resource.maxGeometryAtomicCounters = 0;
            resource.maxGeometryImageUniforms = 0;
            resource.maxGeometryInputComponents = 64;
            resource.maxGeometryOutputComponents = 128;
            resource.maxGeometryOutputVertices = 256;
            resource.maxGeometryTextureImageUnits = 16;
            resource.maxGeometryTotalOutputComponents = 1024;
            resource.maxGeometryUniformComponents = 1024;
            resource.maxGeometryVaryingComponents = 64;
            resource.maxImageSamples = 0;
            resource.maxImageUnits = 8;
            resource.maxLights = 32;
            resource.maxMeshOutputPrimitivesNV = 512;
            resource.maxMeshOutputVerticesNV = 256;
            resource.maxMeshViewCountNV = 4;
            resource.maxMeshWorkGroupSizeX_NV = 32;
            resource.maxMeshWorkGroupSizeY_NV = 1;
            resource.maxMeshWorkGroupSizeZ_NV = 1;
            resource.maxPatchVertices = 32;
            resource.maxProgramTexelOffset = 7;
            resource.maxSamples = 4;
            resource.maxTaskWorkGroupSizeX_NV = 32;
            resource.maxTaskWorkGroupSizeY_NV = 1;
            resource.maxTaskWorkGroupSizeZ_NV = 1;
            resource.maxTessControlAtomicCounterBuffers = 0;
            resource.maxTessControlAtomicCounters = 0;
            resource.maxTessControlImageUniforms = 0;
            resource.maxTessControlInputComponents = 128;
            resource.maxTessControlOutputComponents = 128;
            resource.maxTessControlTextureImageUnits = 16;
            resource.maxTessControlTotalOutputComponents = 4096;
            resource.maxTessControlUniformComponents = 1024;
            resource.maxTessEvaluationAtomicCounterBuffers = 0;
            resource.maxTessEvaluationAtomicCounters = 0;
            resource.maxTessEvaluationImageUniforms = 0;
            resource.maxTessEvaluationInputComponents = 128;
            resource.maxTessEvaluationOutputComponents = 128;
            resource.maxTessEvaluationTextureImageUnits = 16;
            resource.maxTessEvaluationUniformComponents = 1024;
            resource.maxTessGenLevel = 64;
            resource.maxTessPatchComponents = 120;
            resource.maxTextureCoords = 32;
            resource.maxTextureImageUnits = 32;
            resource.maxTextureUnits = 32;
            resource.maxTransformFeedbackBuffers = 4;
            resource.maxTransformFeedbackInterleavedComponents = 64;
            resource.maxVaryingComponents = 60;
            resource.maxVaryingFloats = 64;
            resource.maxVaryingVectors = 8;
            resource.maxVertexAtomicCounterBuffers = 0;
            resource.maxVertexAtomicCounters = 0;
            resource.maxVertexAttribs = 64;
            resource.maxVertexImageUniforms = 0;
            resource.maxVertexOutputComponents = 64;
            resource.maxVertexOutputVectors = 16;
            resource.maxVertexTextureImageUnits = 32;
            resource.maxVertexUniformComponents = 4096;
            resource.maxVertexUniformVectors = 128;
            resource.maxViewports = 16;
            resource.minProgramTexelOffset = -8;

            return resource;
        };

        static const TBuiltInResource resources = GetDefaultResources();

        // Initialize glslang for the first time we run.
        static bool glslangInitialized = false;
        if (!glslangInitialized)
        {
            glslang::InitializeProcess();
            glslangInitialized = true;
        }

        // Set configs of shader.
        EShLanguage language = GetEShShaderType(shaderType);
        int clientInputSemanticsVersion = 100; // maps to, say, #define VULKAN 100
        glslang::EShTargetClientVersion VulkanClientVersion = glslang::EShTargetVulkan_1_1;
        glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_1;

        glslang::TShader shader(language);
        shader.setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientVulkan, clientInputSemanticsVersion);
        shader.setEnvClient(glslang::EShClientVulkan, VulkanClientVersion);
        shader.setEnvTarget(glslang::EShTargetSpv, TargetVersion);

        const char* inputCString = reinterpret_cast<const char*>(code.data());
        const int inputCLength = static_cast<int>(code.size());
        shader.setStringsWithLengths(&inputCString, &inputCLength, 1);

        EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
        const int defaultVersion = 100;

        // Preprocess the shader.
        std::string preprocessedGLSL;
        glslang::TShader::ForbidIncluder forbidIncluder;
        if (!shader.preprocess(&resources, defaultVersion, ENoProfile, false, false, messages, &preprocessedGLSL, forbidIncluder))
        {
            Logger::WriteError(logger, std::string("Shader preprocessing failed: ") + shader.getInfoLog());
            return {};
        }

        // Parse the shader.
        const char* preprocessedConstStr = preprocessedGLSL.c_str();
        shader.setStrings(&preprocessedConstStr, 1);

        if (!shader.parse(&resources, 100, false, messages))
        {
            Logger::WriteError(logger, std::string("Shader parsing failed: ") + shader.getInfoLog());
            return {};
        }

        // Link the shader.
        glslang::TProgram program;
        program.addShader(&shader);

        if (!program.link(messages))
        {
            Logger::WriteError(logger, std::string("Shader linking failed: ") + shader.getInfoLog());
            return {};
        }

        // Get SPIR-V.
        std::vector<unsigned int> spirv;
        spv::SpvBuildLogger spvLogger;
        glslang::SpvOptions spvOptions;
        glslang::GlslangToSpv(*program.getIntermediate(language), spirv, &spvLogger, &spvOptions);

        size_t outputSize = sizeof(unsigned int) * spirv.size();
        if (outputSize == 0)
        {
           Logger::WriteError(logger, "Shader convertion failed.");
            return {};
        }

        std::vector<uint8_t> output(outputSize);
        std::memcpy(output.data(), spirv.data(), outputSize);

        return output;
    }
#else
    std::vector<uint8_t> VulkanGenerator::ConvertGlslToSpriV(const std::vector<uint8_t>&, Type, Logger* logger)
    {
        Logger::WriteError(logger, "Failed to convert GLSL code to SPIR-V. MOLTEN_ENABLE_GLSLANG is not enabled.");
        return {};
    }
#endif

}