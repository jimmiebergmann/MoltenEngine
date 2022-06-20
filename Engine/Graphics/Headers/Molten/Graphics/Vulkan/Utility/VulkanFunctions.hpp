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

#ifndef MOLTEN_GRAPHICS_VULKAN_UTILITY_VULKANFUNCTIONS_HPP
#define MOLTEN_GRAPHICS_VULKAN_UTILITY_VULKANFUNCTIONS_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Graphics/Vulkan/Utility/VulkanTypes.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanExtension.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanLayer.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Version.hpp"
#include <string>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class LogicalDevice;
    struct DeviceImage;


    /** Create N number of fences. */
    MOLTEN_GRAPHICS_API VkResult CreateFences(
        Fences& fences,
        VkDevice logicalDevice,
        const VkFenceCreateFlagBits createFlags,
        const size_t count);

    MOLTEN_GRAPHICS_API VkFramebuffer CreateFramebuffer(
        VkDevice logicalDevice, 
        VkRenderPass renderpass, 
        const VkImageView& imageView, 
        const Vector2ui32 size);

    /** Create N number of semaphores. */
    MOLTEN_GRAPHICS_API VkResult CreateSemaphores(
        Semaphores& semaphores,
        VkDevice logicalDevice,
        const size_t count);

    /** Create Vulkan version(uint32_t) from Molten version,*/
    MOLTEN_GRAPHICS_API uint32_t CreateVersion(const Version& version);


    /** Destroy all fences in vector. */
    MOLTEN_GRAPHICS_API void DestroyFences(
        VkDevice logicalDevice,
        Fences& fences);

    /** Destroy all image vies in vector. */
    MOLTEN_GRAPHICS_API void DestroyImageViews(
        VkDevice logicalDevice,
        ImageViews& imageViews);

    /** Destroy all semaphores in vector. */
    MOLTEN_GRAPHICS_API void DestroySemaphores(
        VkDevice logicalDevice,
        Semaphores& semaphores);


    /** Search for the last in structure, where pNext == nullptr. */
    MOLTEN_GRAPHICS_API const VkBaseInStructure& FindLastBaseInStructure(VkBaseInStructure& baseInStructure);

    /** Search for the last out structure, where pNext == nullptr. */
    MOLTEN_GRAPHICS_API VkBaseOutStructure& FindLastBaseOutStructure(VkBaseOutStructure& baseOutStructure);

    /** Find layer in vector of layers, by name. Returns iterator of found layer, else layers.end(). */
    MOLTEN_GRAPHICS_API Layers::iterator FindLayer(Layers& layers, const std::string& name);

    /** Find layer in vector of extensions, by name. Returns iterator of found extension, else extensions.end(). */
    MOLTEN_GRAPHICS_API Extensions::iterator FindExtension(Extensions& extensions, const std::string& name);


    /** Removing layers by an exluding list of other layers.*/
    MOLTEN_GRAPHICS_API void RemoveLayers(
        Layers& layers,
        const Layers& excludes);

    /** Removing extensions by an exluding list of other extensions.*/
    MOLTEN_GRAPHICS_API void RemoveExtensions(
        Extensions& extensions,
        const Extensions& excludes);

    /** Functions for begining and ending single time commands. */
    /**@{*/
    MOLTEN_GRAPHICS_API Result<> BeginSingleTimeCommands(
        VkCommandBuffer& commandBuffer,
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool);

    MOLTEN_GRAPHICS_API Result<> EndSingleTimeCommands(
        VkCommandBuffer commandBuffer,
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool);
    /**@}*/


    /** Function for changing layout of image. */
    /**@{*/
    MOLTEN_GRAPHICS_API bool TransitionImageLayout(
        VkCommandBuffer commandBuffer,
        VkImage image,
        const VkImageLayout oldLayout,
        const VkImageLayout newLayout);

    MOLTEN_GRAPHICS_API bool TransitionImageLayout(
        VkCommandBuffer commandBuffer,
        DeviceImage& deviceImage,
        const VkImageLayout newLayout);
    /**@}*/

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
