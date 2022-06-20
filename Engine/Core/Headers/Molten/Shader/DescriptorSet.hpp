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

#ifndef MOLTEN_CORE_SHADER_DESCRIPTORSET_HPP
#define MOLTEN_CORE_SHADER_DESCRIPTORSET_HPP

#include "Molten/Core.hpp"
#include <map>

namespace Molten
{

    /** Enumerator of descriptor binding types. */
    enum class DescriptorBindingType : uint8_t
    {
        Sampler1D,
        Sampler2D,
        Sampler3D,
        UniformBuffer
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
