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

#include "Molten/Utility/FpsTracker.hpp"
#include <algorithm>

namespace Molten
{

    FpsTracker::FpsTracker(const size_t averageSampleCount) :
        m_minFrameTime(Time::Infinite),
        m_maxFrameTime(Time::Zero),
        m_registeredFrames(0),
        m_currentSample(0),
        m_frameSamples(std::max(size_t{ 1 }, averageSampleCount), Time::Zero)
    {}

    void FpsTracker::ResetFrameSamples()
    {
        m_minFrameTime = Time::Infinite;
        m_maxFrameTime = Time::Zero;
        m_registeredFrames = 0;
        m_currentSample = 0;
        for(auto& frameSample : m_frameSamples)
        {
            frameSample = Time::Zero;
        }
    }

    void FpsTracker::RegisterSampleFrame(const Time frameTime)
    {
        if (frameTime < m_minFrameTime)
        {
            m_minFrameTime = frameTime;
        }
        if (frameTime > m_maxFrameTime)
        {
            m_maxFrameTime = frameTime;
        }

        ++m_registeredFrames;

        m_frameSamples[m_currentSample] = frameTime;

        ++m_currentSample;
        if(m_currentSample >= m_frameSamples.size())
        {
            m_currentSample = 0;
        }
    }

    Time FpsTracker::GetMinFrameTime() const
    {
        return m_minFrameTime;
    }

    Time FpsTracker::GetMaxFrameTime() const
    {
        return m_maxFrameTime;
    }

    Time FpsTracker::GetAverageFrameTime() const
    {
        const auto usedSampled = std::min(m_registeredFrames, m_frameSamples.size());

        if(usedSampled == 0)
        {
            return Time::Zero;
        }

        Time totalSampleTime = Time::Zero;
        for(size_t i = 0; i < usedSampled; i++)
        {
            totalSampleTime += m_frameSamples[i];
        }



        return totalSampleTime / usedSampled;
    }


}