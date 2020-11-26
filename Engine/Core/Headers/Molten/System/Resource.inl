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

namespace Molten
{

    template<typename TResourceType, typename TDeleterType>
    template<typename ... TResourceArgs>
    Resource<TResourceType, TDeleterType> Resource<TResourceType, TDeleterType>::Create(const DeleterCallback& deleter, TResourceArgs ... args)
    {
        return Resource<TResourceType, TDeleterType>{
            new TResourceType(std::forward<TResourceArgs>(args)...),
            deleter
        };
    }

    template<typename TResourceType, typename TDeleterType>
    template<typename ... TResourceArgs>
    Resource<TResourceType, TDeleterType> Resource<TResourceType, TDeleterType>::CreateDefault(TResourceArgs ... args)
    {
        return Resource<TResourceType, TDeleterType>{
            new TResourceType(std::forward<TResourceArgs>(args)...)
        };
    }

    template<typename TResourceType, typename TDeleterType>
    Resource<TResourceType, TDeleterType>::Resource() :
        std::unique_ptr<TResourceType, std::function<void(TDeleterType*)>>{ nullptr, std::default_delete<TDeleterType>() }
    {}

    template<typename TResourceType, typename TDeleterType>
    Resource<TResourceType, TDeleterType>::Resource(TResourceType* resource) :
        std::unique_ptr<TResourceType, std::function<void(TDeleterType*)>>{ resource, std::default_delete<TDeleterType>() }
    {}

    template<typename TResourceType, typename TDeleterType>
    Resource<TResourceType, TDeleterType>::Resource(TResourceType* resource, const DeleterCallback& deleter) :
        std::unique_ptr<TResourceType, std::function<void(TDeleterType*)>>{
            resource,
            [deleter](TDeleterType* ptr)
            {
                deleter(*ptr);
                delete ptr;
            }
        }
    {}

    template<typename TResourceType, typename TDeleterType>
    Resource<TResourceType, TDeleterType>::Resource(Resource&& resource) noexcept :
        std::unique_ptr<TResourceType, std::function<void(TDeleterType*)>>(std::move(resource))
    {}

    template<typename TResourceType, typename TDeleterType>
    template<typename TOtherResourceType, typename TOtherDeleter>
    Resource<TResourceType, TDeleterType>::Resource(Resource<TOtherResourceType, TOtherDeleter>&& resource) noexcept :
        std::unique_ptr<TResourceType, std::function<void(TDeleterType*)>>(std::move(resource))
    {}

    template<typename TResourceType, typename TDeleterType>
    Resource<TResourceType, TDeleterType>& Resource<TResourceType, TDeleterType>::operator = (Resource&& resource) noexcept
    {
        static_cast<std::unique_ptr<TResourceType, std::function<void(TDeleterType*)>>&>(*this) = std::move(resource);
        return *this;
    }

    template<typename TResourceType, typename TDeleterType>
    template<typename TOtherResourceType, typename TOtherDeleter>
    Resource<TResourceType, TDeleterType>& Resource<TResourceType, TDeleterType>::operator = (Resource<TOtherResourceType, TOtherDeleter>&& resource) noexcept
    {
        static_cast<std::unique_ptr<TResourceType, std::function<void(TDeleterType*)>>&>(*this) = std::move(resource);
        return *this;
    }

}