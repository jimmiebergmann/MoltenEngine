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

#include "Molten/Renderer/Vulkan/VulkanTypes.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{


    // Filtered memory type implementations.
    FilteredMemoryType::FilteredMemoryType() :
        index(0),
        propertyFlags(0)
    {}

    FilteredMemoryType::FilteredMemoryType(
        const uint32_t index,
        const VkMemoryPropertyFlags propertyFlags
    ) :
        index(index),
        propertyFlags(propertyFlags)
    {}


    // Layer implementations.
    /*Layer::Layer(const std::string& name) :
        name(name),
        version(0),
        vulkanVersion(0, 0, 0)
    {}

    Layer::Layer() :
        name(""),
        version(0),
        vulkanVersion(0, 0, 0)
    {}

    Layer::Layer(
        const std::string& name,
        const uint32_t version,
        const Version vulkanVersion
    ) :
        name(name),
        version(version),
        vulkanVersion(vulkanVersion)
    {}

    Layer::Layer(
        const VkLayerProperties& layerProperties
    ) :
        name(layerProperties.layerName),
        version(layerProperties.implementationVersion),
        vulkanVersion(
            VK_VERSION_MAJOR(layerProperties.specVersion),
            VK_VERSION_MINOR(layerProperties.specVersion),
            VK_VERSION_PATCH(layerProperties.specVersion))
    {}

    Layer::Layer(const Layer& layer) :
        name(layer.name),
        version(layer.version),
        vulkanVersion(layer.vulkanVersion)
    {}

    Layer::Layer(Layer&& layer) noexcept :
        name(std::move(layer.name)),
        version(layer.version),
        vulkanVersion(layer.vulkanVersion)
    {
        layer.version = 0;
        layer.vulkanVersion = {};
    }
    Layer& Layer::operator =(const Layer& layer)
    {
        name = layer.name;
        version = layer.version;
        vulkanVersion = layer.vulkanVersion;
        return *this;
    }

    Layer& Layer::operator =(Layer&& layer) noexcept
    {
        name = std::move(layer.name);
        version = layer.version;
        vulkanVersion = layer.vulkanVersion;
        layer.version = 0;
        layer.vulkanVersion = {};
        return *this;
    }*/


    // Instance implementations.
    /*Instance::Instance() :
        handle(VK_NULL_HANDLE),
        extensions{},
        layers{}
    {}*/


    // Logical device implementations.
    LogicalDevice::LogicalDevice() :
        handle(VK_NULL_HANDLE),
        graphicsQueue(VK_NULL_HANDLE),
        presentQueue(VK_NULL_HANDLE)
    {}


    // Physical device Surface capabilities implementations.
    /*PhysicalDeviceSurfaceCapabilities::PhysicalDeviceSurfaceCapabilities() :
        capabilities{},
        formats{},
        presentModes{}
    {}*/


    // Physical device capabilities implementations.
    /*PhysicalDeviceCapabilities::PhysicalDeviceCapabilities() :
        properties{},
        features{},
        extensions{},
        hasPresentSupport(false),
        surfaceCapabilities{},
        queueFamilies{}
    {}*/

    // Physical device features with name implementations.
    PhysicalDeviceFeatureWithName::PhysicalDeviceFeatureWithName(
        VkBool32 VkPhysicalDeviceFeatures::* memberPointer,
        const std::string& name
    ) :
        memberPointer(memberPointer),
        name(name)
    {}





    /* Experimental type for result. */

    /* Experimental class for physical device, providing an interface for accessing it's underlying handle and capabilities. */
    /*PhysicalDevice::PhysicalDevice() :
        m_handle{ VK_NULL_HANDLE },
        m_capabilities{}
    {}

    Result<> PhysicalDevice::Create(VkPhysicalDevice physicalDeviceHandle)
    {
        VkResult result = VkResult::VK_SUCCESS;

        Extensions& extensions = m_capabilities.extensions;
        if ((result = FetchDeviceExtensions(availableExtensions, physicalDevice.handle)) != VkResult::VK_SUCCESS)
        {
            LogError(logger, "Failed to fetch device extensions", result);
            return result;
        }

        return result;
    }

    VkPhysicalDevice& PhysicalDevice::GetHandle()
    {
        return m_handle;
    }
    const VkPhysicalDevice& PhysicalDevice::GetHandle() const
    {
        return m_handle;
    }

    PhysicalDeviceCapabilities& PhysicalDevice::GetCapabilities()
    {
        return m_capabilities;
    }
    const PhysicalDeviceCapabilities& PhysicalDevice::GetCapabilities() const
    {
        return m_capabilities;
    }*/




    // Physical device with capabilities implementations.
    /*PhysicalDeviceWithCapabilities::PhysicalDeviceWithCapabilities() :
        handle(VK_NULL_HANDLE),
        capabilities{},
        graphicsQueueIndex(std::numeric_limits<uint32_t>::max()),
        presentQueueIndex(std::numeric_limits<uint32_t>::max())
    {}

    PhysicalDeviceWithCapabilities::PhysicalDeviceWithCapabilities(
        VkPhysicalDevice device
    ) :
        handle(device),
        capabilities{},
        graphicsQueueIndex(std::numeric_limits<uint32_t>::max()),
        presentQueueIndex(std::numeric_limits<uint32_t>::max())
    {}*/


    // Swapchain implementations.
    SwapChain::SwapChain() :
        handle(VK_NULL_HANDLE),
        imageFormat(VkFormat::VK_FORMAT_UNDEFINED),
        extent{ 0, 0 },
        images{},
        imageViews{},
        //framebuffers{},
        imageAvailableSemaphores{},
        renderFinishedSemaphores{},
        inFlightFences{},
        imagesInFlight{},
        maxFramesInFlight(0),
        currentFrame(0)
    {}

}

MOLTEN_UNSCOPED_ENUM_END

#endif