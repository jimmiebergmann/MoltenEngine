/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERPIN_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERPIN_HPP

#include "Molten/Renderer/Shader.hpp"
#include <vector>
#include <string>

namespace Molten::Shader::Visual
{

    class Node;
    template<typename T> class InputPin;
    template<typename T> class OutputPin;
        

    /** Enumerator of pin directions. */
    enum class PinDirection : uint8_t
    {
        In,
        Out
    };


    /** Visual script pin base class. */
    class MOLTEN_API Pin
    {

    public:

        /**  Constructor. */
        Pin(Node & node, const std::string& name = "");

        /** Destructor. */
        virtual ~Pin();

        /**
         * Connect this pin with target pin, of opposite direction.
         *
         * @return true if pins got connected, false if direction of target pin is not of opposite direction.
         */
        virtual bool Connect(Pin& target) = 0;

        /**
         * Disconnect this pin from all connected pins.
         *
         * @return Number of disconnected pins.
         */
        virtual size_t Disconnect() = 0;

        /**
         * Disconnect this pin from connected pin by index.
         *
         * @return true if pin is disconnected, false if index >= GetConnectionCount().
         */
        virtual bool Disconnect(const size_t index) = 0;

        /**
         * Disconnect this pin from connected pin by target pin.
         *
         * @return true if pin is disconnected, false if target pin is not connected to this pin.
         */
        virtual bool Disconnect(Pin& target) = 0;
  
        /** Get data type of pin. */
        virtual VariableDataType GetDataType() const = 0;

        /** Get size of data type in bytes. */
        virtual size_t GetSizeOfDataType() const = 0;

        /** Get direction of pin, in or out. */
        virtual PinDirection GetDirection() const = 0;

        /**
         * Get number of connected pins of this pin.
         * Output pins can have multiple connections, but input pins can only have one.
         */
        virtual size_t GetConnectionCount() const = 0;

        /**
         * Get connected pin by index.
         *
         * @return Pointer of connected pin, nullptr if index is >= GetConnectionCount() or pin isn't connected.
         */
        /**@{*/
        virtual Pin* GetConnection(const size_t index = 0) = 0;
        virtual const Pin* GetConnection(const size_t index = 0) const = 0;
        /**@}*/

        /** Get all connected pins, wrapped in a vector. */
        virtual std::vector<Pin*> GetConnections() = 0;
        virtual std::vector<const Pin*> GetConnections() const = 0;
        /**@}*/

        /** Get parent node object. */
        /**@{*/
        Node& GetNode();
        const Node& GetNode() const;   
        /**@}*/

        /** Get name of pin. */
        const std::string& GetName() const;

    protected:

        void ConnectInternal(Pin& source, Pin& target);
        void DisconnectInternal(Pin& source, Pin& target);

        virtual void ConnectInternal(Pin& target) = 0;
        virtual void DisconnectInternal(Pin& target) = 0;

    private:

        Pin(const Pin&) = delete;
        Pin(Pin&&) = delete;

        Node& m_node;
        std::string m_name;

    };


    /**
     * Visual script input pin class.
     *
     * @tparam T Data type of pin.
     */
    template<typename T>
    class InputPin : public Pin
    {

        static_assert(VariableTrait<T>::supported, "Input pin of data type T is not supported.");

    public:

        /** Constructor. */
        explicit InputPin(Node& node, const std::string& name = "");

        /** Constructor. */
        explicit InputPin(Node& node, const T& defaultValue, const std::string& name = "");

        /** Destructor. */
        ~InputPin();

        /**
         * Connect this pin with target pin, of opposite direction.
         *
         * @return true if pins got connected, false if direction of target pin is not of opposite direction.
         */
        virtual bool Connect(Pin& target) override;

        /**
         * Disconnect this pin from all connected pins.
         *
         * @return Number of disconnected pins.
         */
        virtual size_t Disconnect() override;

        /**
         * Disconnect this pin from connected pin by index.
         *
         * @return true if pin is disconnected, false if index >= GetConnectionCount().
         */
        virtual bool Disconnect(const size_t index) override;

        /**
         * Disconnect this pin from connected pin by target pin.
         *
         * @return true if pin is disconnected, false if target pin is not connected to this pin.
         */
        virtual bool Disconnect(Pin& target) override;

        /** Get data type of pin. */
        virtual VariableDataType GetDataType() const override;

        /** Get size of data type in bytes. */
        virtual size_t GetSizeOfDataType() const override;

        /** Get direction of pin, in or out. */
        virtual PinDirection GetDirection() const override;

        /**
         * Get number of connected pins of this pin.
         * Output pins can have multiple connections, but input pins can only have one.
         */
        virtual size_t GetConnectionCount() const override;

        /**
         * Get connected pin by index.
         *
         * @return Pointer of connected pin, nullptr if index is >= GetConnectionCount() or pin isn't connected.
         */
         /**@{*/
        virtual Pin* GetConnection(const size_t index = 0) override;
        virtual const Pin* GetConnection(const size_t index = 0) const override;
        /**@}*/

        /** Get all connected pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetConnections() override;
        virtual std::vector<const Pin*> GetConnections() const override;
        /**@}*/

        /** Get default value of pin. */
        T GetDefaultValue() const;

        /** Get default value of pin. */
        void SetDefaultValue(const T& defaultValue);

    protected:

        virtual void ConnectInternal(Pin& target) override;
        virtual void DisconnectInternal(Pin& target) override;

    private:

        InputPin(const InputPin&) = delete;
        InputPin(InputPin&&) = delete;

        T m_defaultValue;
        Pin* m_connection;

        friend class Node;

    };


    /**
     * Visual script output pin class.
     *
     * @tparam T Data type of pin.
     */
    template<typename T>
    class OutputPin : public Pin
    {
        static_assert(VariableTrait<T>::supported, "Input pin of data type T is not supported.");

    public:

        /** Constructor. */
        explicit OutputPin(Node& node, const std::string& name = "");

        /** Destructor. */
        ~OutputPin();

        /**
         * Connect this pin with target pin, of opposite direction.
         *
         * @return true if pins got connected, false if direction of target pin is not of opposite direction.
         */
        virtual bool Connect(Pin& target) override;

        /**
         * Disconnect this pin from all connected pins.
         *
         * @return Number of disconnected pins.
         */
        virtual size_t Disconnect() override;

        /**
         * Disconnect this pin from connected pin by index.
         *
         * @return true if pin is disconnected, false if index >= GetConnectionCount().
         */
        virtual bool Disconnect(const size_t index) override;

        /**
         * Disconnect this pin from connected pin by target pin.
         *
         * @return true if pin is disconnected, false if target pin is not connected to this pin.
         */
        virtual bool Disconnect(Pin& target) override;

        /** Get data type of pin. */
        virtual VariableDataType GetDataType() const override;

        /** Get size of data type in bytes. */
        virtual size_t GetSizeOfDataType() const override;

        /** Get direction of pin, in or out. */
        virtual PinDirection GetDirection() const override;

        /**
         * Get number of connected pins of this pin.
         * Output pins can have multiple connections, but input pins can only have one.
         */
        virtual size_t GetConnectionCount() const override;

        /**
         * Get connected pin by index.
         *
         * @return Pointer of connected pin, nullptr if index is >= GetConnectionCount() or pin isn't connected.
         */
         /**@{*/
        virtual Pin* GetConnection(const size_t index = 0) override;
        virtual const Pin* GetConnection(const size_t index = 0) const override;
        /**@}*/

        /** Get all connected pins, wrapped in a vector. */
        /**@{*/
        virtual std::vector<Pin*> GetConnections() override;
        virtual std::vector<const Pin*> GetConnections() const override;
        /**@}*/

    protected:

        virtual void ConnectInternal(Pin& target) override;
        virtual void DisconnectInternal(Pin& target) override;

    private:

        OutputPin(const OutputPin&) = delete;
        OutputPin(OutputPin&&) = delete;

        std::vector<Pin*> m_connections;

        friend class Node;

    };


    /** Type traits for checking if provided template parameter is an input or outpin pin. */
    template<template<typename> typename TPinType>
    struct PinTraits
    {
        static constexpr bool isInputPin = false;
        static constexpr bool isOutputPin = false;
    };

}

#include "Molten/Renderer/Shader/Visual/VisualShaderPin.inl"

#endif