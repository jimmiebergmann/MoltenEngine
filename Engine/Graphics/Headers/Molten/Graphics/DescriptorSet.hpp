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
    struct MOLTEN_GRAPHICS_API DescriptorBinding
    {

        using BindingVariant = std::variant< 
            RenderResource<UniformBuffer>*, // TODO: Do not use raw pointer.
            CombinedTextureSampler1D,
            CombinedTextureSampler2D,
            CombinedTextureSampler3D
        >;

        DescriptorBinding(
            const uint32_t id,
            RenderResource<UniformBuffer>& uniformBuffer);
        DescriptorBinding(
            const uint32_t id,
            CombinedTextureSampler1D&& combinedTextureSampler1D);
        DescriptorBinding(
            const uint32_t id,
            CombinedTextureSampler2D&& combinedTextureSampler2D);
        DescriptorBinding(
            const uint32_t id,
            CombinedTextureSampler3D&& combinedTextureSampler3D);

        uint32_t id;
        BindingVariant binding;

    };

    /** Descriptor class of descriptor set class. */
    struct MOLTEN_GRAPHICS_API DescriptorSetDescriptor
    {

        DescriptorSetDescriptor();
        DescriptorSetDescriptor(
            RenderResource<Pipeline>* pipeline, // TODO: Do not use raw pointer.
            const uint32_t id,
            DescriptorBinding&& binding
        );
        DescriptorSetDescriptor(
            RenderResource<Pipeline>* pipeline,
            const uint32_t id,
            std::vector<DescriptorBinding>&& bindings
        );

        RenderResource<Pipeline>* pipeline; // TODO: Do not use raw pointer.
        uint32_t id;
        std::vector<DescriptorBinding> bindings;

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

        FramedDescriptorBinding(
            const uint32_t id,
            RenderResource<FramedUniformBuffer>& framedUniformBuffer);
        FramedDescriptorBinding(
            const uint32_t id,
            CombinedFramedTextureSampler1D&& combinedFramedTextureSampler1D);
        FramedDescriptorBinding(
            const uint32_t id,
            CombinedFramedTextureSampler2D&& combinedFramedTextureSampler2D);
        FramedDescriptorBinding(
            const uint32_t id,
            CombinedFramedTextureSampler3D&& combinedFramedTextureSampler3D);

        uint32_t id;
        BindingVariant binding;

    };

    /** Descriptor class of descriptor set class. */
    struct MOLTEN_GRAPHICS_API FramedDescriptorSetDescriptor
    {

        FramedDescriptorSetDescriptor();
        FramedDescriptorSetDescriptor(
            RenderResource<Pipeline>* pipeline, // TODO: Do not use raw pointer.
            const uint32_t id,
            FramedDescriptorBinding&& binding
        );
        FramedDescriptorSetDescriptor(
            RenderResource<Pipeline>* pipeline, // TODO: Do not use raw pointer.
            const uint32_t id,
            std::vector<FramedDescriptorBinding>&& bindings
        );

        RenderResource<Pipeline>* pipeline; // TODO: Do not use raw pointer.
        uint32_t id;
        std::vector<FramedDescriptorBinding> bindings;

    };

}

#endif
