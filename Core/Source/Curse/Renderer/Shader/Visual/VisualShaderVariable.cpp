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

#include "Curse/Renderer/Shader/Visual/VisualShaderVariable.hpp"

namespace Curse::Shader::Visual
{

    // Variable base implementations.
    VariableBase::VariableBase(Script& script) :
        Node(script)
    {}

    NodeType VariableBase::GetType() const
    {
        return NodeType::Variable;
    }


    // Input variable base implementations.
    InputVariableBase::InputVariableBase(Script& script) :
        VariableBase(script)
    {}


    // Output variable base implementations.
    OutputVariableBase::OutputVariableBase(Script& script) :
        VariableBase(script)
    {}


    // Constant variable base implementations.
    ConstantVariableBase::ConstantVariableBase(Script& script) :
        VariableBase(script)
    {}

}