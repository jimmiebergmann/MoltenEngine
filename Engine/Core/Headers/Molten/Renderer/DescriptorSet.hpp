/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_RENDERER_DESCRIPTORSET_HPP
#define MOLTEN_CORE_RENDERER_DESCRIPTORSET_HPP

#include "Molten/Renderer/Shader.hpp"
#include "Molten/Renderer/CombinedTextureSampler.hpp"
#include "Molten/Renderer/RenderResource.hpp"
#include <variant>
#include <map>

namespace Molten
{

    class Pipeline;
    class UniformBuffer;
    class FramedUniformBuffer;

    /** Enumerator of descriptor binding types. */
    enum class DescriptorBindingType : uint8_t
    {
        Sampler1D,
        Sampler2D,
        Sampler3D,
        UniformBuffer
    };

    /** Descriptor set resource object. */
    class MOLTEN_API DescriptorSet
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
    struct MOLTEN_API DescriptorBinding
    {

        using BindingVariant = std::variant< 
            RenderResource<UniformBuffer>*,
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
    struct MOLTEN_API DescriptorSetDescriptor
    {

        DescriptorSetDescriptor();
        DescriptorSetDescriptor(
            RenderResource<Pipeline>* pipeline,
            const uint32_t setId,
            DescriptorBinding&& binding
        );
        DescriptorSetDescriptor(
            RenderResource<Pipeline>* pipeline,
            const uint32_t id,
            std::vector<DescriptorBinding>&& bindings
        );

        RenderResource<Pipeline>* pipeline;
        uint32_t id;
        std::vector<DescriptorBinding> bindings;

    };

    /** Framed descriptor set resource object. */
    class MOLTEN_API FramedDescriptorSet
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
    struct MOLTEN_API FramedDescriptorBinding
    {

        using BindingVariant = std::variant<
            RenderResource<FramedUniformBuffer>*,
            CombinedTextureSampler<1>,
            CombinedTextureSampler<2>,
            CombinedTextureSampler<3>
        >;

        FramedDescriptorBinding(
            const uint32_t id,
            RenderResource<FramedUniformBuffer>& framedUniformBuffer);
        FramedDescriptorBinding(
            const uint32_t id,
            CombinedTextureSampler1D&& combinedTextureSampler1D);
        FramedDescriptorBinding(
            const uint32_t id,
            CombinedTextureSampler2D&& combinedTextureSampler2D);
        FramedDescriptorBinding(
            const uint32_t id,
            CombinedTextureSampler3D&& combinedTextureSampler3D);

        uint32_t id;
        BindingVariant binding;

    };

    /** Descriptor class of descriptor set class. */
    struct MOLTEN_API FramedDescriptorSetDescriptor
    {

        FramedDescriptorSetDescriptor();
        FramedDescriptorSetDescriptor(
            RenderResource<Pipeline>* pipeline,
            const uint32_t id,
            std::vector<FramedDescriptorBinding>&& bindings
        );

        RenderResource<Pipeline>* pipeline;
        uint32_t id;
        std::vector<FramedDescriptorBinding> bindings;

    };

    /** Data structures for mapping arbitrary indices to actually used indices in compiler shader. */
    /**@{*/
    struct MOLTEN_API MappedDescriptorBinding
    {
        MappedDescriptorBinding(
            const uint32_t index,
            const DescriptorBindingType bindingType);

        uint32_t index;
        DescriptorBindingType bindingType;
    };

    using MappedDescriptorBindings = std::map<uint32_t, MappedDescriptorBinding>;

    struct MOLTEN_API MappedDescriptorSet
    {
        MappedDescriptorSet();
        MappedDescriptorSet(const uint32_t index);
        ~MappedDescriptorSet() = default;

        MappedDescriptorSet(const MappedDescriptorSet& mappedDescriptorSet) = default;
        MappedDescriptorSet(MappedDescriptorSet&& mappedDescriptorSet) noexcept;
        MappedDescriptorSet& operator =(const MappedDescriptorSet& mappedDescriptorSet) = default;
        MappedDescriptorSet& operator =(MappedDescriptorSet&& mappedDescriptorSet) noexcept;

        uint32_t index;
        MappedDescriptorBindings bindings;
    };

    using MappedDescriptorSets = std::map<uint32_t, MappedDescriptorSet>;
    /**@}*/

}

#endif
