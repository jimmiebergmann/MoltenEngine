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
#include "Molten/System/Signal.hpp"

namespace Molten
{
    TEST(System, Signal_Connect)
    {
        {
            Signal<int32_t> sig;

            int32_t signaledValue = 0;
            EXPECT_EQ(sig.GetConnectionCount(), size_t(0));
            sig.Connect([&signaledValue](int32_t value)
                {
                    signaledValue = value;
                });
            EXPECT_EQ(sig.GetConnectionCount(), size_t(1));

            EXPECT_EQ(signaledValue, int32_t(0));
            sig(123);
            EXPECT_EQ(signaledValue, int32_t(123));
            sig(456);
            EXPECT_EQ(signaledValue, int32_t(456));

            EXPECT_EQ(sig.GetConnectionCount(), size_t(1));
        }
        {
            Signal<int32_t> sig;

            int32_t signaledValue_1 = 0;
            int32_t signaledValue_2 = 0;

            EXPECT_EQ(sig.GetConnectionCount(), size_t(0));
            sig.Connect([&signaledValue_1](int32_t value)
                {
                    signaledValue_1 = value;
                });
            EXPECT_EQ(sig.GetConnectionCount(), size_t(1));
            sig.Connect([&signaledValue_2](int32_t value)
                {
                    signaledValue_2 = value;
                });
            EXPECT_EQ(sig.GetConnectionCount(), size_t(2));

            EXPECT_EQ(signaledValue_1, int32_t(0));
            EXPECT_EQ(signaledValue_2, int32_t(0));
            sig(123);
            EXPECT_EQ(signaledValue_1, int32_t(123));
            EXPECT_EQ(signaledValue_2, int32_t(123));
            sig(456);
            EXPECT_EQ(signaledValue_1, int32_t(456));
            EXPECT_EQ(signaledValue_2, int32_t(456));

            EXPECT_EQ(sig.GetConnectionCount(), size_t(2));
        }       
    }

    TEST(System, Signal_Ref_Param)
    {
        Signal<int32_t &> sig;

        EXPECT_EQ(sig.GetConnectionCount(), size_t(0));
        sig.Connect([](int32_t& value)
            {
                value = 234;
            });
        EXPECT_EQ(sig.GetConnectionCount(), size_t(1));

        int32_t callbackValue = 0;
        EXPECT_EQ(callbackValue, int32_t(0));
        sig(callbackValue);
        EXPECT_EQ(callbackValue, int32_t(234));
        callbackValue = 0;
        EXPECT_EQ(callbackValue, int32_t(0));
        sig(callbackValue);
        EXPECT_EQ(callbackValue, int32_t(234));

        EXPECT_EQ(sig.GetConnectionCount(), size_t(1));
    }

    TEST(System, Signal_Disconnect)
    {
        {
            Signal<int32_t&> sig;

            int32_t signaledValue = 0;
            EXPECT_EQ(sig.GetConnectionCount(), size_t(0));
            auto conn = sig.Connect([](int32_t& value)
                {
                    value = 234;
                });
            EXPECT_EQ(sig.GetConnectionCount(), size_t(1));

            int32_t callbackValue = 0;
            EXPECT_EQ(signaledValue, int32_t(0));
            sig(callbackValue);
            EXPECT_EQ(callbackValue, int32_t(234));

            EXPECT_EQ(sig.GetConnectionCount(), size_t(1));

            conn.Disconnect();
            EXPECT_EQ(sig.GetConnectionCount(), size_t(0));

            callbackValue = 0;
            sig(callbackValue);
            EXPECT_EQ(signaledValue, int32_t(0));

            conn.Disconnect();
            EXPECT_EQ(sig.GetConnectionCount(), size_t(0));
        }
        {
            Signal<int32_t&> sig;

            int32_t signaledValue = 0;
            EXPECT_EQ(sig.GetConnectionCount(), size_t(0));
            auto conn = sig.Connect([](int32_t& value)
                {
                    value = 234;
                });
            EXPECT_EQ(sig.GetConnectionCount(), size_t(1));

            int32_t callbackValue = 0;
            EXPECT_EQ(signaledValue, int32_t(0));
            sig(callbackValue);
            EXPECT_EQ(callbackValue, int32_t(234));

            EXPECT_EQ(sig.GetConnectionCount(), size_t(1));

            sig.Disconnect(conn);
            EXPECT_EQ(sig.GetConnectionCount(), size_t(0));

            callbackValue = 0;
            sig(callbackValue);
            EXPECT_EQ(signaledValue, int32_t(0));

            sig.Disconnect(conn);
            EXPECT_EQ(sig.GetConnectionCount(), size_t(0));
        }
    }

    TEST(System, Signal_DisconnectAll)
    {
        Signal<int32_t> sig;

        int32_t signaledValue_1 = 0;
        int32_t signaledValue_2 = 0;

        EXPECT_EQ(sig.GetConnectionCount(), size_t(0));
        Signal<int32_t>::Connection conn1;
        conn1 = sig.Connect([&signaledValue_1](int32_t value)
            {
                signaledValue_1 = value;
            });
        EXPECT_EQ(sig.GetConnectionCount(), size_t(1));
        auto conn2 = sig.Connect([&signaledValue_2](int32_t value)
            {
                signaledValue_2 = value;
            });
        EXPECT_EQ(sig.GetConnectionCount(), size_t(2));

        EXPECT_EQ(signaledValue_1, int32_t(0));
        EXPECT_EQ(signaledValue_2, int32_t(0));
        sig(123);
        EXPECT_EQ(signaledValue_1, int32_t(123));
        EXPECT_EQ(signaledValue_2, int32_t(123));
        sig(456);
        EXPECT_EQ(signaledValue_1, int32_t(456));
        EXPECT_EQ(signaledValue_2, int32_t(456));

        EXPECT_EQ(sig.GetConnectionCount(), size_t(2));

        sig.DisconnectAll();
        EXPECT_EQ(sig.GetConnectionCount(), size_t(0));

        conn1.Disconnect();
        conn2.Disconnect();

        EXPECT_EQ(sig.GetConnectionCount(), size_t(0));
    }

    TEST(System, DispatchSignal)
    {
        {
            SignalDispatcher dispatcher;

            bool value = false;

            DispatchSignal<> s1(dispatcher);
            s1.Connect([&]() { value = true;  });

            EXPECT_EQ(value, false);

            s1();

            EXPECT_EQ(value, false);

            dispatcher.Execute();

            EXPECT_EQ(value, true);
        }
        {
            SignalDispatcher dispatcher;

            int32_t ints[2] = { 0, 0 };
            float floats[2] = { 0.0f, 0.0f};

            DispatchSignal<int32_t, float> s1(dispatcher);
            s1.Connect([&](const int32_t v1, const float v2)
            {
                ints[0] = v1; floats[0] = v2;
            });
            s1.Connect([&](const int32_t v1, const float v2)
            {
                ints[1] = v1; floats[1] = v2;
            });

            EXPECT_EQ(ints[0], int32_t{ 0 });
            EXPECT_EQ(ints[1], int32_t{ 0 });
            EXPECT_EQ(floats[0], float{ 0.0f });
            EXPECT_EQ(floats[1], float{ 0.0f });

            s1(123, 1.0f);

            EXPECT_EQ(ints[0], int32_t{ 0 });
            EXPECT_EQ(ints[1], int32_t{ 0 });
            EXPECT_EQ(floats[0], float{ 0.0f });
            EXPECT_EQ(floats[1], float{ 0.0f });

            dispatcher.Execute();

            EXPECT_EQ(ints[0], int32_t{ 123 });
            EXPECT_EQ(ints[1], int32_t{ 123 });
            EXPECT_EQ(floats[0], float{ 1.0f });
            EXPECT_EQ(floats[1], float{ 1.0f });

            s1(1234, 2.0f);
            s1(1236, 4.0f);

            EXPECT_EQ(ints[0], int32_t{ 123 });
            EXPECT_EQ(ints[1], int32_t{ 123 });
            EXPECT_EQ(floats[0], float{ 1.0f });
            EXPECT_EQ(floats[1], float{ 1.0f });

            dispatcher.Execute();

            EXPECT_EQ(ints[0], int32_t{ 1236 });
            EXPECT_EQ(ints[1], int32_t{ 1236 });
            EXPECT_EQ(floats[0], float{ 4.0f });
            EXPECT_EQ(floats[1], float{ 4.0f });
        }
    }

}
