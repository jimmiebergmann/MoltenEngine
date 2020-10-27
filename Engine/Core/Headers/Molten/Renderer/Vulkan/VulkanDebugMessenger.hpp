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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_VULKANDEBUGMESSENGER_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_VULKANDEBUGMESSENGER_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanHeaders.hpp"
#include "Molten/Logger.hpp"
#include <vector>
#include <functional>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten
{
    /** Vulkan namespace, containing helper types and functions. */
    namespace Vulkan
    {

        enum class CreateDebugMessengerResult : uint32_t
        {
            Successful = 0,
            AlreadyCreated,
            NotPrepared,
            InvalidInstance,
            CannotFindCreateFunction,
            CannotFindDestroyFunction,
            CreateFailed
        };

        enum class PrepareDebugMessengerResult : uint32_t
        {
            Successful = 0,
            AlreadyPrepared,
            MissingValidationLayers
        };

        /** Debugger messenger class, which should be used for reporting information from validation layers. */
        class MOLTEN_API DebugMessenger
        {
        public:

            using Callback = std::function<void(Logger::Severity, const char*)>;

            DebugMessenger();
            ~DebugMessenger();

            PrepareDebugMessengerResult Prepare(
                const uint32_t severityFlags,
                Callback callback);

            CreateDebugMessengerResult Create(VkInstance instance);

            void Destroy();

            bool IsCreated() const;

            const VkDebugUtilsMessengerCreateInfoEXT& GetCreateInfo() const;

        private:

            DebugMessenger(const DebugMessenger&) = delete;
            DebugMessenger(DebugMessenger&&) = delete;
            DebugMessenger& operator=(const DebugMessenger&) = delete;
            DebugMessenger& operator=(DebugMessenger&&) = delete;

            bool m_prepared;
            bool m_created;
            VkDebugUtilsMessengerCreateInfoEXT m_messengerCreateInfo;
            PFN_vkCreateDebugUtilsMessengerEXT m_createDebugUtilsMessengerFunction;
            PFN_vkDestroyDebugUtilsMessengerEXT m_destroyDebugUtilsMessengerFunction;
            VkDebugUtilsMessengerEXT m_messenger;
            VkInstance m_instance;
            Callback m_callback;

        };

    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
