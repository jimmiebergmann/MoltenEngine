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


#include "Molten/Renderer/Vulkan/Utility/VulkanExtension.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include <iterator>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Extension implementations.
    Extension::Extension() :
        name(""),
        version(0)
    {}

    Extension::Extension(const std::string& name) :
        name(name),
        version(0)
    {}

    Extension::Extension(
        const std::string& name,
        const uint32_t version
    ) :
        name(name),
        version(version)
    {}

    Extension::Extension(
        const VkExtensionProperties& extensionProperties
    ) :
        name(extensionProperties.extensionName),
        version(extensionProperties.specVersion)
    {}

    Extension::Extension(const Extension& extension) :
        name(extension.name),
        version(extension.version)
    {}

    Extension::Extension(Extension&& extension) noexcept :
        name(std::move(extension.name)),
        version(extension.version)
    {
        extension.version = 0;
    }
    Extension& Extension::operator =(const Extension& extension)
    {
        name = extension.name;
        version = extension.version;
        return *this;
    }

    Extension& Extension::operator =(Extension&& extension) noexcept
    {
        name = std::move(extension.name);
        version = extension.version;
        extension.version = 0;
        return *this;
    }

    // Static implementations.
    /*Result<>*/VkResult FetchInstanceExtensions(
        Extensions& extensions)
    {
         extensions.clear();

        VkResult result = VkResult::VK_SUCCESS;

        uint32_t extensionCount = 0;
        if ((result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        if (!extensionCount)
        {
            return result;
        }

        std::vector<VkExtensionProperties> extensionProperties(extensionCount, VkExtensionProperties{});
        if ((result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data())) != VkResult::VK_SUCCESS)
        {
            return result;
        }
        
        std::copy(extensionProperties.begin(), extensionProperties.end(), std::back_inserter(extensions));
        return result;      
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif