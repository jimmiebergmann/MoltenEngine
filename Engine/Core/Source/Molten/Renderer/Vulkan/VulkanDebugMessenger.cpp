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

#include "Molten/Renderer/Vulkan/VulkanDebugMessenger.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanFunctions.hpp"

#include <algorithm>
#include <string>
#include <set>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten
{

    namespace Vulkan
    {

        // Debug messenger implementations.
        DebugMessenger::DebugMessenger() :
            m_prepared(false),
            m_created(false),
            m_messengerCreateInfo{},
            m_createDebugUtilsMessengerFunction(nullptr),
            m_destroyDebugUtilsMessengerFunction(nullptr),
            m_messenger(VK_NULL_HANDLE),
            m_instance(VK_NULL_HANDLE),
            m_callback(nullptr)

        {}

        DebugMessenger::~DebugMessenger()
        {
            Destroy();
        }

        PrepareDebugMessengerResult DebugMessenger::Prepare(
            const uint32_t severityFlags,
            Callback callback)
        {
            if (m_prepared)
            {
                return PrepareDebugMessengerResult::AlreadyPrepared;
            }

            m_callback = callback;

            /*uint32_t availableLayersCount = 0;
            vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr);
            if (!availableLayersCount)
            {
                return PrepareDebugMessengerResult::MissingValidationLayers;
            }
            std::vector<VkLayerProperties> availableLayers(availableLayersCount);
            vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers.data());

            std::set<std::string> missingLayers(m_validationLayers.begin(), m_validationLayers.end());
            for (auto& layer : availableLayers)
            {
                missingLayers.erase(layer.layerName);
                if (!missingLayers.size())
                {
                    break;
                }
            }
            if (missingLayers.size() != 0)
            {
                return PrepareDebugMessengerResult::MissingValidationLayers;
            }*/

            VkDebugUtilsMessageSeverityFlagsEXT vulkanSeverityFlags = 0;
            vulkanSeverityFlags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Info)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT : 0;
            vulkanSeverityFlags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Debug)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT : 0;
            vulkanSeverityFlags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Warning)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT : 0;
            vulkanSeverityFlags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Error)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT : 0;

            m_messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            m_messengerCreateInfo.messageSeverity = vulkanSeverityFlags;
            m_messengerCreateInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            m_messengerCreateInfo.pUserData = &m_callback;
            m_messengerCreateInfo.pfnUserCallback =
                [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                    void* pUserData) -> VkBool32
            {
                Callback& callback = *static_cast<Callback*>(pUserData);
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

            m_prepared = true;
            return PrepareDebugMessengerResult::Successful;
        }

        CreateDebugMessengerResult DebugMessenger::Create(VkInstance instance)
        {
            if (!m_prepared)
            {
                return CreateDebugMessengerResult::NotPrepared;
            }

            if (m_created)
            {
                return CreateDebugMessengerResult::AlreadyCreated;
            }

            if (instance == VK_NULL_HANDLE)
            {
                return CreateDebugMessengerResult::InvalidInstance;
            }
            m_instance = instance;

            m_createDebugUtilsMessengerFunction = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
            if (!m_createDebugUtilsMessengerFunction)
            {
                return CreateDebugMessengerResult::CannotFindCreateFunction;
            }

            m_destroyDebugUtilsMessengerFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
            if (!m_destroyDebugUtilsMessengerFunction)
            {
                return CreateDebugMessengerResult::CannotFindDestroyFunction;
            }

            if (m_createDebugUtilsMessengerFunction(m_instance, &m_messengerCreateInfo, nullptr, &m_messenger) != VK_SUCCESS)
            {
                return CreateDebugMessengerResult::CreateFailed;
            }

            m_created = true;
            return CreateDebugMessengerResult::Successful;
        }

        void DebugMessenger::Destroy()
        {
            if (m_instance != VK_NULL_HANDLE && m_messenger && m_destroyDebugUtilsMessengerFunction)
            {
                m_destroyDebugUtilsMessengerFunction(m_instance, m_messenger, nullptr);
            }

            m_created = false;
            m_createDebugUtilsMessengerFunction = nullptr;
            m_destroyDebugUtilsMessengerFunction = nullptr;
            m_messenger = VK_NULL_HANDLE;
            m_instance = VK_NULL_HANDLE;
        }

        bool DebugMessenger::IsCreated() const
        {
            return m_created;
        }

        const VkDebugUtilsMessengerCreateInfoEXT& DebugMessenger::GetCreateInfo() const
        {
            return m_messengerCreateInfo;
        }

    }

}

#endif