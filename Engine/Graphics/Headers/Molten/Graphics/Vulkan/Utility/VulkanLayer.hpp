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

#ifndef MOLTEN_GRAPHICS_VULKAN_UTILITY_VULKANLAYER_HPP
#define MOLTEN_GRAPHICS_VULKAN_UTILITY_VULKANLAYER_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Graphics/Vulkan/Utility/VulkanResult.hpp"
#include "Molten/System/Version.hpp"
#include <string>
#include <vector>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    /** A more c++ friendly version of VkLayerProperties. */
    struct MOLTEN_GRAPHICS_API Layer
    {
        Layer();
        explicit Layer(const std::string& name);
        Layer(
            const std::string& name,
            const uint32_t version,
            const Version vulkanVersion);
        Layer(
            const VkLayerProperties& layerProperties);

        Layer(const Layer& layer);
        Layer(Layer&& layer) noexcept;
        Layer& operator =(const Layer& layer);
        Layer& operator =(Layer&& layer) noexcept;

        bool operator ==(const std::string& name) const;
        bool operator !=(const std::string& name) const;

        std::string name;
        uint32_t version;
        Version vulkanVersion;
    };

    using Layers = std::vector<Layer>;


    /** Fetch available instance extensions. */
    MOLTEN_GRAPHICS_API Result<> FetchInstanceLayers(Layers& layers);

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif