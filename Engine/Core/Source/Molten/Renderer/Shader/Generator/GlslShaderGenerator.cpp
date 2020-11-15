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

#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Renderer/Shader/Generator/ShaderGeneratorNode.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Logger.hpp"
#include <memory>
#include <map>
#include <stack>
#include <cstdlib>

#if defined(MOLTEN_ENABLE_GLSLANG)
#include "ThirdParty/glslang/glslang/Public/ShaderLang.h"
#include "ThirdParty/glslang/SPIRV/GlslangToSpv.h"
#endif

namespace Molten::Shader
{

    // Global static implementations.
    static const std::string g_emptyString = "";
    static const std::string g_dataTypeBool = "bool";
    static const std::string g_dataTypeInt = "int";
    static const std::string g_dataTypeFloat = "float";
    static const std::string g_dataTypeVec2 = "vec2";
    static const std::string g_dataTypeVec3 = "vec3";
    static const std::string g_dataTypeVec4 = "vec4";
    static const std::string g_dataTypeMat4 = "mat4";
    static const std::string g_dataTypeSample1D = "sampler1D";
    static const std::string g_dataTypeSample2D = "sampler2D";
    static const std::string g_dataTypeSample3D = "sampler3D";

    static const std::string g_functionCos = "cos";
    static const std::string g_functionSin = "sin";
    static const std::string g_functionTan = "tan";
    static const std::string g_functionMax = "max";
    static const std::string g_functionMin = "min";
    static const std::string g_functionCross = "cross";
    static const std::string g_functionDot = "dot";
    static const std::string g_functionTexture = "texture";

    static const std::string g_arithmeticOperatorTokenAdd = "+";
    static const std::string g_arithmeticOperatorTokenDiv = "/";
    static const std::string g_arithmeticOperatorTokenMul = "*";
    static const std::string g_arithmeticOperatorTokenSub = "-";

    static const std::string g_arithmeticOperatorNameAdd = "add";
    static const std::string g_arithmeticOperatorNameDiv = "div";
    static const std::string g_arithmeticOperatorNameMul = "mul";
    static const std::string g_arithmeticOperatorNameSub = "sub";

    static const std::string& GetVariableDataType(const VariableDataType dataType)
    {
        switch (dataType)
        {
            case VariableDataType::Bool:         return g_dataTypeBool;
            case VariableDataType::Int32:        return g_dataTypeInt;
            case VariableDataType::Float32:      return g_dataTypeFloat;
            case VariableDataType::Vector2f32:   return g_dataTypeVec2;
            case VariableDataType::Vector3f32:   return g_dataTypeVec3;
            case VariableDataType::Vector4f32:   return g_dataTypeVec4;
            case VariableDataType::Matrix4x4f32: return g_dataTypeMat4;
            case VariableDataType::Sampler1D:    return g_dataTypeSample1D;
            case VariableDataType::Sampler2D:    return g_dataTypeSample2D;
            case VariableDataType::Sampler3D:    return g_dataTypeSample3D;
        }
        return g_emptyString;
    }

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

    static std::string GetFloatString(const float value)
    {
        std::string str = std::to_string(value);
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        str.erase(str.find_last_not_of('.') + 1, std::string::npos);
        return str;
    }

    static std::string GetConstantValue(const Visual::ConstantBase& constant)
    {
        switch (constant.GetDataType())
        {
            case VariableDataType::Bool:
                return static_cast<const Visual::Constant<bool>&>(constant).GetValue() ? "true" : "false";
            case VariableDataType::Int32:
                return std::to_string(static_cast<const Visual::Constant<int32_t>&>(constant).GetValue());
            case VariableDataType::Float32:
                return GetFloatString(static_cast<const Visual::Constant<float>&>(constant).GetValue());
            case VariableDataType::Vector2f32:
            {
                const auto& vec = static_cast<const Visual::Constant<Vector2f32>&>(constant).GetValue();
                return "vec2(" + GetFloatString(vec.x) + ", " + GetFloatString(vec.y) + ")";
            }
            case VariableDataType::Vector3f32:
            {
                const auto& vec = static_cast<const Visual::Constant<Vector3f32>&>(constant).GetValue();
                return "vec3(" + GetFloatString(vec.x) + ", " + GetFloatString(vec.y) + ", " + GetFloatString(vec.z) + ")";
            }
            case VariableDataType::Vector4f32:
            {
                const auto& vec = static_cast<const Visual::Constant<Vector4f32>&>(constant).GetValue();
                return "vec4(" + GetFloatString(vec.x) + ", " + GetFloatString(vec.y) + ", " + GetFloatString(vec.z) + ", " + GetFloatString(vec.w) + ")";
            }
            case VariableDataType::Matrix4x4f32:
            {
                const auto& mat = static_cast<const Visual::Constant<Matrix4x4f32>&>(constant).GetValue();
                return "mat4(" + GetFloatString(mat.e[0]) + ", " + GetFloatString(mat.e[1]) + ", " + GetFloatString(mat.e[2]) + ", " + GetFloatString(mat.e[3]) + ", " +
                    GetFloatString(mat.e[4]) + ", " + GetFloatString(mat.e[5]) + ", " + GetFloatString(mat.e[6]) + ", " + GetFloatString(mat.e[7]) + ", " +
                    GetFloatString(mat.e[8]) + ", " + GetFloatString(mat.e[9]) + ", " + GetFloatString(mat.e[10]) + ", " + GetFloatString(mat.e[11]) + ", " +
                    GetFloatString(mat.e[12]) + ", " + GetFloatString(mat.e[13]) + ", " + GetFloatString(mat.e[14]) + ", " + GetFloatString(mat.e[15]) + ")";
            }
            case VariableDataType::Sampler1D:
            case VariableDataType::Sampler2D:
            case VariableDataType::Sampler3D: break;
        }
        return g_emptyString;
    }

    static std::string GetDefaultValue(const Visual::Pin& pin)
    {
        if (pin.GetDirection() != Visual::PinDirection::In)
        {
            //throw Exception("Direction of pin passed to GetGlslDefaultValue is " + std::to_string(static_cast<size_t>(pin.GetDirection())) + ", expecting an input pin.");
            return g_emptyString;
        }

        switch (pin.GetDataType())
        {
            case VariableDataType::Bool:
                return static_cast<const Visual::InputPin<bool>&>(pin).GetDefaultValue() ? "true" : "false";
            case VariableDataType::Int32:
                return std::to_string(static_cast<const Visual::InputPin<int32_t>&>(pin).GetDefaultValue());
            case VariableDataType::Float32:
                return GetFloatString(static_cast<const Visual::InputPin<float>&>(pin).GetDefaultValue());
            case VariableDataType::Vector2f32:
            {
                const auto& vec = static_cast<const Visual::InputPin<Vector2f32>&>(pin).GetDefaultValue();
                return "vec2(" + GetFloatString(vec.x) + ", " + GetFloatString(vec.y) + ")";
            }
            case VariableDataType::Vector3f32:
            {
                const auto& vec = static_cast<const Visual::InputPin<Vector3f32>&>(pin).GetDefaultValue();
                return "vec3(" + GetFloatString(vec.x) + ", " + GetFloatString(vec.y) + ", " + GetFloatString(vec.z) + ")";
            }
            case VariableDataType::Vector4f32:
            {
                const auto& vec = static_cast<const Visual::InputPin<Vector4f32>&>(pin).GetDefaultValue();
                return "vec4(" + GetFloatString(vec.x) + ", " + GetFloatString(vec.y) + ", " + GetFloatString(vec.z) + ", " + GetFloatString(vec.w) + ")";
            }
            case VariableDataType::Matrix4x4f32:
            {
                const auto& mat = static_cast<const Visual::InputPin<Matrix4x4f32>&>(pin).GetDefaultValue();
                return "mat4(" + GetFloatString(mat.e[0]) + ", " + GetFloatString(mat.e[1]) + ", " + GetFloatString(mat.e[2]) + ", " + GetFloatString(mat.e[3]) + ", " +
                    GetFloatString(mat.e[4]) + ", " + GetFloatString(mat.e[5]) + ", " + GetFloatString(mat.e[6]) + ", " + GetFloatString(mat.e[7]) + ", " +
                    GetFloatString(mat.e[8]) + ", " + GetFloatString(mat.e[9]) + ", " + GetFloatString(mat.e[10]) + ", " + GetFloatString(mat.e[11]) + ", " +
                    GetFloatString(mat.e[12]) + ", " + GetFloatString(mat.e[13]) + ", " + GetFloatString(mat.e[14]) + ", " + GetFloatString(mat.e[15]) + ")";
            }
            case VariableDataType::Sampler1D:
            case VariableDataType::Sampler2D:
            case VariableDataType::Sampler3D: break;
        }

        //throw Exception("GetGlslDefaultValue is missing return value for pin.GetDataType() = " + std::to_string(static_cast<size_t>(pin.GetDataType())) + ".");
        return g_emptyString;
    }

    static std::string GetInputNameOrDefault(const GeneratorInputVariable& inputVariable)
    {
        auto* connection = inputVariable.connection;
        if (!connection)
        {
            return GetDefaultValue(inputVariable.pin);
        }

        return connection->name;
    }


    // Glsl shader generator implementations.

#if defined(MOLTEN_ENABLE_GLSLANG)
    std::vector<uint8_t> GlslGenerator::ConvertGlslToSpriV(const std::vector<uint8_t>& code, Type shaderType, Logger* logger)
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

            throw Exception("SPIR-V: Provided shader type is not supported by the shader compiler.");
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
            Logger::WriteError(logger, std::string("SPIR-V: Shader preprocessing failed: ") + shader.getInfoLog());
            return {};
        }

        // Parse the shader.
        const char* preprocessedConstStr = preprocessedGLSL.c_str();
        shader.setStrings(&preprocessedConstStr, 1);

        if (!shader.parse(&resources, 100, false, messages))
        {
            Logger::WriteError(logger, std::string("SPIR-V: Shader parsing failed: ") + shader.getInfoLog());
            return {};
        }

        // Link the shader.
        glslang::TProgram program;
        program.addShader(&shader);

        if (!program.link(messages))
        {
            Logger::WriteError(logger, std::string("SPIR-V: Shader linking failed: ") + shader.getInfoLog());
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
            Logger::WriteError(logger, "SPIR-V: Shader convertion failed.");
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


    GlslGenerator::GlslGenerator() :
        m_script(nullptr),
        m_logger(nullptr)
    {}

    std::vector<uint8_t> GlslGenerator::Generate(const Visual::Script& script, const Compability compability, Logger* logger)
    {
        PrepareGeneration(script, logger);

        if(!GenerateHeader(compability) || !GenerateMain())
        {
            Logger::WriteError(m_logger, "Failed to generate shader code.");
            return {};
        }

        return std::move(m_sourceBuffer);
    }

    // Glsl generator counters implementations.
    GlslGenerator::Counters::Counters() :
        boolCount{ 0 },
        int32Count{ 0 },
        float32Count{ 0 },
        vector2f32Count{ 0 },
        vector3f32Count{ 0 },
        vector4f32Count{ 0 },
        matrix4x4f32Count{ 0 },
        sampler1DCount{ 0 },
        sampler2DCount{ 0 },
        sampler3DCount{ 0 },

        cosCount{ 0 },
        sinCount{ 0 },
        tanCount{ 0 },
        minCount{ 0 },
        maxCount{ 0 },
        dotCount{ 0 },
        crossCount{ 0 },
        textureCount{ 0 },

        addCount{ 0 },
        subCount{ 0 },
        mulCount{ 0 },
        divCount{ 0 }
    {}

    void GlslGenerator::Counters::Reset()
    {
        ResetVariables();
        ResetFunctions();
        ResetOperators();
    }

    void GlslGenerator::Counters::ResetVariables()
    {
        boolCount = 0;
        int32Count = 0;
        float32Count = 0;
        vector2f32Count = 0;
        vector3f32Count = 0;
        vector4f32Count = 0;
        matrix4x4f32Count = 0;
        sampler1DCount = 0;
        sampler2DCount = 0;
        sampler3DCount = 0;
    }

    void GlslGenerator::Counters::ResetFunctions()
    {
        cosCount = 0;
        sinCount = 0;
        tanCount = 0;
        minCount = 0;
        maxCount = 0;
        dotCount = 0;
        crossCount = 0;
        textureCount = 0;
    }

    void GlslGenerator::Counters::ResetOperators()
    {
        addCount = 0;
        subCount = 0;
        mulCount = 0;
        divCount = 0;
    }

    std::string GlslGenerator::Counters::GetNextVariableName(const VariableDataType dataType)
    {
        switch (dataType)
        {
            case VariableDataType::Bool:         return g_dataTypeBool + "_" + std::to_string(boolCount++);
            case VariableDataType::Int32:        return g_dataTypeInt + "_" + std::to_string(int32Count++);
            case VariableDataType::Float32:      return g_dataTypeFloat + "_" + std::to_string(float32Count++);
            case VariableDataType::Vector2f32:   return g_dataTypeVec2 + "_" + std::to_string(vector2f32Count++);
            case VariableDataType::Vector3f32:   return g_dataTypeVec3 + "_" + std::to_string(vector3f32Count++);
            case VariableDataType::Vector4f32:   return g_dataTypeVec4 + "_" + std::to_string(vector4f32Count++);
            case VariableDataType::Matrix4x4f32: return g_dataTypeMat4 + "_" + std::to_string(matrix4x4f32Count++);
            case VariableDataType::Sampler1D:    return g_dataTypeSample1D + "_" + std::to_string(sampler1DCount++);
            case VariableDataType::Sampler2D:    return g_dataTypeSample2D + "_" + std::to_string(sampler2DCount++);
            case VariableDataType::Sampler3D:    return g_dataTypeSample3D + "_" + std::to_string(sampler3DCount++);
        }
        return "";
    }

    const std::string& GlslGenerator::Counters::GetNextVariableName(std::string& name, const VariableDataType dataType)
    {
        switch (dataType)
        {
            case VariableDataType::Bool:         name = g_dataTypeBool + "_" + std::to_string(boolCount++); return g_dataTypeBool;
            case VariableDataType::Int32:        name = g_dataTypeInt + "_" + std::to_string(int32Count++); return g_dataTypeInt;
            case VariableDataType::Float32:      name = g_dataTypeFloat + "_" + std::to_string(float32Count++); return g_dataTypeFloat;
            case VariableDataType::Vector2f32:   name = g_dataTypeVec2 + "_" + std::to_string(vector2f32Count++); return g_dataTypeVec2;
            case VariableDataType::Vector3f32:   name = g_dataTypeVec3 + "_" + std::to_string(vector3f32Count++); return g_dataTypeVec3;
            case VariableDataType::Vector4f32:   name = g_dataTypeVec4 + "_" + std::to_string(vector4f32Count++); return g_dataTypeVec4;
            case VariableDataType::Matrix4x4f32: name = g_dataTypeMat4 + "_" + std::to_string(matrix4x4f32Count++); return g_dataTypeMat4;
            case VariableDataType::Sampler1D:    name = g_dataTypeSample1D + "_" + std::to_string(sampler1DCount++); return g_dataTypeSample1D;
            case VariableDataType::Sampler2D:    name = g_dataTypeSample2D + "_" + std::to_string(sampler2DCount++); return g_dataTypeSample2D;
            case VariableDataType::Sampler3D:    name = g_dataTypeSample3D + "_" + std::to_string(sampler3DCount++); return g_dataTypeSample3D;
        }

        name = "";
        return g_emptyString;
    }

    const std::string& GlslGenerator::Counters::GetNextFunctionName(std::string& name, const Visual::FunctionType functionType)
    {
        switch (functionType)
        {
            // Constructors
            case Visual::FunctionType::CreateVec2: name = g_dataTypeVec2 + "_" + std::to_string(vector2f32Count++); return g_dataTypeVec2;
            case Visual::FunctionType::CreateVec3: name = g_dataTypeVec3 + "_" + std::to_string(vector3f32Count++); return g_dataTypeVec3;
            case Visual::FunctionType::CreateVec4: name = g_dataTypeVec4 + "_" + std::to_string(vector4f32Count++); return g_dataTypeVec4;
            // Trigonometry
            case Visual::FunctionType::Cos: name = g_functionCos + "_" + std::to_string(cosCount++); return g_functionCos;
            case Visual::FunctionType::Sin: name = g_functionSin + "_" + std::to_string(sinCount++); return g_functionSin;
            case Visual::FunctionType::Tan: name = g_functionTan + "_" + std::to_string(tanCount++); return g_functionTan;
            // Mathematics.
            case Visual::FunctionType::Max: name = g_functionMax + "_" + std::to_string(maxCount++); return g_functionMax;
            case Visual::FunctionType::Min: name = g_functionMin + "_" + std::to_string(minCount++); return g_functionMin;
            // Vector.
            case Visual::FunctionType::Cross: name = g_functionCross + "_" + std::to_string(crossCount++); return g_functionCross;
            case Visual::FunctionType::Dot:   name = g_functionDot + "_" + std::to_string(dotCount++); return g_functionDot;

            // Texture
            case Visual::FunctionType::Texture1D:
            case Visual::FunctionType::Texture2D:
            case Visual::FunctionType::Texture3D:   name = g_functionTexture + "_" + std::to_string(textureCount++); return g_functionTexture;
                
        }

        name = "";
        return g_emptyString;
    }

    const std::string& GlslGenerator::Counters::GetNextOperatorName(std::string& name, const Visual::ArithmeticOperatorType operatorType)
    {
        switch(operatorType)
        {
            case Visual::ArithmeticOperatorType::Addition: name = g_arithmeticOperatorNameAdd + "_" + std::to_string(addCount++); return g_arithmeticOperatorTokenAdd;
            case Visual::ArithmeticOperatorType::Subtraction: name = g_arithmeticOperatorNameSub + "_" + std::to_string(subCount++); return g_arithmeticOperatorTokenSub;
            case Visual::ArithmeticOperatorType::Multiplication: name = g_arithmeticOperatorNameMul + "_" + std::to_string(mulCount++); return g_arithmeticOperatorTokenMul;
            case Visual::ArithmeticOperatorType::Division: name = g_arithmeticOperatorNameDiv + "_" + std::to_string(divCount++); return g_arithmeticOperatorTokenDiv;
        }

        name = "";
        return g_emptyString;
    }

    size_t GlslGenerator::CalculateEstimatedScriptSize(const Visual::Script& script)
    {
        // Calculate estimated code length.
        static const size_t estPreMainLength = 70;
        static const size_t estMainLength = 20;
        static const size_t estInputInterfaceLength = 40;
        static const size_t estOutputInterfaceLength = 40;
        static const size_t estVertOutputLength = 35;
        static const size_t estPushConstantLength = 50;
        static const size_t estLocalVariableLength = 35;
        //static const size_t estSamplerLength = 55;
        static const size_t estUniformBufferBlockLength = 60;
        static const size_t estUniformBufferVariableLength = 15;

        const auto& descriptorSets = script.GetDescriptorSetsBase();
        const auto& inputInterface = script.GetInputInterface();
        const auto& outputInterface = script.GetOutputInterface();
        const auto& pushConstants = script.GetPushConstantsBase();

        size_t estimatedDescriptorLength = descriptorSets.GetSetCount() * estUniformBufferBlockLength;
        for (size_t i = 0; i < descriptorSets.GetSetCount(); i++)
        {
            const auto* set = descriptorSets.GetSetBase(i);
            estimatedDescriptorLength += set->GetBindingCount() * estUniformBufferVariableLength;
        }

        const Visual::VertexOutput* vertexOutput =
            (script.GetType() == Type::Vertex) ? static_cast<const Visual::VertexScript&>(script).GetVertexOutput() : nullptr;

        const size_t estimatedSourceLength = estMainLength + estPreMainLength + estimatedDescriptorLength +
            (script.GetNodeCount() * estLocalVariableLength) +
            (inputInterface.GetMemberCount() * estInputInterfaceLength) +
            (outputInterface.GetMemberCount() * estOutputInterfaceLength) +
            (pushConstants.GetMemberCount() * estPushConstantLength) +
            (vertexOutput ? estVertOutputLength : 0);

        return estimatedSourceLength;
    }

    void GlslGenerator::PrepareGeneration(const Visual::Script& script, Logger* logger)
    {
        m_script = &script;
        m_logger = logger;
        m_sourceBuffer.reserve(CalculateEstimatedScriptSize(script));
        m_lineBuffer.reserve(250);
        m_counters.Reset();
        m_visitedOutputVariables.clear();
        m_visitedNodes.clear();
        m_outputNodes.clear();
    }
    
    bool GlslGenerator::GenerateHeader(const Compability compability)
    {
        if(compability == Compability::Glsl)
        {
            return GenerateGlslHeader();
        }

        return GenerateSpirVHeader();
    }

    bool GlslGenerator::GenerateGlslHeader()
    {
        MOLTEN_DEBUG_ASSERT(false, "GlslGenerator::GenerateGlslHeader: This function is not yet implemented.");
        return false;
    }

    bool GlslGenerator::GenerateSpirVHeader()
    {
        m_lineBuffer =
            "#version 450\n"
            "#extension GL_ARB_separate_shader_objects : enable\n\n";

        AppendToSourceBuffer(m_lineBuffer);

        size_t index = 0;

        // Input interface.
        const auto& inputInterface = m_script->GetInputInterface();
        auto inputInterfaceOutputPins = inputInterface.GetOutputPins();
        if (!inputInterfaceOutputPins.empty())
        {
            auto nodeObject = std::make_shared<GeneratorNode>(inputInterface);
            m_visitedNodes.insert({ &inputInterface, nodeObject });

            for (const auto* pin : inputInterfaceOutputPins)
            {
                std::string name = "in_" + std::to_string(index);
                auto outputVariable = nodeObject->CreateOutputVariable(pin, std::move(name));
                m_visitedOutputVariables.insert({ pin, outputVariable });

                m_lineBuffer = "layout(location = " + std::to_string(index) + ") in " + GetVariableDataType(pin->GetDataType()) + " " + name + ";\n";
                AppendToSourceBuffer(m_lineBuffer);

                index++;
            }
            AppendToSourceBuffer("\n");
        }
        
        // Push constants
        const auto& pushConstants = m_script->GetPushConstantsBase();
        auto pushConstantsOutputPins = pushConstants.GetOutputPins();
        if (!inputInterfaceOutputPins.empty())
        {
            auto nodeObject = std::make_shared<GeneratorNode>(pushConstants);
            m_visitedNodes.insert({ &pushConstants, nodeObject });

            m_lineBuffer = "layout(std140, push_constant) uniform s_pc\n{\n";
            AppendToSourceBuffer(m_lineBuffer);

            size_t offset = 0;
            for (const auto* pin : pushConstantsOutputPins)
            {
                const auto pinDataType = pin->GetDataType();
                std::string currentOffset = std::to_string(offset);
                std::string memberName = m_counters.GetNextVariableName(pinDataType);
                std::string pushConstantName = "pc." + memberName;
                auto outputVariable = nodeObject->CreateOutputVariable(pin, std::move(pushConstantName));
                m_visitedOutputVariables.insert({ pin, outputVariable });

                m_lineBuffer = "layout(offset = " + currentOffset + ") " + GetVariableDataType(pinDataType) + " " + memberName + ";\n";
                AppendToSourceBuffer(m_lineBuffer);

                offset += std::max(size_t{16}, GetVariableByteOffset(pin->GetDataType()));
            }
            m_counters.ResetVariables();
            AppendToSourceBuffer("} pc;\n\n");
        }

        // Descriptor sets.
        size_t samplerIndex = 0;
        size_t uboIndex = 0;
        const auto& descriptorSets = m_script->GetDescriptorSetsBase();
        for (size_t i = 0; i < descriptorSets.GetSetCount(); i++)
        {
            const auto* set = descriptorSets.GetSetBase(i);
            const uint32_t setId = set->GetId();
            for (size_t j = 0; j < set->GetBindingCount(); j++)
            {
                auto* binding = set->GetBindingBase(j);
                auto nodeObject = std::make_shared<GeneratorNode>(*binding);
                m_visitedNodes.insert({ binding, nodeObject });

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
                        m_visitedOutputVariables.insert({ pin, outputVariable });

                        m_lineBuffer = 
                            "layout(set = " + std::to_string(setId) + ", binding = " + std::to_string(bindingId) +
                            ") uniform " + GetVariableDataType(pin->GetDataType()) + " " + name + ";\n";
                        AppendToSourceBuffer(m_lineBuffer);

                        ++samplerIndex;
                    } break;
                    case Visual::DescriptorBindingType::UniformBuffer:
                    {
                        const std::string blockName = "ubo_" + std::to_string(uboIndex);

                        m_lineBuffer =
                            "layout(std140, set = " + std::to_string(setId) + ", binding=" + std::to_string(bindingId) +
                            ") uniform s_" + blockName + "\n{\n";
                        AppendToSourceBuffer(m_lineBuffer);

                        size_t uboMemberIndex = 0;
                        for (auto* pin : binding->GetOutputPins())
                        {
                            std::string memberName = m_counters.GetNextVariableName(pin->GetDataType());
                            std::string name = blockName + "." + memberName;
                            auto outputVariable = nodeObject->CreateOutputVariable(pin, std::move(name));
                            m_visitedOutputVariables.insert({ pin, outputVariable });

                            m_lineBuffer = GetVariableDataType(pin->GetDataType()) + " " + memberName + ";\n";
                            AppendToSourceBuffer(m_lineBuffer);

                            ++uboMemberIndex;
                        }

                        AppendToSourceBuffer("} " + blockName + ";\n");

                        ++uboIndex;
                    } break;
                }
            }
            m_counters.ResetVariables();
        }
        if(descriptorSets.GetSetCount() > 0)
        {
            AppendToSourceBuffer("\n");
        }

        // Output interface.
        const auto& outputInterface = m_script->GetOutputInterface();
        auto outputInterfaceInputPins = outputInterface.GetInputPins();
        if (!outputInterfaceInputPins.empty())
        {
            auto nodeObject = std::make_shared<GeneratorOutputNode>(outputInterface);
            m_outputNodes.push_back(nodeObject);

            index = 0;
            for (const auto* pin : outputInterfaceInputPins)
            {
                const std::string name = "out_" + std::to_string(index);

                nodeObject->AddOutputPin(pin, name);

                m_lineBuffer =
                    "layout(location = " + std::to_string(index) + ") out " +
                    GetVariableDataType(pin->GetDataType()) + " " + name + ";\n";
                AppendToSourceBuffer(m_lineBuffer);

                index++;
            }
            AppendToSourceBuffer("\n");
        }

        return true;
    }

    bool GlslGenerator::GenerateMain()
    {
        // Add vertex stage output node.
        const Visual::VertexOutput* vertexOutput = (m_script->GetType() == Type::Vertex) ?
            static_cast<const Visual::VertexScript*>(m_script)->GetVertexOutput() : nullptr;

        if (vertexOutput)
        {
            auto nodeObject = std::make_shared<GeneratorOutputNode>(*vertexOutput);
            m_outputNodes.push_back(nodeObject);
            nodeObject->AddOutputPin(vertexOutput->GetInputPin(), "gl_Position");
        }

        // Create main function.
        AppendToSourceBuffer("void main()\n{\n");

        for (auto& outputNode : m_outputNodes)
        {
            std::stack<GeneratorNodePointer> nodeStack;
            nodeStack.push(outputNode);

            while (!nodeStack.empty())
            {
                auto& topNode = nodeStack.top();

                // Get next input variable of node.
                auto* inputVariable = topNode->GetNextInputVariable();

                // Is null if no more inputs are available and if we should skip to source code creation.
                if (inputVariable)
                {
                    auto newNode = GetNextNode(inputVariable);
                    if(newNode)
                    {
                        nodeStack.push(newNode);
                    }
                    continue;
                }

                // Create source code for current node.
                if(!GenerateNodeSource(topNode))
                {
                    return false;
                }

                // We are finished with this node for now.
                nodeStack.pop();
            }
        }

        AppendToSourceBuffer("}\n");
        return true;
    }

    GeneratorNodePointer GlslGenerator::GetNextNode(GeneratorInputVariable* inputVariable)
    {
        auto* pinConnection = inputVariable->pin.GetConnection();

        // No connection(default value).
        if (!pinConnection)
        {
            return nullptr;
        }

        // Already created output variable, reuse it.
        auto vovIt = m_visitedOutputVariables.find(pinConnection);
        if (vovIt != m_visitedOutputVariables.end())
        {
            inputVariable->connection = vovIt->second.get();
            return nullptr;
        }

        // Get or create new node object if needed.
        GeneratorNodePointer nodePointer;

        auto& node = pinConnection->GetNode();
        auto vnoIt = m_visitedNodes.find(&node);
        if (vnoIt != m_visitedNodes.end())
        {
            nodePointer = vnoIt->second;
        }
        else
        {
            nodePointer = std::make_shared<GeneratorNode>(node);
            m_visitedNodes.insert({ &node, nodePointer });
        }

        // Create new output variable and connect it.
        auto outputVariable = nodePointer->CreateOutputVariable(pinConnection);
        inputVariable->connection = outputVariable.get();
        m_visitedOutputVariables.insert({ pinConnection, outputVariable });

        return nodePointer;
    }

    bool GlslGenerator::GenerateNodeSource(GeneratorNodePointer& node)
    {
        const auto& scriptNode = node->GetNode();
        switch (scriptNode.GetType())
        {
            case Visual::NodeType::Constant: return GenerateConstantNodeSource(node);
            case Visual::NodeType::Function: return GenerateFunctionNodeSource(node);
            case Visual::NodeType::Operator: return GenerateOperatorNodeSource(node);
            case Visual::NodeType::VertexOutput:
            case Visual::NodeType::Output: return GenerateOutputNodeSource(node);
            default: break;
        }

        Logger::WriteError(m_logger, "Code generation for node(type: " + std::to_string((int32_t)scriptNode.GetType()) + ") is missing.");
        return false;
    }

    bool GlslGenerator::GenerateConstantNodeSource(GeneratorNodePointer& node)
    {
        auto& outputVariables = node->GetOutputVariables();
        if (outputVariables.size() != 1)
        {
            Logger::WriteError(m_logger, "Constant node expects 1 output variable, found " + std::to_string(outputVariables.size()) + ".");
            return false;
        }

        auto& outputVariable = outputVariables.front();
        if (!outputVariable->name.empty())
        {
            return true;
        }

        const auto& constantBase = static_cast<const Visual::ConstantBase&>(node->GetNode());

        const auto& dataTypeName = m_counters.GetNextVariableName(outputVariable->name, constantBase.GetDataType());
        if(dataTypeName.empty())
        {
            Logger::WriteError(m_logger, "Failed to get data type name of type " + std::to_string((int32_t)constantBase.GetType()) + ".");
        }

        m_lineBuffer = dataTypeName + " " + outputVariable->name + " = " + GetConstantValue(constantBase) + ";\n";
        AppendToSourceBuffer(m_lineBuffer);

        return true;
    }

    bool GlslGenerator::GenerateFunctionNodeSource(GeneratorNodePointer& node)
    {
        auto& outputVariables = node->GetOutputVariables();
        if (outputVariables.size() != 1)
        {
            Logger::WriteError(m_logger, "Function node expects 1 output variable, found " + std::to_string(outputVariables.size()) + ".");
            return false;
        }

        auto& outputVariable = outputVariables.front();
        if (!outputVariable->name.empty())
        {
            return true;
        }

        const auto& functionBase = static_cast<const Visual::FunctionBase&>(node->GetNode());

        const std::string& functionName = m_counters.GetNextFunctionName(outputVariable->name, functionBase.GetFunctionType());

        auto* outputPin = outputVariable->pin;

        m_lineBuffer = GetVariableDataType(outputPin->GetDataType()) + " " + outputVariable->name + " = " + functionName + "(";
        AppendToSourceBuffer(m_lineBuffer);

        auto& inputVariables = node->GetInputVariables();
        for (size_t i = 0; i < inputVariables.size() - 1; i++)
        {
            m_lineBuffer = GetInputNameOrDefault(inputVariables[i]) + ", ";
            AppendToSourceBuffer(m_lineBuffer);
        }

        m_lineBuffer = GetInputNameOrDefault(inputVariables.back()) + ");\n";
        AppendToSourceBuffer(m_lineBuffer);
        
        return true;
    }

    bool GlslGenerator::GenerateOperatorNodeSource(GeneratorNodePointer& node)
    {
        auto& outputVariables = node->GetOutputVariables();
        if (outputVariables.size() != 1)
        {
            Logger::WriteError(m_logger, "Operator node expects 1 output variable, found " + std::to_string(outputVariables.size()) + ".");
            return false;
        }

        auto& outputVariable = outputVariables.front();
        if (!outputVariable->name.empty())
        {
            return true;
        }

        auto& operatorBase = static_cast<const Visual::OperatorBase&>(node->GetNode());

        switch (operatorBase.GetOperatorType())
        {
            case Visual::OperatorType::Arithmetic:
            {
                auto& inputVariables = node->GetInputVariables();
                if (inputVariables.size() != 2)
                {
                    Logger::WriteError(m_logger, "Arithmetic operator node expects 2 input variable, found " + std::to_string(inputVariables.size()) + ".");
                    return false;
                }

                static size_t index = 0;

                auto& arithmeticOperatorBase = static_cast<const Visual::ArithmeticOperatorBase&>(operatorBase);

                auto arithmeticOperatorType = arithmeticOperatorBase.GetArithmeticOperatorType();
                const std::string& token = m_counters.GetNextOperatorName(outputVariable->name, arithmeticOperatorType);

                auto* outputPin = outputVariable->pin;

                m_lineBuffer = GetVariableDataType(outputPin->GetDataType()) + " " + outputVariable->name + " = " + GetInputNameOrDefault(inputVariables[0]);
                AppendToSourceBuffer(m_lineBuffer);

                m_lineBuffer = " " + token + " " + GetInputNameOrDefault(inputVariables[1]) + ";\n";
                AppendToSourceBuffer(m_lineBuffer);
            } break;
            default: 
            {
                Logger::WriteError(m_logger, "Unhandled operator type: " + std::to_string((int)operatorBase.GetOperatorType()) +".");
                return false;
            }
        }

        return true;
    }

    bool GlslGenerator::GenerateOutputNodeSource(GeneratorNodePointer& node)
    {
        auto& outputVariables = node->GetOutputVariables();
        auto& inputVariables = node->GetInputVariables();

        if (outputVariables.size() != inputVariables.size())
        {
            Logger::WriteError(m_logger, "Number of variables for output node is mismatching: " + 
                std::to_string(outputVariables.size()) + " : " + std::to_string(inputVariables.size()) + ".");
            return false;
        }

        for (size_t i = 0; i < outputVariables.size(); i++)
        {
            m_lineBuffer = outputVariables[i]->name + " = " + GetInputNameOrDefault(inputVariables[i]) + ";\n";
            AppendToSourceBuffer(m_lineBuffer);
        }

        return true;
    }

    void GlslGenerator::AppendToSourceBuffer(const std::string& data)
    {
        std::copy(data.begin(), data.end(), std::back_inserter(m_sourceBuffer));
    }

}