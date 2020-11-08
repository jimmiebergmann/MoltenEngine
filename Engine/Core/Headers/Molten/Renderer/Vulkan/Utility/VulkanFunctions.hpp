/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANFUNCTIONS_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANFUNCTIONS_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanTypes.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanExtension.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanLayer.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Version.hpp"
#include <string>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class LogicalDevice;


    /** Check if all required extensions are present in provided extension container.. */
    /*MOLTEN_API bool CheckRequiredExtensions(
        Extensions& missingExtensions,
        const Extensions& requiredExtensions,
        const Extensions& availableExtensions);

    MOLTEN_API bool CheckRequiredDeviceFeatures(
        PhysicalDeviceFeaturesWithName& missingFeatures,
        const VkPhysicalDeviceFeatures& requiredFeatures,
        const VkPhysicalDeviceFeatures& availableFeatures);*/


    /** Create N number of fences. */
    MOLTEN_API VkResult CreateFences(
        Fences& fences,
        VkDevice logicalDevice,
        const VkFenceCreateFlagBits createFlags,
        const size_t count);

    MOLTEN_API VkFramebuffer CreateFramebuffer(
        VkDevice logicalDevice, 
        VkRenderPass renderpass, 
        const VkImageView& imageView, 
        const Vector2ui32 size);

    /** Create N number of semaphores. */
    MOLTEN_API VkResult CreateSemaphores(
        Semaphores& semaphores,
        VkDevice logicalDevice,
        const size_t count);

    /** Create Vulkan version(uint32_t) from Molten version,*/
    MOLTEN_API uint32_t CreateVersion(const Version& version);


    /** Destroy all fences in vector. */
    MOLTEN_API void DestroyFences(
        VkDevice logicalDevice,
        Fences& fences);

    /** Destroy all image vies in vector. */
    MOLTEN_API void DestroyImageViews(
        VkDevice logicalDevice,
        ImageViews& imageViews);

    /** Destroy all semaphores in vector. */
    MOLTEN_API void DestroySemaphores(
        VkDevice logicalDevice,
        Semaphores& semaphores);


    /** Search for the last in structure, where pNext == nullptr. */
    MOLTEN_API const VkBaseInStructure& FindLastBaseInStructure(VkBaseInStructure& baseInStructure);

    /** Search for the last out structure, where pNext == nullptr. */
    MOLTEN_API VkBaseOutStructure& FindLastBaseOutStructure(VkBaseOutStructure& baseOutStructure);

    /** Find layer in vector of layers, by name. Returns iterator of found layer, else layers.end(). */
    MOLTEN_API Layers::iterator FindLayer(Layers& layers, const std::string& name);

    /** Find layer in vector of extensions, by name. Returns iterator of found extension, else extensions.end(). */
    MOLTEN_API Extensions::iterator FindExtension(Extensions& extensions, const std::string& name);


    /** Removing layers by an exluding list of other layers.*/
    MOLTEN_API void RemoveLayers(
        Layers& layers,
        const Layers& excludes);

    /** Removing extensions by an exluding list of other extensions.*/
    MOLTEN_API void RemoveExtensions(
        Extensions& extensions,
        const Extensions& excludes);

    /** Functions for begining and ending single time commands. */
    /**@{*/
    MOLTEN_API Result<> BeginSingleTimeCommands(
        VkCommandBuffer& commandBuffer,
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool);

    MOLTEN_API Result<> EndSingleTimeCommands(
        VkCommandBuffer commandBuffer,
        LogicalDevice& logicalDevice,
        VkCommandPool commandPool);
    /**@}*/


    /** Function for changing layout of image. */
    MOLTEN_API bool TransitionImageLayout(
        VkCommandBuffer commandBuffer,
        LogicalDevice& logicalDevice,
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout);

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
