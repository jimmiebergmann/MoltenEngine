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

#ifndef MOLTEN_CORE_MEMORY_POINTER_HPP
#define MOLTEN_CORE_MEMORY_POINTER_HPP

#include "Molten/System/Exception.hpp"
#include <atomic>
#include <utility>
#include <type_traits>
#include <functional>

namespace Molten
{


    /**
    * @brief Unique smart pointer class, making it possible to control an objects lifetime.
    *        
    */
    template<typename T>
    class Pointer
    {

    public:

        /**
        * @brief Data type of pointer.
        */
        using Type = T;
        using TypePtr = Type*;
        using TypeRef = Type&;

        /**
        * @brief Function for constructing a new smart pointer object.
        *
        * @param args[in] Arguments are being passed to constructor of T.
        */
        template<typename ... Args>
        static Pointer<T> Create(Args&& ... args);

        /**
        * @brief Constructor.
        */
        Pointer();

        /**
        * @brief Constructing smart pointer by passing an object.
        */
        explicit Pointer(TypePtr rawPointer);

        /**
        * @brief Deleted copy constructor.
        */
        Pointer(const Pointer& pointer) = delete;

        /**
        * @brief Deleted copy constructor.
        */
        template<typename U>
        Pointer(const Pointer<U>& pointer) = delete;

        /**
        * @brief Move constructor, from another smart pointer type.
        *        Used for moving base class smart pointer objects.
        */
        template<typename U>
        Pointer(Pointer<U>&& pointer);

        /**
        * @brief Destructor.
        */
        virtual ~Pointer();

        /**
        * @brief Deleted assignment operator.
        */
        Pointer<T>& operator = (const Pointer&) = delete;

        /**
        * @brief Deleted assigment operator.
        */
        template<typename U>
        Pointer<T>& operator = (const Pointer<U>&) = delete; 

        /**
        * @brief Move Assignment operator.
        */
        template<typename U>
        Pointer<T>& operator = (Pointer<U>&& pointer);

        /**
        * @brief Dereference operator.
        *        Access reference data via this operator.
        */
        TypeRef operator *() const;

        /**
        * @brief Member access operator.
        *        Access pointer data via this operator.
        */
        TypePtr operator->() const;

        /**
        * @brief Access pointer data via this function.
        */
        TypePtr Get() const;

    protected:

        TypePtr m_rawPointer;

        template<typename U> friend class Pointer;
    };


    template<typename T>
    class Pointer<T[]>
    {

    public:

        /**
        * @brief Data type of pointer.
        */
        using Type = T;
        using TypePtr = Type*;
        using TypeRef = Type&;

        /**
        * @brief Function for constructing a new smart pointer object.
        *
        * @param args[in] Arguments are being passed to constructor of T.
        */
        static Pointer<T[]> Create(const size_t size);

        /**
        * @brief Constructor.
        */
        Pointer();

        /**
        * @brief Constructing smart pointer by passing an object.
        */
        explicit Pointer(TypePtr rawPointer);

        /**
        * @brief Deleted copy constructor.
        */
        Pointer(const Pointer& pointer) = delete;

        /**
        * @brief Deleted copy constructor.
        */
        template<typename U>
        Pointer(const Pointer<U[]>& pointer) = delete;

        /**
        * @brief Move constructor, from another smart pointer type.
        *        Used for moving base class smart pointer objects.
        */
        template<typename U>
        Pointer(Pointer<U[]>&& pointer);

        /**
        * @brief Destructor.
        */
        virtual ~Pointer();

        /**
        * @brief Deleted assigment operator.
        */
        Pointer<T[]>& operator = (const Pointer&) = delete;

        /**
        * @brief Deleted assigment operator.
        */
        template<typename U>
        Pointer<T[]>& operator = (const Pointer<U[]>&) = delete;

        /**
        * @brief Move Assignment operator.
        */
        template<typename U>
        Pointer<T[]>& operator = (Pointer<U[]>&& pointer);

        /**
        * @brief Dereference operator.
        *        Access reference data via this operator.
        */
        TypeRef operator *() const;

        /**
        * @brief Member access operator.
        *        Access array data via this operator.
        */
        TypeRef operator[](const size_t index) const;

        /**
        * @brief Member access operator.
        *        Access pointer data via this operator.
        */
        TypePtr operator->() const;

        /**
        * @brief Access pointer data via this function.
        */
        TypePtr Get() const;

    protected:

        TypePtr m_rawPointer;

        template<typename U> friend class Pointer;
    };

    /**
    * @brief A shorter name for smart pointer class.
    */
    template<typename T>
    using Ptr = Pointer<T>;

}

#include "Molten/Memory/Pointer.inl"

#endif
