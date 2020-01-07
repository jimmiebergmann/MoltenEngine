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
#include "Curse/Renderer/Material.hpp"
#include "Curse/Renderer/Material/MaterialNode.hpp"
#include "Curse/Renderer/Material/MaterialPin.hpp"

namespace Curse
{

    namespace Material
    {

        TEST(MaterialPin, Input)
        {
            Script script;
            Node* nodePtr = script.CreateOutputNode<bool>();
            Node& node = *nodePtr;

            InputPin<float> pin(node, "test name");
            EXPECT_STREQ(pin.GetName().c_str(), "test name");
            EXPECT_EQ(pin.GetDirection(), PinDirection::In);
            EXPECT_EQ(pin.GetConnectionCount(), size_t(0));
            EXPECT_EQ(pin.GetConnection(), nullptr);
            EXPECT_EQ(pin.GetConnection(0), nullptr);
            EXPECT_EQ(pin.GetConnection(1), nullptr);
            EXPECT_EQ(pin.GetConnections().size(), size_t(0));
        }

        TEST(MaterialPin, Input_DefaultValue)
        {
            Script script;
            Node* nodePtr = script.CreateOutputNode<bool>();
            Node& node = *nodePtr;

            InputPin<int32_t> pin(node, 1234, "test name");
            EXPECT_STREQ(pin.GetName().c_str(), "test name");
            EXPECT_EQ(pin.GetDefaultValue(), int32_t(1234));
            EXPECT_EQ(pin.GetDirection(), PinDirection::In);
            EXPECT_EQ(pin.GetConnectionCount(), size_t(0));
            EXPECT_EQ(pin.GetConnection(), nullptr);
            EXPECT_EQ(pin.GetConnection(0), nullptr);
            EXPECT_EQ(pin.GetConnection(1), nullptr);
            EXPECT_EQ(pin.GetConnections().size(), size_t(0));

            pin.SetDefaultValue(5432);
            EXPECT_EQ(pin.GetDefaultValue(), int32_t(5432));
        }

        TEST(MaterialPin, Input_DataType)
        {
            Script script;
            Node* nodePtr = script.CreateOutputNode<bool>();
            Node& node = *nodePtr;

            {
                InputPin<bool> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(bool));
            }
            /* {
                 InputPin<int8_t> pin;
                 EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                 EXPECT_EQ(pin.GetDataTypeIndex(), typeid(int8_t));
             }
             {
                 InputPin<int16_t> pin;
                 EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                 EXPECT_EQ(pin.GetDataTypeIndex(), typeid(int16_t));
             }*/
            {
                InputPin<int32_t> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(int32_t));
            }
            /* {
                 InputPin<int64_t> pin;
                 EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                 EXPECT_EQ(pin.GetDataTypeIndex(), typeid(int64_t));
             }
             {
                 InputPin<uint8_t> pin;
                 EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                 EXPECT_EQ(pin.GetDataTypeIndex(), typeid(uint8_t));
             }
             {
                 InputPin<uint16_t> pin;
                 EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                 EXPECT_EQ(pin.GetDataTypeIndex(), typeid(uint16_t));
             }
             {
                 InputPin<uint32_t> pin;
                 EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                 EXPECT_EQ(pin.GetDataTypeIndex(), typeid(uint32_t));
             }
             {
                 InputPin<uint64_t> pin;
                 EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                 EXPECT_EQ(pin.GetDataTypeIndex(), typeid(uint64_t));
             }*/
            {
                InputPin<float> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(float));
            }
            /*{
                InputPin<double> pin;
                EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(double));
            }  */
            {
                InputPin<Curse::Vector2f32> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(Curse::Vector2f32));
            }
            {
                InputPin<Curse::Vector3f32> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(Curse::Vector3f32));
            }
            {
                InputPin<Curse::Vector4f32> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::In);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(Curse::Vector4f32));
            }
        }

        TEST(MaterialPin, Input_Connection)
        {
            Script script;
            Node* nodePtr = script.CreateOutputNode<bool>();
            Node& node = *nodePtr;

            {
                InputPin<float> pin1(node);
                InputPin<float> pin2(node);

                EXPECT_EQ(pin1.GetConnections().size(), size_t(0));
                EXPECT_EQ(pin2.GetConnections().size(), size_t(0));
                EXPECT_FALSE(pin1.Connect(pin2));
                EXPECT_EQ(pin1.GetConnections().size(), size_t(0));
                EXPECT_EQ(pin2.GetConnections().size(), size_t(0));
                EXPECT_FALSE(pin2.Connect(pin1));
                EXPECT_EQ(pin1.GetConnections().size(), size_t(0));
                EXPECT_EQ(pin2.GetConnections().size(), size_t(0));
            }
            {
                InputPin<float> in(node);
                OutputPin<float> out(node);

                const auto& in_const = in;
                const auto& out_const = out;

                EXPECT_EQ(in.GetConnections().size(), size_t(0));
                EXPECT_EQ(out.GetConnections().size(), size_t(0));

                {
                    EXPECT_TRUE(in.Connect(out));

                    EXPECT_EQ(in.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out.GetConnections().size(), size_t(1));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(1));

                    EXPECT_EQ(in.GetConnection(), &out);
                    EXPECT_EQ(out.GetConnection(), &in);
                    EXPECT_EQ(in_const.GetConnection(), &out_const);
                    EXPECT_EQ(out_const.GetConnection(), &in_const);

                    EXPECT_EQ(in.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out.GetConnections().size(), size_t(1));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(1));

                    EXPECT_EQ(in.GetConnections()[0], &out);
                    EXPECT_EQ(out.GetConnections()[0], &in);
                    EXPECT_EQ(in_const.GetConnections()[0], &out_const);
                    EXPECT_EQ(out_const.GetConnections()[0], &in_const);
                }
                {
                    EXPECT_EQ(in.Disconnect(), size_t(1));

                    EXPECT_EQ(in.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out.GetConnections().size(), size_t(0));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(0));

                    EXPECT_EQ(in.GetConnection(), nullptr);
                    EXPECT_EQ(out.GetConnection(), nullptr);
                    EXPECT_EQ(in_const.GetConnection(), nullptr);
                    EXPECT_EQ(out_const.GetConnection(), nullptr);

                    EXPECT_EQ(in.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out.GetConnections().size(), size_t(0));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(0));
                }
                {
                    EXPECT_TRUE(in.Connect(out));

                    EXPECT_EQ(in.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out.GetConnections().size(), size_t(1));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(1));

                    EXPECT_EQ(in.GetConnection(), &out);
                    EXPECT_EQ(out.GetConnection(), &in);
                    EXPECT_EQ(in_const.GetConnection(), &out_const);
                    EXPECT_EQ(out_const.GetConnection(), &in_const);

                    EXPECT_EQ(in.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out.GetConnections().size(), size_t(1));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(1));

                    EXPECT_EQ(in.GetConnections()[0], &out);
                    EXPECT_EQ(out.GetConnections()[0], &in);
                    EXPECT_EQ(in_const.GetConnections()[0], &out_const);
                    EXPECT_EQ(out_const.GetConnections()[0], &in_const);
                }
            }
            {
                InputPin<float> in(node);
                OutputPin<float> out[2] = { node, node };

                EXPECT_TRUE(in.Connect(out[0]));
                EXPECT_TRUE(in.Connect(out[1]));
                EXPECT_EQ(in.GetConnections().size(), size_t(1));
                EXPECT_EQ(out[0].GetConnections().size(), size_t(0));
                EXPECT_EQ(out[1].GetConnections().size(), size_t(1));
                EXPECT_EQ(in.GetConnections()[0], &out[1]);

                EXPECT_FALSE(in.Disconnect(1));
                EXPECT_EQ(in.GetConnections().size(), size_t(1));
                EXPECT_EQ(out[0].GetConnections().size(), size_t(0));
                EXPECT_EQ(out[1].GetConnections().size(), size_t(1));
                EXPECT_EQ(in.GetConnections()[0], &out[1]);

                EXPECT_TRUE(in.Disconnect(0));
                EXPECT_EQ(in.GetConnections().size(), size_t(0));
                EXPECT_EQ(out[0].GetConnections().size(), size_t(0));
                EXPECT_EQ(out[1].GetConnections().size(), size_t(0));;
            }
            {
                InputPin<float> in(node);
                OutputPin<float> out[2] = { node, node };

                EXPECT_TRUE(in.Connect(out[0]));

                EXPECT_EQ(in.GetConnections().size(), size_t(1));
                EXPECT_EQ(in.GetConnections()[0], &out[0]);
                EXPECT_EQ(out[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(out[0].GetConnections()[0], &in);
                EXPECT_EQ(out[1].GetConnections().size(), size_t(0));

                EXPECT_FALSE(in.Disconnect(out[1]));
                EXPECT_EQ(in.GetConnections().size(), size_t(1));
                EXPECT_EQ(in.GetConnections()[0], &out[0]);
                EXPECT_EQ(out[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(out[0].GetConnections()[0], &in);
                EXPECT_EQ(out[1].GetConnections().size(), size_t(0));

                EXPECT_TRUE(in.Disconnect(out[0]));
                EXPECT_EQ(in.GetConnections().size(), size_t(0));
                EXPECT_EQ(out[0].GetConnections().size(), size_t(0));
                EXPECT_EQ(out[1].GetConnections().size(), size_t(0));
            }
        }

        TEST(MaterialPin, Output)
        {
            Script script;
            Node* nodePtr = script.CreateOutputNode<bool>();
            Node& node = *nodePtr;

            OutputPin<float> pin(node, "test name");
            EXPECT_STREQ(pin.GetName().c_str(), "test name");
            EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
            EXPECT_EQ(pin.GetConnectionCount(), size_t(0));
            EXPECT_EQ(pin.GetConnection(), nullptr);
            EXPECT_EQ(pin.GetConnection(0), nullptr);
            EXPECT_EQ(pin.GetConnection(1), nullptr);
            EXPECT_EQ(pin.GetConnections().size(), size_t(0));
        }

        TEST(MaterialPin, Output_DataType)
        {
            Script script;
            Node* nodePtr = script.CreateOutputNode<bool>();
            Node& node = *nodePtr;

            {
                OutputPin<bool> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(bool));
            }
            /*{
                OutputPin<int8_t> pin;
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(int8_t));
            }
            {
                OutputPin<int16_t> pin;
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(int16_t));
            }*/
            {
                OutputPin<int32_t> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(int32_t));
            }
            /*{
                OutputPin<int64_t> pin;
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(int64_t));
            }
            {
                OutputPin<uint8_t> pin;
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(uint8_t));
            }
            {
                OutputPin<uint16_t> pin;
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(uint16_t));
            }
            {
                OutputPin<uint32_t> pin;
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(uint32_t));
            }
            {
                OutputPin<uint64_t> pin;
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(uint64_t));
            }*/
            {
                OutputPin<float> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(float));
            }
            /*{
                OutputPin<double> pin;
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(double));
            }*/
            {
                OutputPin<Curse::Vector2f32> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(Curse::Vector2f32));
            }
            {
                OutputPin<Curse::Vector3f32> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(Curse::Vector3f32));
            }
            {
                OutputPin<Curse::Vector4f32> pin(node);
                EXPECT_EQ(pin.GetDirection(), PinDirection::Out);
                EXPECT_EQ(pin.GetDataTypeIndex(), typeid(Curse::Vector4f32));
            }
        }

        TEST(MaterialPin, Output_Connection)
        {
            Script script;
            Node* nodePtr = script.CreateOutputNode<bool>();
            Node& node = *nodePtr;

            {
                OutputPin<float> pin1(node);
                OutputPin<float> pin2(node);

                EXPECT_EQ(pin1.GetConnections().size(), size_t(0));
                EXPECT_EQ(pin2.GetConnections().size(), size_t(0));
                EXPECT_FALSE(pin1.Connect(pin2));
                EXPECT_EQ(pin1.GetConnections().size(), size_t(0));
                EXPECT_EQ(pin2.GetConnections().size(), size_t(0));
                EXPECT_FALSE(pin2.Connect(pin1));
                EXPECT_EQ(pin1.GetConnections().size(), size_t(0));
                EXPECT_EQ(pin2.GetConnections().size(), size_t(0));
            }
            {
                InputPin<float> in(node);
                OutputPin<float> out(node);

                const auto& in_const = in;
                const auto& out_const = out;

                EXPECT_EQ(in.GetConnections().size(), size_t(0));
                EXPECT_EQ(out.GetConnections().size(), size_t(0));

                {
                    EXPECT_TRUE(out.Connect(in));

                    EXPECT_EQ(in.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out.GetConnections().size(), size_t(1));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(1));

                    EXPECT_EQ(in.GetConnection(), &out);
                    EXPECT_EQ(out.GetConnection(), &in);
                    EXPECT_EQ(in_const.GetConnection(), &out_const);
                    EXPECT_EQ(out_const.GetConnection(), &in_const);

                    EXPECT_EQ(in.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out.GetConnections().size(), size_t(1));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(1));

                    EXPECT_EQ(in.GetConnections()[0], &out);
                    EXPECT_EQ(out.GetConnections()[0], &in);
                    EXPECT_EQ(in_const.GetConnections()[0], &out_const);
                    EXPECT_EQ(out_const.GetConnections()[0], &in_const);
                }
                {
                    EXPECT_EQ(out.Disconnect(), size_t(1));

                    EXPECT_EQ(in.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out.GetConnections().size(), size_t(0));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(0));

                    EXPECT_EQ(in.GetConnection(), nullptr);
                    EXPECT_EQ(out.GetConnection(), nullptr);
                    EXPECT_EQ(in_const.GetConnection(), nullptr);
                    EXPECT_EQ(out_const.GetConnection(), nullptr);

                    EXPECT_EQ(in.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out.GetConnections().size(), size_t(0));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(0));
                }
                {
                    EXPECT_TRUE(out.Connect(in));

                    EXPECT_EQ(in.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out.GetConnections().size(), size_t(1));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(1));

                    EXPECT_EQ(in.GetConnection(), &out);
                    EXPECT_EQ(out.GetConnection(), &in);
                    EXPECT_EQ(in_const.GetConnection(), &out_const);
                    EXPECT_EQ(out_const.GetConnection(), &in_const);

                    EXPECT_EQ(in.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out.GetConnections().size(), size_t(1));
                    EXPECT_EQ(in_const.GetConnections().size(), size_t(1));
                    EXPECT_EQ(out_const.GetConnections().size(), size_t(1));

                    EXPECT_EQ(in.GetConnections()[0], &out);
                    EXPECT_EQ(out.GetConnections()[0], &in);
                    EXPECT_EQ(in_const.GetConnections()[0], &out_const);
                    EXPECT_EQ(out_const.GetConnections()[0], &in_const);
                }
            }
            {
                InputPin<float> in[3] = { node, node, node };
                OutputPin<float> out(node);

                EXPECT_TRUE(out.Connect(in[0]));
                EXPECT_TRUE(out.Connect(in[1]));
                EXPECT_TRUE(out.Connect(in[2]));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(1));
                EXPECT_EQ(out.GetConnections().size(), size_t(3));
                EXPECT_EQ(out.GetConnections()[0], &in[0]);
                EXPECT_EQ(out.GetConnections()[1], &in[1]);
                EXPECT_EQ(out.GetConnections()[2], &in[2]);

                EXPECT_FALSE(out.Disconnect(4));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(1));
                EXPECT_EQ(out.GetConnections().size(), size_t(3));
                EXPECT_EQ(out.GetConnections()[0], &in[0]);
                EXPECT_EQ(out.GetConnections()[1], &in[1]);
                EXPECT_EQ(out.GetConnections()[2], &in[2]);

                EXPECT_TRUE(out.Disconnect(1));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(1));
                EXPECT_EQ(out.GetConnections().size(), size_t(2));
                EXPECT_EQ(out.GetConnections()[0], &in[0]);
                EXPECT_EQ(out.GetConnections()[1], &in[2]);

                EXPECT_FALSE(out.Disconnect(2));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(1));
                EXPECT_EQ(out.GetConnections().size(), size_t(2));
                EXPECT_EQ(out.GetConnections()[0], &in[0]);
                EXPECT_EQ(out.GetConnections()[1], &in[2]);

                EXPECT_TRUE(out.Disconnect(1));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(0));
                EXPECT_EQ(out.GetConnections().size(), size_t(1));
                EXPECT_EQ(out.GetConnections()[0], &in[0]);

                EXPECT_FALSE(out.Disconnect(1));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(0));
                EXPECT_EQ(out.GetConnections().size(), size_t(1));
                EXPECT_EQ(out.GetConnections()[0], &in[0]);

                EXPECT_TRUE(out.Disconnect(0));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(0));
                EXPECT_EQ(out.GetConnections().size(), size_t(0));
            }
            {
                InputPin<float> in[3] = { node, node, node };
                OutputPin<float> out(node);

                EXPECT_TRUE(out.Connect(in[0]));
                EXPECT_TRUE(out.Connect(in[1]));
                EXPECT_TRUE(out.Connect(in[2]));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(1));
                EXPECT_EQ(out.GetConnections().size(), size_t(3));
                EXPECT_EQ(out.GetConnections()[0], &in[0]);
                EXPECT_EQ(out.GetConnections()[1], &in[1]);
                EXPECT_EQ(out.GetConnections()[2], &in[2]);

                EXPECT_TRUE(out.Disconnect(in[1]));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(1));
                EXPECT_EQ(out.GetConnections().size(), size_t(2));
                EXPECT_EQ(out.GetConnections()[0], &in[0]);
                EXPECT_EQ(out.GetConnections()[1], &in[2]);

                EXPECT_TRUE(out.Disconnect(in[2]));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(1));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(0));
                EXPECT_EQ(out.GetConnections().size(), size_t(1));
                EXPECT_EQ(out.GetConnections()[0], &in[0]);

                EXPECT_TRUE(out.Disconnect(in[0]));
                EXPECT_EQ(in[0].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[1].GetConnections().size(), size_t(0));
                EXPECT_EQ(in[2].GetConnections().size(), size_t(0));
                EXPECT_EQ(out.GetConnections().size(), size_t(0));
            }
            {
                {
                    OutputPin<float> out1(node);
                    OutputPin<float> out2(node);

                    EXPECT_FALSE(out1.Connect(out2));

                    EXPECT_EQ(out1.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out1.GetConnection(), nullptr);
                    EXPECT_EQ(out1.GetConnections().size(), size_t(0));

                    EXPECT_EQ(out2.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out2.GetConnection(), nullptr);
                    EXPECT_EQ(out2.GetConnections().size(), size_t(0));
                }
                {
                    OutputPin<float> out1(node);
                    InputPin<int32_t> in1(node);

                    EXPECT_FALSE(out1.Connect(in1));

                    EXPECT_EQ(out1.GetConnections().size(), size_t(0));
                    EXPECT_EQ(out1.GetConnection(), nullptr);
                    EXPECT_EQ(out1.GetConnections().size(), size_t(0));

                    EXPECT_EQ(in1.GetConnections().size(), size_t(0));
                    EXPECT_EQ(in1.GetConnection(), nullptr);
                    EXPECT_EQ(in1.GetConnections().size(), size_t(0));
                }
            }
        }

    }

}