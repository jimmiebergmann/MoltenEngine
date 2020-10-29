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


#include "Molten/Renderer/Vulkan/Utility/VulkanLayer.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Layer implementations.
    Layer::Layer(const std::string& name) :
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
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif