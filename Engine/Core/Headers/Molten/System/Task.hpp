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

#ifndef MOLTEN_CORE_SYSTEM_TASK_HPP
#define MOLTEN_CORE_SYSTEM_TASK_HPP

#include "Molten/System/ThreadPool.hpp"
#include "Molten/System/Time.hpp"
#include <variant>
#include <vector>

namespace Molten
{

    /** Forward declarations. */
    class Task;
    class ParallelTaskGroup;

    using TaskSharedPointer = std::shared_ptr<Task>;
    using ParallelTaskGroupSharedPointer = std::shared_ptr<ParallelTaskGroup>;


    /** Task object with execution time tracking. */
    class MOLTEN_API Task
    {

    public:

        using Function = std::function<void()>;

        /** Task construction by providing an execution function. */
        explicit Task(Function&& function);

        /** Task construction by providing an execution function and name. */
        Task(
            Function&& function, 
            std::string&& name);

        /** Default destructor. */
        ~Task() = default;

        /** Deleted copy and move constructors/operators. */
        /**@{*/
        Task(const Task&) = delete;
        Task(Task&&) = delete;
        Task& operator = (const Task&) = delete;
        Task& operator = (Task&&) = delete;
        /**@}*/

        /** Execute task function. */
        void operator ()();

        /** Gets last execution time. */
        [[nodiscard]] Time GetLastExecutionTime() const;

    private:

        Function m_function;
        std::string m_name;
        Time m_executionTime;

    };


    /** Parallel task group, with an interface to execute tasks in parallel. */
    class MOLTEN_API ParallelTaskGroup
    {

    public:

        using Tasks = std::vector<TaskSharedPointer>;

        /** Constructor.  */
        explicit ParallelTaskGroup(ThreadPool& threadPool);

        /** Destructor. Current thread is blocked until execution is complete. */
        ~ParallelTaskGroup();

        /** Deleted copy and move constructors/operators. */
        /**@{*/
        ParallelTaskGroup(const ParallelTaskGroup&) = delete;
        ParallelTaskGroup(ParallelTaskGroup&&) = delete;
        ParallelTaskGroup& operator = (const ParallelTaskGroup&) = delete;
        ParallelTaskGroup& operator = (ParallelTaskGroup&&) = delete;
        /**@}*/

        /** Schedules and execute all tasks in parallel.
         *  Tasks are ordered by last execution time in descending order.
         *  This function is a modal function, causing the current thread to pause until all tasks are complete.
         */
        void Execute();

        /** Gets iterator to beginning of task list. */
        /**@{*/
        [[nodiscard]] Tasks::iterator begin();
        [[nodiscard]] Tasks::const_iterator begin() const;
        /**@}*/

        /** Gets iterator to ending of task list. */
        /**@{*/
        [[nodiscard]] Tasks::iterator end();
        [[nodiscard]] Tasks::const_iterator end() const;
        /**@}*/

        /** Constructing a new entry in place. Supports tasks as TEntryType. */
        template<typename TEntryType, typename ... TTaskArgs>
        std::shared_ptr<TEntryType> Emplace(TTaskArgs ... taskArgs);

    private:

        ThreadPool& m_threadPool;
        std::mutex m_executeMutex;
        Tasks m_tasks;

    };

    /** Serial task group, with an interface to execute tasks or parallel task groups in serial. */
    class MOLTEN_API SerialTaskGroup
    {

    public:

        using Entry = std::variant<TaskSharedPointer, ParallelTaskGroupSharedPointer>;
        using Entries = std::vector<Entry>;

        /** Constructor.  */
        explicit SerialTaskGroup(ThreadPool& threadPool);

        /** Default destructor. */
        ~SerialTaskGroup() = default;

        /** Deleted copy and move constructors/operators. */
        /**@{*/
        SerialTaskGroup(const SerialTaskGroup&) = delete;
        SerialTaskGroup(SerialTaskGroup&&) = delete;
        SerialTaskGroup& operator = (const SerialTaskGroup&) = delete;
        SerialTaskGroup& operator = (SerialTaskGroup&&) = delete;
        /**@}*/

        /** Schedules and execute all entries in serial.
         *  This function is a modal function, causing the current thread to pause until all tasks are complete.
         */
        void Execute();

        /** Gets iterator to beginning of entry list. */
        /**@{*/
        [[nodiscard]] Entries::iterator begin();
        [[nodiscard]] Entries::const_iterator begin() const;
        /**@}*/

        /** Gets iterator to ending of entry list. */
        /**@{*/
        [[nodiscard]] Entries::iterator end();
        [[nodiscard]] Entries::const_iterator end() const;
        /**@}*/

        /** Constructing a new entry in place at front. Supports tasks and parallel task groups as TEntryType. */
        template<typename TEntryType, typename ... TTaskArgs>
        std::shared_ptr<TEntryType> EmplaceFront(TTaskArgs ... taskArgs);

        /** Constructing a new entry in place at back. Supports tasks and parallel task groups as TEntryType. */
        template<typename TEntryType, typename ... TTaskArgs>
        std::shared_ptr<TEntryType> EmplaceBack(TTaskArgs ... taskArgs);

        /** Constructing a new entry in place at iterator position. Supports tasks and parallel task groups as TEntryType. */
        template<typename TEntryType, typename ... TTaskArgs>
        std::shared_ptr<TEntryType> Emplace(Entries::const_iterator it, TTaskArgs ... taskArgs);

    private:

        ThreadPool& m_threadPool;
        Entries m_entries;

    };

}

#include "Molten/System/Task.inl"

#endif