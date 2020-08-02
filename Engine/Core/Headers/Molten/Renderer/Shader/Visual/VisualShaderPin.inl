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

namespace Molten::Shader::Visual
{

    // Input pin implementations.
    template<typename T>
    inline InputPin<T>::InputPin(Node& node, const std::string& name) :
        Pin(node, name),
        m_defaultValue(VariableTrait<T>::DefaultValue),
        m_connection(nullptr)
    { }

    template<typename T>
    inline InputPin<T>::InputPin(Node& node, const T& defaultValue, const std::string& name) :
        Pin(node, name),
        m_defaultValue(defaultValue),
        m_connection(nullptr)
    { }

    template<typename T>
    inline InputPin<T>::~InputPin()
    {
        Disconnect();
    }

    template<typename T>
    inline bool InputPin<T>::Connect(Pin& target)
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
    inline size_t InputPin<T>::Disconnect()
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
    inline bool InputPin<T>::Disconnect(const size_t index)
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
    inline bool InputPin<T>::Disconnect(Pin& target)
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
        return VariableTrait<T>::DataType;
    }    

    template<typename T>
    inline PinDirection InputPin<T>::GetDirection() const
    {
        return PinDirection::In;
    }

    template<typename T>
    inline size_t InputPin<T>::GetConnectionCount() const
    {
        return m_connection ? 1 : 0;
    }

    template<typename T>
    inline Pin* InputPin<T>::GetConnection(const size_t index)
    {
        if (index != 0)
        {
            return nullptr;
        }
        return m_connection;
    }

    template<typename T>
    inline const Pin* InputPin<T>::GetConnection(const size_t index) const
    {
        if (index != 0)
        {
            return nullptr;
        }
        return m_connection;
    }

    template<typename T>
    inline std::vector<Pin*> InputPin<T>::GetConnections()
    {
        if (!m_connection)
        {
            return {};
        }
        return { m_connection };
    }

    template<typename T>
    inline std::vector<const Pin*> InputPin<T>::GetConnections() const
    {
        if (!m_connection)
        {
            return {};
        }
        return { m_connection };
    }

    template<typename T>
    inline T InputPin<T>::GetDefaultValue() const
    {
        return m_defaultValue;
    }

    template<typename T>
    void InputPin<T>::SetDefaultValue(const T& defaultValue)
    {
        m_defaultValue = defaultValue;
    }

    template<typename T>
    inline void InputPin<T>::ConnectInternal(Pin& target)
    {
        m_connection = &target;
    }

    template<typename T>
    inline void InputPin<T>::DisconnectInternal(Pin& )
    {
        m_connection = nullptr;
    }


    // Output pin implementations.
    template<typename T>
    inline OutputPin<T>::OutputPin(Node& node, const std::string& name) :
        Pin(node, name)
    { }

    template<typename T>
    inline OutputPin<T>::~OutputPin()
    {
        Disconnect();
    }

    template<typename T>
    inline bool OutputPin<T>::Connect(Pin& target)
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
    inline size_t OutputPin<T>::Disconnect()
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
    inline bool OutputPin<T>::Disconnect(const size_t index)
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
    inline bool OutputPin<T>::Disconnect(Pin& target)
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
    inline VariableDataType OutputPin<T>::GetDataType() const
    {
        return VariableTrait<T>::DataType;
    }

    template<typename T>
    inline PinDirection OutputPin<T>::GetDirection() const
    {
        return PinDirection::Out;
    }

    template<typename T>
    inline size_t OutputPin<T>::GetConnectionCount() const
    {
        return m_connections.size();
    }

    template<typename T>
    inline Pin* OutputPin<T>::GetConnection(const size_t index)
    {
        if (index >= m_connections.size())
        {
            return nullptr;
        }
        return m_connections.at(index);
    }

    template<typename T>
    inline const Pin* OutputPin<T>::GetConnection(const size_t index) const
    {
        if (index >= m_connections.size())
        {
            return nullptr;
        }
        return m_connections.at(index);
    }

    template<typename T>
    inline std::vector<Pin*> OutputPin<T>::GetConnections()
    {
        return m_connections;
    }

    template<typename T>
    inline std::vector<const Pin*> OutputPin<T>::GetConnections() const
    {
        return { m_connections.begin(), m_connections.end() };
    }

    template<typename T>
    inline void OutputPin<T>::ConnectInternal(Pin& target)
    {
        m_connections.push_back(&target);
    }

    template<typename T>
    inline void OutputPin<T>::DisconnectInternal(Pin& target)
    {
        auto it = std::find(m_connections.begin(), m_connections.end(), &target);
        if (it == m_connections.end())
        {
            return;
        }
        m_connections.erase(it);
    }

}