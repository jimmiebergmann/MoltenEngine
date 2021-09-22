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

#ifndef MOLTEN_CORE_RENDERER_VULKANUNIFORMBUFFER_HPP
#define MOLTEN_CORE_RENDERER_VULKANUNIFORMBUFFER_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/UniformBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceBuffer.hpp"
#include <vector>

namespace Molten
{

    /** Vulkan uniform buffer. */
    class MOLTEN_API VulkanUniformBuffer final : public UniformBuffer
    {

    public:

        using Base = UniformBuffer;

        VulkanUniformBuffer() = delete;
        explicit VulkanUniformBuffer(Vulkan::DeviceBuffer && deviceBuffer) noexcept;

        Vulkan::DeviceBuffer deviceBuffer;

    };


    /** Vulkan framed uniform buffer. */
    class MOLTEN_API VulkanFramedUniformBuffer final : public FramedUniformBuffer
    {

    public:

        using Base = FramedUniformBuffer;

        VulkanFramedUniformBuffer() = delete;
        explicit VulkanFramedUniformBuffer(std::vector<Vulkan::DeviceBuffer> && deviceBuffers) noexcept;

        std::vector<Vulkan::DeviceBuffer> deviceBuffers;

    };

}

#endif

#endif