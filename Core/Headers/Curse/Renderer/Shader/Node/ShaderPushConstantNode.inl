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

#include <algorithm>

namespace Curse
{

    namespace Shader
    {

        // Uniform node implementations.
        template<typename T>
        inline size_t PushConstantNode<T>::GetOutputPinCount() const
        {
            return 1;
        }

        template<typename T>
        inline Pin* PushConstantNode<T>::GetOutputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }
        template<typename T>
        inline const Pin* PushConstantNode<T>::GetOutputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }

        template<typename T>
        inline std::vector<Pin*> PushConstantNode<T>::GetOutputPins()
        {
            return { &m_output };
        }
        template<typename T>
        inline std::vector<const Pin*> PushConstantNode<T>::GetOutputPins() const
        {
            return { &m_output };
        }

        template<typename T>
        inline PushConstantNode<T>::PushConstantNode(Script& script) :
            PushConstantNodeBase(script),
            m_output(*this)
        { }

        template<typename T>
        inline PushConstantNode<T>::~PushConstantNode()
        { }


        // Uniform array node implementations.
        template<typename T, size_t Size>
        inline bool PushConstantArrayNode<T, Size>::IsArray() const
        {
            return true;
        }

        template<typename T, size_t Size>
        inline size_t PushConstantArrayNode<T, Size>::GetOutputPinCount() const
        {
            return Size;
        }

        template<typename T, size_t Size>
        inline Pin* PushConstantArrayNode<T, Size>::GetOutputPin(const size_t index)
        {
            if (index >= Size)
            {
                return nullptr;
            }
            return m_outputs[index].get();
        }
        template<typename T, size_t Size>
        inline const Pin* PushConstantArrayNode<T, Size>::GetOutputPin(const size_t index) const
        {
            if (index >= Size)
            {
                return nullptr;
            }
            return m_outputs[index].get();
        }

        template<typename T, size_t Size>
        inline std::vector<Pin*> PushConstantArrayNode<T, Size>::GetOutputPins()
        {
            std::vector<Pin*> pins(Size);
            for (size_t i = 0; i < Size; i++)
            {
                pins[i] = m_outputs[i].get();
            }
            return std::move(pins);
        }
        template<typename T, size_t Size>
        inline std::vector<const Pin*> PushConstantArrayNode<T, Size>::GetOutputPins() const
        {
            std::vector<const Pin*> pins(Size);
            for (size_t i = 0; i < Size; i++)
            {
                pins[i] = m_outputs[i].get();
            }
            return std::move(pins);
        }

        template<typename T, size_t Size>
        inline PushConstantArrayNode<T, Size>::PushConstantArrayNode(Script& script) :
            PushConstantNodeBase(script)
        {
            InitOutputPins();
        }

        template<typename T, size_t Size>
        inline PushConstantArrayNode<T, Size>::~PushConstantArrayNode()
        { }

        template<typename T, size_t Size>
        inline void PushConstantArrayNode<T, Size>::InitOutputPins()
        {
            for (size_t i = 0; i < Size; i++)
            {
                m_outputs[i] = std::move(std::make_unique<OutputPin<T> >(*this));
            }
        }


        // Push constant block implementations.
        template<typename T>
        inline PushConstantNode<T>* PushConstantBlock::AppendNode()
        {
            auto node = new PushConstantNode<T>(m_script);
            m_nodes.push_back(node);
            m_pinCount += node->GetOutputPinCount();
            return node;
        }

        template<typename T, size_t Size>
        PushConstantArrayNode<T, Size>* PushConstantBlock::AppendNode()
        {
            auto arrayNode = new PushConstantArrayNode<T, Size>(m_script);
            m_nodes.push_back(arrayNode);
            m_pinCount += arrayNode->GetOutputPinCount();
            return arrayNode;
        }

    }

}