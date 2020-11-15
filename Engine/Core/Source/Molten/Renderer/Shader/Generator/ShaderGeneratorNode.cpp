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

#include "Molten/Renderer/Shader/Generator/ShaderGeneratorNode.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderNode.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderPin.hpp"

namespace Molten::Shader
{

    // Generator input variable implementations.
    GeneratorOutputVariable::GeneratorOutputVariable(const Visual::Pin* pin, std::string&& name) :
        pin(pin), name(name)
    {}


    // Generator output variable implementations.
    GeneratorInputVariable::GeneratorInputVariable(const Visual::Pin& pin) :
        pin(pin),
        connection(nullptr)
    {}


    // Generator node implementations.
    GeneratorNode::GeneratorNode(const Visual::Node& node) :
        m_node(node),
        m_inputVariables(CreateInputVariables(node)),
        m_inputVariableIterator{ m_inputVariables.begin() },
        m_outputVariables(m_node.GetOutputPinCount(), nullptr)
    {}

    GeneratorOutputVariablePointer GeneratorNode::CreateOutputVariable(const Visual::Pin* pin, std::string&& name)
    {
        auto outputPins = m_node.GetOutputPins();
        for (size_t i = 0; i < outputPins.size(); i++)
        {
            if (pin == outputPins[i])
            {
                auto outputVariable = std::make_shared<GeneratorOutputVariable>(pin, std::move(name));
                m_outputVariables[i] = outputVariable;
                return outputVariable;
            }
        }

        MOLTEN_DEBUG_ASSERT(false, "Passed pin is not contained by this node.");
        return nullptr;
    }

    const Visual::Node& GeneratorNode::GetNode() const
    {
        return m_node;
    }

    const GeneratorNode::InputVariableContainer& GeneratorNode::GetInputVariables() const
    {
        return m_inputVariables;
    }

    const GeneratorNode::OutputVariablePointerContainer& GeneratorNode::GetOutputVariables() const
    {
        return m_outputVariables;
    }

    GeneratorInputVariable* GeneratorNode::GetNextInputVariable()
    {
        if (m_inputVariableIterator != m_inputVariables.end())
        {
            GeneratorInputVariable* inputVariable = &(*m_inputVariableIterator);
            ++m_inputVariableIterator;
            return inputVariable;
        }
        return nullptr;
    }

    GeneratorNode::InputVariableContainer GeneratorNode::CreateInputVariables(const Visual::Node& node)
    {
        auto inputPins = node.GetInputPins();
        InputVariableContainer inputVariables;
        inputVariables.reserve(inputPins.size());
        for (auto* pin : inputPins)
        {
            inputVariables.push_back({ *pin });
        }
        return inputVariables;
    }


    // Generator output node implementations.
    GeneratorOutputNode::GeneratorOutputNode(const Visual::Node& node) :
        GeneratorNode(node)
    {
        m_outputVariables.clear();
    }

    void GeneratorOutputNode::AddOutputPin(const Visual::Pin* pin, const std::string& name)
    {
        m_outputVariables.push_back(std::make_shared<GeneratorOutputVariable>(pin, std::string(name)));
    }


}