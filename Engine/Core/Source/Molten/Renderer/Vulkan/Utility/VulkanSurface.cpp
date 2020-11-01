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


#include "Molten/Renderer/Vulkan/Utility/VulkanSurface.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanInstance.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanExtension.hpp"
#include "Molten/Renderer/RenderTarget.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    const Extension& Surface::GetPlatformExtension()
    {
    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
        static const auto surfaceExtension = Extension{ "VK_KHR_win32_surface" };
#   elif MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX
        static const auto surfaceExtension = Extension{ "VK_KHR_xlib_surface" };
    #else
        static const auto surfaceExtension = Extension{ "" };
    #endif
        return surfaceExtension;
    }

    Surface::Surface() :
        m_handle(VK_NULL_HANDLE),
        m_instance(nullptr),
        m_renderTarget(nullptr)
    {}

    Surface::~Surface()
    {
        Destroy();
    }

    Result<> Surface::Create(Instance& instance, RenderTarget& renderTarget)
    {   
        Destroy();

    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

        VkResult result = VkResult::VK_SUCCESS;

        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.hwnd = renderTarget.GetWin32Window();
        surfaceInfo.hinstance = renderTarget.GetWin32Instance();

        if ((result = vkCreateWin32SurfaceKHR(instance.GetHandle(), &surfaceInfo, nullptr, &m_handle)) != VK_SUCCESS)
        {
            return result;
        }
        if (m_handle == VK_NULL_HANDLE)
        {
            return VkResult::VK_ERROR_UNKNOWN;
        }

        m_instance = &instance;
        m_renderTarget = &renderTarget;

        return result;

    #else

        return VkResult::VK_ERROR_FEATURE_NOT_PRESENT;

    #endif
       
    }

    void Surface::Destroy()
    {
        if (m_handle && m_instance->GetHandle())
        {
            vkDestroySurfaceKHR(m_instance->GetHandle(), m_handle, nullptr);
            m_handle = VK_NULL_HANDLE;
            m_instance = nullptr;
        }
    }

    bool Surface::IsCreated() const
    {
        return m_handle != VK_NULL_HANDLE;
    }

    VkSurfaceKHR Surface::GetHandle() const
    {
        return m_handle;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif