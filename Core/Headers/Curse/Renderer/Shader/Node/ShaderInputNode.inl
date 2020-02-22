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

        // Input node implementations.
        template<typename T>
        inline size_t InputNode<T>::GetOutputPinCount() const
        {
            return 1;
        }

        template<typename T>
        inline Pin* InputNode<T>::GetOutputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }
        template<typename T>
        inline const Pin* InputNode<T>::GetOutputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }

        template<typename T>
        inline std::vector<Pin*> InputNode<T>::GetOutputPins()
        {
            return { &m_output };
        }
        template<typename T>
        inline std::vector<const Pin*> InputNode<T>::GetOutputPins() const
        {
            return { &m_output };
        }

        template<typename T>
        inline InputNode<T>::InputNode(Script& script) :
            InputNodeBase(script),
            m_output(*this)
        { }

        template<typename T>
        inline InputNode<T>::~InputNode()
        { }


        // Input block implementations.
        template<typename T>
        inline InputNode<T>* InputBlock::AppendNode()
        {
            auto inputNode = new InputNode<T>(m_script);
            m_nodes.push_back(inputNode);
            m_pinCount += inputNode->GetOutputPinCount();
            return inputNode;
        }

    }

}