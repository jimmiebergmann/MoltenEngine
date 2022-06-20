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

#ifndef MOLTEN_GRAPHICS_VULKAN_VULKANDESCRIPTORSET_HPP
#define MOLTEN_GRAPHICS_VULKAN_VULKANDESCRIPTORSET_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Graphics/DescriptorSet.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanTypes.hpp"

namespace Molten
{

    /** Vulkan descriptor set. */
    class MOLTEN_GRAPHICS_API VulkanDescriptorSet final : public DescriptorSet
    {

    public:

        using Base = DescriptorSet;

        VulkanDescriptorSet() = delete;
        VulkanDescriptorSet(
            const uint32_t index,
            VkDescriptorSet descriptorSet,
            VkDescriptorPool descriptorPool);

        uint32_t index;
        VkDescriptorSet descriptorSet;
        VkDescriptorPool descriptorPool;

    };


    /** Vulkan framed descriptor set. */
    class MOLTEN_GRAPHICS_API VulkanFramedDescriptorSet : public FramedDescriptorSet
    {

    public:

        using Base = FramedDescriptorSet;

        VulkanFramedDescriptorSet() = delete;
        VulkanFramedDescriptorSet(
            const uint32_t index,
            Vulkan::DescriptorSets&& descriptorSets,
            VkDescriptorPool descriptorPool);

        uint32_t index;
        Vulkan::DescriptorSets descriptorSets;
        VkDescriptorPool descriptorPool;

    };

}

#endif

#endif
