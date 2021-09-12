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

#ifndef MOLTEN_CORE_RENDERER_VULKANTEXTURE_HPP
#define MOLTEN_CORE_RENDERER_VULKANTEXTURE_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Texture.hpp"
#include "Molten/Renderer/Vulkan/VulkanHeaders.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceImage.hpp"
#include "Molten/Renderer/Vulkan/VulkanTextureFrame.hpp"

namespace Molten
{

    template<size_t VDimensions>
    class VulkanTexture : public Texture<VDimensions>
    {

    public:

        using Base = Texture<VDimensions>;

        VulkanTexture();
        VulkanTexture(
            Vulkan::DeviceImage&& deviceImage,
            VkImageView imageView,
            const uint8_t bytesPerPixel);

        ~VulkanTexture() override = default;

        /** Move constructor and assignment operator. */
        /**@{*/
        VulkanTexture(VulkanTexture&& vulkanTexture) noexcept;
        VulkanTexture& operator = (VulkanTexture&& vulkanTexture) noexcept;
        /**@}*/

        /** Deleted copy constructor and assignment operator. */
        /**@{*/
        VulkanTexture(const VulkanTexture&) = delete;
        VulkanTexture& operator = (const VulkanTexture&) = delete;
        /**@}*/

        Vulkan::DeviceImage deviceImage;
        VkImageView imageView;
        uint8_t bytesPerPixel;

    };

    using VulkanTexture1D = VulkanTexture<1>;
    using VulkanTexture2D = VulkanTexture<2>;
    using VulkanTexture3D = VulkanTexture<3>;


    template<size_t VDimensions>
    class VulkanFramedTexture : public FramedTexture<VDimensions>
    {

    public:

        using Base = Texture<VDimensions>;  

        VulkanFramedTexture();
        VulkanFramedTexture(
            VulkanTextureFrames&& frames,
            const uint8_t bytesPerPixel);

        ~VulkanFramedTexture() override = default;

        /** Move constructor and assignment operator. */
        /**@{*/
        VulkanFramedTexture(VulkanFramedTexture&& vulkanTexture) noexcept;
        VulkanFramedTexture& operator = (VulkanFramedTexture&& vulkanTexture) noexcept;
        /**@}*/

        /** Deleted copy constructor and assignment operator. */
        /**@{*/
        VulkanFramedTexture(const VulkanFramedTexture&) = delete;
        VulkanFramedTexture& operator = (const VulkanFramedTexture&) = delete;
        /**@}*/

        VulkanTextureFrames frames;
        uint8_t bytesPerPixel;

    };

    using VulkanFramedTexture1D = VulkanFramedTexture<1>;
    using VulkanFramedTexture2D = VulkanFramedTexture<2>;
    using VulkanFramedTexture3D = VulkanFramedTexture<3>;

}

#include "Molten/Renderer/Vulkan/VulkanTexture.inl"

#endif

#endif