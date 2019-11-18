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

#ifndef CURSE_CORE_MEMORY_REFERENCE_HPP
#define CURSE_CORE_MEMORY_REFERENCE_HPP

#include "Curse/System/Exception.hpp"
#include <atomic>
#include <utility>
#include <type_traits>
#include <functional>

namespace Curse
{

    /**
    * @brief Smart shared pointer class, called "reference" in Curse.
    */
    template<typename T>
    class Reference
    {

    public:

        /**
        * @brief Data type of reference.
        */
        using Type = T;
        using TypePtr = Type*;
        using TypeRef = Type&;

        /**
        * @brief Deleter type.
        */
        using Deleter = std::function<void(TypePtr object)>;

        /**
        * @brief Function for constructing a new reference object.
        *        Use the constructor directly to set a custom deleter.
        *
        * @param args[in] Arguments being passed to constructor of T.
        */
        template<typename ... Args>
        static Reference<T> Create(Args&& ... args);

        /**
        * @brief Constructor.
        */
        Reference();

        /**
        * @brief Constructing reference by passing an object.
        */
        Reference(TypePtr object);

        /**
        * @brief Constructing reference by passing an object and deleter function.
        */
        Reference(TypePtr object, Deleter deleter);

        /**
        * @brief Copy constructor.
        */
        Reference(const Reference& ref);

        /**
        * @brief Copy constructor, from another reference type.
        *        Used for assignment of base class reference objects.
        */
        template<typename U>
        Reference(const Reference<U>& ref);

        /**
        * @brief Assigment operator.
        */
        Reference<T>& operator = (const Reference& ref);

        /**
        * @brief Assigment operator, from another reference type.
        *        Used for assignment of base class reference objects.
        */
        template<typename U>
        Reference<T>& operator = (const Reference<U>& ref);

        /**
        * @brief Move constructor.
        */
        Reference(Reference&& ref);

        /**
        * @brief Move constructor, from another reference type.
        *        Used for moving base class reference objects.
        */
        template<typename U>
        Reference(Reference<U>&& ref);

        /**
        * @brief Move Assignment operator.
        */
        Reference<T>& operator = (Reference&& ref);

        /**
        * @brief Move Assignment operator, from another reference type.
        *        Used for moving base class reference objects.
        */
        template<typename U>
        Reference<T>& operator = (Reference<U>&& ref);

        /**
        * @brief Dereference operator.
        *        Access reference data via this operator.
        */
        TypeRef operator *() const;

        /**
        * @brief Member access operator.
        *        Access reference data via this operator.
        */
        TypePtr operator->() const;

        /**
        * @brief Destructor.
        */
        virtual ~Reference();

        /**
        * @brief Access reference data via this function.
        */
        TypePtr Get() const;

        /**
        * @brief Get use count of object.
        */
        size_t GetUseCount() const;

    protected:

        struct Controller
        {
            Controller(TypePtr object, Deleter deleter);
            Controller(const Controller&) = delete;
            Controller(Controller&&) = delete;
            ~Controller();

            TypePtr m_object;
            std::atomic_size_t m_counter;
            Deleter m_deleter;
        };

        Reference(Controller * controlObject);

        Controller* m_controller;

        template<typename U> friend class Reference;
    };

    /**
    * @brief Smart shared pointer array class, called "reference" in Curse.
    */
    template<typename T>
    class Reference<T[]>
    {

    public:

        /**
        * @brief Data type of reference.
        */
        using Type = std::remove_reference_t<decltype(*std::declval<T[]>())>;
        using TypePtr = Type*;
        using TypeRef = Type&;

        /**
        * @brief Deleter type.
        */
        using Deleter = std::function<void(TypePtr object)>;

        /**
        * @brief Function for constructing a new array reference object.
        *        Use the constructor directly to set a custom deleter.
        *
        * @param size[in] Size of new array object.
        */
        static Reference<T[]> Create(const size_t size);

        /**
        * @brief Constructor.
        */
        Reference();

        /**
        * @brief Constructing reference by passing an object.
        */
        Reference(TypePtr object);

        /**
        * @brief Constructing reference by passing an object and deleter function.
        */
        Reference(TypePtr object, Deleter deleter);

        /**
        * @brief Copy constructor.
        */
        Reference(const Reference& ref);

        /**
        * @brief Copy constructor, from another reference type.
        *        Used for assignment of base class reference objects.
        */
        template<typename U>
        Reference(const Reference<U[]>& ref);

        /**
        * @brief Assigment operator.
        */
        Reference<T[]>& operator = (const Reference& ref);

        /**
        * @brief Assigment operator, from another reference type.
        *        Used for assignment of base class reference objects.
        */
        template<typename U>
        Reference<T[]>& operator = (const Reference<U[]>& ref);

        /**
        * @brief Move constructor.
        */
        Reference(Reference&& ref);

        /**
        * @brief Move constructor, from another reference type.
        *        Used for moving base class reference objects.
        */
        template<typename U>
        Reference(Reference<U[]>&& ref);

        /**
        * @brief Move Assignment operator.
        */
        Reference<T[]>& operator = (Reference&& ref);

        /**
        * @brief Move Assignment operator, from another reference type.
        *        Used for moving base class reference objects.
        */
        template<typename U>
        Reference<T[]>& operator = (Reference<U[]>&& ref);

        /**
        * @brief Dereference operator.
        *        Access reference data via this operator.
        */
        TypeRef operator *() const;

        /**
        * @brief Member access operator.
        *        Access reference data via this operator.
        */
        TypePtr operator->() const;

        /**
        * @brief Member access operator.
        *        Access reference data element via this operator.
        */
        TypeRef operator[](const size_t index) const;

        /**
        * @brief Destructor.
        */
        virtual ~Reference();

        /**
        * @brief Access reference data via this function.
        */
        TypePtr Get() const;

        /**
        * @brief Get use count of object.
        */
        size_t GetUseCount() const;

    protected:

        struct Controller
        {
            Controller(TypePtr object, Deleter deleter);
            Controller(const Controller&) = delete;
            Controller(Controller&&) = delete;
            ~Controller();

            TypePtr m_object;
            std::atomic_size_t m_counter;
            Deleter m_deleter;
        };

        Reference(Controller* controlObject);

        Controller* m_controller;

        template<typename U> friend class Reference;

    };


    /**
    * @brief A shorter name for Reference class.
    */
    template<typename T>
    using Ref = Reference<T>;

}

#include "Reference.inl"

#endif
