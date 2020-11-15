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

#ifndef MOLTEN_CORE_RENDERER_SHADER_GENERATORNODE_HPP
#define MOLTEN_CORE_RENDERER_SHADER_GENERATORNODE_HPP

#include "Molten/Core.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Molten
{
    class Logger;
}
namespace Molten::Shader::Visual
{
    class Pin;
    class Node;
    class Script;
}

namespace Molten::Shader
{

    struct GeneratorOutputVariable;
    using GeneratorOutputVariablePointer = std::shared_ptr<GeneratorOutputVariable>;

    struct GeneratorInputVariable;

    class GeneratorNode;
    using GeneratorNodePointer = std::shared_ptr<GeneratorNode>;

    class GeneratorOutputNode;
    using GeneratorOutputNodePointer = std::shared_ptr<GeneratorOutputNode>;


    struct MOLTEN_API GeneratorOutputVariable
    {
        GeneratorOutputVariable(
            const Visual::Pin* pin,
            std::string&& name = "");

        const Visual::Pin* pin;
        std::string name;
    };


    struct MOLTEN_API GeneratorInputVariable
    {
        GeneratorInputVariable(const Visual::Pin& pin);

        const Visual::Pin& pin;
        GeneratorOutputVariable* connection;
    };


    class MOLTEN_API GeneratorNode
    {

    public:

        using InputVariableContainer = std::vector<GeneratorInputVariable>;
        using OutputVariablePointerContainer = std::vector<GeneratorOutputVariablePointer>;
        using InputVariableIterator = typename InputVariableContainer::iterator;

        explicit GeneratorNode(const Visual::Node& node);
        virtual ~GeneratorNode() = default;

        GeneratorOutputVariablePointer CreateOutputVariable(const Visual::Pin* pin, std::string&& name = "");

        const Visual::Node& GetNode() const;

        const InputVariableContainer& GetInputVariables() const;

        const OutputVariablePointerContainer& GetOutputVariables() const;

        GeneratorInputVariable* GetNextInputVariable();

    protected:

        static InputVariableContainer CreateInputVariables(const Visual::Node& node);

        const Visual::Node& m_node;
        InputVariableContainer m_inputVariables;
        InputVariableIterator m_inputVariableIterator;
        OutputVariablePointerContainer m_outputVariables;

    };


    class MOLTEN_API GeneratorOutputNode : public GeneratorNode
    {

    public:

        explicit GeneratorOutputNode(const Visual::Node& node);

        void AddOutputPin(const Visual::Pin* pin, const std::string& name);

    };


}

#endif