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

namespace Molten::Shader::Visual
{

    // Input pin implementations.
    template<typename T>
    InputPin<T>::InputPin(Node& node, const std::string& name) :
        Pin(node, name),
        m_defaultValue(VariableTrait<T>::defaultValue),
        m_connection(nullptr)
    { }

    template<typename T>
    InputPin<T>::InputPin(Node& node, const T& defaultValue, const std::string& name) :
        Pin(node, name),
        m_defaultValue(defaultValue),
        m_connection(nullptr)
    { }

    template<typename T>
    InputPin<T>::~InputPin()
    {
        Disconnect();
    }

    template<typename T>
    bool InputPin<T>::ConnectBase(Pin& target)
    {
        if (target.GetDataType() != GetDataType() ||
            &target.GetNode() == &GetNode() ||
            //&(target.GetNode().GetScript()) != &(GetNode().GetScript()) ||
            target.GetDirection() != PinDirection::Out)
        {
            return false;
        }

        if (&target == m_connection)
        {
            return true;
        }

        if (m_connection)
        {
            Pin::DisconnectInternal(*m_connection, *this);
        }     
        Pin::ConnectInternal(target, *this);
        m_connection = &target;

        return true;
    }

    template<typename T>
    bool InputPin<T>::Connect(OutputPin<T>& target)
    {
        if (&target.GetNode() == &GetNode())
        {
            return false;
        }

        if (&target == m_connection)
        {
            return true;
        }

        if (m_connection)
        {
            Pin::DisconnectInternal(*m_connection, *this);
        }
        Pin::ConnectInternal(target, *this);
        m_connection = &target;

        return true;
    }

    template<typename T>
    size_t InputPin<T>::Disconnect()
    {
        if (!m_connection)
        {
            return 0;
        }

        Pin::DisconnectInternal(*m_connection, *this);
        m_connection = nullptr;

        return 1;
    }

    template<typename T>
    bool InputPin<T>::Disconnect(const size_t index)
    {
        if (!m_connection || index != 0)
        {
            return false;
        }

        Pin::DisconnectInternal(*m_connection, *this);
        m_connection = nullptr;

        return true;
    }

    template<typename T>
    bool InputPin<T>::Disconnect(Pin& target)
    {
        if (!m_connection || &target != m_connection)
        {
            return false;
        }

        Pin::DisconnectInternal(*m_connection, *this);
        m_connection = nullptr;

        return true;
    }

    template<typename T>
    VariableDataType InputPin<T>::GetDataType() const
    {
        return VariableTrait<T>::dataType;
    }

    template<typename T>
    size_t InputPin<T>::GetSizeOfDataType() const
    {
        return sizeof(T);
    }

    template<typename T>
    PinDirection InputPin<T>::GetDirection() const
    {
        return PinDirection::In;
    }

    template<typename T>
    size_t InputPin<T>::GetConnectionCount() const
    {
        return m_connection ? 1 : 0;
    }

    template<typename T>
    Pin* InputPin<T>::GetConnection(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return m_connection;
    }

    template<typename T>
    const Pin* InputPin<T>::GetConnection(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return m_connection;
    }

    template<typename T>
    std::vector<Pin*> InputPin<T>::GetConnections()
    {
        if (!m_connection)
        {
            return {};
        }
        return { m_connection };
    }

    template<typename T>
    std::vector<const Pin*> InputPin<T>::GetConnections() const
    {
        if (!m_connection)
        {
            return {};
        }
        return { m_connection };
    }

    template<typename T>
    T InputPin<T>::GetDefaultValue() const
    {
        return m_defaultValue;
    }

    template<typename T>
    void InputPin<T>::SetDefaultValue(const T& defaultValue)
    {
        m_defaultValue = defaultValue;
    }

    template<typename T>
    void InputPin<T>::ConnectInternal(Pin& target)
    {
        m_connection = &target;
    }

    template<typename T>
    void InputPin<T>::DisconnectInternal(Pin& )
    {
        m_connection = nullptr;
    }


    // Output pin implementations.
    template<typename T>
    OutputPin<T>::OutputPin(Node& node, const std::string& name) :
        Pin(node, name)
    { }

    template<typename T>
    OutputPin<T>::~OutputPin()
    {
        Disconnect();
    }

    template<typename T>
    bool OutputPin<T>::ConnectBase(Pin& target)
    {
        if (target.GetDataType() != GetDataType() ||
            &target.GetNode() == &GetNode() ||
            //&(target.GetNode().GetScript()) != &(GetNode().GetScript()) ||
            target.GetDirection() != PinDirection::In)
        {
            return false;
        }

        auto it = std::find(m_connections.begin(), m_connections.end(), &target);
        if (it != m_connections.end())
        {
            return true;
        }

        Pin::ConnectInternal(target, *this);
        m_connections.push_back(&target);

        return true;
    }

    template<typename T>
    bool OutputPin<T>::Connect(InputPin<T>& target)
    {
        if (&target.GetNode() == &GetNode())
        {
            return false;
        }

        auto it = std::find(m_connections.begin(), m_connections.end(), &target);
        if (it != m_connections.end())
        {
            return true;
        }

        Pin::ConnectInternal(target, *this);
        m_connections.push_back(&target);

        return true;
    }

    template<typename T>
    size_t OutputPin<T>::Disconnect()
    {
        size_t count = m_connections.size();

        for (auto & input : m_connections)
        {
            Pin::DisconnectInternal(*input, *this);
        }
        m_connections.clear();

        return count;
    }

    template<typename T>
    bool OutputPin<T>::Disconnect(const size_t index)
    {
        if (index >= m_connections.size())
        {
            return false;
        }

        auto it = m_connections.begin() + index;

        Pin::DisconnectInternal(**it, *this);
        m_connections.erase(it);

        return true;
    }

    template<typename T>
    bool OutputPin<T>::Disconnect(Pin& target)
    {
        auto it = std::find(m_connections.begin(), m_connections.end(), &target);
        if (it == m_connections.end())
        {
            return false;
        }

        Pin::DisconnectInternal(**it, *this);
        m_connections.erase(it);

        return true;
    }

    template<typename T>
    VariableDataType OutputPin<T>::GetDataType() const
    {
        return VariableTrait<T>::dataType;
    }

    template<typename T>
    size_t OutputPin<T>::GetSizeOfDataType() const
    {
        return sizeof(T);
    }

    template<typename T>
    PinDirection OutputPin<T>::GetDirection() const
    {
        return PinDirection::Out;
    }

    template<typename T>
    size_t OutputPin<T>::GetConnectionCount() const
    {
        return m_connections.size();
    }

    template<typename T>
    Pin* OutputPin<T>::GetConnection(const size_t index)
    {
        if (index >= m_connections.size())
        {
            return nullptr;
        }
        return m_connections.at(index);
    }

    template<typename T>
    const Pin* OutputPin<T>::GetConnection(const size_t index) const
    {
        if (index >= m_connections.size())
        {
            return nullptr;
        }
        return m_connections.at(index);
    }

    template<typename T>
    std::vector<Pin*> OutputPin<T>::GetConnections()
    {
        return m_connections;
    }

    template<typename T>
    std::vector<const Pin*> OutputPin<T>::GetConnections() const
    {
        return { m_connections.begin(), m_connections.end() };
    }

    template<typename T>
    void OutputPin<T>::ConnectInternal(Pin& target)
    {
        m_connections.push_back(&target);
    }

    template<typename T>
    void OutputPin<T>::DisconnectInternal(Pin& target)
    {
        auto it = std::find(m_connections.begin(), m_connections.end(), &target);
        if (it == m_connections.end())
        {
            return;
        }
        m_connections.erase(it);
    }

    
    // Pin traits implementations.
    template<>
    struct PinTraits<InputPin>
    {
        static constexpr bool isInputPin = true;
        static constexpr bool isOutputPin = false;
    };
    template<>
    struct PinTraits<OutputPin>
    {
        static constexpr bool isInputPin = false;
        static constexpr bool isOutputPin = true;
    };

}