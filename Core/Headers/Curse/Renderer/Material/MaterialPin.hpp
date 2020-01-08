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

#ifndef CURSE_CORE_RENDERER_MATERIAL_MATERIALPIN_HPP
#define CURSE_CORE_RENDERER_MATERIAL_MATERIALPIN_HPP

#include "Curse/Types.hpp"
#include "Curse/Math/Vector.hpp"
#include <vector>
#include <string>
#include <typeindex>
#include <algorithm>

namespace Curse
{

    namespace Material
    {

        class Node;
        template<typename T> class InputPin;
        template<typename T> class OutputPin;
        

        /**
        * @brief Enumerator of available data types.
        */
        enum class PinDataType : uint8_t
        {
            Bool,
            Int32,
            Float32,
            Vector2f32,
            Vector3f32,
            Vector4f32
        };

        /**
        * @brief Enumerator of pin directions.
        */
        enum class PinDirection : uint8_t
        {
            In,
            Out
        };


        /**
        * @brief Type trait for checking if data type of material node is supported.
        */
        template<typename T>
        struct PinDefault
        {
        };
        template<> struct PinDefault<bool>
        {
            inline static const bool Value = false;
        }; 
        template<> struct PinDefault<int32_t>
        {
            inline static const int32_t Value = 0;
        };
        template<> struct PinDefault<float>
        {
            inline static const float Value = 0.0f;
        };
        template<> struct PinDefault<Vector2f32>
        {
            inline static const Vector2f32 Value = { 0.0f };
        };
        template<> struct PinDefault<Vector3f32>
        {
            inline static const Vector3f32 Value = { 0.0f };
        };
        template<> struct PinDefault<Vector4f32>
        {
            inline static const Vector4f32 Value = { 0.0f };
        };


        /**
        * @brief Base class of visual script pin.
        */
        class Pin
        {

        public:

            /**
            * @brief Constructor.
            */
            Pin(Node & node, const std::string& name = "");

            /**
            * @brief Destructor.
            */
            virtual ~Pin();

            /**
            * @brief Connect pin with target pin, of opposite direction.
            *
            * @return true if target pin got connected, false if direction of target pin is not of opposite direction.
            */
            virtual bool Connect(Pin& target) = 0;

            /**
            * @brief Disconnect all connected pins.
            *
            * @return Number of disconnected pins.
            */
            virtual size_t Disconnect() = 0;

            /**
            * @brief Disconnect connected pin by index.
            *
            * @return true if pin is disconnected, false if index >= GetConnectionCount().
            */
            virtual bool Disconnect(const size_t index) = 0;

            /**
            * @brief Disconnect connected pin by target pin.
            *
            * @return true if pin is disconnected, false if target pin is not connected to this pin.
            */
            virtual bool Disconnect(Pin& target) = 0;
  
            /**
            * @brief Get data type of pin.
            */
            virtual PinDataType GetDataType() const = 0;

            /**
            * @brief Get type index of pin data type.
            */
            virtual std::type_index GetDataTypeIndex() const = 0;

            /**
            * @brief Get direction of pin, in or out.
            */
            virtual PinDirection GetDirection() const = 0;

            /**
            * @brief Get number of connections.
            *        Output pins can have multiple connections, but input pins can only have one.
            */
            virtual size_t GetConnectionCount() const = 0;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of connected pin, nullptr if index is >= GetConnectionCount(), or if no pin is connected.
            */
            virtual Pin* GetConnection(const size_t index = 0) = 0;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of pin, nullptr if index is >= GetConnectionCount().
            */
            virtual const Pin* GetConnection(const size_t index = 0) const = 0;

            /**
            * @brief Get all connections, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetConnections() = 0;

            /**
            * @brief Get all connections, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetConnections() const = 0;

            /**
            * @brief Get parent node object.
            */
            Node& GetNode();

            /**
            * @brief Get parent node object.
            */
            const Node& GetNode() const;   

            /**
            * @brief Get name of pin.
            */
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
        * @brief Visual script input pin class.
        *
        * @tparam T Data type of pin.
        */
        template<typename T>
        class InputPin : public Pin
        {

        public:

            /**
            * @brief Constructor.
            */
            InputPin(Node& node, const std::string& name = "");

            /**
            * @brief Constructor.
            */
            InputPin(Node& node, const T& defaultValue, const std::string& name = "");

            /**
            * @brief Destructor.
            */
            virtual ~InputPin();

            /**
            * @brief Connect pin with target pin, of opposite direction.
            *        The current connected pin is being disconnect, if target is different from the currently connected one.
            *
            * @return true if target pin got connected, false if direction of target pin is not equal to PinDirection::Out.
            */
            virtual bool Connect(Pin& target) override;
            
            /**
            * @brief Disconnect current connected pins.
            *
            * @return 1(number of connected pins) if disconnected, else 0.
            */
            virtual size_t Disconnect() override;

            /**
            * @brief Disconnect connected pin by index.
            *
            * @return true if pin got disconnected, false if no pin currently is connected, or if index != 0.
            */
            virtual bool Disconnect(const size_t index) override;

            /**
            * @brief Disconnect connected pin by target pin.
            *
            * @return true if pin got disconnected, false if target pin is not connected to this pin.
            */
            virtual bool Disconnect(Pin& target) override;

            /**
            * @brief Get data type of pin
            */
            virtual PinDataType GetDataType() const override;

            /**
            * @brief Get type index of pin data type.
            */
            virtual std::type_index GetDataTypeIndex() const override;

            /**
            * @brief Get direction of pin, in or out.
            */
            virtual PinDirection GetDirection() const override;

            /**
            * @brief Get default value of pin.
            */
            virtual T GetDefaultValue() const;

            /**
            * @brief Get default value of pin.
            */
            virtual void SetDefaultValue(const T& defaultValue);

            /**
            * @brief Get number of connections.
            *
            * @return 1 if output pin is connected, else 0.
            */
            virtual size_t GetConnectionCount() const override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of pin, nullptr if index is >= GetConnectionCount().
            */
            virtual Pin* GetConnection(const size_t index = 0) override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of pin, nullptr if index is >= GetConnectionCount().
            */
            virtual const Pin* GetConnection(const size_t index = 0) const override;

            /**
            * @brief Get all connections, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetConnections() override;

            /**
            * @brief Get all connections, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetConnections() const override;           

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
        * @brief Visual script output pin class.
        *
        * @tparam T Data type of pin.
        */
        template<typename T>
        class OutputPin : public Pin
        {

        public:

            /**
            * @brief Constructor.
            */
            OutputPin(Node& node, const std::string& name = "");

            /**
            * @brief Destructor.
            */
            virtual ~OutputPin();

            /**
            * @brief Connect pin with target pin, of opposite direction.
            *
            * @return true if target pin got connected, false if direction of target pin is not equal to PinDirection::In.
            */
            virtual bool Connect(Pin& target) override;

            /**
            * @brief Disconnect current connected pins.
            *
            * @return number of disconnected pins.
            */
            virtual size_t Disconnect() override;

            /**
            * @brief Disconnect connected pin by index.
            *
            * @return true if pin is disconnected, false if index >= GetConnectionCount().
            */
            virtual bool Disconnect(const size_t index) override;

            /**
            * @brief Disconnect connected pin by target pin.
            *
            * @return true if pin got disconnected, false if target pin is not connected to this pin.
            */
            virtual bool Disconnect(Pin& target) override;

            /**
            * @brief Get data type of pin
            */
            virtual PinDataType GetDataType() const override;

            /**
            * @brief Get type index of pin data type.
            */
            virtual std::type_index GetDataTypeIndex() const override;

            /**
            * @brief Get direction of pin, in or out.
            */
            virtual PinDirection GetDirection() const override;

            /**
            * @brief Get number of connections.
            */
            virtual size_t GetConnectionCount() const override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of pin, nullptr if index is >= GetConnectionCount().
            */
            virtual Pin* GetConnection(const size_t index = 0) override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of pin, nullptr if index is >= GetConnectionCount().
            */
            virtual const Pin* GetConnection(const size_t index = 0) const override;

            /**
            * @brief Get all connections, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetConnections() override;

            /**
            * @brief Get all connections, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetConnections() const override;

        protected:

            virtual void ConnectInternal(Pin& target) override;
            virtual void DisconnectInternal(Pin& target) override;

        private:

            OutputPin(const OutputPin&) = delete;
            OutputPin(OutputPin&&) = delete;

            std::vector<Pin*> m_connections;

            friend class Node;

        };

    }

}

#include "MaterialPin.inl"

#endif