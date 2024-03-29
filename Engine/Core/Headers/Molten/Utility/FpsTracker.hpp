/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_UTILITY_FPSTRACKER_HPP
#define MOLTEN_CORE_UTILITY_FPSTRACKER_HPP

#include "Molten/System/Time.hpp"
#include <vector>

namespace Molten
{
    /** Utility class for tracking fps over time. */
    class MOLTEN_API FpsTracker
    {

    public:

        explicit FpsTracker(const size_t averageSampleCount);

        void ResetFrameSamples();
        void RegisterSampleFrame(const Time frameTime);

        [[nodiscard]] Time GetMinFrameTime() const;
        [[nodiscard]] Time GetMaxFrameTime() const;
        [[nodiscard]] Time GetAverageFrameTime() const;

    private:

        Time m_minFrameTime;
        Time m_maxFrameTime;
        size_t m_registeredFrames;
        size_t m_currentSample;
        std::vector<Time> m_frameSamples;

    };

}

#endif
