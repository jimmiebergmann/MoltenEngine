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

namespace Molten
{
    
    // Thread pool implementations.
    template<typename TInvocable, typename TReturn, typename>
    std::future<TReturn> ThreadPool::Execute(TInvocable&& invocable)
    {
        auto worker = GetFreeWorker();
        return worker->Execute<TReturn>(std::move(invocable));
    }

    template<typename TInvocable, typename>
    void ThreadPool::ExecuteDiscard(TInvocable&& invocable)
    {
        auto worker = GetFreeWorker();
        return worker->ExecuteDiscard(std::move(invocable));
    }


    // Thread pool worker implementations.
    template<typename TReturn>
    std::future<TReturn> ThreadPool::Worker::Execute(std::function<TReturn()>&& function)
    {
        auto promise = std::make_shared<std::promise<TReturn>>();
        auto future = promise->get_future();

        m_function = [function = std::move(function), promise]()
        {   
            try
            {
                if constexpr (std::is_same_v<TReturn, void> == true)
                {
                    function();
                    promise->set_value();
                }
                else
                {
                    promise->set_value(function());
                }
            }
            catch(...)
            {
                promise->set_exception(std::current_exception());
            }
        };

        m_workSemaphore.NotifyOne();

        return future;
    }

    inline void ThreadPool::Worker::ExecuteDiscard(std::function<void()>&& function)
    {
        m_function = [function = std::move(function)]()
        {
            try
            {
                function();
            }
            catch (...)
            {
                
            }
        };

        m_workSemaphore.NotifyOne();
    }

}