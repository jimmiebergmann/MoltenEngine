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


#include "Curse/Renderer/Vulkan/RendererVulkan.hpp"

#if defined(CURSE_ENABLE_VULKAN)

#include "Curse/Window/WindowBase.hpp"
#include "Curse/System/Exception.hpp"

namespace Curse
{

    RendererVulkan::RendererVulkan() :
        m_instance(VK_NULL_HANDLE),
        m_debugMessenger(),
        m_physicalDevice(VK_NULL_HANDLE)
    {
    }

    RendererVulkan::RendererVulkan(const WindowBase& window, const Version& version, DebugCallback debugCallback) :
        RendererVulkan()
    {
        Open(window, version, debugCallback);
    }

    RendererVulkan::~RendererVulkan()
    {
        Close();
    }

    void RendererVulkan::Open(const WindowBase& /*window*/, const Version& version, DebugCallback debugCallback)
    {
        Close();

        auto newVersion = version;
        if (newVersion == Version::None)
        {
            newVersion = Version(1, 1);
        }
        auto curseVersion = CURSE_MAKE_VERSION;
        auto engineVersion = VK_MAKE_VERSION(curseVersion.Major, curseVersion.Minor, curseVersion.Patch);
        auto vulkanVersion = VK_MAKE_VERSION(newVersion.Major, newVersion.Minor, newVersion.Patch);

        // Get required extensions.
        std::vector<const char*> extensions;
        if (!GetRequiredExtensions(extensions))
        {
            throw Exception("RendererVulkan: The required extensions are unavailable.");
        }

        // Create instance.
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Curse Engine Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Curse Engine";
        appInfo.engineVersion = engineVersion;
        appInfo.apiVersion = vulkanVersion;

        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.enabledExtensionCount = 0; // WE NEED EXTENSIONS!!!
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr;
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugMessageInfo = {};
        bool debuggerIsAvailable = SetupDebugger(instanceInfo, debugMessageInfo, debugCallback);

        VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_instance);
        if (result != VK_SUCCESS)
        {
            switch (result)
            {
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                throw Exception("RendererVulkan: Driver for version " + version.AsString() + " of Vulkan is unavailable.");
                break;
            default:
                break;
            } 
            throw Exception("RendererVulkan: Failed to create Vulkan instance.");
        }

        if (debuggerIsAvailable)
        {
            m_debugMessenger.CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)GetVulkanFunction("vkCreateDebugUtilsMessengerEXT");
            m_debugMessenger.DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)GetVulkanFunction("vkDestroyDebugUtilsMessengerEXT");

            if (!m_debugMessenger.CreateDebugUtilsMessengerEXT)
            {
                m_debugMessenger.callback("Failed to get function pointer for vkCreateDebugUtilsMessengerEXT.");
                return;
            }
            else if (!m_debugMessenger.DestroyDebugUtilsMessengerEXT)
            {
                m_debugMessenger.callback("Failed to get function pointer for \"vkDestroyDebugUtilsMessengerEXT\".");
                return;
            }

            if (m_debugMessenger.CreateDebugUtilsMessengerEXT(m_instance, &debugMessageInfo, nullptr, &m_debugMessenger.messenger) != VK_SUCCESS)
            {
                m_debugMessenger.callback("Failed to set up debug messenger.");
            }
        }

        // Get physical device.
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    }

    void RendererVulkan::Close()
    {
        
        if (m_instance)
        {
            if (m_debugMessenger.messenger && m_debugMessenger.DestroyDebugUtilsMessengerEXT)
            {
                m_debugMessenger.DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger.messenger, nullptr);
            }                      

            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
            m_physicalDevice = VK_NULL_HANDLE;
        }

        m_debugMessenger.messenger = VK_NULL_HANDLE;
        m_debugMessenger.callback = nullptr;
        m_debugMessenger.CreateDebugUtilsMessengerEXT = nullptr;
        m_debugMessenger.DestroyDebugUtilsMessengerEXT = nullptr;
    }

    Renderer::BackendApi RendererVulkan::GetBackendApi() const
    {
        return Renderer::BackendApi::Vulkan;
    }

    Version RendererVulkan::GetVersion() const
    {
        return m_version;
    }

    void RendererVulkan::SwapBuffers()
    {
        //::SwapBuffers(m_deviceContext);
    }

    RendererVulkan::DebugMessenger::DebugMessenger() :
        messenger(VK_NULL_HANDLE),
        CreateDebugUtilsMessengerEXT(nullptr),
        DestroyDebugUtilsMessengerEXT(nullptr),
        callback(nullptr)
    { }

    PFN_vkVoidFunction RendererVulkan::GetVulkanFunction(const char* functionName) const
    {
        return vkGetInstanceProcAddr(m_instance, functionName);
    }

    bool RendererVulkan::SetupDebugger(VkInstanceCreateInfo& instanceInfo, VkDebugUtilsMessengerCreateInfoEXT& debugMessageInfo, DebugCallback debugCallback)
    {
        if (!debugCallback)
        {
            return false;
        }

        m_debugMessenger.callback = debugCallback;

        static const char * validationLayerName = "VK_LAYER_KHRONOS_validation";
        static const std::vector<const char*> validationLayers = { validationLayerName };

        uint32_t availableLayersCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(availableLayersCount);
        vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers.data());

        bool foundLayer = false;
        for (auto& layer : availableLayers)
        {
            if (std::strcmp(layer.layerName, validationLayerName) == 0)
            {
                foundLayer = true;
                break;
            }
        }

        if (!foundLayer)
        {
            m_debugMessenger.callback("Failed to find validation layer \"" + std::string(validationLayerName) + "\".");
            return false;
        }

        instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceInfo.ppEnabledLayerNames = validationLayers.data();
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessageInfo;

        debugMessageInfo = {};
        debugMessageInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessageInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessageInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessageInfo.pUserData = &m_debugMessenger.callback;

        debugMessageInfo.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
                                              VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                              void* pUserData) -> VkBool32
        {
            DebugCallback* callback = static_cast<DebugCallback*>(pUserData);
            (*callback)(pCallbackData->pMessage);
            return VK_FALSE;
        };

        return true;
    }

    bool RendererVulkan::GetRequiredExtensions(std::vector<const char*> & out) const
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        
        bool foundExtension = false;
        for (const auto& extension : extensions)
        {
            if (std::strcmp(extension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
            {
                out.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                foundExtension = true;
                break;
            }
        }

        return foundExtension;
    }

}

#endif