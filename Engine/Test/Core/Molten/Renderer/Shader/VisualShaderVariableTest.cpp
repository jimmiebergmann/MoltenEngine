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

#include "Test.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"

namespace Molten::Shader::Visual
{

    TEST(Shader, VisualScript_Variable)
    {
        FragmentScript script;
        Structure<InputVariable> vertexInput(script);
        
        {
            InputVariable<bool>* member = vertexInput.AddMember<bool>();
            EXPECT_EQ(member->GetDataType(), VariableDataType::Bool);
            EXPECT_EQ(member->GetSizeOf(), size_t(1));
        }
        {
            InputVariable<int32_t>* member = vertexInput.AddMember<int32_t>();
            EXPECT_EQ(member->GetDataType(), VariableDataType::Int32);
            EXPECT_EQ(member->GetSizeOf(), size_t(4));
        }
        {
            InputVariable<float>* member = vertexInput.AddMember<float>();
            EXPECT_EQ(member->GetDataType(), VariableDataType::Float32);
            EXPECT_EQ(member->GetSizeOf(), size_t(4));
        }     
        {
            InputVariable<Vector2f32>* member = vertexInput.AddMember<Vector2f32>();
            EXPECT_EQ(member->GetDataType(), VariableDataType::Vector2f32);
            EXPECT_EQ(member->GetSizeOf(), size_t(8));
        }
        {
            InputVariable<Vector3f32>* member = vertexInput.AddMember<Vector3f32>();
            EXPECT_EQ(member->GetDataType(), VariableDataType::Vector3f32);
            EXPECT_EQ(member->GetSizeOf(), size_t(12));
        }
        {
            InputVariable<Vector4f32>* member = vertexInput.AddMember<Vector4f32>();
            EXPECT_EQ(member->GetDataType(), VariableDataType::Vector4f32);
            EXPECT_EQ(member->GetSizeOf(), size_t(16));
        }
        {
            InputVariable<Matrix4x4f32>* member = vertexInput.AddMember<Matrix4x4f32>();
            EXPECT_EQ(member->GetDataType(), VariableDataType::Matrix4x4f32);
            EXPECT_EQ(member->GetSizeOf(), size_t(64));
        }

        // What about samplers?
        // Sampler2D,
        // Sampler3D
    }

}