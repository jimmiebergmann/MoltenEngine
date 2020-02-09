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

#include "ShaderFunctions.hpp"

namespace Curse
{

    namespace Shader
    {

        template<typename T>
        inline ConstantNode<T>* Script::CreateConstantNode(const T& value)
        {
            static_assert(DataTypeTrait<T>::Supported,
                "Specified data type of constant node is not supported.");

            auto constant = new ConstantNode<T>(*this, value);
            m_allNodes.insert(constant);
            return constant;
        }

        template<typename Func>
        inline auto Script::CreateFunctionNode()
        {
            static_assert(std::is_base_of<FunctionNodeBase, Func>::value,
                "Specified template parameter is not base of FunctionNode.");

            auto func = new Func(*this);
            m_allNodes.insert(func);
            return func;
        }

        inline UniformBlock* Script::CreateUniformBlock(const size_t binding)
        {
            if (m_uniformBlocks.find(binding) != m_uniformBlocks.end())
            {
                return nullptr;
            }
            
            auto block = new UniformBlock(*this);
            m_uniformBlocks.insert({ binding, block });
            return block;
        }

        /*template<typename T>
        inline UniformNode<T>* Script::CreateUniformNode(const size_t location)
        {
            if (m_uniformNodes.find(location) != m_uniformNodes.end())
            {
                return nullptr;
            }

            auto uniformNode = new UniformNode<T>(*this, location);
            m_allNodes.insert(uniformNode);
            m_uniformNodes.insert({ location, uniformNode });
            return uniformNode;
        }*/

        template<typename T>
        inline OperatorNode<T>* Script::CreateOperatorNode(const Operator op)
        {
            auto opNode = new OperatorNode<T>(*this, op);
            m_allNodes.insert(opNode);
            return opNode;
        }

        template<typename T>
        inline VaryingInNode<T>* Script::CreateVaryingInNode()
        {
            auto varying = new VaryingInNode<T>(*this);
            m_allNodes.insert(varying);
            m_varyingInNodes.push_back(varying);
            return varying;
        }

        template<typename T>
        inline VaryingOutNode<T>* Script::CreateVaryingOutNode()
        {
            auto varying = new VaryingOutNode<T>(*this);
            m_allNodes.insert(varying);
            m_varyingOutNodes.push_back(varying);
            return varying;
        }

    }

}