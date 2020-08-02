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

namespace Molten
{

    namespace Shader
    {

        // Uniform node implementations.
        template<typename T>
        inline size_t UniformNode<T>::GetOutputPinCount() const
        {
            return 1;
        }

        template<typename T>
        inline Pin* UniformNode<T>::GetOutputPin(const size_t index)
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }
        template<typename T>
        inline const Pin* UniformNode<T>::GetOutputPin(const size_t index) const
        {
            if (index != 0)
            {
                return nullptr;
            }
            return &m_output;
        }

        template<typename T>
        inline std::vector<Pin*> UniformNode<T>::GetOutputPins()
        {
            return { &m_output };
        }
        template<typename T>
        inline std::vector<const Pin*> UniformNode<T>::GetOutputPins() const
        {
            return { &m_output };
        }

        template<typename T>
        inline UniformNode<T>::UniformNode(Script& script) :
            UniformNodeBase(script),
            m_output(*this)
        { }

        template<typename T>
        inline UniformNode<T>::~UniformNode()
        { }


        // Uniform array node implementations.
        template<typename T, size_t Size>
        inline bool UniformArrayNode<T, Size>::IsArray() const
        {
            return true;
        }

        template<typename T, size_t Size>
        inline size_t UniformArrayNode<T, Size>::GetOutputPinCount() const
        {
            return Size;
        }

        template<typename T, size_t Size>
        inline Pin* UniformArrayNode<T, Size>::GetOutputPin(const size_t index)
        {
            if (index >= Size)
            {
                return nullptr;
            }
            return m_outputs[index].get();
        }
        template<typename T, size_t Size>
        inline const Pin* UniformArrayNode<T, Size>::GetOutputPin(const size_t index) const
        {
            if (index >= Size)
            {
                return nullptr;
            }
            return m_outputs[index].get();
        }

        template<typename T, size_t Size>
        inline std::vector<Pin*> UniformArrayNode<T, Size>::GetOutputPins()
        {
            std::vector<Pin*> pins(Size);
            for (size_t i = 0; i < Size; i++)
            {
                pins[i] = m_outputs[i].get();
            }
            return std::move(pins);
        }
        template<typename T, size_t Size>
        inline std::vector<const Pin*> UniformArrayNode<T, Size>::GetOutputPins() const
        {
            std::vector<const Pin*> pins(Size);
            for (size_t i = 0; i < Size; i++)
            {
                pins[i] = m_outputs[i].get();
            }
            return std::move(pins);
        }

        template<typename T, size_t Size>
        inline UniformArrayNode<T, Size>::UniformArrayNode(Script& script) :
            UniformNodeBase(script)
        {
            InitOutputPins();
        }

        template<typename T, size_t Size>
        inline UniformArrayNode<T, Size>::~UniformArrayNode()
        { }

        template<typename T, size_t Size>
        inline void UniformArrayNode<T, Size>::InitOutputPins()
        {
            for (size_t i = 0; i < Size; i++)
            {
                m_outputs[i] = std::move(std::make_unique<OutputPin<T> >(*this));
            }
        }


        // Uniform block implementations.
        template<typename T>
        inline UniformNode<T>* UniformBlock::AppendNode()
        {
            auto uniformNode = new UniformNode<T>(m_script);
            m_nodes.push_back(uniformNode);
            m_pinCount += uniformNode->GetOutputPinCount();
            return uniformNode;
        }

        template<typename T, size_t Size>
        UniformArrayNode<T, Size>* UniformBlock::AppendNode()
        {
            auto uniformArrayNode = new UniformArrayNode<T, Size>(m_script);
            m_nodes.push_back(uniformArrayNode);
            m_pinCount += uniformArrayNode->GetOutputPinCount();
            return uniformArrayNode;
        }

    }

}