/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#include "Molten/Shader/Visual/VisualShaderNode.hpp"

namespace Molten::Shader::Visual
{

    // Shader script node implementations.
    Script& Node::GetScript()
    {
        return m_script;
    }
    const Script& Node::GetScript() const
    {
        return m_script;
    }

    size_t Node::GetInputPinCount() const
    {
        return 0;
    }

    size_t Node::GetOutputPinCount() const
    {
        return 0;
    }

    Pin* Node::GetInputPin(const size_t)
    {
        return nullptr;
    }
    const Pin* Node::GetInputPin(const size_t) const
    {
        return nullptr;
    }

    std::vector<Pin*> Node::GetInputPins()
    {
        return {};
    }
    std::vector<const Pin*> Node::GetInputPins() const
    {
        return {};
    }

    Pin* Node::GetOutputPin(const size_t)
    {
        return nullptr;
    }
    const Pin* Node::GetOutputPin(const size_t) const
    {
        return nullptr;
    }

    std::vector<Pin*> Node::GetOutputPins()
    {
        return {};
    }
    std::vector<const Pin*> Node::GetOutputPins() const
    {
        return {};
    }

    Node::Node(Script& script) :
        m_script(script)
    { }

}