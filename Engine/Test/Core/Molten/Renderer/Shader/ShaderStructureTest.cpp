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
#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"

namespace Molten::Shader::Visual
{

    TEST(Shader, InputVariable_Structure)
    {
        FragmentScript script;

        {
            Structure<InputVariable> vertexInput(script);
            InputVariable<Vector3f32>* m1 = vertexInput.AddMember<Vector3f32>();
            InputVariable<Vector2f32>* m2 = vertexInput.AddMember<Vector2f32>();
            InputVariable<Vector4f32>* m3 = vertexInput.AddMember<Vector4f32>();

            EXPECT_NE(m1, nullptr);
            EXPECT_NE(m2, nullptr);
            EXPECT_NE(m3, nullptr);
            EXPECT_EQ(vertexInput.GetMemberCount(), size_t(3));

            EXPECT_NE(m1->GetOutputPin(), nullptr);
            EXPECT_NE(m1->GetOutputPin(0), nullptr);
            EXPECT_EQ(m1->GetOutputPin(1), nullptr);

            {
                InputVariable<Vector3f32>* pm1 = vertexInput.GetMember<Vector3f32>(0);
                InputVariable<Vector2f32>* pm2 = vertexInput.GetMember<Vector2f32>(1);
                InputVariable<Vector4f32>* pm3 = vertexInput.GetMember<Vector4f32>(2);

                EXPECT_EQ(m1, pm1);
                EXPECT_EQ(m2, pm2);
                EXPECT_EQ(m3, pm3);
            }
            {
                InputVariableBase* pm1 = vertexInput.GetMember(0);
                InputVariableBase* pm2 = vertexInput.GetMember(1);
                InputVariableBase* pm3 = vertexInput.GetMember(2);

                EXPECT_EQ(m1, pm1);
                EXPECT_EQ(m2, pm2);
                EXPECT_EQ(m3, pm3);
            }            
            {
                InputVariableBase* pm1 = vertexInput[0];
                InputVariableBase* pm2 = vertexInput[1];
                InputVariableBase* pm3 = vertexInput[2];

                EXPECT_EQ(m1, pm1);
                EXPECT_EQ(m2, pm2);
                EXPECT_EQ(m3, pm3);
            }
            {
                // Check const correctness
                const auto& constVertexInput = vertexInput;

                auto* pm1 = constVertexInput.GetMember<Vector3f32>(0);
                EXPECT_TRUE(std::is_const< std::remove_pointer_t<decltype(pm1)>>::value);

                auto* pm2 = constVertexInput.GetMember(0);
                EXPECT_TRUE(std::is_const<std::remove_pointer_t<decltype(pm2)>>::value);

                auto* pm3 = constVertexInput[0];
                EXPECT_TRUE(std::is_const< std::remove_pointer_t<decltype(pm3)>>::value);
            }
        }
    }

    TEST(Shader, OutputVariable_Structure)
    {
        FragmentScript script;

        {
            Structure<OutputVariable> vertexOutput(script);
            EXPECT_NE(vertexOutput.AddMember<Vector4f32>(), nullptr);
            EXPECT_EQ(vertexOutput.GetMemberCount(), size_t(1));
        }
    }

}