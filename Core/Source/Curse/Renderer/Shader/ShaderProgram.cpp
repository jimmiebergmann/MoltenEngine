/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#include "Curse/Renderer/Shader/ShaderProgram.hpp"
#include "Curse/System/Exception.hpp"
#include "Curse/System/FileSystem.hpp"

#if defined(CURSE_ENABLE_GLSLANG)
#include <glslang/public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>
#endif


namespace Curse
{

    namespace Shader
    {

#if defined(CURSE_ENABLE_GLSLANG)
        static EShLanguage GetShaderType(const ShaderType type)
        {
            switch (type)
            {
                case ShaderType::Vertex:   return EShLanguage::EShLangVertex;
                case ShaderType::Fragment: return EShLanguage::EShLangFragment;
                default: break;
            }

            throw Exception("Provided shader type is not supported by the shader compiler.");
        }

        static TBuiltInResource GetDefaultResources()
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

            return std::move(resource);
        }

        std::vector<uint8_t> Program::Compile(const ShaderFormat inputFormat, const ShaderType inputType,
            const std::vector<uint8_t>& inputData, const ShaderFormat outputFormat, std::string& errorMessage)
        {
            errorMessage = "";

            if (inputFormat != ShaderFormat::Glsl)
            {
                errorMessage = "Only GLSL is supported as input format for shader compiler.";
                return {};
            }
            if (outputFormat != ShaderFormat::SpirV)
            {
                errorMessage = "Only SPRI-V is supported as output format for shader compiler.";
                return {};
            }

            static const TBuiltInResource resources = GetDefaultResources();

            // Initialize glslang for the first time we run.
            static bool glslangInitialized = false;
            if (!glslangInitialized)
            {
                glslang::InitializeProcess();
                glslangInitialized = true;
            }

            // Set configs of shader.
            EShLanguage shaderType = GetShaderType(inputType);
            int clientInputSemanticsVersion = 100; // maps to, say, #define VULKAN 100
            glslang::EShTargetClientVersion VulkanClientVersion = glslang::EShTargetVulkan_1_1;
            glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_1;

            glslang::TShader shader(shaderType);
            shader.setEnvInput(glslang::EShSourceGlsl, shaderType, glslang::EShClientVulkan, clientInputSemanticsVersion);
            shader.setEnvClient(glslang::EShClientVulkan, VulkanClientVersion);
            shader.setEnvTarget(glslang::EShTargetSpv, TargetVersion);

            const char* inputCString = reinterpret_cast<const char*>(inputData.data());
            const int  inputCLength = static_cast<const int>(inputData.size());
            shader.setStringsWithLengths(&inputCString, &inputCLength, 1);

            EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
            const int defaultVersion = 100;

            // Preprocess the shader.
            std::string preprocessedGLSL;
            DirStackFileIncluder includer;
            if (!shader.preprocess(&resources, defaultVersion, ENoProfile, false, false, messages, &preprocessedGLSL, includer))
            {
                errorMessage = std::string("Shader preprocessing failed: ") + shader.getInfoLog();
                return {};
            }

            // Parse the shader.
            const char* preprocessedConstStr = preprocessedGLSL.c_str();
            shader.setStrings(&preprocessedConstStr, 1);

            if (!shader.parse(&resources, 100, false, messages))
            {
                errorMessage = std::string("Shader parsing failed: ") + shader.getInfoLog();
                return {};
            }

            // Link the shader.
            glslang::TProgram program;
            program.addShader(&shader);

            if (!program.link(messages))
            {
                errorMessage = std::string("Shader linking failed: ") + shader.getInfoLog();
                return {};
            }

            // Get SPIR-V.
            std::vector<unsigned int> spirv;
            spv::SpvBuildLogger logger;
            glslang::SpvOptions spvOptions;
            glslang::GlslangToSpv(*program.getIntermediate(shaderType), spirv, &logger, &spvOptions);

            size_t outputSize = sizeof(unsigned int) * spirv.size();
            if (outputSize == 0)
            {
                return {};
            }

            std::vector<uint8_t> output(outputSize);
            std::memcpy(output.data(), spirv.data(), outputSize);

            return std::move(output);
        }

#else

        std::vector<uint8_t> Program::Compile(const ShaderFormat, const ShaderType,
                                              const std::vector<uint8_t>&, const ShaderFormat, std::string& errorMessage)
        {
            errorMessage = "Shader compilation is not enabled in this Curse build. Enable it by adding CURSE_ENABLE_GLSLANG flag in cmake generator.";
            return {};
        }

#endif

        ShaderType Program::GetType() const
        {
            return type;
        }


        // Shader descriptor implementations.
       /* ProgramDescriptor::ProgramDescriptor() :
            type(ShaderType::Vertex),
            filename(nullptr),
            data(nullptr),
            dataSize(0)
        { }

        ProgramDescriptor::ProgramDescriptor(const ShaderType type, const char* filename) :
            type(type),
            filename(filename),
            data(nullptr),
            dataSize(0)
        { }

        ProgramDescriptor::ProgramDescriptor(const ShaderType type, const uint8_t* data, const size_t dataSize) :
            type(type),
            filename(nullptr),
            data(data),
            dataSize(dataSize)
        { }*/

    }

}
