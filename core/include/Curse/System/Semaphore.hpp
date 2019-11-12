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

#include "Curse/Types.hpp"
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace Curse
{

    /**
    * @brief Sempahore class.
    *        Providing a lockable object,
    *        making it possible to block the current thread, until being notified and unblocked.
    *
    * Calling Wait() or TryWait() on multiple threads is undefined behaviour.
    */
    class CURSE_API Semaphore
    {

    public:

        /**
        * @brief Constructor.
        *        Initialized as unblocked.
        */
        Semaphore();

        /**
        * @brief Destructor of semaphore.
        *        Thread is being unblocked at destruction.
        */
        ~Semaphore();

        /**
        * @brief Block current thread.
        */
        void Wait();

        /**
        * @brief Block current thread by given maximum duration.
        */
        void WaitFor(const std::chrono::duration<float> & duration);

        /**
        * @brief Unblocks all threads being blocked by a call to Wait() or TryWait().
        */
        void NotifyAll();

        /**
        * @brief Unblocks any waiting thread.
        */
        void NotifyOne();

    private:

        int32_t m_value;
        std::mutex m_mutex;
        std::condition_variable m_condition;

    };

}

