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
#include "Curse/Renderer/Shader/Visual/VisualShaderScript.hpp"

namespace Curse::Shader::Visual
{

    TEST(Shader, Script_CreateOutputNode)
    {
        FragmentScript script;

        {
            OutputVariable<bool>* out = script.GetOutputInterface().AddMember<bool>();
            EXPECT_EQ(out->GetInputPinCount(), size_t(1));
            EXPECT_NE(out->GetInputPin(), nullptr);
            EXPECT_NE(out->GetInputPin(0), nullptr);
            EXPECT_EQ(out->GetInputPin(1), nullptr);
            EXPECT_EQ(out->GetInputPin(), out->GetInputPin(0));
            EXPECT_EQ(out->GetInputPins().size(), size_t(1));
            if (out->GetInputPins().size())
            {
                EXPECT_EQ(out->GetInputPins()[0], out->GetInputPin());
            }
            EXPECT_EQ(out->GetInputPin()->GetDataType(), VariableDataType::Bool);
        }
        {
            OutputVariable<int32_t>* out = script.GetOutputInterface().AddMember<int32_t>();
            EXPECT_EQ(out->GetInputPinCount(), size_t(1));
            EXPECT_NE(out->GetInputPin(), nullptr);
            EXPECT_NE(out->GetInputPin(0), nullptr);
            EXPECT_EQ(out->GetInputPin(1), nullptr);
            EXPECT_EQ(out->GetInputPin(), out->GetInputPin(0));
            EXPECT_EQ(out->GetInputPins().size(), size_t(1));
            if (out->GetInputPins().size())
            {
                EXPECT_EQ(out->GetInputPins()[0], out->GetInputPin());
            }
            EXPECT_EQ(out->GetInputPin()->GetDataType(), VariableDataType::Int32);
        }
        {
            OutputVariable<Vector2f32>* out = script.GetOutputInterface().AddMember<Vector2f32>();
            EXPECT_EQ(out->GetInputPinCount(), size_t(1));
            EXPECT_NE(out->GetInputPin(), nullptr);
            EXPECT_NE(out->GetInputPin(0), nullptr);
            EXPECT_EQ(out->GetInputPin(1), nullptr);
            EXPECT_EQ(out->GetInputPin(), out->GetInputPin(0));
            EXPECT_EQ(out->GetInputPins().size(), size_t(1));
            if (out->GetInputPins().size())
            {
                EXPECT_EQ(out->GetInputPins()[0], out->GetInputPin());
            }
            EXPECT_EQ(out->GetInputPin()->GetDataType(), VariableDataType::Vector2f32);
        }
        {
            OutputVariable<Vector3f32>* out = script.GetOutputInterface().AddMember<Vector3f32>();
            EXPECT_EQ(out->GetInputPinCount(), size_t(1));
            EXPECT_NE(out->GetInputPin(), nullptr);
            EXPECT_NE(out->GetInputPin(0), nullptr);
            EXPECT_EQ(out->GetInputPin(1), nullptr);
            EXPECT_EQ(out->GetInputPin(), out->GetInputPin(0));
            EXPECT_EQ(out->GetInputPins().size(), size_t(1));
            if (out->GetInputPins().size())
            {
                EXPECT_EQ(out->GetInputPins()[0], out->GetInputPin());
            }
            EXPECT_EQ(out->GetInputPin()->GetDataType(), VariableDataType::Vector3f32);
        }
        {
            OutputVariable<Vector4f32>* out = script.GetOutputInterface().AddMember<Vector4f32>();
            EXPECT_EQ(out->GetInputPinCount(), size_t(1));
            EXPECT_NE(out->GetInputPin(), nullptr);
            EXPECT_NE(out->GetInputPin(0), nullptr);
            EXPECT_EQ(out->GetInputPin(1), nullptr);
            EXPECT_EQ(out->GetInputPin(), out->GetInputPin(0));
            EXPECT_EQ(out->GetInputPins().size(), size_t(1));
            if (out->GetInputPins().size())
            {
                EXPECT_EQ(out->GetInputPins()[0], out->GetInputPin());
            }
            EXPECT_EQ(out->GetInputPin()->GetDataType(), VariableDataType::Vector4f32);
        }
        
    }

}