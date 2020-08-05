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

    private:

        Benchmarker(const Benchmarker&) = delete;
        Benchmarker(Benchmarker&&) = delete;
        Benchmarker& operator= (const Benchmarker&) = delete;
        Benchmarker& operator= (Benchmarker&&) = delete;

        /** Get time as double and it's unit. Units are split up by multiples of 1000.*/
        std::pair<double, std::string> GetFittingUnits(const Time& time);

        std::string m_description;
        Molten::Clock m_clock;

    };

}

#endif