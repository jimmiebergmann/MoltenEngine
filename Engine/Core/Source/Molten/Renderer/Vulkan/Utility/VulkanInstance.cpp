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


#include "Molten/Renderer/Vulkan/Utility/VulkanInstance.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    Instance::Instance() :
        m_handle(VK_NULL_HANDLE),
        m_extensions{},
        m_layers{}
    {}

    Result<> Instance::Create(VkInstance instance)
    {
        VkResult result = VkResult::VK_SUCCESS;

        /*
        

        auto engineVersionVulkan = CreateVersion(engineVersion);
        auto vulkanVersionVulkan = CreateVersion(vulkanVersion);

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

        // Create instance.
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

        if (debugMessenger)
        {
            instanceInfo.pNext = reinterpret_cast<const VkBaseOutStructure*>(&debugMessenger->GetCreateInfo());
        }

        VkInstance instanceHandle = VK_NULL_HANDLE;
        if ((result = vkCreateInstance(&instanceInfo, nullptr, &instanceHandle)) != VkResult::VK_SUCCESS)
        {
            return result;
        }

        /*if ((result = Vulkan::FetchInstanceExtensions(instance.extensions)) != VK_SUCCESS)
        {
            return result;
        }

        if ((result = Vulkan::FetchInstanceLayers(instance.layers)) != VK_SUCCESS)
        {
            return result;
        }*/


        return result;
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