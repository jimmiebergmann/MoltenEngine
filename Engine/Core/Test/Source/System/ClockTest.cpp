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
#include "Molten/System/Clock.hpp"
#include <thread>

namespace Molten
{
    TEST(System, Clock)
    {
        Clock clock;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto time = clock.GetTime();
        EXPECT_GE(time, Milliseconds(99));
        EXPECT_LE(time, Milliseconds(1000));
        
        clock.Reset(Milliseconds(1000));

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        time = clock.GetTime();
        EXPECT_GE(time, Milliseconds(1198));
        EXPECT_LE(time, Milliseconds(2000));
    }

}
