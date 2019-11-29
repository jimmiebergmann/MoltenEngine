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

#include "Curse/Renderer/Shader.hpp"
#include "Curse/System/Exception.hpp"
#include "Curse/System/FileSystem.hpp"

#if defined(CURSE_ENABLE_GLSLANG)
#include <glslang/public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>
#endif


namespace Curse
{

#if defined(CURSE_ENABLE_GLSLANG)
    static EShLanguage GetShaderType(const Shader::Type type)
    {
        switch (type)
        {
            case Shader::Type::Vertex:   return EShLanguage::EShLangVertex;
            case Shader::Type::Fragment: return EShLanguage::EShLangFragment;
            default: break;
        }

        throw Exception("Provided shader type is not supported by the shader compiler.");
    }

    std::vector<uint8_t> Shader::Compile(const Shader::Format inputFormat, const Shader::Type inputType,
        const std::vector<uint8_t>& inputData, const Shader::Format outputFormat,
        std::string& errorMessage)
    {
        errorMessage = "";

        if (inputFormat != Format::Glsl)
        {
            errorMessage = "Only GLSL is supported as input format for shader compiler.";
            return {};
        }
        if (outputFormat != Format::SpirV)
        {
            errorMessage = "Only SPRI-V is supported as output format for shader compiler.";
            return {};
        }

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

        const TBuiltInResource defaultTBuiltInResource = { };
        TBuiltInResource resources = defaultTBuiltInResource;
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
        const char* PreprocessedCStr = preprocessedGLSL.c_str();
        shader.setStrings(&PreprocessedCStr, 1);

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

    std::vector<uint8_t> Shader::Compile(const Shader::Format, const Shader::Type,
        const std::vector<uint8_t>&, const Shader::Format,
        std::string& errorMessage)
    {
        errorMessage = "Shader compilation is not enabled in this Curse build. Enable it by adding CURSE_ENABLE_GLSLANG flag in cmake generator.";
        return {};
    }

#endif


    // Shader descriptor implementations.
    ShaderDescriptor::ShaderDescriptor() :
        type(Shader::Type::Vertex),
        filename(nullptr),
        data(nullptr),
        dataSize(0)
    { }

    ShaderDescriptor::ShaderDescriptor(const Shader::Type type, const char* filename) :
        type(type),
        filename(filename),
        data(nullptr),
        dataSize(0)
    { }

    ShaderDescriptor::ShaderDescriptor(const Shader::Type type, const uint8_t* data, const size_t dataSize) :
        type(type),
        filename(nullptr),
        data(data),
        dataSize(dataSize)
    { }

    Shader::Type Shader::GetType() const
    {
        return type;
    }

}
