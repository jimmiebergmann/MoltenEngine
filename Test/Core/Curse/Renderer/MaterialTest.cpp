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
#include "Curse/Renderer/Material/MaterialScript.hpp"

namespace Curse
{

    namespace Material
    {
        TEST(Material, Script_GenerateGlsl)
        {
            Script script;

            auto output = script.CreateOutputNode<Curse::Vector4f32>();
            auto color = script.CreateVaryingNode<Curse::Material::VaryingType::Color>();
            auto mult = script.CreateOperatorNode<Curse::Vector4f32>(Curse::Material::Operator::Multiplication);
            auto add = script.CreateOperatorNode<Curse::Vector4f32>(Curse::Material::Operator::Addition);
            auto const1 = script.CreateConstantNode<Curse::Vector4f32>({ 0.0f, 0.0f, 0.3f, 0.0f });
            auto const2 = script.CreateConstantNode<Curse::Vector4f32>({ 1.0f, 0.5f, 0.0f, 1.0f });

            output->GetInputPin()->Connect(*add->GetOutputPin());

            add->GetInputPin(0)->Connect(*mult->GetOutputPin());
            add->GetInputPin(1)->Connect(*const1->GetOutputPin());

            mult->GetInputPin(0)->Connect(*color->GetOutputPin());
            mult->GetInputPin(1)->Connect(*const2->GetOutputPin());

            auto source = script.GenerateGlsl();

            static const std::string expectedSource =
                "#version 450\n"
                "#extension GL_ARB_separate_shader_objects : enable\n"
                "layout(location = 0) in vec4 v_var_0;\n"
                "layout(location = 0) out vec4 o_var_0;\n"
                "void main(){\n"
                "vec4 l_var_0 = vec4(1, 0.5, 0, 1);\n"
                "vec4 l_var_1 = v_var_0 * l_var_0;\n"
                "vec4 l_var_2 = vec4(0, 0, 0.3, 0);\n"
                "vec4 l_var_3 = l_var_1 + l_var_2;\n"
                "o_var_0 = l_var_3;\n"
                "}\n";

            EXPECT_STREQ(source.c_str(), expectedSource.c_str());
        }

    }

}