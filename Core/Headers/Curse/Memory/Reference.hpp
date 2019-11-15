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

#include "Curse/Types.hpp"
#include <atomic>

namespace Curse
{

    /**
    * @brief Smart shared pointer class, being called "reference" in Curse.
    */
    template<typename T>
    class Reference
    {

    public:

        /**
        * @brief Data type of reference.
        */
        using Type = T;

        /**
        * @brief Function for constructing a new reference object.
        */
        static Reference<T> Create();

        /**
        * @brief Constructor.
        */
        Reference();

        /**
        * @brief Copy constructor. Increments the counter.
        */
        template<typename U>
        Reference(const Reference<U>& ref);

        /**
        * @brief Move constructor. Counter is not incremented.
        */
        template<typename U>
        Reference(Reference<U>&& ref);

        /**
        * @brief Destructor.
        */
        ~Reference();

        /**
        * @brief Get use count of object.
        */
        size_t GetUseCount() const;

    private:

        struct ControlObject
        {
            ControlObject(T* object);
            ControlObject(const ControlObject&) = delete;
            ControlObject(ControlObject&&) = delete;
            ~ControlObject();

            T* m_object;
            size_t m_counter;
        };

        Reference(ControlObject * controlObject);

        ControlObject* m_controlObject;

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
