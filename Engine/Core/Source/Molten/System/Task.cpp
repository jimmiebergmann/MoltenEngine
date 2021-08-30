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
   
        auto& futures = CreateFutures(m_tasks.size());

        // Execute threads in parallel.
        for (size_t i = 0; i < m_tasks.size(); i++)
        {
            auto& task = m_tasks[i];
            *futures[i] = m_threadPool.Execute([&task]()
            {
                (*task)();
            });
        }

        // Wait for all task to finish. Call get() in order to check for exceptions.
        for(size_t i = 0; i < m_tasks.size(); i++)
        {
            futures[i]->get();
        }
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

    ParallelTaskGroup::Futures& ParallelTaskGroup::CreateFutures(const size_t count)
    {
        const auto currentCount = m_futures.size();

        // Current future count is bigger or equal to requested count. Just clear available futures.
        if(const auto countDiff = static_cast<int64_t>(count) - static_cast<int64_t>(currentCount); countDiff <= 0)
        {
            for (auto it = m_futures.begin(); it != m_futures.begin() + count; ++it)
            {
                **it = {};
            }
            return m_futures;
        }

        // Requested futures is more than current future count. Clear already existed futures and initialize new ones.
        m_futures.resize(count);
        for (auto it = m_futures.begin(); it != m_futures.begin() + currentCount; ++it)
        {
            **it = {};
        }

        for(auto it = m_futures.begin() + currentCount; it != m_futures.end(); ++it)
        {
            *it = std::make_unique<std::future<void>>();
        }

        return m_futures;
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