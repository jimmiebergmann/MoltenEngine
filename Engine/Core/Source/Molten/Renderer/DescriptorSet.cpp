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


#include "Molten/Renderer/DescriptorSet.hpp"
#include <limits>

namespace Molten
{

    // Descriptor binding implementations.
    DescriptorBinding::DescriptorBinding(
        const uint32_t id,
        RenderResource<UniformBuffer>& uniformBuffer
    ) :
        id(id),
        binding(&uniformBuffer)
    {}

    DescriptorBinding::DescriptorBinding(
        const uint32_t id,
        CombinedTextureSampler1D&& combinedTextureSampler1D
    ) :
        id(id),
        binding(combinedTextureSampler1D)
    {}

    DescriptorBinding::DescriptorBinding(
        const uint32_t id,
        CombinedTextureSampler2D&& combinedTextureSampler2D
    ) :
        id(id),
        binding(combinedTextureSampler2D)
    {}

    DescriptorBinding::DescriptorBinding(
        const uint32_t id,
        CombinedTextureSampler3D&& combinedTextureSampler3D
    ) :
        id(id),
        binding(combinedTextureSampler3D)
    {}

    // Descriptor set descriptor implementations.
    DescriptorSetDescriptor::DescriptorSetDescriptor() :
        pipeline(nullptr),
        id(std::numeric_limits<uint32_t>::max())
    {}

    DescriptorSetDescriptor::DescriptorSetDescriptor(
        RenderResource<Pipeline>* pipeline,
        const uint32_t id,
        DescriptorBinding&& binding
    ) :
        pipeline(pipeline),
        id(id),
        bindings{ binding }
    {}

    DescriptorSetDescriptor::DescriptorSetDescriptor(
        RenderResource<Pipeline>* pipeline,
        const uint32_t id,
        std::vector<DescriptorBinding>&& bindings
    ) :
        pipeline(pipeline),
        id(id),
        bindings(std::move(bindings))
    {}


    // Framed descriptor binding implementations.
    FramedDescriptorBinding::FramedDescriptorBinding(
        const uint32_t id,
        RenderResource<FramedUniformBuffer>& framedUniformBuffer
    ) :
        id(id),
        binding(&framedUniformBuffer)
    {}

    FramedDescriptorBinding::FramedDescriptorBinding(
        const uint32_t id,
        CombinedTextureSampler1D&& combinedTextureSampler1D
    ) :
        id(id),
        binding(combinedTextureSampler1D)
    {}

    FramedDescriptorBinding::FramedDescriptorBinding(
        const uint32_t id,
        CombinedTextureSampler2D&& combinedTextureSampler2D
    ) :
        id(id),
        binding(combinedTextureSampler2D)
    {}

    FramedDescriptorBinding::FramedDescriptorBinding(
        const uint32_t id,
        CombinedTextureSampler3D&& combinedTextureSampler3D
    ) :
        id(id),
        binding(combinedTextureSampler3D)
    {}


    // Framed descriptor set descriptor implementations.
    FramedDescriptorSetDescriptor::FramedDescriptorSetDescriptor() :
        pipeline(nullptr),
        id(std::numeric_limits<uint32_t>::max())
    {}

    FramedDescriptorSetDescriptor::FramedDescriptorSetDescriptor(
        RenderResource<Pipeline>* pipeline,
        const uint32_t id,
        std::vector<FramedDescriptorBinding>&& bindings
    ) :
        pipeline(pipeline),
        id(id),
        bindings(std::move(bindings))
    {}


    // Mapped descriptor binding implementations.
    MappedDescriptorBinding::MappedDescriptorBinding(
        const uint32_t index,
        const DescriptorBindingType bindingType
    ) :
        index(index),
        bindingType(bindingType)
    {}


    // Mapped descriptor set implementations.
    MappedDescriptorSet::MappedDescriptorSet() :
        index(std::numeric_limits<uint32_t>::max())
    {}

    MappedDescriptorSet::MappedDescriptorSet(const uint32_t index) :
        index(index)
    {}

    MappedDescriptorSet::MappedDescriptorSet(MappedDescriptorSet&& mappedDescriptorSet) noexcept :
        index(mappedDescriptorSet.index),
        bindings(std::move(mappedDescriptorSet.bindings))
    {
        mappedDescriptorSet.index = std::numeric_limits<uint32_t>::max();
    }

    MappedDescriptorSet& MappedDescriptorSet::operator =(MappedDescriptorSet&& mappedDescriptorSet) noexcept
    {
        index = mappedDescriptorSet.index;
        bindings = std::move(mappedDescriptorSet.bindings);
        mappedDescriptorSet.index = std::numeric_limits<uint32_t>::max();
        return *this;
    }

}