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

    template<typename T>
    inline Reference<T> Reference<T>::Create()
    {
        return { new ControlObject(new T) };
    }

    template<typename T>
    inline Reference<T>::Reference() :
        m_controlObject(nullptr)
    {
    }

    template<typename T>
    template<typename U>
    inline Reference<T>::Reference(const Reference<U>& ref) :
        m_controlObject(ref.m_controlObject)
    {
        if (m_controlObject)
        {
            m_controlObject.m_counter++;
        }
    }

    template<typename T>
    template<typename U>
    inline Reference<T>::Reference(Reference<U>&& ref) :
        m_controlObject(reinterpret_cast<ControlObject*>(ref.m_controlObject))
    {
        static_assert(std::is_same<T, U>::value || std::is_base_of<T, U>::value,
            "T is not same or base of U");

        ref.m_controlObject = nullptr;

        if (m_controlObject)
        {
            m_controlObject->m_counter++;
        }
    }

    template<typename T>
    inline Reference<T>::~Reference()
    {
        if (m_controlObject)
        {
            m_controlObject->m_counter--;
            if(!m_controlObject->m_counter && m_controlObject->m_object)
            {
                delete m_controlObject;
            }
            
        }
    }

    template<typename T>
    inline size_t Reference<T>::GetUseCount() const
    {
        return m_controlObject ? m_controlObject->m_counter : 0;
    }

    template<typename T>
    inline Reference<T>::Reference(ControlObject* controlObject) :
        m_controlObject(controlObject)
    {
    }


    template<typename T>
    inline Reference<T>::ControlObject::ControlObject(T* object) :
        m_object(object),
        m_counter(0)
    {
    }

    template<typename T>
    inline Reference<T>::ControlObject::~ControlObject()
    {
        if (m_object)
        {
            delete m_object;
        }
    }

}