/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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
#include "Molten/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Shader/Visual/VisualShaderNode.hpp"
#include "Molten/Shader/Visual/VisualShaderPin.hpp"

namespace Molten::Shader::Visual
{

    struct MyInputStruct : Structure<NodeType::Constant, InputPin, void, bool, int32_t, float, Vector2f32, Vector3f32, Vector4f32>
    {
        MyInputStruct(Script& script) :
            Structure<NodeType::Constant, InputPin, void, bool, int32_t, float, Vector2f32, Vector3f32, Vector4f32>(script)
        {}
    };

    TEST(Shader, VisualShader_InputPin)
    {
        FragmentScript script;
        MyInputStruct myStruct(script);
        Node& node = myStruct;

        InputPin<float> pin(node, "test name");
        EXPECT_STREQ(pin.GetName().c_str(), "test name");
        EXPECT_EQ(pin.GetDirection(), PinDirection::In);
        EXPECT_EQ(pin.GetConnectionCount(), size_t{ 0 });
        EXPECT_EQ(pin.GetConnection(), nullptr);
        EXPECT_EQ(pin.GetConnection(0), nullptr);
        EXPECT_EQ(pin.GetConnection(1), nullptr);
        EXPECT_EQ(pin.GetConnections().size(), size_t{ 0 });
    }
    
    TEST(Shader, VisualShader_InputPin_DefaultValue)
    {
        FragmentScript script;
        MyInputStruct myStruct(script);
        Node& node = myStruct;

        InputPin<int32_t> pin(node, 1234, "test name");
        EXPECT_STREQ(pin.GetName().c_str(), "test name");
        EXPECT_EQ(pin.GetDefaultValue(), int32_t(1234));
        EXPECT_EQ(pin.GetDirection(), PinDirection::In);
        EXPECT_EQ(pin.GetConnectionCount(), size_t{ 0 });
        EXPECT_EQ(pin.GetConnection(), nullptr);
        EXPECT_EQ(pin.GetConnection(0), nullptr);
        EXPECT_EQ(pin.GetConnection(1), nullptr);
        EXPECT_EQ(pin.GetConnections().size(), size_t{ 0 });

        pin.SetDefaultValue(5432);
        EXPECT_EQ(pin.GetDefaultValue(), int32_t(5432));
    }

    TEST(Shader, VisualShader_InputPin_DataType)
    {
        FragmentScript script;
        MyInputStruct myStruct(script);
        Node& node = myStruct;

        {
            InputPin<bool> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::In);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Bool);
        }
        {
            InputPin<int32_t> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::In);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Int32);
        }      
        {
            InputPin<float> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::In);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Float32);
        }          
        {
            InputPin<Vector2f32> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::In);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Vector2f32);
        }
        {
            InputPin<Vector3f32> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::In);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Vector3f32);
        }
        {
            InputPin<Vector4f32> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::In);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Vector4f32);
        }
    }

    TEST(Shader, VisualShader_InputPin_Connection)
    {
        FragmentScript script;
        MyInputStruct myStruct1(script);
        MyInputStruct myStruct2(script);
        Node& node1 = myStruct1;
        Node& node2 = myStruct2;

        {
            InputPin<float> pin1(node1);
            InputPin<float> pin2(node2);

            EXPECT_EQ(pin1.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(pin2.GetConnections().size(), size_t{ 0 });
            EXPECT_FALSE(pin1.ConnectBase(pin2));
            EXPECT_EQ(pin1.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(pin2.GetConnections().size(), size_t{ 0 });
            EXPECT_FALSE(pin2.ConnectBase(pin1));
            EXPECT_EQ(pin1.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(pin2.GetConnections().size(), size_t{ 0 });
        }
        {
            InputPin<float> in(node1);
            OutputPin<float> out(node1);
            EXPECT_FALSE(in.Connect(out));
        }
        {
            InputPin<float> in(node1);
            OutputPin<float> out(node2);

            const auto& in_const = in;
            const auto& out_const = out;

            EXPECT_EQ(in.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 0 });

            {
                EXPECT_TRUE(in.Connect(out));

                EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 1 });

                EXPECT_EQ(in.GetConnection(), &out);
                EXPECT_EQ(out.GetConnection(), &in);
                EXPECT_EQ(in_const.GetConnection(), &out_const);
                EXPECT_EQ(out_const.GetConnection(), &in_const);

                EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 1 });

                EXPECT_EQ(in.GetConnections()[0], &out);
                EXPECT_EQ(out.GetConnections()[0], &in);
                EXPECT_EQ(in_const.GetConnections()[0], &out_const);
                EXPECT_EQ(out_const.GetConnections()[0], &in_const);
            }
            {
                EXPECT_EQ(in.Disconnect(), size_t{ 1 });

                EXPECT_EQ(in.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 0 });

                EXPECT_EQ(in.GetConnection(), nullptr);
                EXPECT_EQ(out.GetConnection(), nullptr);
                EXPECT_EQ(in_const.GetConnection(), nullptr);
                EXPECT_EQ(out_const.GetConnection(), nullptr);

                EXPECT_EQ(in.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 0 });
            }
            {
                EXPECT_TRUE(in.Connect(out));

                EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 1 });

                EXPECT_EQ(in.GetConnection(), &out);
                EXPECT_EQ(out.GetConnection(), &in);
                EXPECT_EQ(in_const.GetConnection(), &out_const);
                EXPECT_EQ(out_const.GetConnection(), &in_const);

                EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 1 });

                EXPECT_EQ(in.GetConnections()[0], &out);
                EXPECT_EQ(out.GetConnections()[0], &in);
                EXPECT_EQ(in_const.GetConnections()[0], &out_const);
                EXPECT_EQ(out_const.GetConnections()[0], &in_const);
            }
        }
        {
            InputPin<float> in(node1);
            OutputPin<float> outA(node2);
            OutputPin<float> outB(node2);

            EXPECT_TRUE(in.Connect(outA));
            EXPECT_TRUE(in.Connect(outB));
            EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(outA.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(outB.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(in.GetConnections()[0], &outB);

            EXPECT_FALSE(in.Disconnect(1));
            EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(outA.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(outB.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(in.GetConnections()[0], &outB);

            EXPECT_TRUE(in.Disconnect(0));
            EXPECT_EQ(in.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(outA.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(outB.GetConnections().size(), size_t{ 0 });;
        }
        {
            InputPin<float> in(node1);
            OutputPin<float> outA(node2);
            OutputPin<float> outB(node2);

            EXPECT_TRUE(in.Connect(outA));

            EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(in.GetConnections()[0], &outA);
            EXPECT_EQ(outA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(outA.GetConnections()[0], &in);
            EXPECT_EQ(outB.GetConnections().size(), size_t{ 0 });

            EXPECT_FALSE(in.Disconnect(outB));
            EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(in.GetConnections()[0], &outA);
            EXPECT_EQ(outA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(outA.GetConnections()[0], &in);
            EXPECT_EQ(outB.GetConnections().size(), size_t{ 0 });

            EXPECT_TRUE(in.Disconnect(outA));
            EXPECT_EQ(in.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(outA.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(outB.GetConnections().size(), size_t{ 0 });
        }
    }

    TEST(Shader, VisualShader_OutputPin)
    {
        FragmentScript script;
        MyInputStruct myStruct(script);
        Node& node = myStruct;

        OutputPin<float> pin(node, "test name");
        EXPECT_STREQ(pin.GetName().c_str(), "test name");
        EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
        EXPECT_EQ(pin.GetConnectionCount(), size_t{ 0 });
        EXPECT_EQ(pin.GetConnection(), nullptr);
        EXPECT_EQ(pin.GetConnection(0), nullptr);
        EXPECT_EQ(pin.GetConnection(1), nullptr);
        EXPECT_EQ(pin.GetConnections().size(), size_t{ 0 });
    }

    TEST(Shader, VisualShader_OutputPin_DataType)
    {
        FragmentScript script;
        MyInputStruct myStruct(script);
        Node& node = myStruct;

        {
            OutputPin<bool> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Bool);
        }
        {
            OutputPin<int32_t> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Int32);
        }
        {
            OutputPin<float> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Float32);
        }
        {
            OutputPin<Vector2f32> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Vector2f32);
        }
        {
            OutputPin<Vector3f32> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Vector3f32);
        }
        {
            OutputPin<Vector4f32> pin(node);
            EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
            EXPECT_EQ(pin.GetDataType(), VariableDataType::Vector4f32);
        }
    }

    TEST(Shader, VisualShader_OutputPin_Connection)
    {
        FragmentScript script;
        MyInputStruct myStruct1(script);
        MyInputStruct myStruct2(script);
        Node& node1 = myStruct1;
        Node& node2 = myStruct2;

        {
            OutputPin<float> pin1(node1);
            OutputPin<float> pin2(node2);

            EXPECT_EQ(pin1.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(pin2.GetConnections().size(), size_t{ 0 });
            EXPECT_FALSE(pin1.ConnectBase(pin2));
            EXPECT_EQ(pin1.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(pin2.GetConnections().size(), size_t{ 0 });
            EXPECT_FALSE(pin2.ConnectBase(pin1));
            EXPECT_EQ(pin1.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(pin2.GetConnections().size(), size_t{ 0 });
        }
        {
            InputPin<float> in(node1);
            OutputPin<float> out(node2);

            const auto& in_const = in;
            const auto& out_const = out;

            EXPECT_EQ(in.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 0 });

            {
                EXPECT_TRUE(out.Connect(in));

                EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 1 });

                EXPECT_EQ(in.GetConnection(), &out);
                EXPECT_EQ(out.GetConnection(), &in);
                EXPECT_EQ(in_const.GetConnection(), &out_const);
                EXPECT_EQ(out_const.GetConnection(), &in_const);

                EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 1 });

                EXPECT_EQ(in.GetConnections()[0], &out);
                EXPECT_EQ(out.GetConnections()[0], &in);
                EXPECT_EQ(in_const.GetConnections()[0], &out_const);
                EXPECT_EQ(out_const.GetConnections()[0], &in_const);
            }
            {
                EXPECT_EQ(out.Disconnect(), size_t{ 1 });

                EXPECT_EQ(in.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 0 });

                EXPECT_EQ(in.GetConnection(), nullptr);
                EXPECT_EQ(out.GetConnection(), nullptr);
                EXPECT_EQ(in_const.GetConnection(), nullptr);
                EXPECT_EQ(out_const.GetConnection(), nullptr);

                EXPECT_EQ(in.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 0 });
            }
            {
                EXPECT_TRUE(out.Connect(in));

                EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 1 });

                EXPECT_EQ(in.GetConnection(), &out);
                EXPECT_EQ(out.GetConnection(), &in);
                EXPECT_EQ(in_const.GetConnection(), &out_const);
                EXPECT_EQ(out_const.GetConnection(), &in_const);

                EXPECT_EQ(in.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(in_const.GetConnections().size(), size_t{ 1 });
                EXPECT_EQ(out_const.GetConnections().size(), size_t{ 1 });

                EXPECT_EQ(in.GetConnections()[0], &out);
                EXPECT_EQ(out.GetConnections()[0], &in);
                EXPECT_EQ(in_const.GetConnections()[0], &out_const);
                EXPECT_EQ(out_const.GetConnections()[0], &in_const);
            }
        }
        {
            InputPin<float> inA(node1);
            InputPin<float> inB(node1);
            InputPin<float> inC(node1);
            OutputPin<float> out(node2);

            EXPECT_TRUE(out.Connect(inA));
            EXPECT_TRUE(out.Connect(inB));
            EXPECT_TRUE(out.Connect(inC));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 3 });
            EXPECT_EQ(out.GetConnections()[0], &inA);
            EXPECT_EQ(out.GetConnections()[1], &inB);
            EXPECT_EQ(out.GetConnections()[2], &inC);

            EXPECT_FALSE(out.Disconnect(4));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 3 });
            EXPECT_EQ(out.GetConnections()[0], &inA);
            EXPECT_EQ(out.GetConnections()[1], &inB);
            EXPECT_EQ(out.GetConnections()[2], &inC);

            EXPECT_TRUE(out.Disconnect(1));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 2 });
            EXPECT_EQ(out.GetConnections()[0], &inA);
            EXPECT_EQ(out.GetConnections()[1], &inC);

            EXPECT_FALSE(out.Disconnect(2));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 2 });
            EXPECT_EQ(out.GetConnections()[0], &inA);
            EXPECT_EQ(out.GetConnections()[1], &inC);

            EXPECT_TRUE(out.Disconnect(1));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(out.GetConnections()[0], &inA);

            EXPECT_FALSE(out.Disconnect(1));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(out.GetConnections()[0], &inA);

            EXPECT_TRUE(out.Disconnect(0));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 0 });
        }
        {
            InputPin<float> inA(node1);
            InputPin<float> inB(node1);
            InputPin<float> inC(node1);
            OutputPin<float> out(node2);

            EXPECT_TRUE(out.Connect(inA));
            EXPECT_TRUE(out.Connect(inB));
            EXPECT_TRUE(out.Connect(inC));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 3 });
            EXPECT_EQ(out.GetConnections()[0], &inA);
            EXPECT_EQ(out.GetConnections()[1], &inB);
            EXPECT_EQ(out.GetConnections()[2], &inC);

            EXPECT_TRUE(out.Disconnect(inB));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 2 });
            EXPECT_EQ(out.GetConnections()[0], &inA);
            EXPECT_EQ(out.GetConnections()[1], &inC);

            EXPECT_TRUE(out.Disconnect(inC));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 1 });
            EXPECT_EQ(out.GetConnections()[0], &inA);

            EXPECT_TRUE(out.Disconnect(inA));
            EXPECT_EQ(inA.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inB.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(inC.GetConnections().size(), size_t{ 0 });
            EXPECT_EQ(out.GetConnections().size(), size_t{ 0 });
        }
        {
            {
                OutputPin<float> out1(node1);
                OutputPin<float> out2(node2);

                EXPECT_FALSE(out1.ConnectBase(out2));

                EXPECT_EQ(out1.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out1.GetConnection(), nullptr);
                EXPECT_EQ(out1.GetConnections().size(), size_t{ 0 });

                EXPECT_EQ(out2.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out2.GetConnection(), nullptr);
                EXPECT_EQ(out2.GetConnections().size(), size_t{ 0 });
            }
            {
                OutputPin<float> out1(node1);
                InputPin<int32_t> in1(node2);

                EXPECT_FALSE(out1.ConnectBase(in1));

                EXPECT_EQ(out1.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(out1.GetConnection(), nullptr);
                EXPECT_EQ(out1.GetConnections().size(), size_t{ 0 });

                EXPECT_EQ(in1.GetConnections().size(), size_t{ 0 });
                EXPECT_EQ(in1.GetConnection(), nullptr);
                EXPECT_EQ(in1.GetConnections().size(), size_t{ 0 });
            }
        }
    }
    
}