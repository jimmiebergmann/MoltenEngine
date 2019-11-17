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
    template<typename ... Args>
    inline Reference<T> Reference<T>::Create(Args&& ... args)
    {
        return { new Controller(new T(std::forward<Args>(args)...)) };
    }

    template<typename T>
    inline Reference<T>::Reference() :
        m_controller(nullptr)
    {
    }

    template<typename T>
    Reference<T>::Reference(T* object) :
        m_controller(object ? new Controller(object) : nullptr)
    {
    }

    template<typename T>
    Reference<T>::Reference(T* object, Deleter deleter) :
        m_controller(object ? new Controller(object, deleter) : nullptr)
    {
    }

    template<typename T>
    inline Reference<T>::Reference(const Reference& ref) :
        m_controller(nullptr)
    {
        m_controller = ref.m_controller;
        if (m_controller)
        {
            ++m_controller->m_counter;
        }
    }

    template<typename T>
    template<typename U>
    inline Reference<T>::Reference(const Reference<U>& ref) :
        m_controller(nullptr)
    {
        static_assert(std::is_same<T, U>::value || std::is_base_of<T, U>::value || std::is_base_of<U, T>::value,
            "Data types T and U are not related.");

        m_controller = reinterpret_cast<Controller*>(ref.m_controller);
        if (m_controller)
        {
            ++m_controller->m_counter;
        }
    }

    template<typename T>
    inline Reference<T>& Reference<T>::operator = (const Reference& ref)
    {
        Controller* old = m_controller;
        m_controller = ref.m_controller;

        if (old != m_controller)
        {
            if (m_controller)
            {
                ++m_controller->m_counter;
            }
            if (old)
            {
                const size_t counter = --old->m_counter;
                if (!counter && old->m_object)
                {
                    delete old;
                }
            }
        }

        return *this;
    }

    template<typename T>
    template<typename U>
    inline Reference<T>& Reference<T>::operator = (const Reference<U>& ref)
    {
        static_assert(std::is_same<T, U>::value || std::is_base_of<T, U>::value || std::is_base_of<U, T>::value,
            "Data types T and U are not related.");

        Controller* old = m_controller;
        m_controller = reinterpret_cast<Controller*>(ref.m_controller);

        if (old != m_controller)
        {
            if (m_controller)
            {
                ++m_controller->m_counter;
            }
            if (old)
            {
                const size_t counter = --old->m_counter;
                if (!counter && old->m_object)
                {
                    delete old;
                }
            }
        }

        return *this;
    }

    template<typename T>
    inline Reference<T>::Reference(Reference&& ref) :
        m_controller(nullptr)
    {
        Controller* old = m_controller;
        m_controller = ref.m_controller;
        ref.m_controller = nullptr;

        if (old && old != m_controller)
        {
            const size_t counter = --old->m_counter;
            if (!counter && old->m_object)
            {
                delete old;
            }
        }
    }

    template<typename T>
    template<typename U>
    inline Reference<T>::Reference(Reference<U>&& ref) :
        m_controller(nullptr)
    {
        static_assert(std::is_same<T, U>::value || std::is_base_of<T, U>::value || std::is_base_of<U, T>::value,
            "Data types T and U are not related.");

        Controller* old = m_controller;
        m_controller = reinterpret_cast<Controller*>(ref.m_controller);
        ref.m_controller = nullptr;

        if (old && old != m_controller)
        {
            const size_t counter = --old->m_counter;
            if (!counter && old->m_object)
            {
                delete old;
            }
        }
    }

    template<typename T>
    inline Reference<T>& Reference<T>::operator = (Reference&& ref)
    {
        Controller* old = m_controller;
        m_controller = ref.m_controller;
        ref.m_controller = nullptr;

        if (old && old != m_controller)
        {
            const size_t counter = --old->m_counter;
            if (!counter && old->m_object)
            {
                delete old;
            }
        }

        return *this;
    }

    template<typename T>
    template<typename U>
    inline Reference<T>& Reference<T>::operator = (Reference<U>&& ref)
    {
        static_assert(std::is_same<T, U>::value || std::is_base_of<T, U>::value || std::is_base_of<U, T>::value,
            "Data types T and U are not related.");

        Controller* old = m_controller;
        m_controller = reinterpret_cast<Controller*>(ref.m_controller);
        ref.m_controller = nullptr;

        if (old && old != m_controller)
        {
            const size_t counter = --old->m_counter;
            if (!counter && old->m_object)
            {
                delete old;
            }
        }

        return *this;
    }

    template<typename T>
    inline T& Reference<T>::operator *() const
    {
        if (m_controller)
        {
            return *m_controller->m_object;
        }

        throw Exception("Accessing null ptr.");
    }

    template<typename T>
    inline T* Reference<T>::operator->() const
    {
        return m_controller ? m_controller->m_object : nullptr;
    }

    template<typename T>
    inline Reference<T>::~Reference()
    {
        if (m_controller)
        {
            size_t counter = --m_controller->m_counter;
            if (!counter && m_controller->m_object)
            {
                delete m_controller;
            }
        }
    }

    template<typename T>
    inline T* Reference<T>::Get() const
    {
        return m_controller ? m_controller->m_object : nullptr;
    }

    template<typename T>
    inline size_t Reference<T>::GetUseCount() const
    {
        return m_controller ? static_cast<size_t>(m_controller->m_counter) : 0;
    }

    template<typename T>
    inline Reference<T>::Reference(Controller* controlObject) :
        m_controller(controlObject)
    {
    }


    template<typename T>
    inline Reference<T>::Controller::Controller(T* object) :
        m_object(object),
        m_counter(1),
        m_deleter([](T * object)
            {
                delete object;
            })
    {
    }

    template<typename T>
    inline Reference<T>::Controller::Controller(T* object, Deleter deleter) :
        m_object(object),
        m_counter(1),
        m_deleter(deleter)
    {
    }

    template<typename T>
    inline Reference<T>::Controller::~Controller()
    {
        if (m_object)
        {
            m_deleter(m_object);
        }
    }

}