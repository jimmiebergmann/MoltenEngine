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

#ifndef CURSE_CORE_RENDERER_VULKANSHADERPROGRAM_HPP
#define CURSE_CORE_RENDERER_VULKANSHADERPROGRAM_HPP

#include "Curse/Renderer/Shader/ShaderStage.hpp"

#if defined(CURSE_ENABLE_VULKAN)
#include "Curse/Renderer/Vulkan/Vulkan.hpp"

namespace Curse::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Curse
{

    class VulkanRenderer;


    class CURSE_API VulkanVertexShaderStage : public Shader::VertexStage
    {

    private:

        VulkanVertexShaderStage(VkShaderModule module, const Shader::Visual::VertexScript& script);
        VulkanVertexShaderStage(const VulkanVertexShaderStage&) = delete;
        VulkanVertexShaderStage(VulkanVertexShaderStage&&) = delete;
        ~VulkanVertexShaderStage() = default;

        VkShaderModule module;
        const Shader::Visual::VertexScript& script;

        friend class VulkanRenderer;

    };

    class CURSE_API VulkanFragmentShaderStage : public Shader::FragmentStage
    {

    private:

        VulkanFragmentShaderStage(VkShaderModule module, const Shader::Visual::FragmentScript& script);
        VulkanFragmentShaderStage(const VulkanFragmentShaderStage&) = delete;
        VulkanFragmentShaderStage(VulkanFragmentShaderStage&&) = delete;
        ~VulkanFragmentShaderStage() = default;

        VkShaderModule module;
        const Shader::Visual::FragmentScript& script;

        friend class VulkanRenderer;

    };

}

#endif

#endif