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

    // Plain pointer implementation.
    template<typename T>
    template<typename ... Args>
    inline Pointer<T> Pointer<T>::Create(Args&& ... args)
    {
        return { new T(std::forward<Args>(args)...) };
    }

    template<typename T>
    inline Pointer<T>::Pointer() :
        m_rawPointer(nullptr)
    { }

    template<typename T>
    inline Pointer<T>::Pointer(TypePtr rawPointer) :
        m_rawPointer(rawPointer)
    { }

    template<typename T>
    template<typename U>
    inline Pointer<T>::Pointer(Pointer<U>&& pointer) :
        m_rawPointer(pointer.m_rawPointer)
    {
        pointer.m_rawPointer = nullptr;
    }

    template<typename T>
    inline Pointer<T>::~Pointer()
    {
        if (m_rawPointer)
        {
            delete m_rawPointer;
        }
    }

    template<typename T>
    template<typename U>
    inline Pointer<T>& Pointer<T>::operator = (Pointer<U>&& pointer)
    {
        m_rawPointer = pointer.m_rawPointer;
        pointer.m_rawPointer = nullptr;
        return *this;
    }

    template<typename T>
    inline typename Pointer<T>::TypeRef Pointer<T>::operator *() const
    {
        return *m_rawPointer;
    }

    template<typename T>
    inline typename Pointer<T>::TypePtr Pointer<T>::operator->() const
    {
        return m_rawPointer;
    }

    template<typename T>
    inline typename Pointer<T>::TypePtr Pointer<T>::Get() const
    {
        return m_rawPointer;
    }


    // Array pointer implementation.
    template<typename T>
    inline Pointer<T[]> Pointer<T[]>::Create(const size_t size)
    {
        return { new T[size] };
    }

    template<typename T>
    inline Pointer<T[]>::Pointer() :
        m_rawPointer(nullptr)
    { }

    template<typename T>
    inline Pointer<T[]>::Pointer(TypePtr rawPointer) :
        m_rawPointer(rawPointer)
    { }

    template<typename T>
    template<typename U>
    inline Pointer<T[]>::Pointer(Pointer<U[]>&& pointer) :
        m_rawPointer(pointer.m_rawPointer)
    {
        pointer.m_rawPointer = nullptr;
    }

    template<typename T>
    inline Pointer<T[]>::~Pointer()
    {
        if (m_rawPointer)
        {
            delete[] m_rawPointer;
        }
    }

    template<typename T>
    template<typename U>
    inline Pointer<T[]>& Pointer<T[]>::operator = (Pointer<U[]>&& pointer)
    {
        m_rawPointer = pointer.m_rawPointer;
        pointer.m_rawPointer = nullptr;
        return *this;
    }

    template<typename T>
    inline typename Pointer<T[]>::TypeRef Pointer<T[]>::operator *() const
    {
        return *m_rawPointer;
    }

    template<typename T>
    inline typename Pointer<T[]>::TypeRef Pointer<T[]>::operator[](const size_t index) const
    {
        return m_rawPointer[index];
    }

    template<typename T>
    inline typename Pointer<T[]>::TypePtr Pointer<T[]>::operator->() const
    {
        return m_rawPointer;
    }

    template<typename T>
    inline typename Pointer<T[]>::TypePtr Pointer<T[]>::Get() const
    {
        return m_rawPointer;
    }

}