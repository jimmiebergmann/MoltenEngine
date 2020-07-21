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

#ifndef CURSE_CORE_RENDERER_SHADER_NODE_SHADERVARIABLE_HPP
#define CURSE_CORE_RENDERER_SHADER_NODE_SHADERVARIABLE_HPP

#include "Curse/Renderer/Shader/ShaderNode.hpp"

namespace Curse::Shader
{

    class CURSE_API VariableBase : public Node
    {

    public:

        VariableBase(Script& script);
        virtual ~VariableBase() = default;

        virtual NodeType GetType() const override; 
    };


    class CURSE_API InputVariableBase : public VariableBase
    {

    public:

        InputVariableBase(Script& script);
        virtual ~InputVariableBase() = default;

    private:

        using VariableBase::GetInputPin;
        using VariableBase::GetInputPins;
        using VariableBase::GetInputPinCount;

    };


    class CURSE_API OutputVariableBase : public VariableBase
    {

    public:

        OutputVariableBase(Script& script);
        virtual ~OutputVariableBase() = default;

    private:

        using VariableBase::GetOutputPin;
        using VariableBase::GetOutputPins;
        using VariableBase::GetOutputPinCount;

    };


    /**
    * @brief The variable class is a general input/output pin variable.
    */
    template<typename TDataType>
    class Variable : public VariableBase
    {

    public:

        using Base = VariableBase;
        using DataType = TDataType;

        Variable(Script& script);

        virtual size_t GetInputPinCount() const override;
        virtual size_t GetOutputPinCount() const override;
        virtual Pin* GetInputPin(const size_t index = 0) override;
        virtual const Pin* GetInputPin(const size_t index = 0) const override;
        virtual std::vector<Pin*> GetInputPins() override;
        virtual std::vector<const Pin*> GetInputPins() const override;
        virtual Pin* GetOutputPin(const size_t index = 0) override;
        virtual const Pin* GetOutputPin(const size_t index = 0) const override;
        virtual std::vector<Pin*> GetOutputPins() override;
        virtual std::vector<const Pin*> GetOutputPins() const override;

    private:

        InputPin<DataType> m_inputPin;
        OutputPin<DataType> m_outputPin;

    };


    /**
    * @brief Input variables only consist of an input pin.
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

    private:

        OutputPin<DataType> m_outputPin;

    };


    /**
    * @brief Input variables only consist of an output pin.
    */
    template<typename TDataType>
    class OutputVariable : public OutputVariableBase
    {

    public:

        using Base = OutputVariableBase;
        using DataType = TDataType;

        OutputVariable(Script& script);

        virtual size_t GetInputPinCount() const override;
        virtual Pin* GetInputPin(const size_t index = 0) override;
        virtual const Pin* GetInputPin(const size_t index = 0) const override;
        virtual std::vector<Pin*> GetInputPins() override;
        virtual std::vector<const Pin*> GetInputPins() const override;        

    private:

        InputPin<DataType> m_inputPin;

    };

}

#include "Curse/Renderer/Shader/Node/ShaderVariable.inl"

#endif