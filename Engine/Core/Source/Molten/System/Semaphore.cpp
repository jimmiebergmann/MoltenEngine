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

#include "Molten/System/Semaphore.hpp"

namespace Molten
{

    Semaphore::Semaphore() :
        m_value(0),
        m_waitCount(0)
    {
    }

    size_t Semaphore::GetWaitCount() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_waitCount;
    }

    void Semaphore::NotifyAll()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_value += static_cast<int32_t>(m_waitCount);
        m_condition.notify_all();
    }

    void Semaphore::NotifyOne()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        ++m_value;
        m_condition.notify_one();
    }

    void Semaphore::Wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        ++m_waitCount;
        while (!m_value)
        {
            m_condition.wait(lock);
        }
        --m_waitCount;
        --m_value;
    }

    void Semaphore::WaitFor(const Time& time)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        ++m_waitCount;
        while (!m_value)
        {
            auto waitTime = std::chrono::nanoseconds(time.AsNanoseconds<std::chrono::nanoseconds::rep>());
            if (m_condition.wait_for(lock, waitTime) == std::cv_status::timeout)
            {
                break;
            }
        }
        --m_waitCount;
        --m_value;
    }

}
