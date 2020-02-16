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
        class VertexScript;
        class FragmentScript;
        class InputBlock;
        class OutputBlock;
        class UniformBlock;
        


        /**
        * @brief Enumerator of node types.
        */
        enum class NodeType : uint8_t
        {
            Constant,       ///< Local constant, only present in fragment shader.
            Function,       ///< Built-in shader function.
            Input,          ///< Input data from previous shader stage, or data from vertex buffer.
            Operator,       ///< Operator node in local space.
            Output,         ///< Output data for next shader stage, or data for framebuffer.
            Uniform,        ///< Uniform node, single object being sent runtime from client.
            VertexOutput    ///< Output data of vertex position, being outputted in the vertex shader stage.
        };

        /**
        * @brief Enumerator of operator types.
        */
        enum class OperatorType : uint8_t
        {
            Arithmetic
        };

        /**
        * @brief Enumerator of arithmetic operator types.
        */
        enum class ArithmeticOperatorType : uint8_t
        {
            Addition,
            Division,
            Multiplication,
            Subtraction
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
            Dot,

            // Sampler
            Texture2D,
            Texture3D
        };


        /**
        * @brief shader script node base class.
        */
        class CURSE_API Node
        {

        public:

            /**
            * @brief Get parent shader script.
            */
            /**@{*/
            Script& GetScript();
            const Script& GetScript() const;
            /**@}*/

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const;

            /**
            * @brief Get number of output pins.
            */
            virtual size_t GetOutputPinCount() const;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            /**@{*/
            virtual Pin* GetInputPin(const size_t index = 0);
            virtual const Pin* GetInputPin(const size_t index = 0) const;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetInputPins();
            virtual std::vector<const Pin*> GetInputPins() const;
            /**@}*/

            /**
            * @brief Get output pin by index.
            *
            * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
            */
            /**@{*/
            virtual Pin* GetOutputPin(const size_t index = 0);
            virtual const Pin* GetOutputPin(const size_t index = 0) const;
            /**@}*/

            /**
            * @brief Get all output pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetOutputPins();
            virtual std::vector<const Pin*> GetOutputPins() const;
            /**@}*/

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const = 0;

        protected:

            Node(Script& script);
            Node(const Node&) = delete;
            Node(Node&&) = delete;
            virtual ~Node();

        private:

            Script& m_script; ///< Parent shader script.

            friend class VertexScript;
            friend class FragmentScript;

        };


        /**
        * @brief Base clas of constant node, of shader script.
        */
        class CURSE_API ConstantNodeBase : public Node
        {

        public:            

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

            /**
            * @brief Get data type of constant.
            */
            virtual VariableDataType GetDataType() const = 0;

        protected:

            ConstantNodeBase(Script& script);
            ConstantNodeBase(const ConstantNodeBase&) = delete;
            ConstantNodeBase(ConstantNodeBase&&) = delete;
            virtual ~ConstantNodeBase();

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
            * @brief Get value of constant node.
            */
            const T& GetValue() const;

            /**
            * @brief Set value of constant node.
            */
            void SetValue(const T& value);

            /**
            * @brief Get data type of constant.
            */
            virtual VariableDataType GetDataType() const override;

            /**
            * @brief Get number of output pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get output pin by index.
            *
            * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
            */
            /**@{*/
            virtual Pin* GetOutputPin(const size_t index = 0) override;
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all output pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetOutputPins() override;
            virtual std::vector<const Pin*> GetOutputPins() const override;
            /**@}*/

        protected:

            ConstantNode(Script& script, const T & value);
            ConstantNode(const ConstantNode&) = delete;
            ConstantNode(ConstantNode&&) = delete;
            ~ConstantNode();

        private:

            OutputPin<T> m_output;
            T m_value;

            friend class VertexScript;
            friend class FragmentScript;

        };


        /**
        * @brief Function node base class of shader script.
        */
        class CURSE_API FunctionNodeBase : public Node
        {

        public:           

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

            /**
            * @brief Get function type.
            */
            virtual FunctionType GetFunctionType() const = 0;            

        protected:
          
            FunctionNodeBase(Script& script);
            FunctionNodeBase(const FunctionNodeBase&) = delete;
            FunctionNodeBase(FunctionNodeBase&&) = delete;
            virtual ~FunctionNodeBase();

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
            * @brief Get number of output pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            /**@{*/
            virtual Pin* GetInputPin(const size_t index = 0) override;
            virtual const Pin* GetInputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetInputPins() override;
            virtual std::vector<const Pin*> GetInputPins() const override;
            /**@}*/

            /**
            * @brief Get output pin by index.
            *
            * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
            */
            /**@{*/
            virtual Pin* GetOutputPin(const size_t index = 0) override;
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all output pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetOutputPins() override;
            virtual std::vector<const Pin*> GetOutputPins() const override;
            /**@}*/

        protected:

            FunctionNode(Script& script);
            FunctionNode(const FunctionNode&) = delete;
            FunctionNode(FunctionNode&&) = delete;
            ~FunctionNode();

        private:

            static constexpr size_t OutputPinCount = 1;
            static constexpr size_t InputPinCount = sizeof...(InputTypes);

            template<typename CurrentType, typename ...>
            void InitInputPin(const size_t index = 0);

            std::array<std::unique_ptr<Pin>, InputPinCount> m_inputs;
            std::unique_ptr<Pin> m_output;

            friend class VertexScript;
            friend class FragmentScript;

        };


        /**
        * @brief Operator node base class of shader script.
        */
        class CURSE_API OperatorNodeBase : public Node
        {

        public:          

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

            /*
            * @brief Get operator type.
            */
            virtual OperatorType GetOperatorType() const = 0;

        protected:

            OperatorNodeBase(Script& script);
            OperatorNodeBase(const OperatorNodeBase&) = delete;
            OperatorNodeBase(OperatorNodeBase&&) = delete;
            virtual ~OperatorNodeBase();

        };


        /**
        * @brief Arithmetic operator node base class of shader script.
        */
        class CURSE_API ArithmeticOperatorNodeBase : public OperatorNodeBase
        {

        public:

            /*
            * @brief Get operator type.
            */
            virtual OperatorType GetOperatorType() const override;

            /*
            * @brief Get arithmetic operator type.
            */
            virtual ArithmeticOperatorType GetArithmeticOperatorType() const = 0;

        protected:

            ArithmeticOperatorNodeBase(Script& script);
            ArithmeticOperatorNodeBase(const ArithmeticOperatorNodeBase&) = delete;
            ArithmeticOperatorNodeBase(ArithmeticOperatorNodeBase&&) = delete;
            virtual ~ArithmeticOperatorNodeBase();

        };


        /**
        * @brief Operator node of shader script.
        */
        template<ArithmeticOperatorType Operator, typename OutputType, typename LeftType, typename RightType>
        class ArithmeticOperatorNode : public ArithmeticOperatorNodeBase
        {

        public:

            /*
            * @brief Get arithmetic operator type.
            */
            virtual ArithmeticOperatorType GetArithmeticOperatorType() const override;

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const override;

            /**
            * @brief Get number of output pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            /**@{*/
            virtual Pin* GetInputPin(const size_t index = 0) override;
            virtual const Pin* GetInputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetInputPins() override;
            virtual std::vector<const Pin*> GetInputPins() const override;
            /**@}*/

            /**
            * @brief Get output pin by index.
            *
            * @return Pointer of output pin at given index, nullptr if index is >= GetOutputPinCount().
            */
            /**@{*/
            virtual Pin* GetOutputPin(const size_t index = 0) override;
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all output pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetOutputPins() override;
            virtual std::vector<const Pin*> GetOutputPins() const override;
            /**@}*/

        protected:

            ArithmeticOperatorNode(Script& script);
            ArithmeticOperatorNode(const ArithmeticOperatorNode&) = delete;
            ArithmeticOperatorNode(ArithmeticOperatorNode&&) = delete;
            ~ArithmeticOperatorNode();

        private:

            InputPin<LeftType> m_inputLeft;
            InputPin<RightType> m_inputRight;
            OutputPin<OutputType> m_output;

            friend class VertexScript;
            friend class FragmentScript;

        };


        /**
        * @brief Base clas of input node, of shader script.
        */
        class CURSE_API InputNodeBase : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

        protected:

            InputNodeBase(Script& script);
            InputNodeBase(const InputNodeBase&) = delete;
            InputNodeBase(InputNodeBase&&) = delete;
            virtual ~InputNodeBase();

            friend class InputBlock;

        };


        /**
        * @brief Output node of shader script.
        */
        template<typename T>
        class InputNode : public InputNodeBase
        {

        public:

            /**
            * @brief Get number of output pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            /**@{*/
            virtual Pin* GetOutputPin(const size_t index = 0) override;
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetOutputPins() override;
            virtual std::vector<const Pin*> GetOutputPins() const override;
            /**@}*/

        private:

            InputNode(Script& script);
            InputNode(const InputNode&) = delete;
            InputNode(InputNode&&) = delete;
            ~InputNode();

            OutputPin<T> m_output;

            friend class InputBlock;

        };


        /**
        * @brief Input node block node.
        */
        class CURSE_API InputBlock
        {

        public:

            /**
            * @brief Append new input node to this block.
            */
            template<typename T>
            InputNode<T>* AppendNode();

            /**
            * @brief Removes the node from the block, disconnects all connections of node
            *        and deallocates the pointer.
            */
            void DestroyNode(InputNodeBase* inputNode);

            /**
            * @brief Get number of nodes in this block.
            */
            size_t GetNodeCount() const;

            /**
            * @brief Get number of output pins in this block.
            */
            size_t GetOutputPinCount() const;

            /**
            * @brief Get all output nodes.
            */
            /**@{*/
            std::vector<InputNodeBase*> GetNodes();
            std::vector<const InputNodeBase*> GetNodes() const;
            /**@}*/

            /**
            * @brief Compare and check the layout compability between this and target output block.
            *
            * @return True if layouts are compatible, else false.
            */
            bool CheckCompability(const OutputBlock& block) const;

        private:

            InputBlock(Script& script);
            InputBlock(const InputBlock&) = delete;
            InputBlock(InputBlock&&) = delete;
            ~InputBlock();

            Script& m_script;
            std::vector<InputNodeBase*> m_nodes;
            size_t m_pinCount;

            friend class VertexScript;
            friend class FragmentScript;

        };


        /**
        * @brief Base clas of output node, of shader script.
        */
        class CURSE_API OutputNodeBase : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

        protected:

            OutputNodeBase(Script& script);
            OutputNodeBase(const OutputNodeBase&) = delete;
            OutputNodeBase(OutputNodeBase&&) = delete;
            virtual ~OutputNodeBase();

            friend class OutputBlock;

        };


        /**
        * @brief Output node of shader script.
        */
        template<typename T>
        class OutputNode : public OutputNodeBase
        {

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
            /**@{*/
            virtual Pin* GetInputPin(const size_t index = 0) override;
            virtual const Pin* GetInputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetInputPins() override;
            virtual std::vector<const Pin*> GetInputPins() const override;
            /**@}*/

        private:

            OutputNode(Script& script);
            OutputNode(const OutputNode&) = delete;
            OutputNode(OutputNode&&) = delete;
            ~OutputNode();

            InputPin<T> m_input;

            friend class OutputBlock;

        };


        /**
        * @brief Output node block node.
        */
        class CURSE_API OutputBlock
        {

        public:

            /**
            * @brief Append new output node to this block.
            */
            template<typename T>
            OutputNode<T>* AppendNode();

            /**
            * @brief Removes the node from the block, disconnects all connections of node
            *        and deallocates the pointer.
            */
            void DestroyNode(OutputNodeBase* outputNode);

            /**
            * @brief Get number of nodes in this block.
            */
            size_t GetNodeCount() const;

            /**
            * @brief Get number of input pins in this block.
            */
            size_t GetInputPinCount() const;

            /**
            * @brief Get all output nodes.
            */
            /**@{*/
            std::vector<OutputNodeBase*> GetNodes();
            std::vector<const OutputNodeBase*> GetNodes() const;
            /**@}*/

        private:

            OutputBlock(Script& script);           
            OutputBlock(const OutputBlock&) = delete;
            OutputBlock(OutputBlock&&) = delete;
            ~OutputBlock();

            Script& m_script;
            std::vector<OutputNodeBase*> m_nodes;
            size_t m_pinCount;

            friend class VertexScript;
            friend class FragmentScript;

        };


        /**
        * @brief Output node of shader script.
        */
        class VertexOutputNode : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

            /**
            * @brief Get number of input pins.
            */
            virtual size_t GetInputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            /**@{*/
            virtual Pin* GetInputPin(const size_t index = 0) override;
            virtual const Pin* GetInputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetInputPins() override;
            virtual std::vector<const Pin*> GetInputPins() const override;
            /**@}*/

        private:

            VertexOutputNode(Script& script);
            VertexOutputNode(const VertexOutputNode&) = delete;
            VertexOutputNode(VertexOutputNode&&) = delete;
            ~VertexOutputNode();

            InputPin<Curse::Vector3f32> m_input;

            friend class VertexScript;

        };


        /**
        * @brief Base clas of uniform node, of shader script.
        */
        class CURSE_API UniformNodeBase : public Node
        {

        public:

            /**
            * @brief Get type of node.
            */
            virtual NodeType GetType() const override;

        protected:

            UniformNodeBase(Script& script);
            UniformNodeBase(const UniformNodeBase&) = delete;
            UniformNodeBase(UniformNodeBase&&) = delete;
            virtual ~UniformNodeBase();

            friend class UniformBlock;

        };


        /**
       * @brief Output node of shader script.
       */
        template<typename T>
        class UniformNode : public UniformNodeBase
        {

        public:

            /**
            * @brief Get number of output pins.
            */
            virtual size_t GetOutputPinCount() const override;

            /**
            * @brief Get input pin by index.
            *
            * @return Pointer of input pin at given index, nullptr if index is >= GetInputPinCount().
            */
            /**@{*/
            virtual Pin* GetOutputPin(const size_t index = 0) override;
            virtual const Pin* GetOutputPin(const size_t index = 0) const override;
            /**@}*/

            /**
            * @brief Get all input pins, wrapped in a vector.
            */
            /**@{*/
            virtual std::vector<Pin*> GetOutputPins() override;
            virtual std::vector<const Pin*> GetOutputPins() const override;
            /**@}*/

        private:

            UniformNode(Script& script);
            UniformNode(const UniformNode&) = delete;
            UniformNode(UniformNode&&) = delete;
            ~UniformNode();

            OutputPin<T> m_output;

            friend class UniformBlock;

        };


        /**
        * @brief Uniform node block node.
        */
        class CURSE_API UniformBlock
        {

        public:

            /**
            * @brief Append new input node to this block.
            */
            template<typename T>
            UniformNode<T>* AppendNode();

            /**
            * @brief Removes the node from the block, disconnects all connections of node
            *        and deallocates the pointer.
            */
            void DestroyNode(UniformNodeBase* uniformNode);

            /**
            * @brief Get number of nodes in this block.
            */
            size_t GetNodeCount() const;

            /**
            * @brief Get number of output pins in this block.
            */
            size_t GetOutputPinCount() const;

            /**
            * @brief Get all output nodes.
            */
            /**@{*/
            std::vector<UniformNodeBase*> GetNodes();
            std::vector<const UniformNodeBase*> GetNodes() const;
            /**@}*/

            /**
            * @brief Get id of this block.
            */
            uint32_t GetId() const;

            /**
            * @brief Compare and check the layout compability between this and target uniform block.
            *
            * @return True if layouts are compatible, else false.
            */
            bool CheckCompability(const UniformBlock& block) const;

        private:

            UniformBlock(Script& script, const uint32_t id);
            UniformBlock(const UniformBlock&) = delete;
            UniformBlock(UniformBlock&&) = delete;
            ~UniformBlock();

            Script& m_script;
            uint32_t m_id;
            std::vector<UniformNodeBase*> m_nodes;
            size_t m_pinCount;

            friend class VertexScript;
            friend class FragmentScript;

        };

    }
}

#include "ShaderNode.inl"

#endif