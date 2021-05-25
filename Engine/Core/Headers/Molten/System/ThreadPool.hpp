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

#ifndef MOLTEN_CORE_SYSTEM_THREADPOOL_HPP
#define MOLTEN_CORE_SYSTEM_THREADPOOL_HPP

#include "Molten/System/Semaphore.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <future>
#include <atomic>
#include <exception>
#include <type_traits>

namespace Molten
{

    /** Thread pool class, with an interface for executing functions without having to care about individual thread,
     *  with support for future results of any movable type.
     *  All threads are launched at construction and stopped/destroyed at pool destruction.
     */
    class MOLTEN_API ThreadPool
    {

    public:

        /** Constructor.
        *  Launching maximum number of possible concurrent threads if threadCount == 0.
        *  Provided reservedThreads is subtracted from number of launched threads,
        *  this is useful if other threads are running in the same application and you want to run maximum number of threads - some reserved ones.
        *  Number of launched threads is always at least 1.
        */
        explicit ThreadPool(
            const size_t threadCount = 0,
            const size_t reservedThreads = 0);

        /** Destructor. All workers are stopped. */
        ~ThreadPool();

        /** Deleted copy/move constructors/operators. */
        /**@{*/
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator = (const ThreadPool&) = delete;
        ThreadPool& operator = (ThreadPool&&) = delete;
        /**@}*/

        /** Get number of launched workers. */
        [[nodiscard]] size_t GetWorkerCount() const;

        /** Blocks current thread until a worker is free and ready for work, and then executes provided invocable type on that worker's thread.
         *  @return A future value of provided invocable types return type.
         *  @example std::future<int> value = pool.Execute([](){ return 10; });
         */
        template<typename TInvocable, typename TReturn = std::decay_t<std::invoke_result_t<TInvocable>>, typename = std::enable_if_t<std::is_invocable_v<TInvocable>>>
        [[nodiscard]] std::future<TReturn> Execute(TInvocable&& invocable);

        /** Blocks current thread until a worker is free and ready for work, and then executes provided invocable type on that worker's thread.
         *  This function is faster than Execute, but lacks the possibility to get a return value from invocable object and check for execution completion.
         */
        template<typename TInvocable, typename = std::enable_if_t<std::is_invocable_v<TInvocable>>>
        void ExecuteDiscard(TInvocable&& invocable);

    private:

        class Worker
        {

        public:

            Worker();
            ~Worker();

            Worker(const Worker&) = delete;
            Worker(Worker&&) = delete;
            Worker& operator = (const Worker&) = delete;
            Worker& operator = (Worker&&) = delete;

            void Start(std::function<void()>&& freeWorkerFunction);
            void Stop();
            
            template<typename TReturn>
            [[nodiscard]] std::future<TReturn> Execute(std::function<TReturn()>&& function);

            void ExecuteDiscard(std::function<void()>&& function);

        private:

            std::atomic_bool m_running;
            std::function<void()> m_freeFunction;
            std::thread m_thread;
            std::function<void()> m_function;
            Semaphore m_workSemaphore;
            
        };

        using WorkerPointer = std::unique_ptr<Worker>;

        Worker* GetFreeWorker();

        std::vector<WorkerPointer> m_workers;
        std::vector<Worker*> m_freeWorkers;
        Semaphore m_freeWorkersSemaphore;
        std::mutex m_workerMutex;

    };

}

#include "Molten/System/ThreadPool.inl"

#endif