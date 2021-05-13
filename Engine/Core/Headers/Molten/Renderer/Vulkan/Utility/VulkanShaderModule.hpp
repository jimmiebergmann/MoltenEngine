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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANSHADERMODULE_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANSHADERMODULE_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include <vector>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class LogicalDevice;


    /** Vulkan image, backed by device memory. */
    class MOLTEN_API ShaderModule
    {

    public:

        ShaderModule();
        ~ShaderModule();

        ShaderModule(const ShaderModule&) = delete;
        ShaderModule& operator = (const ShaderModule&) = delete;

        ShaderModule(ShaderModule&& shaderModule) noexcept;
        ShaderModule& operator =(ShaderModule&& shaderModule) noexcept;

        /** Create image. Device memory is allocated but not populated with anything. */
        Result<> Create(
            LogicalDevice& logicalDevice,
            const std::vector<uint8_t>& spirvCode);

        void Destroy();

        bool IsCreated() const;

        VkShaderModule GetHandle() const;

        LogicalDevice& GetLogicalDevice();
        const LogicalDevice& GetLogicalDevice() const;

        bool HasLogicalDevice() const;

    private:

        VkShaderModule m_handle;
        LogicalDevice* m_logicalDevice;

    };

    using ShaderModules = std::vector<ShaderModule>;

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif
