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

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Graphics/Vulkan/Utility/VulkanInstance.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanFunctions.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Debug messenger descriptor implementations.
    DebugCallbackDescriptor::DebugCallbackDescriptor() :
        severityFlags(0),
        callback{}
    {}

    DebugCallbackDescriptor::DebugCallbackDescriptor(
        uint32_t severityFlags,
        Callback callback
    ) :
        severityFlags(severityFlags),
        callback(callback)
    {}


    // Instance implementations.
    Instance::Instance() :
        m_handle(VK_NULL_HANDLE),
        m_extensions{},
        m_layers{},
        m_debugCallbackDesc{},
        m_debugHandle(VK_NULL_HANDLE),
        m_createDebugUtilsMessengerFunction(nullptr),
        m_destroyDebugUtilsMessengerFunction(nullptr)
    {}

    Instance::~Instance()
    {
        Destroy();
    }

    Result<> Instance::Create(
        const Version& vulkanVersion,
        const std::string& engineName,
        const Version& engineVersion,
        const std::string& applicationName,
        const Version& applicationVersion,
        const Extensions& enabledExtensions,
        const Layers& enabledLayers,
        DebugCallbackDescriptor debugCallbackDescriptor)
    {
        // Create pointer vectors to extensions and layers.
        std::vector<const char*> ptrExtensions(enabledExtensions.size(), nullptr);
        for (size_t i = 0; i < enabledExtensions.size(); i++)
        {
            ptrExtensions[i] = enabledExtensions[i].name.c_str();
        }

        std::vector<const char*> ptrEnabledLayers(enabledLayers.size(), nullptr);
        for (size_t i = 0; i < enabledLayers.size(); i++)
        {
            ptrEnabledLayers[i] = enabledLayers[i].name.c_str();
        }

        // Load debugger.
        VkDebugUtilsMessengerCreateInfoEXT m_debugCreateInfo = {};
        m_debugCallbackDesc = debugCallbackDescriptor;
        bool loadDebugMessenger = m_debugCallbackDesc.severityFlags != 0 && m_debugCallbackDesc.callback != nullptr;     
        
        if (loadDebugMessenger)
        {
            ptrExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            VkDebugUtilsMessageSeverityFlagsEXT vulkanSeverityFlags = 0;
            vulkanSeverityFlags |= (m_debugCallbackDesc.severityFlags & static_cast<uint32_t>(Logger::Severity::Info)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT : 0;
            vulkanSeverityFlags |= (m_debugCallbackDesc.severityFlags & static_cast<uint32_t>(Logger::Severity::Debug)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT : 0;
            vulkanSeverityFlags |= (m_debugCallbackDesc.severityFlags & static_cast<uint32_t>(Logger::Severity::Warning)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT : 0;
            vulkanSeverityFlags |= (m_debugCallbackDesc.severityFlags & static_cast<uint32_t>(Logger::Severity::Error)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT : 0;

            m_debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            m_debugCreateInfo.messageSeverity = vulkanSeverityFlags;
            m_debugCreateInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            m_debugCreateInfo.pUserData = &m_debugCallbackDesc.callback;
            m_debugCreateInfo.pfnUserCallback =
                [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                    void* pUserData) -> VkBool32
            {
                auto& callback = *static_cast<DebugCallbackDescriptor::Callback*>(pUserData);
                if (callback)
                {
                    auto getSeverity = [](auto vulkanSeverity) -> Logger::Severity
                    {
                        switch (vulkanSeverity)
                        {
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: return Logger::Severity::Info;
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return Logger::Severity::Debug;
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return Logger::Severity::Warning;
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: return Logger::Severity::Error;
                        default: break;
                        }

                        return Logger::Severity::Info;
                    };

                    std::invoke(callback, getSeverity(messageSeverity), pCallbackData->pMessage);
                }
                return VK_FALSE;
            };
        }

        // Create instance.
        auto engineVersionVulkan = CreateVersion(engineVersion);
        auto vulkanVersionVulkan = CreateVersion(vulkanVersion);

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = applicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = engineName.c_str();
        appInfo.engineVersion = engineVersionVulkan;
        appInfo.apiVersion = vulkanVersionVulkan;

        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr; 
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(ptrExtensions.size());
        instanceInfo.ppEnabledExtensionNames = ptrExtensions.data();
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(ptrEnabledLayers.size());
        instanceInfo.ppEnabledLayerNames = ptrEnabledLayers.data();

        if (loadDebugMessenger)
        {
            instanceInfo.pNext = &m_debugCreateInfo;
        }

        VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_handle);
        if (result != VkResult::VK_SUCCESS)
        {
            return result;
        }

        if (loadDebugMessenger)
        {
            m_createDebugUtilsMessengerFunction = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_handle, "vkCreateDebugUtilsMessengerEXT");
            m_destroyDebugUtilsMessengerFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_handle, "vkDestroyDebugUtilsMessengerEXT");

            if (m_createDebugUtilsMessengerFunction && m_destroyDebugUtilsMessengerFunction)
            {
                m_createDebugUtilsMessengerFunction(m_handle, &m_debugCreateInfo, nullptr, &m_debugHandle);
            }
        }

        return result;
    }

    void Instance::Destroy()
    {
        if (m_handle)
        {
            if (m_debugHandle && m_destroyDebugUtilsMessengerFunction)
            {
                m_destroyDebugUtilsMessengerFunction(m_handle, m_debugHandle, nullptr);
            }

            m_debugHandle = VK_NULL_HANDLE;
            m_createDebugUtilsMessengerFunction = nullptr;
            m_destroyDebugUtilsMessengerFunction = nullptr;
            
            vkDestroyInstance(m_handle, nullptr);
            m_handle = VK_NULL_HANDLE;
        }     
    }

    VkInstance& Instance::GetHandle()
    {
        return m_handle;
    }
    const VkInstance& Instance::GetHandle() const
    {
        return m_handle;
    }

    Extensions& Instance::GetExtensions()
    {
        return m_extensions;
    }
    const Extensions& Instance::GetExtensions() const
    {
        return m_extensions;
    }

    Layers& Instance::GetLayers()
    {
        return m_layers;
    }
    const Layers& Instance::GetLayers() const
    {
        return m_layers;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif