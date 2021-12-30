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

#include "Molten/System/ThreadPool.hpp"

namespace Molten
{

    // Global implementations.
    static size_t CalculateWorkerCount(
        const size_t threadCount,
        const size_t minThreadCount,
        const size_t reservedThreads)
    {
        size_t count = threadCount;
        if (count == 0)
        {
            count = std::thread::hardware_concurrency();
        }

        if (reservedThreads >= count)
        {
            return minThreadCount;
        }

        return std::max(count - reservedThreads, minThreadCount);
    }


    // Thread pool implementations.
    ThreadPool::ThreadPool(
        const size_t threadCount,
        const size_t minThreadCount,
        const size_t reservedThreads)
    {
        const auto workerCount = CalculateWorkerCount(threadCount, minThreadCount, reservedThreads);

        m_workers.reserve(workerCount);
        m_freeWorkers.reserve(workerCount);

        for (size_t i = 0; i < workerCount; i++)
        {
            auto worker = std::make_unique<Worker>();

            auto freeWorkerFunction = [this, worker = worker.get()]()
            {
                std::lock_guard workerLock(m_workerMutex);
                m_freeWorkers.push_back(worker);
                m_freeWorkersSemaphore.NotifyOne();
            };

            worker->Start(freeWorkerFunction);
            m_workers.push_back(std::move(worker));
        }
    }

    ThreadPool::~ThreadPool()
    {
        for(auto& worker : m_workers)
        {
            worker->Stop();
        }

        for (auto& worker : m_workers)
        {
            worker.reset();
        }
    }

    size_t ThreadPool::GetWorkerCount() const
    {
        return m_workers.size();
    }

    ThreadPool::Worker* ThreadPool::GetFreeWorker()
    {
        m_freeWorkersSemaphore.Wait();

        std::lock_guard workerLock(m_workerMutex);

        auto* worker = m_freeWorkers.back();
        m_freeWorkers.pop_back();

        return worker;
    }

    ThreadPool::Worker* ThreadPool::TryGetFreeWorker()
    {
        std::lock_guard workerLock(m_workerMutex);

        if(m_freeWorkers.empty())
        {
            return nullptr;
        }

        auto* worker = m_freeWorkers.back();
        m_freeWorkers.pop_back();

        return worker;
    }


    // Thread pool worker implementations.
    ThreadPool::Worker::Worker() :
        m_running(false)
    {}

    void ThreadPool::Worker::Start(std::function<void()>&& freeWorkerFunction)
    {
        m_freeFunction = std::move(freeWorkerFunction);
        m_running = true;

        m_thread = std::thread([this]()
        {
            m_freeFunction();

            while(m_running)
            {
                m_workSemaphore.Wait();
                if(!m_running)
                {
                    return;
                }

                m_function();
                m_freeFunction();
            }
        });
    }

    void ThreadPool::Worker::Stop()
    {
        m_running = false;
        m_workSemaphore.NotifyOne();
    }

    ThreadPool::Worker::~Worker()
    {
        if(m_thread.joinable())
        {
            m_thread.join();
        }
    }

}