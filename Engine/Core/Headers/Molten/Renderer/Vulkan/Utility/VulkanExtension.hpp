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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANEXTENSION_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANEXTENSION_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include <string>
#include <vector>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class PhysicalDevice;


    /** A more c++ friendly version of VkExtensionProperties. */
    struct MOLTEN_API Extension
    {
        Extension();
        explicit Extension(const std::string& name);
        Extension(
            const std::string& name,
            const uint32_t version);
        Extension(const VkExtensionProperties& extensionProperties);

        Extension(const Extension& extension);
        Extension(Extension&& extension) noexcept;
        Extension& operator =(const Extension& extension);
        Extension& operator =(Extension&& extension) noexcept;

        bool operator ==(const std::string& name) const;
        bool operator !=(const std::string& name) const;

        std::string name;
        uint32_t version;
        
    };

    using Extensions = std::vector<Extension>;


    /** Fetch available device extensions. */
    MOLTEN_API Result<> FetchDeviceExtensions(
        Extensions& extensions,
        const VkPhysicalDevice physicalDevice);

    /** Fetch available instance extensions. */
    MOLTEN_API Result<> FetchInstanceExtensions(Extensions& extensions);

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif