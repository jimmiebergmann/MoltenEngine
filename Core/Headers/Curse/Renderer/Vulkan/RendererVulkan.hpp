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

#ifndef CURSE_CORE_RENDERER_VULKAN_RENDERERVULKAN_HPP
#define CURSE_CORE_RENDERER_VULKAN_RENDERERVULKAN_HPP

#include "Curse/Core.hpp"

#if defined(CURSE_ENABLE_VULKAN)

#include "Curse/Renderer/Renderer.hpp"
#include "vulkan/vulkan.h"
#include <vector>
//#include "Curse/Platform/Win32Headers.hpp"

namespace Curse
{

    /**
    * @brief Vulkan renderer class.
    */
    class CURSE_API RendererVulkan : public Renderer
    {

    public:

        /**
        * @brief Constructor.
        */
        RendererVulkan();

        /**
        * @brief Constructs and creates renderer.
        *
        * @param window[in] Render target window.
        */
        RendererVulkan(const WindowBase& window, const Version& version, DebugCallback debugCallback = nullptr);

        /**
        * @brief Virtual destructor.
        */
        ~RendererVulkan();

        /**
        * @brief Opens renderer.
        *
        * @param window[in] Render target window.
        */
        virtual void Open(const WindowBase& window, const Version& version = Version::None, DebugCallback debugCallback = nullptr);

        /**
        * @brief Closing renderer.
        */
        virtual void Close();

        /**
        * @brief Get backend API type.
        */
        virtual BackendApi GetBackendApi() const;

        /**
        * @brief Get renderer API version.
        */
        virtual Version GetVersion() const;

        /**
        * @brief Swap buffers.
        *        Necessary to do in order to present the frame, if double/tripple buffering is used.
        */
        virtual void SwapBuffers();

    private:

        struct DebugMessenger
        {
            DebugMessenger();

            VkDebugUtilsMessengerEXT messenger;
            PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
            PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
            DebugCallback callback;
        };

        PFN_vkVoidFunction GetVulkanFunction(const char* functionName) const;
        bool SetupDebugger(VkInstanceCreateInfo& instanceInfo, VkDebugUtilsMessengerCreateInfoEXT& debugMessageInfo, DebugCallback debugCallback);
        bool GetRequiredExtensions(std::vector<const char*>& extensions) const;

        Version m_version;
        VkInstance m_instance;
        DebugMessenger m_debugMessenger;     
        VkPhysicalDevice m_physicalDevice;

    };

}

#endif

#endif