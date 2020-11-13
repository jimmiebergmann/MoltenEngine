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
#include "Molten/Renderer/Shader/Generator/VulkanShaderGenerator.hpp"

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

        auto& pc1 = script.GetPushConstants().AddMember<float>();

        compsToVec4_1.GetInputPin(0)->Connect(*const1.GetOutputPin());
        compsToVec4_1.GetInputPin(1)->Connect(*sin1.GetOutputPin());
        compsToVec4_1.GetInputPin(2)->Connect(input1);
        compsToVec4_1.GetInputPin(2)->Connect(pc1);

        auto* set1 = script.GetDescriptorSets().AddSet(0);
        auto* ubo1 = set1->AddBinding<FragmentUniformBuffer>(0);
        auto& ubo1_m1 = ubo1->AddPin<Vector4f32>();

        auto& addVec4_1 = script.CreateOperator<Shader::Visual::Operators::AddVec4f32>();
        addVec4_1.GetInputPin(0)->Connect(ubo1_m1);
        addVec4_1.GetInputPin(1)->Connect(*compsToVec4_1.GetOutputPin());

        auto& output1 = script.GetOutputInterface().AddMember<Vector4f32>();
        output1.Connect(*addVec4_1.GetOutputPin());

        /*auto& output2 = script.GetOutputInterface().AddMember<Vector4f32>();

        auto& input1 = script.GetInputInterface().AddMember<Vector4f32>();
        auto& mult = script.CreateOperator<Shader::Visual::Operators::MultVec4f32>();
        auto& add1 = script.CreateOperator<Shader::Visual::Operators::AddVec4f32>();
        auto& add2 = script.CreateOperator<Shader::Visual::Operators::AddVec4f32>();
        auto& var1 = script.CreateConstant<Vector4f32>({ 0.0f, 0.0f, 0.3f, 0.0f });
        auto& var2 = script.CreateConstant<Vector4f32>({ 1.0f, 0.5f, 0.0f, 1.0f });
        auto* set1 = script.GetDescriptorSets().AddSet(0);
        auto* set2 = script.GetDescriptorSets().AddSet(1);
        set1->AddBinding<Sampler1D>(0);
        set1->AddBinding<Sampler2D>(1);
        set2->AddBinding<Sampler3D>(0);
        auto* ubo1 = set1->AddBinding<FragmentUniformBuffer>(2);
        auto* ubo2 = set1->AddBinding<FragmentUniformBuffer>(3);
        ubo1->AddPin<bool>();
        ubo1->AddPin<int32_t>();
        ubo1->AddPin<float>();
        ubo1->AddPin<Vector2f32>();
        ubo1->AddPin<Vector3f32>();
        auto& ubo1_vec4 = ubo1->AddPin<Vector4f32>();
        ubo1->AddPin<Matrix4x4f32>();
        ubo2->AddPin<float>();

        script.GetPushConstants().AddMember<Vector4f32>();
        script.GetPushConstants().AddMember<Vector2f32>();
        script.GetPushConstants().AddMember<float>();

        output1.Connect(*add2.GetOutputPin());
        output2.Connect(*add1.GetOutputPin());

        add1.GetInputPin(0)->Connect(ubo1_vec4);
        add1.GetInputPin(1)->Connect(*var1.GetOutputPin());

        add2.GetInputPin(0)->Connect(*mult.GetOutputPin());
        add2.GetInputPin(1)->Connect(*add1.GetOutputPin());

        mult.GetInputPin(0)->Connect(input1);
        mult.GetInputPin(1)->Connect(*var2.GetOutputPin());*/

        std::vector<uint8_t> source;    
        {
            Molten::Test::Benchmarker bench1("Generate GLSL");
            source = VulkanGenerator::GenerateGlsl(script, nullptr);
        }
        
        ASSERT_GT(source.size(), size_t(0));
        const std::string sourceStr(source.begin(), source.end());

        static const std::string expectedSource =
            "#version 450\n"
            "#extension GL_ARB_separate_shader_objects : enable\n"
            "layout(location = 0) in vec4 in_0;\n"
            "layout(std140, push_constant) uniform s_pc\n"
            "{\n"
            "layout(offset = 0) vec4 mem0;\n"
            "layout(offset = 16) vec2 mem16;\n"
            "layout(offset = 24) float mem24;\n"
            "} pc;\n"
            "layout(set = 0, binding = 0) uniform sampler1D sampler_0;\n"
            "layout(set = 0, binding = 1) uniform sampler2D sampler_1;\n"
            "layout(std140, set = 0, binding=2) uniform s_ubo_0\n"
            "{\n"
            "bool val_0;\n"
            "int val_1;\n"
            "float val_2;\n"
            "vec2 val_3;\n"
            "vec3 val_4;\n"
            "vec4 val_5;\n"
            "mat4 val_6;\n"
            "} ubo_0;\n"
            "layout(std140, set = 0, binding=3) uniform s_ubo_1\n"
            "{\n"
            "float val_0;\n"
            "} ubo_1;\n"
            "layout(set = 1, binding = 0) uniform sampler3D sampler_2;\n"
            "layout(location = 0) out vec4 out_0;\n"
            "void main()\n"
            "{\n"
            "vec4 vec4_0 = vec4(1, 0.5, 0, 1);\n"
            "vec4 mul_1 = in_0 * vec4_0;\n"
            "vec4 vec4_2 = vec4(0, 0, 0.3, 0);\n"
            "vec4 add_3 = ubo_0.val_5 + vec4_2;\n"
            "vec4 add_4 = mul_1 + add_3;\n"
            "out_0 = add_4;\n"
            "}\n";

        EXPECT_STREQ(sourceStr.c_str(), expectedSource.c_str());

#if defined(MOLTEN_ENABLE_VULKAN)
        std::vector<uint8_t> spirv;
        {
            Molten::Test::Benchmarker bench1("Generate SPIR-V");
            spirv = VulkanGenerator::ConvertGlslToSpriV(source, Shader::Type::Fragment);
        }
        
        EXPECT_GE(spirv.size(), size_t(0));
#endif*/
    }
    /*
    TEST(Shader, VisualShader_DefaultPinValue)
    {
        // Cos
        {
            FragmentScript script;
            auto output = script.GetOutputInterface().AddMember<Vector4f32>();
            auto cos = script.CreateFunction<Shader::Visual::Functions::CosVec4f32>();

            output->GetInputPin()->Connect(*cos->GetOutputPin());
            static_cast<Shader::Visual::InputPin<Vector4f32>*>(cos->GetInputPin())->SetDefaultValue({2.1f, 3.5f, 4.7f, 5.2f});

            std::vector<uint8_t> source;
            {
                Molten::Test::Benchmarker bench("GLSL generator - default pin");
                source = VulkanGenerator::GenerateGlsl(script, nullptr);
            }
            EXPECT_GT(source.size(), size_t(0));
            const std::string sourceStr(source.begin(), source.end());

            static const std::string expectedSource =
                "#version 450\n"
                "#extension GL_ARB_separate_shader_objects : enable\n"
                "layout(location = 0) out vec4 out_0;\n"
                "void main(){\n"
                "vec4 cos_0 = cos(vec4(2.1, 3.5, 4.7, 5.2));\n"
                "out_0 = cos_0;\n"
                "}\n";

            EXPECT_STREQ(sourceStr.c_str(), expectedSource.c_str());
        }
    }

    TEST(Shader, VisualShader_Functions)
    {
        // Cos
        {
            FragmentScript script;
            auto output = script.GetOutputInterface().AddMember<Vector4f32>();
            auto var1 = script.CreateConstantVariable<Vector4f32>({ 1.0f, 2.0f, 3.0f, 4.0f });
            auto cos = script.CreateFunction<Shader::Visual::Functions::CosVec4f32>();

            output->GetInputPin()->Connect(*cos->GetOutputPin());
            cos->GetInputPin()->Connect(*var1->GetOutputPin());

            std::vector<uint8_t> source;
            {
                Molten::Test::Benchmarker bench("GLSL generator - function");
                source = VulkanGenerator::GenerateGlsl(script, nullptr);
            }
            const std::string sourceStr(source.begin(), source.end());

            static const std::string expectedSource =
                "#version 450\n"
                "#extension GL_ARB_separate_shader_objects : enable\n"
                "layout(location = 0) out vec4 out_0;\n"
                "void main(){\n"
                "vec4 vec4_0 = vec4(1, 2, 3, 4);\n"
                "vec4 cos_1 = cos(vec4_0);\n"
                "out_0 = cos_1;\n"
                "}\n";

            EXPECT_STREQ(sourceStr.c_str(), expectedSource.c_str());
        }
        // Sin
        {
            FragmentScript script;
            auto output = script.GetOutputInterface().AddMember<Vector3f32>();
            auto var1 = script.CreateConstantVariable<Vector3f32>({ 1.0f, 2.0f, 3.0f });
            auto cos = script.CreateFunction<Shader::Visual::Functions::SinVec3f32>();

            output->GetInputPin()->Connect(*cos->GetOutputPin());
            cos->GetInputPin()->Connect(*var1->GetOutputPin());

            auto source = VulkanGenerator::GenerateGlsl(script, nullptr);
            const std::string sourceStr(source.begin(), source.end());

            static const std::string expectedSource =
                "#version 450\n"
                "#extension GL_ARB_separate_shader_objects : enable\n"
                "layout(location = 0) out vec3 out_0;\n"
                "void main(){\n"
                "vec3 vec3_0 = vec3(1, 2, 3);\n"
                "vec3 sin_1 = sin(vec3_0);\n"
                "out_0 = sin_1;\n"
                "}\n";

            EXPECT_STREQ(sourceStr.c_str(), expectedSource.c_str());
        }
        // Tan
        {
            FragmentScript script;
            auto output = script.GetOutputInterface().AddMember<Vector2f32>();
            auto var1 = script.CreateConstantVariable<Vector2f32>({ 1.0f, 2.0f });
            auto cos = script.CreateFunction<Shader::Visual::Functions::TanVec2f32>();

            output->GetInputPin()->Connect(*cos->GetOutputPin());
            cos->GetInputPin()->Connect(*var1->GetOutputPin());

            auto source = VulkanGenerator::GenerateGlsl(script, nullptr);
            const std::string sourceStr(source.begin(), source.end());

            static const std::string expectedSource =
                "#version 450\n"
                "#extension GL_ARB_separate_shader_objects : enable\n"
                "layout(location = 0) out vec2 out_0;\n"
                "void main(){\n"
                "vec2 vec2_0 = vec2(1, 2);\n"
                "vec2 tan_1 = tan(vec2_0);\n"
                "out_0 = tan_1;\n"
                "}\n";

            EXPECT_STREQ(sourceStr.c_str(), expectedSource.c_str());
        }
    }
    */
}