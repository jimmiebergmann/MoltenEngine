/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_SYSTEM_RESOURCE_HPP
#define MOLTEN_CORE_SYSTEM_RESOURCE_HPP

#include "Molten/Core.hpp"
#include <memory>
#include <functional>

namespace Molten
{

    /** Resource type, used for managing different types of resources, such as GPU resources.
     *  This type provides a simple interface for creating user managed resources,
     *  without forcing the user to give back the resource to the creator.
     *  It's possible to provide a deleter function,
     *  which makes it possible for the resource creator to run custom cleanup code at resource destruction.
     *
     *  Underlying type is std::unique_ptr.
     *  All methods from std::unique_ptr are exposed.
     *
     * @see Resource
     * @see DerivedResource
     */
    template<typename TResourceType, typename TDeleterType = TResourceType>
    class Resource : public std::unique_ptr<TResourceType, std::function<void(TDeleterType*)>>
    {

    public:

        using DeleterCallback = std::function<void(TDeleterType&)>;

        /** Helper functions for allocating a new resource with args passed to resource type's constuctor.
         *  Provided deleter function is attached as deleter.
         */
        template<typename ... TResourceArgs>
        static Resource Create(const DeleterCallback& deleter, TResourceArgs ... args);

        /** Helper functions for allocating a new resource with args passed to resource type's constuctor.
         *  Default deleter function is attached.
         */
        template<typename ... TResourceArgs>
        static Resource CreateDefault(TResourceArgs ... args);

        /** Default constructor. Setting underlying pointer and deleter to nullptr. */
        Resource();

        /** Default destructor. */
        ~Resource() = default;

        /** Manual resource creation. The user should pass a pointer to new resource type. */
        /**@{*/
        explicit Resource(TResourceType* resource);
        Resource(TResourceType* resource, const DeleterCallback& deleter);
        /**@}*/

        /** Move constructors. Underlying pointer and deleter is moved to this resource. */
        /**@{*/
        Resource(Resource&& resource) noexcept;
        template<typename TOtherResourceType, typename TOtherDeleter>
        Resource(Resource<TOtherResourceType, TOtherDeleter>&& resource) noexcept;
        /**@}*/

        /** Move operators. Underlying pointer and deleter is moved to this resource. */
        /**@{*/
        Resource& operator = (Resource&& resource) noexcept;
        template<typename TOtherResourceType, typename TOtherDeleter>
        Resource& operator = (Resource<TOtherResourceType, TOtherDeleter>&& resource) noexcept;
        /**@}*/

        /** Deleted copy operations. */
        /**@{*/
        Resource(const Resource&) = delete;
        Resource& operator =(const Resource&) = delete;
        /**@}*/
    };


    /** Derived resource type. Use this type for creating derived resource types. */
    template<typename TResourceType>
    using DerivedResource = Resource<TResourceType, typename TResourceType::Base>;


}

#include "Molten/System/Resource.inl"

#endif
