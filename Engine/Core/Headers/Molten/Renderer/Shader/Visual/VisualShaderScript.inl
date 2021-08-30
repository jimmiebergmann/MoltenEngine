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

#include "Molten/Renderer/Shader/Visual/VisualShaderFunctions.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderOperators.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderComposites.hpp"

namespace Molten::Shader::Visual
{

    // Vertex shader script implementations.
    template<typename TDataType>
    Constant<TDataType>& VertexScript::CreateConstant(const TDataType& value)
    {
        auto constant = new Constant<TDataType>(*this, value);
        m_nodes.insert(constant);
        return *constant;
    }

    template<typename TFunction>
    TFunction& VertexScript::CreateFunction()
    {
        // CHECK FUNCTION TYPE.
        static_assert(std::is_base_of<FunctionBase, TFunction>::value,
            "Specified template parameter is not base of FunctionNode.");

        auto func = new TFunction(*this);
        m_nodes.insert(func);
        return *func;
    }

    template<typename TOperator>
    TOperator& VertexScript::CreateOperator()
    {
        static_assert(std::is_base_of<OperatorBase, TOperator>::value, "Specified template parameter is not base of OperatorBase.");
        static_assert(Operators::Trait<TOperator>::Supported, "Passed operator node is not supported.");

        auto op = new TOperator(*this);
        m_nodes.insert(op);
        return *op;
    }

    template<typename TComposite>
    TComposite& VertexScript::CreateComposite()
    {
        static_assert(std::is_base_of<CompositeBase, TComposite>::value, "Specified template parameter is not base of OperatorBase.");

        auto comp = new TComposite(*this);
        m_nodes.insert(comp);
        return *comp;
    }


    // Fragment shader script implementations.
    template<typename TDataType>
    Constant<TDataType>& FragmentScript::CreateConstant(const TDataType& value)
    {
        auto constant = new Constant<TDataType>(*this, value);
        m_nodes.insert(constant);
        return *constant;
    }

    template<typename TFunction>
    TFunction& FragmentScript::CreateFunction()
    {
        // CHECK FUNCTION TYPE.
        static_assert(std::is_base_of<FunctionBase, TFunction>::value,
            "Specified template parameter is not base of FunctionNode.");

        auto func = new TFunction(*this);
        m_nodes.insert(func);
        return *func;
    }

    template<typename TOperator>
    TOperator& FragmentScript::CreateOperator()
    {
        static_assert(std::is_base_of<OperatorBase, TOperator>::value, "Specified template parameter is not base of OperatorBase.");
        static_assert(Operators::Trait<TOperator>::Supported, "Passed operator node is not supported.");

        auto op = new TOperator(*this);
        m_nodes.insert(op);
        return *op;
    }

    template<typename TComposite>
    TComposite& FragmentScript::CreateComposite()
    {
        static_assert(std::is_base_of<CompositeBase, TComposite>::value, "Specified template parameter is not base of OperatorBase.");

        auto comp = new TComposite(*this);
        m_nodes.insert(comp);
        return *comp;
    }

}