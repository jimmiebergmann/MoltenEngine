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

#include "Test.hpp"
#include "Molten/Logger.hpp"
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Renderer/Shader/Generator/SpirvShaderGenerator.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include <fstream>

namespace Molten::Shader
{

    TEST(Shader, SpirvShaderGenerator)
    {
        Logger logger;

        Visual::FragmentScript fragmentScript;
        {
            auto& script = fragmentScript;

            /*auto& compsToVec4_1 = script.CreateFunction<Visual::Functions::CompsToVec4f32>();
            auto& const1 = script.CreateConstant<float>(4.25f);

            auto& sin1 = script.CreateFunction<Visual::Functions::Sinf32>();
            auto& const2 = script.CreateConstant<float>(6.0f);
            sin1.GetInputPin()->Connect(*const2.GetOutputPin());

            auto& input1 = script.GetInputInterface().AddMember<float>();

            auto& pc1 = script.GetPushConstants().AddMember<float>(0);

            compsToVec4_1.GetInputPin(0)->Connect(*const1.GetOutputPin());
            compsToVec4_1.GetInputPin(1)->Connect(*sin1.GetOutputPin());
            compsToVec4_1.GetInputPin(2)->Connect(input1);
            compsToVec4_1.GetInputPin(2)->Connect(pc1);

            auto* set1 = script.GetDescriptorSets().AddSet(10);
            auto* ubo1 = set1->AddBinding<Visual::FragmentUniformBuffer>(5);
            auto& ubo1_m1 = ubo1->AddPin<Vector4f32>();

            auto& addVec4_1 = script.CreateOperator<Visual::Operators::AddVec4f32>();
            addVec4_1.GetInputPin(0)->Connect(ubo1_m1);
            addVec4_1.GetInputPin(1)->Connect(*compsToVec4_1.GetOutputPin());

            auto& pc2 = script.GetPushConstants().AddMember<Vector4f32>(1);

            auto& multVec4_1 = script.CreateOperator<Visual::Operators::MultVec4f32>();
            multVec4_1.GetInputPin(0)->Connect(pc2);
            multVec4_1.GetInputPin(1)->Connect(*addVec4_1.GetOutputPin());

            auto* sampler1 = set1->AddBinding<Sampler2D>(10);
            auto& input2 = script.GetInputInterface().AddMember<Vector2f32>();

            auto& texture1 = script.CreateFunction<Visual::Functions::Texture2D>();
            texture1.GetInputPin(0)->Connect(*sampler1->GetOutputPin());
            texture1.GetInputPin(1)->Connect(input2);

            auto& subVec4_1 = script.CreateOperator<Visual::Operators::SubVec4f32>();
            subVec4_1.GetInputPin(0)->Connect(*multVec4_1.GetOutputPin());
            subVec4_1.GetInputPin(1)->Connect(*texture1.GetOutputPin());

            auto& output1 = script.GetOutputInterface().AddMember<Vector4f32>();
            output1.Connect(*subVec4_1.GetOutputPin());
            */

            auto& input1 = script.GetInputInterface().AddMember<Vector4f32>();
            auto& input2 = script.GetInputInterface().AddMember<Vector4f32>();

            auto& sin1 = script.CreateFunction<Visual::Functions::SinVec4f32>();
            sin1.GetInput<0>().Connect(input2);

            auto& cos1 = script.CreateFunction<Visual::Functions::CosVec4f32>();
            cos1.GetInput<0>().Connect(sin1.GetOutput());

            auto& tan1 = script.CreateFunction<Visual::Functions::TanVec4f32>();
            tan1.GetInput<0>().Connect(cos1.GetOutput());

            auto& div1 = script.CreateOperator<Visual::Operators::DivVec4f32>();
            div1.GetLeftInput().Connect(tan1.GetOutput());
            div1.GetRightInput().Connect(input1.GetPin());

            auto& mult1 = script.CreateOperator<Visual::Operators::MultVec4f32>();
            mult1.GetLeftInput().SetDefaultValue({ 0.5f, 1.0f, 2.0f, 3.0f });
            mult1.GetRightInput().Connect(div1.GetOutput());

            auto& add1 = script.CreateOperator<Visual::Operators::AddVec4f32>();
            add1.GetLeftInput().Connect(mult1.GetOutput());
            add1.GetRightInput().SetDefaultValue({ 2.0f, 3.0f, 4.0f, 5.0f });

            auto& add2 = script.CreateOperator<Visual::Operators::AddVec4f32>();
            add2.GetLeftInput().SetDefaultValue({1.0f, 2.0f, 3.0f, 4.0f});
            add2.GetRightInput().Connect(add1.GetOutput());

            auto& output1 = script.GetOutputInterface().AddMember<Vector4f32>();
            output1.Connect(add2.GetOutput());
        }

        SpirvGenerator::Template spirvTemplate;
        {
            Molten::Test::Benchmarker bench("Generate SPIR-V template");
            EXPECT_TRUE(SpirvGenerator::CreateTemplate(spirvTemplate, { &fragmentScript }, &logger));
        }

        SpirvGenerator generator(&logger);
        Spirv::Words source;
        {
            Molten::Test::Benchmarker bench("Generate SPIR-V source code");
            source = generator.Generate(fragmentScript, &spirvTemplate, true);
        }

        EXPECT_FALSE(source.empty());

        // Debug dump.
#if 1
        {
            std::ofstream fout("SpirvTest/SpirvGenerator.spiv", std::ios::binary);
            fout.write(reinterpret_cast<char*>(source.data()), source.size() * sizeof(Spirv::Word) );
            fout.close();
        }
        {
            Molten::Test::Benchmarker bench1("OLD - Generate GLSL source code");

            GlslGenerator glslGenerator;
            auto glslCode = glslGenerator.Generate(fragmentScript, Shader::GlslGenerator::Compability::SpirV, nullptr);

            bench1.Stop();

            EXPECT_FALSE(glslCode.empty());

            Molten::Test::Benchmarker bench2("OLD - Generate SPIR-V source code");

            const auto scriptType = fragmentScript.GetType();
            auto spirvCode2 = Shader::GlslGenerator::ConvertGlslToSpriV(glslCode, scriptType);

            bench2.Stop();

            EXPECT_FALSE(spirvCode2.empty());

            {
                std::ofstream fout("SpirvTest/GlslGenerator.frag", std::ios::binary);
                fout.write(reinterpret_cast<char*>(glslCode.data()), glslCode.size());
                fout.close();
            }
            {
                std::ofstream fout("SpirvTest/SpirvGeneratorOld.spiv", std::ios::binary);
                fout.write(reinterpret_cast<char*>(spirvCode2.data()), spirvCode2.size());
                fout.close();
            }
        }

#endif

    }

}