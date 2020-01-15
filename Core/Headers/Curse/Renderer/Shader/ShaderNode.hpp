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

#ifndef CURSE_CORE_RENDERER_SHADER_SHADERNODE_HPP
#define CURSE_CORE_RENDERER_SHADER_SHADERNODE_HPP

#include "Curse/Types.hpp"
#include "Curse/Math/Vector.hpp"
#include "Curse/Renderer/Shader/ShaderPin.hpp"
#include <memory>
#include <array>

namespace Curse
{

    namespace Shader
    {

        /**
        * @brief Forward declarations.
        */
        class Script;


        /**
        * @brief Enumerator of node types.
        */
        enum class NodeType : uint8_t
        {
            Constant,   ///< Local constant, only present in fragment shader.
            Function,   ///< Built-in shader function.
            Operator,   ///< Operator node in local space.
            Output,     ///< Output node, resulting in fragment colors.
            //Uniform,  ///< Uniform node, single object being sent runtime from client.
            Varying,    ///< Varying node, sent from the vertex or geometry shader.    
        };

        /**
        * @brief Enumerator of varying node type.
        */
        enum class VaryingType : uint8_t
        {
            Color,      ///< Vertex color.
            Normal,     ///< Normal direction.
            Position    ///< Vertex position. 
        };

        /**
        * @brief Enumerator of arithmetic operators.
        */
        enum class Operator : uint8_t
        {
            Addition,
            Subtraction,
            Multiplication,
            Division
        };

        /**
        * @brief Enumerator of functions.
        */
        enum class FunctionType : uint16_t
        {
            // Trigonometry
            Cos,
            Sin,
            Tan,

            // Mathematics.
            Max,
            Min,

            // Vector.
            Cross,
            Dot
        };


        /**
        * @brief Type trait for checking if data type of shader script node is supported.
        */
        template<typename T>
        struct DataTypeTrait
        {
            static const bool Supported = false;
        };
        template<> struct DataTypeTrait<bool>
        {
            static const bool Supported = true;
        };
        template<> struct DataTypeTrait<int32_t>
        {
            static const bool Supported = true;
        };
        template<> struct DataTypeTrait<float>
        {
            static const bool Supported = true;
        };
        template<> struct DataTypeTrait<Vector2f32>
        {
            static const bool Supported = true;
        };
        template<> struct DataTypeTrait<Vector3f32>
        {
            static const bool Supported = true;
        };
        template<> struct DataTypeTrait<Vector4f32>
        {
            static const bool Supported = true;
        };


        /**
        * @brief Type trait for retreiving data type of varying node types.
        */
        template<VaryingType T>
        struct VaryingTypeTrait
        { };
        template<> struct VaryingTypeTrait<VaryingType::Color>
        {
            using DataType = Vector4f32;
        };
        template<> struct VaryingTypeTrait<VaryingType::Normal>
        {
            using DataType = Vector3f32;
        };
        template<> struct VaryingTypeTrait<VaryingType::Position>
        {
            using DataType = Vector3f32;
        };


        /**
        * @brief shader script node base class.
        */
        class CURSE_API Node
        {

        public:

            /**
            * @brief Virtual destructor.
            */
            virtual ~Node();

            /**
            * @brief Get parent shader script.
            */
            Script& GetScript();

            /**
            * @brief Get shader script.
            */
            const Script& GetScript() const;

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const;

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetOutputPinCount() const;


            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual Pin* GetInputPin(const size_t index = 0);

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual const Pin* GetInputPin(const size_t index = 0) const;

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetInputPins();

            /**
            * @brief  Get all input pins, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetInputPins() const;


            /**
            * @brief Get output pin by index.
            *
            * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
            */
            virtual Pin* GetOutputPin(const size_t index = 0);

            /**
            * @brief Get output pin by index.
            *
            * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
            */
            virtual const Pin* GetOutputPin(const size_t index = 0) const;

            /**
            * @brief Get all output pins, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetOutputPins();

            /**
            * @brief  Get all output pins, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetOutputPins() const;


            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const = 0;

        protected:

            /**
            * @brief Constructor.
            */
            Node(Script& script);

        private:

            Script& m_script; ///< Parent shader script.

            friend class Script;

        };


        /**
        * @brief Output node of shader script.
        */
        template<typename T>
        class OutputNode : public Node
        {

            static_assert(DataTypeTrait<T>::Supported, "Data type of output node is not supported.");

        public:

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual Pin* GetInputPin(const size_t index = 0) override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual const Pin* GetInputPin(const size_t index = 0) const override;

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetInputPins() override;

            /**
            * @brief  Get all input pins, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetInputPins() const override;

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

        protected:

            /**
            * @brief Constructor.
            */
            OutputNode(Script& script);

        private:

            InputPin<T> m_pin;

            friend class Script;

        };


        /**
        * @brief Varying node of shader script.
        */
        template<VaryingType T>
        class VaryingNode : public Node
        {

        public:

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual Pin* GetOutputPin(const size_t index = 0) override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetOutputPins() override;

            /**
            * @brief  Get all input pins, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetOutputPins() const override;

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

        protected:

            /**
            * @brief Constructor.
            */
            VaryingNode(Script& script);

        private:

            using DataType = typename VaryingTypeTrait<T>::DataType;

            OutputPin<DataType> m_pin;

            friend class Script;

        };


        /**
        * @brief Base clas of constant node, of shader script.
        */
        class ConstantNodeBase : public Node
        {

        public:

            /**
            * @brief Get data type of constant.
            */
            virtual PinDataType GetDataType() const = 0;

            /**
            * @brief Get type index of constant data type.
            */
            virtual std::type_index GetDataTypeIndex() const = 0;

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

        protected:

            /**
            * @brief Constructor.
            */
            ConstantNodeBase(Script& script);

        };

        /**
        * @brief Constant node of shader script.
        *        The stored value is not constant and can be changed at any time,
        *        but it's used as a constant in the generated shader code.
        */
        template<typename T>
        class ConstantNode : public ConstantNodeBase
        {

        public:

            /**
            * @brief Get data type of constant.
            */
            virtual PinDataType GetDataType() const override;

            /**
            * @brief Get type index of constant data type.
            */
            virtual std::type_index GetDataTypeIndex() const override;

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual Pin* GetOutputPin(const size_t index = 0) override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetOutputPins() override;

            /**
            * @brief  Get all input pins, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetOutputPins() const override;

            /**
            * @brief Get value of constant node.
            */
            const T& GetValue() const;

            /**
            * @brief Set value of constant node.
            */
            void SetValue(const T & value) const;

        protected:

            /**
            * @brief Constructor.
            */
            ConstantNode(Script& script, const T & value);

        private:

            OutputPin<T> m_output;
            T m_value;

            friend class Script;

        };


        /**
        * @brief Base clas of operator node, of shader script.
        */
        class OperatorNodeBase : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

            /*
            * @brief Get operator type.
            */
            Operator GetOperator() const;

        protected:

            /**
            * @brief Constructor.
            */
            OperatorNodeBase(Script& script, const Operator op);

        private:

            const Operator m_operator;

        };

        /**
        * @brief Operator node of shader script.
        */
        template<typename T>
        class OperatorNode : public OperatorNodeBase
        {

        public:

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const override;

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual Pin* GetInputPin(const size_t index = 0) override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual const Pin* GetInputPin(const size_t index = 0) const override;

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetInputPins() override;

            /**
            * @brief  Get all input pins, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetInputPins() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual Pin* GetOutputPin(const size_t index = 0) override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetOutputPins() override;

            /**
            * @brief  Get all input pins, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetOutputPins() const override;

        protected:

            /**
            * @brief Constructor.
            */
            OperatorNode(Script& script, const Operator op);

        private:

            InputPin<T> m_inputA;
            InputPin<T> m_inputB;
            InputPin<T> * m_inputs[2];
            OutputPin<T> m_output;

            friend class Script;

        };


        /**
        * @brief Function node base class of shader script.
        */
        class FunctionNodeBase : public Node
        {

        public:

            /**
            * @brief Get function type.
            */
            virtual FunctionType GetFunctionType() const = 0;

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

        protected:

            /**
            * @brief Constructor.
            */
            FunctionNodeBase(Script& script);

        };

        /**
        * @brief Function node of shader script.
        */
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        class FunctionNode : public FunctionNodeBase
        {

        public:

            /**
            * @brief Get function type.
            */
            virtual FunctionType GetFunctionType() const override;

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const override;

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual Pin* GetInputPin(const size_t index = 0) override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual const Pin* GetInputPin(const size_t index = 0) const override;

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetInputPins() override;

            /**
            * @brief  Get all input pins, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetInputPins() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual Pin* GetOutputPin(const size_t index = 0) override;

            /**
            * @brief Get connected pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            virtual std::vector<Pin*> GetOutputPins() override;

            /**
            * @brief  Get all input pins, wrapped in a vector.
            */
            virtual std::vector<const Pin*> GetOutputPins() const override;

        protected:

            /**
            * @brief Constructor.
            */
            FunctionNode(Script& script);

        private:

            static constexpr size_t OutputPinCount = 1;
            static constexpr size_t InputPinCount = sizeof...(InputTypes);

            template<typename CurrentType, typename ...>
            void InitInputPin(const size_t index = 0);

            std::array<std::unique_ptr<Pin>, InputPinCount> m_inputs;
            std::unique_ptr<Pin> m_output;

            friend class Script;

        };
    }
}

#include "ShaderNode.inl"

#endif