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

    TEST(Shader, SpirvShaderGenerator_FragmentScript_Constants)
    {
        Logger logger;

        Visual::FragmentScript fragmentScript;
        {
            auto& script = fragmentScript;

            auto& const1 = script.CreateConstant<Vector4f32>({ 1.0f, 2.0f, 3.0f, 4.0f });
            auto& const2 = script.CreateConstant<Vector4f32>({ 1.0f, 2.0f, 3.0f, 4.0f });
            auto& const3 = script.CreateConstant<Vector4f32>({ 1.0f, 2.0f, 3.0f, 5.0f });

            auto& output1 = script.GetOutputInterface().AddMember<Vector4f32>();
            auto& output2 = script.GetOutputInterface().AddMember<Vector4f32>();
            auto& output3 = script.GetOutputInterface().AddMember<Vector4f32>();
            output1.Connect(const1.GetOutput());
            output2.Connect(const2.GetOutput());
            output3.Connect(const3.GetOutput());
        }

        SpirvGenerator generator(&logger);
        const SpirvGeneratorDescriptor descriptor = { &fragmentScript, {}, true, true };
        const auto result = generator.Generate(descriptor);
        EXPECT_FALSE(result.source.empty());
        EXPECT_EQ(result.source.size(), size_t{ 135 });

#if 0
        {
            std::ofstream fout("SpirvTest/SpirvGenerator.spiv", std::ios::binary);
            fout.write(reinterpret_cast<const char*>(source.data()), source.size() * sizeof(Spirv::Word));
            fout.close();
        }
#endif
    }

    TEST(Shader, SpirvShaderGenerator_FragmentScript_PushConstants)
    {
        Logger logger;

        Visual::FragmentScript fragmentScript;
        {
            auto& script = fragmentScript;

            auto& pc1 = script.GetPushConstants().AddMember<Vector4f32>(4);
            auto& pc2 = script.GetPushConstants().AddMember<Vector4f32>(5);
            auto& pc3 = script.GetPushConstants().AddMember<Vector4f32>(6);

            auto& mult1 = script.CreateOperator<Visual::Operators::MultVec4f32>();
            mult1.GetLeftInput().Connect(pc1.GetPin());
            mult1.GetRightInput().Connect(pc1.GetPin());

            auto& add1 = script.CreateOperator<Visual::Operators::AddVec4f32>();
            add1.GetLeftInput().Connect(pc2.GetPin());
            add1.GetRightInput().Connect(pc3.GetPin());

            auto& add2 = script.CreateOperator<Visual::Operators::AddVec4f32>();
            add2.GetLeftInput().Connect(pc3.GetPin());
            add2.GetRightInput().Connect(mult1.GetOutput());

            auto& add3 = script.CreateOperator<Visual::Operators::AddVec4f32>();
            add3.GetLeftInput().Connect(add1.GetOutput());
            add3.GetRightInput().Connect(add2.GetOutput());

            auto& output1 = script.GetOutputInterface().AddMember<Vector4f32>();
            output1.Connect(add3.GetOutput());
        }

        SpirvGenerator generator(&logger);
        const SpirvGeneratorDescriptor descriptor = { &fragmentScript, {}, true, true };
        const auto result = generator.Generate(descriptor);
        EXPECT_FALSE(result.source.empty());
        EXPECT_EQ(result.source.size(), size_t{ 187 });

#if 0
        {
            std::ofstream fout("SpirvTest/SpirvGenerator.spiv", std::ios::binary);
            fout.write(reinterpret_cast<const char*>(source.data()), source.size() * sizeof(Spirv::Word));
            fout.close();
        }
#endif
    }

    TEST(Shader, SpirvShaderGenerator_FragmentScript_OperatorsAndFunctions)
    {
        Logger logger;

        Visual::FragmentScript fragmentScript;
        {
            auto& script = fragmentScript;

            auto& input1 = script.GetInputInterface().AddMember<Vector4f32>();
            auto& input2 = script.GetInputInterface().AddMember<Vector4f32>();

            auto& sin1 = script.CreateFunction<Visual::Functions::SinVec4f32>();
            sin1.GetInput<0>().Connect(input2);

            auto& cos1 = script.CreateFunction<Visual::Functions::CosVec4f32>();
            cos1.GetInput<0>().Connect(sin1.GetOutput());

            auto& tan1 = script.CreateFunction<Visual::Functions::TanVec4f32>();
            tan1.GetInput<0>().Connect(cos1.GetOutput());

            auto& max1 = script.CreateFunction<Visual::Functions::MaxVec4f32>();
            max1.GetInput<0>().Connect(tan1.GetOutput());
            max1.GetInput<1>().SetDefaultValue({ 1.0f, 2.0f, 3.0f, 3.0f });

            auto& min1 = script.CreateFunction<Visual::Functions::MinVec4f32>();
            min1.GetInput<0>().Connect(max1.GetOutput());
            min1.GetInput<1>().SetDefaultValue({ 1.0f, 2.0f, 3.0f, 3.0f });

            auto& div1 = script.CreateOperator<Visual::Operators::DivVec4f32>();
            div1.GetLeftInput().Connect(min1.GetOutput());
            div1.GetRightInput().Connect(input1.GetPin());

            auto& mult1 = script.CreateOperator<Visual::Operators::MultVec4f32>();
            mult1.GetLeftInput().SetDefaultValue({ 0.5f, 1.0f, 2.0f, 3.0f });
            mult1.GetRightInput().Connect(div1.GetOutput());

            auto& add1 = script.CreateOperator<Visual::Operators::AddVec4f32>();
            add1.GetLeftInput().Connect(mult1.GetOutput());
            add1.GetRightInput().SetDefaultValue({ 2.0f, 3.0f, 4.0f, 5.0f });

            auto& sub1 = script.CreateOperator<Visual::Operators::SubVec4f32>();
            sub1.GetLeftInput().SetDefaultValue({ 1.0f, 2.0f, 3.0f, 4.0f });
            sub1.GetRightInput().Connect(add1.GetOutput());

            auto& output1 = script.GetOutputInterface().AddMember<Vector4f32>();
            auto& output2 = script.GetOutputInterface().AddMember<Vector4f32>();
            output1.Connect(sub1.GetOutput());
            output2.Connect(add1.GetOutput());
        }

        SpirvGenerator generator(&logger);
        const SpirvGeneratorDescriptor descriptor = { &fragmentScript, {}, true, true };
        const auto result = generator.Generate(descriptor);
        EXPECT_FALSE(result.source.empty());
        EXPECT_EQ(result.source.size(), size_t{ 263 });

#if 0
        {
            std::ofstream fout("SpirvTest/SpirvGenerator.spiv", std::ios::binary);
            fout.write(reinterpret_cast<const char*>(source.data()), source.size() * sizeof(Spirv::Word));
            fout.close();
        }
#endif
    }

    TEST(Shader, SpirvShaderGenerator_FragmentScript_Sampler)
    {
        Logger logger;

        Visual::FragmentScript fragmentScript;
        {
            auto& script = fragmentScript;

            auto* sampler1 = script.GetDescriptorSets().AddSet(1)->AddBinding<Sampler2D>(2);

            auto& texture1 = script.CreateFunction<Visual::Functions::Texture2D>();
            texture1.GetInput<0>().Connect(sampler1->GetOutput());
            texture1.GetInput<1>().SetDefaultValue({ 0.0f, 0.0f });

            auto& texture2 = script.CreateFunction<Visual::Functions::Texture2D>();
            texture2.GetInput<0>().Connect(sampler1->GetOutput());
            texture2.GetInput<1>().SetDefaultValue({ 1.0f, 0.0f });

            auto& output1 = script.GetOutputInterface().AddMember<Vector4f32>();
            output1.Connect(texture1.GetOutput());

            auto& output2 = script.GetOutputInterface().AddMember<Vector4f32>();
            output2.Connect(texture2.GetOutput());
        }

        SpirvGenerator generator(&logger);
        const SpirvGeneratorDescriptor descriptor = { &fragmentScript, {}, true, true };
        const auto result = generator.Generate(descriptor);
        EXPECT_FALSE(result.source.empty());
        EXPECT_EQ(result.source.size(), size_t{ 164 });

#if 0
        {
            std::ofstream fout("SpirvTest/SpirvGenerator.spiv", std::ios::binary);
            fout.write(reinterpret_cast<const char*>(source.data()), source.size() * sizeof(Spirv::Word));
            fout.close();
        }
#endif
    }

    TEST(Shader, SpirvShaderGenerator_FragmentScript_UniformBuffer)
    {
        Logger logger;

        Visual::FragmentScript fragmentScript;
        {
            auto& script = fragmentScript;

            auto* set1 = script.GetDescriptorSets().AddSet(1);
            auto* ubo1 = set1->AddBinding<Visual::FragmentUniformBuffer>(2);
            auto& ubo1_1 = ubo1->AddPin<Vector4f32>();
            auto& ubo1_2 = ubo1->AddPin<Vector4f32>();

            auto* set2 = script.GetDescriptorSets().AddSet(2);
            auto* ubo2 = set2->AddBinding<Visual::FragmentUniformBuffer>(2);
            auto& ubo2_1 = ubo2->AddPin<Vector4f32>();

            auto& output1 = script.GetOutputInterface().AddMember<Vector4f32>();
            auto& output2 = script.GetOutputInterface().AddMember<Vector4f32>();
            auto& output3 = script.GetOutputInterface().AddMember<Vector4f32>();
            output1.Connect(ubo1_1);
            output2.Connect(ubo1_2);
            output3.Connect(ubo2_1);
        }

        SpirvGenerator generator(&logger);
        const SpirvGeneratorDescriptor descriptor = { &fragmentScript, {}, true, true };
        const auto result = generator.Generate(descriptor);
        EXPECT_FALSE(result.source.empty());
        EXPECT_EQ(result.source.size(), size_t{ 220 });

#if 0
        {
            std::ofstream fout("SpirvTest/SpirvGenerator.spiv", std::ios::binary);
            fout.write(reinterpret_cast<const char*>(source.data()), source.size() * sizeof(Spirv::Word));
            fout.close();
        }
#endif
    }

    TEST(Shader, SpirvShaderGenerator_FragmentScript_Composite)
    {
        Logger logger;

        Visual::FragmentScript fragmentScript;
        {
            auto& script = fragmentScript;
            
            auto& input1 = script.GetInputInterface().AddMember<Vector2f32>();
            auto& input2 = script.GetInputInterface().AddMember<float>();

            auto& vec2Comp1 = script.CreateComposite<Visual::Composites::Vec2f32FromFloat32>();
            vec2Comp1.GetInput<0>().Connect(input2);
            vec2Comp1.GetInput<1>().SetDefaultValue(5.0f);

            auto& vec3Comp1 = script.CreateComposite<Visual::Composites::Vec3f32FromVec2f32Float32>();
            vec3Comp1.GetInput<0>().Connect(input1);
            vec3Comp1.GetInput<1>().Connect(input2);

            auto& vec4Comp1 = script.CreateComposite<Visual::Composites::Vec4f32FromVec2f32>();
            vec4Comp1.GetInput<0>().Connect(input1);
            vec4Comp1.GetInput<1>().Connect(input1);

            auto& output1 = script.GetOutputInterface().AddMember<Vector2f32>();
            output1.Connect(vec2Comp1.GetOutput());

            auto& output2 = script.GetOutputInterface().AddMember<Vector3f32>();
            output2.Connect(vec3Comp1.GetOutput());

            auto& output3 = script.GetOutputInterface().AddMember<Vector4f32>();
            output3.Connect(vec4Comp1.GetOutput());
        }

        SpirvGenerator generator(&logger);
        const SpirvGeneratorDescriptor descriptor = { &fragmentScript, {}, true, true };
        const auto result = generator.Generate(descriptor);
        EXPECT_FALSE(result.source.empty());
        EXPECT_EQ(result.source.size(), size_t{ 191 });

#if 0
        {
            std::ofstream fout("SpirvTest/SpirvGenerator.spiv", std::ios::binary);
            fout.write(reinterpret_cast<const char*>(source.data()), source.size() * sizeof(Spirv::Word) );
            fout.close();
        }
#endif
    }

    TEST(Shader, SpirvShaderGenerator_VertexScript)
    {
        Logger logger;

        Visual::VertexScript vertexScript;
        {
            auto& script = vertexScript;

            auto& const1 = script.CreateConstant<Vector4f32>({ 1.0f, 2.0f, 3.0f, 4.0f });

            script.GetVertexOutput()->GetInputPin()->Connect(const1.GetOutput());

        }

        SpirvGenerator generator(&logger);
        const SpirvGeneratorDescriptor descriptor = { &vertexScript, {}, true, true };
        const auto result = generator.Generate(descriptor);
        EXPECT_FALSE(result.source.empty());
        EXPECT_EQ(result.source.size(), size_t{ 119 });

#if 0
        {
            std::ofstream fout("SpirvTest/SpirvGenerator.spiv", std::ios::binary);
            fout.write(reinterpret_cast<const char*>(source.data()), source.size() * sizeof(Spirv::Word));
            fout.close();
        }
#endif

    }

    TEST(Shader, SpirvShaderGenerator_VertexAndFragmentScript)
    {
        Logger logger;

        Visual::FragmentScript fragmentScript;
        {
            auto& script = fragmentScript;

            auto& in1 = script.GetInputInterface().AddMember<Vector3f32>();
            script.GetInputInterface().AddMember<Vector4f32>();

            auto& out1 = script.GetOutputInterface().AddMember<Vector3f32>();
            out1.Connect(in1);
        }

        Visual::VertexScript vertexScript;
        {
            auto& script = vertexScript;

            auto& const1 = script.CreateConstant<Vector4f32>({ 1.0f, 2.0f, 3.0f, 4.0f });
            script.GetVertexOutput()->GetInputPin()->Connect(const1.GetOutput());

            script.GetInputInterface().AddMember<Vector4f32>();

            auto& in1 = script.GetInputInterface().AddMember<Vector3f32>();
            auto& in2 = script.GetInputInterface().AddMember<Vector4f32>();

            auto& out1 = script.GetOutputInterface().AddMember<Vector3f32>();
            auto& out2 = script.GetOutputInterface().AddMember<Vector4f32>();

            out1.Connect(in1);
            out2.Connect(in2);
        }

        auto& vertexOutputs = vertexScript.GetOutputInterface();
        auto& fragmentInputs = fragmentScript.GetInputInterface();
        ASSERT_TRUE(vertexOutputs.CompareStructure(fragmentInputs));

        SpirvGenerator generator(&logger);

        const SpirvGeneratorDescriptor fragDescriptor = { &fragmentScript, {}, true, true };
        const auto fragResult = generator.Generate(fragDescriptor);
        EXPECT_FALSE(fragResult.source.empty());
        EXPECT_EQ(fragResult.source.size(), size_t{ 90 });
        EXPECT_FALSE(fragResult.ignoredInputIndices.empty());

        const SpirvGeneratorDescriptor vertDescriptor = { &vertexScript, fragResult.ignoredInputIndices, false, true };
        const auto vertResult = generator.Generate(vertDescriptor);
        EXPECT_FALSE(vertResult.source.empty());
        EXPECT_EQ(vertResult.source.size(), size_t{ 194 });
        EXPECT_TRUE(vertResult.ignoredInputIndices.empty());
#if 0
        {
            std::ofstream fout("SpirvTest/SpirvGeneratorFrag.spiv", std::ios::binary);
            fout.write(reinterpret_cast<const char*>(fragResult.source.data()), fragResult.source.size() * sizeof(Spirv::Word));
            fout.close();
        }
        {
            std::ofstream fout("SpirvTest/SpirvGeneratorVert.spiv", std::ios::binary);
            fout.write(reinterpret_cast<const char*>(vertResult.source.data()), vertResult.source.size() * sizeof(Spirv::Word));
            fout.close();
        }
#endif
    }

}