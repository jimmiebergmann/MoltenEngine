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

#include "Test.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Logger.hpp"

static Molten::Logger g_logger;

namespace Molten::Shader::Visual
{

    TEST(Shader, VisualShader_GenerateGlsl_LargeScript)
    {
        FragmentScript script;
        
        auto& compsToVec4_1 = script.CreateFunction<Shader::Visual::Functions::CompsToVec4f32>();
        auto& const1 = script.CreateConstant<float>(4.0f);
        
        auto& sin1 = script.CreateFunction<Shader::Visual::Functions::Sinf32>();
        auto& const2 = script.CreateConstant<float>(6.0f);
        sin1.GetInputPin()->Connect(*const2.GetOutputPin());

        auto& input1 = script.GetInputInterface().AddMember<float>();

        auto& pc1 = script.GetPushConstants().AddMember<float>(0);

        compsToVec4_1.GetInputPin(0)->Connect(*const1.GetOutputPin());
        compsToVec4_1.GetInputPin(1)->Connect(*sin1.GetOutputPin());
        compsToVec4_1.GetInputPin(2)->Connect(input1);
        compsToVec4_1.GetInputPin(2)->Connect(pc1);

        auto* set1 = script.GetDescriptorSets().AddSet(10);
        auto* ubo1 = set1->AddBinding<FragmentUniformBuffer>(5);
        auto& ubo1_m1 = ubo1->AddPin<Vector4f32>();

        auto& addVec4_1 = script.CreateOperator<Shader::Visual::Operators::AddVec4f32>();
        addVec4_1.GetInputPin(0)->Connect(ubo1_m1);
        addVec4_1.GetInputPin(1)->Connect(*compsToVec4_1.GetOutputPin());

        auto& pc2 = script.GetPushConstants().AddMember<Vector4f32>(1);

        auto& multVec4_1 = script.CreateOperator<Shader::Visual::Operators::MultVec4f32>();
        multVec4_1.GetInputPin(0)->Connect(pc2);
        multVec4_1.GetInputPin(1)->Connect(*addVec4_1.GetOutputPin());

        auto* sampler1 = set1->AddBinding<Sampler2D>(10);
        auto& input2 = script.GetInputInterface().AddMember<Vector2f32>();

        auto& texture1 = script.CreateFunction<Shader::Visual::Functions::Texture2D>();
        texture1.GetInputPin(0)->Connect(*sampler1->GetOutputPin());
        texture1.GetInputPin(1)->Connect(input2);

        auto& subVec4_1 = script.CreateOperator<Shader::Visual::Operators::SubVec4f32>();
        subVec4_1.GetInputPin(0)->Connect(*multVec4_1.GetOutputPin());
        subVec4_1.GetInputPin(1)->Connect(*texture1.GetOutputPin());

        auto& output1 = script.GetOutputInterface().AddMember<Vector4f32>();
        output1.Connect(*subVec4_1.GetOutputPin());

        GlslGenerator generator;
        std::vector<uint8_t> source;
        Shader::GlslGenerator::GlslTemplate glslTemplate;
        {
            Molten::Test::Benchmarker bench1("Generate GLSL template");
            GlslGenerator::GenerateGlslTemplate(glslTemplate, { &script }, &g_logger);
        }
        {
            Molten::Test::Benchmarker bench1("Generate GLSL code");
            source = generator.Generate(script, GlslGenerator::Compability::SpirV, &glslTemplate, &g_logger);
        }
        
        ASSERT_GT(source.size(), size_t{0});
        const std::string sourceStr(source.begin(), source.end());

        const char* const expectedSource =
            "#version 450\n"
            "#extension GL_ARB_separate_shader_objects : enable\n"
            "\n"
            "layout(location = 0) in float in_0;\n"
            "layout(location = 1) in vec2 in_1;\n"
            "\n"
            "layout(std140, push_constant) uniform s_pc\n"
            "{\n"
            "layout(offset = 0) float float_0;\n"
            "layout(offset = 16) vec4 vec4_0;\n"
            "} pc;\n"
            "\n"
            "layout(std140, set = 0, binding=0) uniform s_ubo_0\n"
            "{\n"
            "vec4 vec4_0;\n"
            "} ubo_0;\n"
            "layout(set = 0, binding = 1) uniform sampler2D sampler_0;\n"
            "\n"
            "layout(location = 0) out vec4 out_0;\n"
            "\n"
            "void main()\n"
            "{\n"
            "float float_0 = 4;\n"
            "float float_1 = 6;\n"
            "float sin_0 = sin(float_1);\n"
            "vec4 vec4_0 = vec4(float_0, sin_0, pc.float_0, 0);\n"
            "vec4 add_0 = ubo_0.vec4_0 + vec4_0;\n"
            "vec4 mul_0 = pc.vec4_0 * add_0;\n"
            "vec4 texture_0 = texture(sampler_0, in_1);\n"
            "vec4 sub_0 = mul_0 - texture_0;\n"
            "out_0 = sub_0;\n"
            "}\n";

        EXPECT_STREQ(sourceStr.c_str(), expectedSource);

#if defined(MOLTEN_ENABLE_VULKAN)
        std::vector<uint8_t> spirv;
        {
            Molten::Test::Benchmarker bench1("Generate SPIR-V");
            spirv = GlslGenerator::ConvertGlslToSpriV(source, Shader::Type::Fragment, &g_logger);
        }
        
        EXPECT_GE(spirv.size(), size_t(0));
#endif
    }
   
}