/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_SYSTEM_SEMAPHORE_HPP
#define MOLTEN_CORE_SYSTEM_SEMAPHORE_HPP

#include "Molten/System/Time.hpp"
#include <mutex>
#include <condition_variable>

namespace Molten
{

    /** Thread safe sempahore class.
    *   Providing a lockable object,
    *   making it possible to block the current thread, until being notified and unblocked.
    */
    class MOLTEN_CORE_API Semaphore
    {

    public:

        /** Initializes semaphore by value. Default value is 0.
         * Negative number will cause the Wait() to block current thread until NotifyOne() is called (std::abs(value) - 1) times, or if NotifyAll is called.
         * Positive number will cause Wait() to not block the current thread for (value - 1) times called.
         */
        explicit Semaphore(int32_t value = 0);

        /** Default destructor. */
        ~Semaphore() = default;

        /** Deleted copy and move constructors/operators. */
        /**@{*/
        Semaphore(const Semaphore&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator = (const Semaphore&) = delete;
        Semaphore& operator = (Semaphore&&) = delete;
        /**@}*/

        /** Get number of blocked threads by Wait() or WaitFor(). */
        [[nodiscard]] size_t GetWaitCount() const;

        /** Unblocks all threads being blocked by a call to Wait() or TryWait(). */
        void NotifyAll();

        /** Unblocks any waiting thread.
         *   Increments value of semaphore.
         */
        void NotifyOne();

        /** lock current thread. */
        void Wait();

        /** Block current thread by some maximum amount of time. */
        void WaitFor(const Time & time);

    private:

        int32_t m_value;
        mutable std::mutex m_mutex;
        std::condition_variable m_condition;
        size_t m_waitCount;

    };

}

#endif