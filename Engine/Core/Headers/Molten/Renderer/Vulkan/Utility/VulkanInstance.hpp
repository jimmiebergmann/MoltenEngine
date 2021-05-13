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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANINSTANCE_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANINSTANCE_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanExtension.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanLayer.hpp"
#include "Molten/System/Version.hpp"
#include "Molten/Logger.hpp"
#include <string>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    struct MOLTEN_API DebugCallbackDescriptor
    {
        using Callback = std::function<void(Molten::Logger::Severity, const char*)>;

        DebugCallbackDescriptor();
        DebugCallbackDescriptor(
            uint32_t severityFlags,
            Callback callback);

        uint32_t severityFlags;
        Callback callback;
    };

    /** Vulkan instance class. */
    class MOLTEN_API Instance
    {

    public:

        Instance();
        ~Instance();

        Result<> Create(
            const Version& vulkanVersion,
            const std::string& engineName,
            const Version& engineVersion,
            const std::string& applicationName,
            const Version& applicationVersion,
            const Extensions& enabledExtensions,
            const Layers& enabledLayers,
            DebugCallbackDescriptor debugCallbackDescriptor = {});

        void Destroy();

        VkInstance& GetHandle();
        const VkInstance& GetHandle() const;

        Extensions& GetExtensions();
        const Extensions& GetExtensions() const;

        Layers& GetLayers();
        const Layers& GetLayers() const;

    private:

        VkInstance m_handle;
        Extensions m_extensions;
        Layers m_layers;

        VkDebugUtilsMessengerEXT m_debugHandle;
        DebugCallbackDescriptor m_debugCallbackDesc;     
        PFN_vkCreateDebugUtilsMessengerEXT m_createDebugUtilsMessengerFunction;
        PFN_vkDestroyDebugUtilsMessengerEXT m_destroyDebugUtilsMessengerFunction;

    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif