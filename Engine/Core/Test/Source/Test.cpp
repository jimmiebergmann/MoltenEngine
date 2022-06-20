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
#include <iostream>

namespace Molten::Test
{

    void PrintInfo(const std::string& message)
    {
        std::cout << "\033[0;32m" << "[          ] " << "\033[0;0m";
        std::cout << "\033[0;36m" << message << "\033[0;0m" << std::endl;
    }

    Benchmarker::Benchmarker(const std::string& description) :
        m_description(description),
        m_stopped(false)
    {}

    Benchmarker::~Benchmarker()
    {
        Stop();
    }

    void Benchmarker::Stop()
    {
        if(m_stopped)
        {
            return;
        }

        auto time = m_clock.GetTime();
        auto [convertedTime, convertedUnit] = GetConvertedTime(time);
        PrintInfo("Benchmarked \"" + m_description + "\", took " + std::to_string(convertedTime) + " " + convertedUnit + ".");

        m_stopped = true;
    }

    std::pair<double, std::string> Benchmarker::GetConvertedTime(const Time& time)
    {
        auto ns = time.AsNanoseconds<uint64_t>();
        if (ns >= 1000000000UL)
        {
            return { time.AsSeconds<double>(), "s" };
        }
        else if (ns >= 1000000UL)
        {
            return { time.AsMilliseconds<double>(), "ms" };
        }
        else if (ns >= 1000UL)
        {
            return { time.AsMicroseconds<double>(), "us" };
        }

        return { time.AsNanoseconds<double>(), "ns" };
    }

}