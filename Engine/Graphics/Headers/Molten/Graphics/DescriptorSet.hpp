/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#ifndef MOLTEN_GRAPHICS_DESCRIPTORSET_HPP
#define MOLTEN_GRAPHICS_DESCRIPTORSET_HPP

#include "Molten/Graphics/CombinedTextureSampler.hpp"
#include "Molten/Graphics/RenderResource.hpp"
#include "Molten/Shader/DescriptorSet.hpp"
#include <variant>
#include <vector>
#include <map>
#include <limits>

namespace Molten
{

    class Pipeline;
    class UniformBuffer;
    class FramedUniformBuffer;

    /** Descriptor set resource object. */
    class MOLTEN_GRAPHICS_API DescriptorSet
    {

    public:

        DescriptorSet() = default;
        virtual ~DescriptorSet() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        DescriptorSet(const DescriptorSet&) = delete;
        DescriptorSet(DescriptorSet&&) = delete;
        DescriptorSet& operator =(const DescriptorSet&) = delete;
        DescriptorSet& operator =(DescriptorSet&&) = delete;
        /**@}*/

    };

    /** Descriptor class of descriptor binding class. */
    struct MOLTEN_GRAPHICS_API DescriptorBindingDescriptor
    {
        using BindingVariant = std::variant< 
            RenderResource<UniformBuffer>*, // TODO: Do not use raw pointer.
            CombinedTextureSampler1D,
            CombinedTextureSampler2D,
            CombinedTextureSampler3D
        >;

        uint32_t id = std::numeric_limits<uint32_t>::max();
        BindingVariant binding;
    };

    /** Descriptor class of descriptor set class. */
    struct MOLTEN_GRAPHICS_API DescriptorSetDescriptor
    {
        RenderResource<Pipeline>* pipeline = nullptr; // TODO: Do not use raw pointer.
        uint32_t id = std::numeric_limits<uint32_t>::max();
        std::vector<DescriptorBindingDescriptor> bindings = {};
    };


    /** Framed descriptor set resource object. */
    class MOLTEN_GRAPHICS_API FramedDescriptorSet
    {

    public:

        FramedDescriptorSet() = default;
        virtual ~FramedDescriptorSet() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        FramedDescriptorSet(const FramedDescriptorSet&) = delete;
        FramedDescriptorSet(FramedDescriptorSet&&) = delete;
        FramedDescriptorSet& operator =(const FramedDescriptorSet&) = delete;
        FramedDescriptorSet& operator =(FramedDescriptorSet&&) = delete;
        /**@}*/

    };


    /** Framed descriptor class of descriptor binding class. */
    struct MOLTEN_GRAPHICS_API FramedDescriptorBinding
    {
        using BindingVariant = std::variant<
            RenderResource<FramedUniformBuffer>*, // TODO: Do not use raw pointer.
            CombinedFramedTextureSampler1D,
            CombinedFramedTextureSampler2D,
            CombinedFramedTextureSampler3D
        >;

        uint32_t id = std::numeric_limits<uint32_t>::max();
        BindingVariant binding;
    };

    /** Descriptor class of descriptor set class. */
    struct MOLTEN_GRAPHICS_API FramedDescriptorSetDescriptor
    {
        RenderResource<Pipeline>* pipeline = nullptr; // TODO: Do not use raw pointer.
        uint32_t id = std::numeric_limits<uint32_t>::max();
        std::vector<FramedDescriptorBinding> bindings = {};
    };

}

#endif
