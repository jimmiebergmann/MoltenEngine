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
#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"

namespace Molten::Shader::Visual
{

    struct MyInputStruct : Structure<InputPin, Vector2f32, Vector3f32, Vector4f32>
    {
        MyInputStruct(Script& script) :
            Structure<InputPin, Vector2f32, Vector3f32, Vector4f32>(script)
        {}

        NodeType GetType() const override
        {
            return NodeType::Variable;
        }
    };

    struct MyOutputStruct : Structure<OutputPin, Vector2f32, Vector3f32, Vector4f32>
    {
        MyOutputStruct(Script& script) :
            Structure<OutputPin, Vector2f32, Vector3f32, Vector4f32>(script)
        {}

        NodeType GetType() const override
        {
            return NodeType::Variable;
        }
    };

    TEST(Shader, VisualScript_InputStructure)
    {
        FragmentScript script;

        {
            MyInputStruct myStruct(script);
            InputPin<Vector3f32>& m1 = myStruct.AddPin<Vector3f32>();
            InputPin<Vector2f32>& m2 = myStruct.AddPin<Vector2f32>();
            InputPin<Vector4f32>& m3 = myStruct.AddPin<Vector4f32>();
            EXPECT_EQ(myStruct.GetPinCount(), size_t(3));
            EXPECT_EQ(myStruct.GetInputPins().size(), size_t(3));
            EXPECT_EQ(myStruct.GetOutputPins().size(), size_t(0));

            ASSERT_EQ(myStruct.GetOutputPin(), nullptr);
            ASSERT_EQ(myStruct.GetOutputPin(0), nullptr);
            ASSERT_EQ(myStruct.GetOutputPin(1), nullptr);
            ASSERT_EQ(myStruct.GetOutputPin(2), nullptr);
            EXPECT_EQ(myStruct.GetOutputPin(3), nullptr);

            EXPECT_EQ(myStruct.GetInputPins()[0], myStruct.GetInputPin(0));
            EXPECT_EQ(myStruct.GetInputPins()[1], myStruct.GetInputPin(1));
            EXPECT_EQ(myStruct.GetInputPins()[2], myStruct.GetInputPin(2));

            EXPECT_EQ(myStruct.GetInputPin(), myStruct.GetInputPin(0));

            {
                InputPin<Vector3f32>* pm1 = myStruct.GetInputPin<Vector3f32>(0);
                InputPin<Vector2f32>* pm2 = myStruct.GetInputPin<Vector2f32>(1);
                InputPin<Vector4f32>* pm3 = myStruct.GetInputPin<Vector4f32>(2);
                InputPin<Vector4f32>* pm4 = myStruct.GetInputPin<Vector4f32>(3);

                EXPECT_EQ(pm1, &m1);
                EXPECT_EQ(pm2, &m2);
                EXPECT_EQ(pm3, &m3);
                EXPECT_EQ(pm4, nullptr);
            }
            {
                Pin* pm1 = myStruct.GetInputPin(0);
                Pin* pm2 = myStruct.GetInputPin(1);
                Pin* pm3 = myStruct.GetInputPin(2);
                Pin* pm4 = myStruct.GetInputPin(3);

                EXPECT_EQ(pm1, &m1);
                EXPECT_EQ(pm2, &m2);
                EXPECT_EQ(pm3, &m3);
                EXPECT_EQ(pm4, nullptr);
            }            
            {
                // Check const correctness
                const auto& constMyStruct = myStruct;

                auto* pm1 = constMyStruct.GetInputPin<Vector3f32>(0);
                EXPECT_TRUE(std::is_const< std::remove_pointer_t<decltype(pm1)>>::value);

                auto* pm2 = constMyStruct.GetInputPin(0);
                EXPECT_TRUE(std::is_const<std::remove_pointer_t<decltype(pm2)>>::value);
            }
        }
    }
    
    TEST(Shader, VisualScript_OutputStructure)
    {
        FragmentScript script;

        {
            MyOutputStruct myStruct(script);
            OutputPin<Vector3f32>& m1 = myStruct.AddPin<Vector3f32>();
            OutputPin<Vector2f32>& m2 = myStruct.AddPin<Vector2f32>();
            OutputPin<Vector4f32>& m3 = myStruct.AddPin<Vector4f32>();
            EXPECT_EQ(myStruct.GetPinCount(), size_t(3));
            EXPECT_EQ(myStruct.GetOutputPins().size(), size_t(3));
            EXPECT_EQ(myStruct.GetInputPins().size(), size_t(0));
            

            ASSERT_EQ(myStruct.GetInputPin(), nullptr);
            ASSERT_EQ(myStruct.GetInputPin(0), nullptr);
            ASSERT_EQ(myStruct.GetInputPin(1), nullptr);
            ASSERT_EQ(myStruct.GetInputPin(2), nullptr);
            EXPECT_EQ(myStruct.GetInputPin(3), nullptr);

            EXPECT_EQ(myStruct.GetOutputPins()[0], myStruct.GetOutputPin(0));
            EXPECT_EQ(myStruct.GetOutputPins()[1], myStruct.GetOutputPin(1));
            EXPECT_EQ(myStruct.GetOutputPins()[2], myStruct.GetOutputPin(2));

            EXPECT_EQ(myStruct.GetOutputPin(), myStruct.GetOutputPin(0));

            {
                OutputPin<Vector3f32>* pm1 = myStruct.GetOutputPin<Vector3f32>(0);
                OutputPin<Vector2f32>* pm2 = myStruct.GetOutputPin<Vector2f32>(1);
                OutputPin<Vector4f32>* pm3 = myStruct.GetOutputPin<Vector4f32>(2);
                OutputPin<Vector4f32>* pm4 = myStruct.GetOutputPin<Vector4f32>(3);

                EXPECT_EQ(pm1, &m1);
                EXPECT_EQ(pm2, &m2);
                EXPECT_EQ(pm3, &m3);
                EXPECT_EQ(pm4, nullptr);
            }
            {
                Pin* pm1 = myStruct.GetOutputPin(0);
                Pin* pm2 = myStruct.GetOutputPin(1);
                Pin* pm3 = myStruct.GetOutputPin(2);
                Pin* pm4 = myStruct.GetOutputPin(3);

                EXPECT_EQ(pm1, &m1);
                EXPECT_EQ(pm2, &m2);
                EXPECT_EQ(pm3, &m3);
                EXPECT_EQ(pm4, nullptr);
            }
            {
                // Check const correctness
                const auto& constMyStruct = myStruct;

                auto* pm1 = constMyStruct.GetOutputPin<Vector3f32>(0);
                EXPECT_TRUE(std::is_const< std::remove_pointer_t<decltype(pm1)>>::value);

                auto* pm2 = constMyStruct.GetOutputPin(0);
                EXPECT_TRUE(std::is_const<std::remove_pointer_t<decltype(pm2)>>::value);
            }
        }
    }

    TEST(Shader, VisualScript_Structure_Remove)
    {
        FragmentScript script;

        {
            MyInputStruct myStruct(script);
            myStruct.AddPin<Vector3f32>();
            InputPin<Vector2f32>& m2 = myStruct.AddPin<Vector2f32>();
            InputPin<Vector4f32>& m3 = myStruct.AddPin<Vector4f32>();
            ASSERT_EQ(myStruct.GetPinCount(), size_t(3));
            EXPECT_EQ(myStruct.GetSizeOf(), size_t(36));

            myStruct.RemovePin(0);
            ASSERT_EQ(myStruct.GetPinCount(), size_t(2));
            EXPECT_EQ(myStruct.GetInputPin(0), &m2);
            EXPECT_EQ(myStruct.GetInputPin(1), &m3);
            EXPECT_EQ(myStruct.GetSizeOf(), size_t(24));

            myStruct.RemovePin(1);
            ASSERT_EQ(myStruct.GetPinCount(), size_t(1));
            EXPECT_EQ(myStruct.GetInputPin(0), &m2);
            EXPECT_EQ(myStruct.GetSizeOf(), size_t(8));

            myStruct.RemovePin(0);
            ASSERT_EQ(myStruct.GetPinCount(), size_t(0));
            EXPECT_EQ(myStruct.GetSizeOf(), size_t(0));
        }
        {
            MyInputStruct myStruct(script);
            myStruct.AddPin<Vector3f32>();
            myStruct.AddPin<Vector2f32>();
            myStruct.AddPin<Vector4f32>();
            ASSERT_EQ(myStruct.GetPinCount(), size_t(3));
            EXPECT_EQ(myStruct.GetSizeOf(), size_t(36));

            myStruct.RemoveAllPins();
            ASSERT_EQ(myStruct.GetPinCount(), size_t(0));
            EXPECT_EQ(myStruct.GetSizeOf(), size_t(0));
        }
    }
}