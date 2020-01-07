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

    namespace Material
    {

        template<typename T>
        inline OperatorNode<T>* Script::CreateOperatorNode(const Operator op)
        {
            auto opNode = new OperatorNode<T>(*this, op);
            m_allNodes.insert(opNode);
            return opNode;
        }

        template<typename T>
        inline OutputNode<T>* Script::CreateOutputNode()
        {
            static_assert(DataTypeTrait<T>::Supported,
                "Specified data type of output node is not supported.");

            auto output = new OutputNode<T>(*this);
            m_allNodes.insert(output);
            m_outputNodes.push_back(output);
            return output;
        }

        template<VaryingType T>
        inline VaryingNode<T>* Script::CreateVaryingNode()
        {
            auto varying = new VaryingNode<T>(*this);
            m_allNodes.insert(varying);
            m_varyingNodes.push_back(varying);
            return varying;
        }

    }

}