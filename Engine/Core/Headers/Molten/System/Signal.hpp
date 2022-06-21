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

#ifndef MOLTEN_CORE_SYSTEM_SIGNAL_HPP
#define MOLTEN_CORE_SYSTEM_SIGNAL_HPP

#include "Molten/Core.hpp"
#include <vector>
#include <map>
#include <tuple>
#include <functional>

namespace Molten
{

    /** Forward declarations. */
    /**@{*/
    template<typename ... Args> class Signal;
    class DispatchSignalBase;
    template<typename ... Args> class DispatchSignal;
    class SignalDispatcher;
    /**@}*/


    /** Signal class.
    *   Supporting multiple callback connections for a single signal.
    *   All connections are destroyed at destruction, making any stored Connection object useless.
    *   Signals are not thread safe. Signaled callbacks are executed at the emitter's thread.
    */
    template<typename ... Args>
    class Signal
    {

    public:

        /** Callback function type. */
        using Callback = std::function<void(Args...)>;

        /** Connection class.  Used for disconnecting existing connection of signal. */
        class Connection
        {

        public:

            Connection();
            ~Connection() = default;

            /** Copy and move constructor/assignment operators. */
            /**@{*/
            Connection(const Connection& connection);
            Connection(Connection&& connection) noexcept;

            Connection& operator =(const Connection& connection);
            Connection& operator =(Connection&& connection) noexcept;
            /**@}*/

            /** Disconnect this connection for signal.
            */
            void Disconnect();

        private:

            struct Controller
            {
                Connection* connection;
            };

            Signal* m_signal;
            Controller * m_controller;

            friend class Signal;

        };

        Signal() = default;
        ~Signal();

        /** Connect signal to callback function.
        *
        * @return Connection object, used for disconnecting from signal at any time.
        */
        Connection Connect(const Callback & callback);

        /** Disconnect existing connection. */
        void Disconnect(Connection& connection);

        /**  Disconnect all connections. */
        void DisconnectAll();

        /**  Get number of connections. */
        [[nodiscard]] size_t GetConnectionCount() const;

        /** Operator for executing all connected callback functions, with input parameters. */
        void operator ()(Args ... args) const;

    private:

        Signal(const Signal&) = delete;
        Signal(Signal&&) = delete;
        Signal& operator =(const Signal&) = delete;
        Signal& operator =(Signal&&) = delete;

        using Connections = std::map<typename Connection::Controller*, Callback>;

        Connections m_connections;

        friend class Connection;

    };


    class MOLTEN_CORE_API SignalDispatcher
    {

    public:

        SignalDispatcher() = default;
        ~SignalDispatcher() = default;

        /** Deleted copy and move constructor/assignment operators. */
        /**@{*/
        SignalDispatcher(const SignalDispatcher&) = delete;
        SignalDispatcher(SignalDispatcher&&) = delete;

        SignalDispatcher& operator =(const SignalDispatcher&) = delete;
        SignalDispatcher& operator =(SignalDispatcher&&) = delete;
        /**@}*/

        /** Execute all signals. */
        void Execute();

    private:

        template<typename ... Args> friend class DispatchSignal;

        void QueueSignal(DispatchSignalBase& dispatchSignalBase);

        using DispatchSignalBases = std::vector<DispatchSignalBase*>;

        DispatchSignalBases m_queuedSignals;

    };


    class MOLTEN_CORE_API DispatchSignalBase
    {

    protected:

        DispatchSignalBase() = default;

        using SignaledCallback = std::function<void()>;
        using SignaledCallbacks = std::vector<SignaledCallback>;

        SignaledCallbacks m_signaledCallbacks;

    private:

        friend class SignalDispatcher;

    };


    /** Dispatch signal class.
    *   Supporting multiple callback connections for a single signal.
    *   Signaled callbacks are executed by provided signal dispatcher.
    */
    template<typename ... Args>
    class DispatchSignal : public DispatchSignalBase
    {

    public:

        /** Callback function type. */
        using Callback = std::function<void(Args...)>;

        explicit DispatchSignal(SignalDispatcher& dispatcher);
        ~DispatchSignal() = default;

        /** Deleted copy and move constructor/assignment operators. */
        /**@{*/
        DispatchSignal(const DispatchSignal&) = delete;
        DispatchSignal(DispatchSignal&&) = delete;

        DispatchSignal& operator =(const DispatchSignal&) = delete;
        DispatchSignal& operator =(DispatchSignal&&) = delete;
        /**@}*/

        /** Connect signal to callback function. */
        void Connect(const Callback& callback);
        void Connect(Callback&& callback);

        /** Operator for signaling callback. Callback function is not executed immediately, but when the dispatcher is executed. */
        void operator ()(Args&& ... args);

    private:

        using Callbacks = std::vector<Callback>;

        SignalDispatcher& m_dispatcher;
        Callbacks m_callbacks;

    };


}

#include "Molten/System/Signal.inl"

#endif