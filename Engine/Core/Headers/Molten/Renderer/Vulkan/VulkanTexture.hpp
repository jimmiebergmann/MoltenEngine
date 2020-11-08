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

#ifndef MOLTEN_CORE_RENDERER_VULKANTEXTURE_HPP
#define MOLTEN_CORE_RENDERER_VULKANTEXTURE_HPP

#include "Molten/Renderer/Texture.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include "Molten/Renderer/Vulkan/VulkanHeaders.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanImage.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanImageSampler.hpp"

namespace Molten
{

    class VulkanRenderer;

    class MOLTEN_API VulkanTexture : public Texture
    {

    private:

        VulkanTexture(
            Vulkan::Image&& image,
            Vulkan::ImageSampler imageSampler);

        Vulkan::Image image;
        Vulkan::ImageSampler imageSampler;

        friend class VulkanRenderer;

    };

}

#endif

#endif