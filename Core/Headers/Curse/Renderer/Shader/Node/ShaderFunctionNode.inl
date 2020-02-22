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

namespace Curse
{

    namespace Shader
    {

        // Function node base implementations.
        inline NodeType FunctionNodeBase::GetType() const
        {
            return NodeType::Function;
        }

        inline FunctionNodeBase::FunctionNodeBase(Script& script) :
            Node(script)
        { }

        inline FunctionNodeBase::~FunctionNodeBase()
        { }


        // Function node implementations.
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline FunctionType FunctionNode<_FunctionType, OutputType, InputTypes...>::GetFunctionType() const
        {
            return _FunctionType;
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline size_t FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPinCount() const
        {
            return InputPinCount;
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline size_t FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPinCount() const
        {
            return OutputPinCount;
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline Pin* FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPin(const size_t index)
        {
            if (index >= m_inputs.size())
            {
                return nullptr;
            }
            return m_inputs[index].get();
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline const Pin* FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPin(const size_t index) const
        {
            if (index >= m_inputs.size())
            {
                return nullptr;
            }
            return m_inputs[index].get();
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline std::vector<Pin*> FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPins()
        {
            std::vector<Pin*> pins;
            for (const auto& pin : m_inputs)
            {
                pins.push_back(pin.get());
            }

            return pins;
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline std::vector<const Pin*> FunctionNode<_FunctionType, OutputType, InputTypes...>::GetInputPins() const
        {
            std::vector<const Pin*> pins;
            for (const auto& pin : m_inputs)
            {
                pins.push_back(pin.get());
            }

            return pins;
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline Pin* FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return m_output.get();
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline const Pin* FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return m_output.get();
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline std::vector<Pin*> FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPins()
        {
            auto output = m_output.get();
            if (!output)
            {
                return {};
            }
            return { output };
        }
        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline std::vector<const Pin*> FunctionNode<_FunctionType, OutputType, InputTypes...>::GetOutputPins() const
        {
            auto output = m_output.get();
            if (!output)
            {
                return {};
            }
            return { output };
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline FunctionNode<_FunctionType, OutputType, InputTypes...>::FunctionNode(Script& script) :
            FunctionNodeBase(script),
            m_output(std::make_unique<OutputPin<OutputType> >(*this))
        {
            InitInputPin<InputTypes...>();
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        inline FunctionNode<_FunctionType, OutputType, InputTypes...>::~FunctionNode()
        {
        }

        template<FunctionType _FunctionType, typename OutputType, typename ... InputTypes>
        template<typename CurrentType, typename ...>
        inline void FunctionNode<_FunctionType, OutputType, InputTypes...>::InitInputPin(const size_t index)
        {
            if (index == InputPinCount)
            {
                return;
            }

            auto inputPin = std::make_unique<InputPin<CurrentType> >(*this);
            m_inputs[index] = std::move(inputPin);

            InitInputPin<CurrentType>(index + 1);
        }

    }

}