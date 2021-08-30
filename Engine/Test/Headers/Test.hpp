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

#ifndef MOLTEN_TEST_HPP
#define MOLTEN_TEST_HPP

#include "ThirdParty/googletest/googletest/include/gtest/gtest.h"

/** Nested tests. */
#define NESTED_TEST_FUNCTION_WITH_MESSAGE(function, message) \
{ \
    SCOPED_TRACE(message); \
    ASSERT_NO_FATAL_FAILURE((function)); \
}
#define NESTED_TEST_FUNCTION(function) NESTED_TEST_FUNCTION_WITH_MESSAGE(function, " <-- Failure occured here.\n")

/** Vector2/3 compares. */
#define EXPECT_VECTOR2_NEAR(lhs, rhs, abs_error) \
    EXPECT_NEAR((lhs).x, (rhs).x, abs_error); \
    EXPECT_NEAR((lhs).y, (rhs).y, abs_error)

#define EXPECT_VECTOR3_NEAR(lhs, rhs, abs_error) \
    EXPECT_NEAR((lhs).x, (rhs).x, abs_error); \
    EXPECT_NEAR((lhs).y, (rhs).y, abs_error); \
    EXPECT_NEAR((lhs).z, (rhs).z, abs_error)


#include "Molten/System/Clock.hpp"
#include <string>

namespace Molten::Test
{

    /** Googletest style info printer. */
    void PrintInfo(const std::string& message);

    /** Googletest style time benchmarker class. This object will print the result at destruction. */
    class Benchmarker
    {

    public:

        explicit Benchmarker(const std::string& description);
        ~Benchmarker();

        void Stop();

    private:

        Benchmarker(const Benchmarker&) = delete;
        Benchmarker(Benchmarker&&) = delete;
        Benchmarker& operator= (const Benchmarker&) = delete;
        Benchmarker& operator= (Benchmarker&&) = delete;

        /** Get time as double and it's unit. Units are split up by multiples of 1000.*/
        std::pair<double, std::string> GetConvertedTime(const Time& time);

        std::string m_description;
        Clock m_clock;
        bool m_stopped;

    };

}

#endif