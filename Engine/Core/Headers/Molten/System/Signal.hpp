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

#ifndef MOLTEN_CORE_SYSTEM_SIGNAL_HPP
#define MOLTEN_CORE_SYSTEM_SIGNAL_HPP

#include "Molten/Types.hpp"
#include <functional>
#include <vector>
#include <map>

namespace Molten
{

    /**
    * @brief Signal class.
    *        Supporting multiple callback connections for a single signal.
    *        All connections are destroyed at destruction, making any stored Connection object useless.
    *        Signals are not thread safe. Signaled callbacks are executed at the emitter's thread.
    */
    template<typename ... Args>
    class Signal
    {

    public:

        /**
        * @brief Callback function type.
        */
        using Callback = std::function<void(Args...)>;

        /**
        * @brief Connection class.
        *        Used for disconnecting existing connection of signal.
        */
        class Connection
        {

        public:

            /**
            * @brief Constructor.
            */
            Connection();

            /**
            * @brief Copy constructor.
            */
            Connection(const Connection& connection);

            /**
            * @brief Move constructor.
            */
            Connection(Connection&& connection) noexcept;

            /**
            * @brief Copy assignment operator.
            */
            Connection& operator =(const Connection&);

            /**
            * @brief Move assignment operator.
            */
            Connection& operator =(Connection&&);

            /**
            * @brief Disconnect this connection for signal.
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

        /**
        * @brief Constructor.
        */
        Signal();

        /**
        * @brief Destructor.
        */
        ~Signal();

        /**
        * @brief Connect signal to callback function.
        *
        * @return Connection object, used for disconnecting from signal at any time.
        */
        Connection Connect(const Callback & callback);

        /**
        * @brief Disconnect existing connection.
        */
        void Disconnect(Connection& connection);

        /**
        * @brief Disconnect all connections.
        */
        void DisconnectAll();

        /**
        * @brief Get number of connections.
        */
        size_t GetConnectionCount() const;

        /**
        * @brief Operator for executing all connected callback functions, with input parameters.
        */
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

}

#include "Molten/System/Signal.inl"

#endif