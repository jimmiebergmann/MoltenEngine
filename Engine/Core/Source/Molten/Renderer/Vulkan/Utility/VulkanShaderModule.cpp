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

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanShaderModule.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    // Shader module implementations.
    ShaderModule::ShaderModule() :
        m_handle(VK_NULL_HANDLE),
        m_logicalDevice(nullptr)
    {}

    ShaderModule::~ShaderModule()
    {
        Destroy();
    }

    ShaderModule::ShaderModule(ShaderModule&& shaderModule) noexcept :
        m_handle(shaderModule.m_handle),
        m_logicalDevice(shaderModule.m_logicalDevice)
    {
        shaderModule.m_handle = VK_NULL_HANDLE;
        shaderModule.m_logicalDevice = nullptr;
    }

    ShaderModule& ShaderModule::operator =(ShaderModule&& shaderModule) noexcept
    {
        m_handle = shaderModule.m_handle;
        m_logicalDevice = shaderModule.m_logicalDevice;
        shaderModule.m_handle = VK_NULL_HANDLE;
        shaderModule.m_logicalDevice = nullptr;
        return *this;
    }

    Result<> ShaderModule::Create(
        LogicalDevice& logicalDevice,
        const std::vector<uint8_t>& spirvCode)
    {
        Destroy();

        m_logicalDevice = &logicalDevice;

        VkShaderModuleCreateInfo shaderModuleInfo = {};
        shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleInfo.codeSize = spirvCode.size();
        shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(spirvCode.data());

        return vkCreateShaderModule(m_logicalDevice->GetHandle(), &shaderModuleInfo, nullptr, &m_handle);
    }

    void ShaderModule::Destroy()
    {
        if (m_logicalDevice)
        {
            auto logicalDeviceHandle = m_logicalDevice->GetHandle();

            if (m_handle != VK_NULL_HANDLE)
            {
                vkDestroyShaderModule(logicalDeviceHandle, m_handle, nullptr);
                m_handle = VK_NULL_HANDLE;
            }

            m_logicalDevice = nullptr;
        }
    }

    bool ShaderModule::IsCreated() const
    {
        return m_handle != VK_NULL_HANDLE;
    }

    VkShaderModule ShaderModule::GetHandle() const
    {
        return m_handle;
    }

    LogicalDevice& ShaderModule::GetLogicalDevice()
    {
        return *m_logicalDevice;
    }
    const LogicalDevice& ShaderModule::GetLogicalDevice() const
    {
        return *m_logicalDevice;
    }

    bool ShaderModule::HasLogicalDevice() const
    {
        return m_logicalDevice != nullptr;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif