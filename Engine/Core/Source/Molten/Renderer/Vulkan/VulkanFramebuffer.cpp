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

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanFramebuffer.hpp"

namespace Molten
{

    // Vulkan framebuffer frame implementations.
    VulkanFramebuffer::Frame::Frame() :
        texture{},
        framebuffer(VK_NULL_HANDLE)
    {}

    VulkanFramebuffer::Frame::Frame(Frame&& frame) noexcept :
        texture(std::move(frame.texture)),
        framebuffer(frame.framebuffer)
    {
        frame.framebuffer = VK_NULL_HANDLE;
    }

    VulkanFramebuffer::Frame& VulkanFramebuffer::Frame::operator = (Frame&& frame) noexcept
    {
        texture = std::move(frame.texture);
        framebuffer = frame.framebuffer;
        frame.framebuffer = VK_NULL_HANDLE;

        return *this;
    }


    // Vulkan framebuffer implementations.
    VulkanFramebuffer::VulkanFramebuffer(
        Frames&& frames,
        const Vector2ui32& dimensions,
        VkCommandPool commandPool,
        std::vector<VkCommandBuffer>&& commandBuffers
    ) :
        frames(std::move(frames)),
        dimensions(dimensions),
        commandPool(commandPool),
        commandBuffers(std::move(commandBuffers))
    {}


}

#endif