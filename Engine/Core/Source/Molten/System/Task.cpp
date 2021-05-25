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

#include "Molten/System/Task.hpp"
#include "Molten/System/Clock.hpp"
#include <algorithm>

namespace Molten
{

    // Task implementations.
    Task::Task(Function&& function) :
        m_function(std::move(function))
    {}

    Task::Task(
        Function&& function,
        std::string&& name
    ) :
        m_function(std::move(function)),
        m_name(std::move(name))
    {}


    void Task::operator ()()
    {
        const Clock clock;
        m_function();
        m_executionTime = clock.GetTime();
    }

    Time Task::GetLastExecutionTime() const
    {
        return m_executionTime;
    }


    // Parallel task group implementations.
    ParallelTaskGroup::ParallelTaskGroup(ThreadPool& threadPool) :
        m_threadPool(threadPool)
    {}

    ParallelTaskGroup::~ParallelTaskGroup()
    {
        std::lock_guard executeGuard(m_executeMutex);
    }

    void ParallelTaskGroup::Execute()
    {
        std::lock_guard executeGuard(m_executeMutex);

        if(m_tasks.empty())
        {
            return;
        }

        // Order tasks by last execution time in descending order.
        std::sort(m_tasks.begin(), m_tasks.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs->GetLastExecutionTime() > rhs->GetLastExecutionTime();
        });

        // Execute threads in parallel.
        auto counterSemaphore = std::make_shared<Semaphore>(static_cast<uint32_t>(m_tasks.size() - 1));

        for (auto& task : m_tasks)
        {
            m_threadPool.ExecuteDiscard([task, counterSemaphore]()
            {
                (*task)();
                counterSemaphore->NotifyOne();
            });
        }

        // Wait for all tasks to finish.
        counterSemaphore->Wait();
    }

    ParallelTaskGroup::Tasks::iterator ParallelTaskGroup::begin()
    {
        return m_tasks.begin();
    }
    ParallelTaskGroup::Tasks::const_iterator ParallelTaskGroup::begin() const
    {
        return m_tasks.begin();
    }
 
    ParallelTaskGroup::Tasks::iterator ParallelTaskGroup::end()
    {
        return m_tasks.end();
    }
    ParallelTaskGroup::Tasks::const_iterator ParallelTaskGroup::end() const
    {
        return m_tasks.end();
    }


    // Serial task group implementations.
    SerialTaskGroup::SerialTaskGroup(ThreadPool& threadPool) :
        m_threadPool(threadPool)
    {}

    void SerialTaskGroup::Execute()
    {
        for(auto& entry : m_entries)
        {
            std::visit([&](auto variant)
            {
                using VariantType = std::decay_t<decltype(variant)>;
                if constexpr (std::is_same_v<VariantType, TaskSharedPointer> == true)
                {
                    (*variant)();
                }
                else
                {
                    variant->Execute();
                }
            }, entry);
        }
    }

    SerialTaskGroup::Entries::iterator SerialTaskGroup::begin()
    {
        return m_entries.begin();
    }
    SerialTaskGroup::Entries::const_iterator SerialTaskGroup::begin() const
    {
        return m_entries.begin();
    }

    SerialTaskGroup::Entries::iterator SerialTaskGroup::end()
    {
        return m_entries.end();
    }

    SerialTaskGroup::Entries::const_iterator SerialTaskGroup::end() const
    {
        return m_entries.end();
    }

}