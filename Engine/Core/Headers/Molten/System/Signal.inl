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


namespace Molten
{

    // Connection implementations.
    template<typename ... Args>
    Signal<Args...>::Connection::Connection() :
        m_signal(nullptr),
        m_controller(nullptr)
    { }

    template<typename ... Args>
    Signal<Args...>::Connection::Connection(const Connection& connection) :
        m_signal(connection.m_signal),
        m_controller(connection.m_controller)
    { }

    template<typename ... Args>
    Signal<Args...>::Connection::Connection(Connection&& connection) noexcept :
        m_signal(connection.m_signal),
        m_controller(connection.m_controller)
    {
        connection.m_signal = nullptr;
        connection.m_controller = nullptr;
    }

    template<typename ... Args>
    typename Signal<Args...>::Connection& Signal<Args...>::Connection::operator =(const Connection& connection)
    {
        m_signal = connection.m_signal;
        m_controller = connection.m_controller;
        return *this;
    }

    template<typename ... Args>
    typename Signal<Args...>::Connection& Signal<Args...>::Connection::operator =(Connection&& connection) noexcept
    {
        m_signal = connection.m_signal;
        m_controller = connection.m_controller;
        connection.m_signal = nullptr;
        connection.m_controller = nullptr;
        return *this;
    }

    template<typename ... Args>
    void Signal<Args...>::Connection::Disconnect()
    {
        if (!m_signal)
        {
            return;
        }

        m_signal->Disconnect(*this);
    }


    // Signal implementations.
    template<typename ... Args>
    Signal<Args...>::~Signal()
    {
        DisconnectAll();
    }

    template<typename ... Args>
    typename Signal<Args...>::Connection Signal<Args...>::Connect(const Callback& callback)
    {
        Connection connection;
        connection.m_signal = this;
        connection.m_controller = new typename Connection::Controller;

        m_connections.insert({ connection.m_controller, callback });

        return connection;
    }

    template<typename ... Args>
    void Signal<Args...>::Disconnect(Connection& connection)
    {
        if (!connection.m_controller)
        {
            return;
        }

        auto it = m_connections.find(connection.m_controller);
        if (it == m_connections.end())
        {
            return;
        }

        delete it->first;
        m_connections.erase(it);
    }

    template<typename ... Args>
    void Signal<Args...>::DisconnectAll()
    {
        for (auto& connection : m_connections)
        {
            delete connection.first;
        }
        m_connections.clear();
    }

    template<typename ... Args>
    size_t Signal<Args...>::GetConnectionCount() const
    {
        return m_connections.size();
    }

    template<typename ... Args>
    void Signal<Args...>::operator ()(Args ... args) const
    {
        for (auto& connection : m_connections)
        {
            connection.second(std::forward<Args...>(args...));
        }
    }

    template<>
    inline void Signal<>::operator ()() const
    {
        for (auto& connection : m_connections)
        {
            connection.second();
        }
    }


    // Signal dispatcher implementations.
    inline void SignalDispatcher::Execute()
    {
        for(auto* queuedSignal : m_queuedSignals)
        {
            for(auto& signaledCallback : queuedSignal->m_signaledCallbacks)
            {
                signaledCallback();
            }

            queuedSignal->m_signaledCallbacks.clear();
        }

        m_queuedSignals.clear();
    }

    inline void SignalDispatcher::QueueSignal(DispatchSignalBase& dispatchSignalBase)
    {
        m_queuedSignals.push_back(&dispatchSignalBase);
    }


    // Dispatch signal implementations.
    template<typename ... Args>
    DispatchSignal<Args...>::DispatchSignal(SignalDispatcher& dispatcher) :
        m_dispatcher(dispatcher)
    {}

    template<typename ... Args>
    void DispatchSignal<Args...>::Connect(const Callback& callback)
    {
        m_callbacks.push_back(callback);
    }

    template<typename ... Args>
    void DispatchSignal<Args...>::Connect(Callback&& callback)
    {
        m_callbacks.push_back(std::move(callback));
    }

    template<typename ... Args>
    void DispatchSignal<Args...>::operator ()(Args&& ... args)
    {
        m_signaledCallbacks.push_back([&, tupleArgs = std::make_tuple(std::forward<Args>(args)...)]()
        {
            for (auto& callback : m_callbacks)
            {
                std::apply(callback, tupleArgs);
            }
        });

        m_dispatcher.QueueSignal(*this);
    }

}