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

        

        //inline UniformBlock* Script::CreateUniformBlock(const uint32_t id)
        //{
        //    if (m_uniformBlocks.find(id) != m_uniformBlocks.end())
        //    {
        //        return nullptr;
        //    }
        //    
        //    auto block = new UniformBlock(*this, id);
        //    m_uniformBlocks.insert({ id, block });
        //    return block;
        //}

        

        //template<typename T>
        //inline VaryingInNode<T>* Script::CreateVaryingInNode(const uint32_t id)
        //{
        //    if (m_varyingInNodes.find(id) != m_varyingInNodes.end())
        //    {
        //        return nullptr;
        //    }

        //    auto varying = new VaryingInNode<T>(*this, id);
        //    m_allNodes.insert(varying);
        //    m_varyingInNodes.insert({ id, varying });
        //    return varying;
        //}

        //template<typename T>
        //inline VaryingOutNode<T>* Script::CreateVaryingOutNode(const uint32_t id)
        //{
        //    if (m_varyingOutNodes.find(id) != m_varyingOutNodes.end())
        //    {
        //        return nullptr;
        //    }

        //    auto varying = new VaryingOutNode<T>(*this, id);
        //    m_allNodes.insert(varying);
        //    m_varyingOutNodes.insert({ id, varying });
        //    return varying;
        //}

        // Vertex shader script implementations.
        template<typename T>
        inline ConstantNode<T>* VertexScript::CreateConstantNode(const T& value)
        {
            // CHECK TYPE.
            auto constant = new ConstantNode<T>(*this, value);
            m_allNodes.insert(constant);
            return constant;
        }

        template<typename Func>
        inline Func* VertexScript::CreateFunctionNode()
        {
            // CHECK FUNCTION TYPE.
            static_assert(std::is_base_of<FunctionNodeBase, Func>::value,
                "Specified template parameter is not base of FunctionNode.");

            auto func = new Func(*this);
            m_allNodes.insert(func);
            return func;
        }

        template<typename Op>
        inline Op* VertexScript::CreateOperatorNode()
        {
            // CHECK OPERATOR TYPE.
            static_assert(std::is_base_of<OperatorNodeBase, Op>::value,
                "Specified template parameter is not base of OperatorNodeBase.");

            auto op = new Op(*this);
            m_allNodes.insert(op);
            return op;
        }


        // Fragment shader script implementations.
        template<typename T>
        inline ConstantNode<T>* FragmentScript::CreateConstantNode(const T& value)
        {
            // CHECK TYPE.
            auto constant = new ConstantNode<T>(*this, value);
            m_allNodes.insert(constant);
            return constant;
        }

        template<typename Func>
        inline Func* FragmentScript::CreateFunctionNode()
        {
            // CHECK FUNCTION TYPE.
            static_assert(std::is_base_of<FunctionNodeBase, Func>::value,
                "Specified template parameter is not base of FunctionNode.");

            auto func = new Func(*this);
            m_allNodes.insert(func);
            return func;
        }

        template<typename Op>
        inline Op* FragmentScript::CreateOperatorNode()
        {
            // CHECK OPERATOR TYPE.
            static_assert(std::is_base_of<OperatorNodeBase, Op>::value,
                "Specified template parameter is not base of OperatorNodeBase.");

            auto op = new Op(*this);
            m_allNodes.insert(op);
            return op;
        }

    }

}