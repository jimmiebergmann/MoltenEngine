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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERVARIABLE_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERVARIABLE_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderNode.hpp"

namespace Molten::Shader::Visual
{

    /** Enumerator of variable types. */
    enum class VariableType : uint32_t
    {
        Constant,
        Input,
        Output
    };

    
    /** Base class of all variable types. */
    class MOLTEN_API VariableBase : public Node
    {

    public:

        VariableBase(Script& script);
        virtual ~VariableBase() = default;

        virtual NodeType GetType() const override; 

        /** @brief Get data type of variable. */
        virtual VariableDataType GetDataType() const = 0;

        /** @brief Get variable type. */
        virtual VariableType GetVariableType() const = 0;

        /** Get sizes of variable in bytes. Kind of like sizeof(...). */
        virtual size_t GetSizeOf() const = 0;

    };


    /** Base class of input variable type. */
    class MOLTEN_API InputVariableBase : public VariableBase
    {

    public:

        InputVariableBase(Script& script);
        virtual ~InputVariableBase() = default;

    private:

        using VariableBase::GetInputPin;
        using VariableBase::GetInputPins;
        using VariableBase::GetInputPinCount;

    };


    /** Base class of ouput variable type. */
    class MOLTEN_API OutputVariableBase : public VariableBase
    {

    public:

        OutputVariableBase(Script& script);
        virtual ~OutputVariableBase() = default;

    private:

        using VariableBase::GetOutputPin;
        using VariableBase::GetOutputPins;
        using VariableBase::GetOutputPinCount;

    };


    /** Base class of constant variable type. */
    class MOLTEN_API ConstantVariableBase : public VariableBase
    {

    public:

        ConstantVariableBase(Script& script);
        virtual ~ConstantVariableBase() = default;

    private:

        using VariableBase::GetInputPin;
        using VariableBase::GetInputPins;
        using VariableBase::GetInputPinCount;

    };


    /**
     * Visual shader script input variable node.
     * Input variables only consist of an output pin.
     */
    template<typename TDataType>
    class InputVariable : public InputVariableBase
    {

    public:

        using Base = InputVariableBase;
        using DataType = TDataType;

        InputVariable(Script& script);

        virtual size_t GetOutputPinCount() const override;
        virtual Pin* GetOutputPin(const size_t index = 0) override;
        virtual const Pin* GetOutputPin(const size_t index = 0) const override;
        virtual std::vector<Pin*> GetOutputPins() override;
        virtual std::vector<const Pin*> GetOutputPins() const override;

        /** @brief Get data type of variable. */
        virtual VariableDataType GetDataType() const override;

        /** @brief Get variable type. */
        virtual VariableType GetVariableType() const override;

        /** Get sizes of variable in bytes. Kind of like sizeof(...). */
        virtual size_t GetSizeOf() const override;

    private:

        /** Copy and move operations are not allowed. */
        /**@{*/
        InputVariable(const InputVariable& copy) = delete;
        InputVariable(const InputVariable&& move) = delete;
        InputVariable& operator =(const InputVariable& copy) = delete;
        InputVariable& operator =(const InputVariable&& move) = delete;
        /**@*/

        OutputPin<DataType> m_outputPin;

    };


    /** 
     * Visual shader script output variable node.
     * Output variables only consist of an input pin. 
     */
    template<typename TDataType>
    class OutputVariable : public OutputVariableBase
    {

    public:

        using Base = OutputVariableBase;
        using DataType = TDataType;

        OutputVariable(Script& script);
        OutputVariable(Script& script, const DataType& defaultValue);

        virtual size_t GetInputPinCount() const override;
        virtual Pin* GetInputPin(const size_t index = 0) override;
        virtual const Pin* GetInputPin(const size_t index = 0) const override;
        virtual std::vector<Pin*> GetInputPins() override;
        virtual std::vector<const Pin*> GetInputPins() const override;   

        /** @brief Get data type of variable. */
        virtual VariableDataType GetDataType() const override;

        /** @brief Get variable type. */
        virtual VariableType GetVariableType() const override;

        /** Get sizes of variable in bytes. Kind of like sizeof(...). */
        virtual size_t GetSizeOf() const override;

    private:

        /** Copy and move operations are not allowed. */
        /**@{*/
        OutputVariable(const OutputVariable& copy) = delete;
        OutputVariable(const OutputVariable&& move) = delete;
        OutputVariable& operator =(const OutputVariable& copy) = delete;
        OutputVariable& operator =(const OutputVariable&& move) = delete;
        /**@*/

        InputPin<DataType> m_inputPin;

    };

    /**
     * Visual shader script constant variable node.
     * The stored value is not constant and can be changed at any time,
     * but it's used as a constant in the generated shader code.
     */
    template<typename TDataType>
    class ConstantVariable : public ConstantVariableBase
    {

    public:

        using Base = InputVariableBase;
        using DataType = TDataType;

        ConstantVariable(Script& script, const DataType& value);
        ConstantVariable(Script& script, DataType&& value);

        virtual size_t GetOutputPinCount() const override;
        virtual Pin* GetOutputPin(const size_t index = 0) override;
        virtual const Pin* GetOutputPin(const size_t index = 0) const override;
        virtual std::vector<Pin*> GetOutputPins() override;
        virtual std::vector<const Pin*> GetOutputPins() const override;

        /** @brief Get data type of variable. */
        virtual VariableDataType GetDataType() const override;

        /** @brief Get variable type. */
        virtual VariableType GetVariableType() const override;

        /** Get sizes of variable in bytes. Kind of like sizeof(...). */
        virtual size_t GetSizeOf() const override;

        /** @brief Get constant value. */
        const DataType& GetValue() const;

        /** @brief Set constant value. */
        /**@{*/
        void SetValue(const DataType& value);
        void SetValue(DataType&& value);
        /**@}*/

    private:

        /** Copy and move operations are not allowed. */
        /**@{*/
        ConstantVariable(const ConstantVariable& copy) = delete;
        ConstantVariable(const ConstantVariable&& move) = delete;
        ConstantVariable& operator =(const ConstantVariable& copy) = delete;
        ConstantVariable& operator =(const ConstantVariable&& move) = delete;
        /**@*/

        OutputPin<DataType> m_outputPin;
        DataType m_value;
    };

}

#include "Molten/Renderer/Shader/Visual/VisualShaderVariable.inl"

#endif