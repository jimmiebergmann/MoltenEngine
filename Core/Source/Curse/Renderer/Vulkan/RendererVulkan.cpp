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

#include "Curse/Renderer/Vulkan/ShaderVulkan.hpp"
#include "Curse/Renderer/Vulkan/TextureVulkan.hpp"
#include "Curse/Renderer/Vulkan/PipelineVulkan.hpp"
#include "Curse/Window/WindowBase.hpp"
#include "Curse/System/Exception.hpp"
#include "Curse/System/FileSystem.hpp"
#include <map>
#include <set>
#include <algorithm>

namespace Curse
{

    // Static helper functions.
    static VkShaderStageFlagBits GetShaderStageFlag(const Shader::Type type)
    {
        switch (type)
        {
            case Shader::Type::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
            case Shader::Type::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
            default: break;
        }

        throw Exception("Provided shader type is not supported by the Vulkan renderer.");
    }

    static VkPrimitiveTopology GetPrimitiveTopology(const Pipeline::Topology topology)
    {
        switch (topology)
        {
            case Pipeline::Topology::PointList:     return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case Pipeline::Topology::LineList:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case Pipeline::Topology::LineStrip:     return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case Pipeline::Topology::TriangleList:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case Pipeline::Topology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            default: break;
        }
        throw Exception("Provided primitive topology is not supported by the Vulkan renderer.");
    }

    static VkPolygonMode GetPrimitiveTopology(const Pipeline::PolygonMode polygonMode)
    {
        switch (polygonMode)
        {
            case Pipeline::PolygonMode::Point: return VK_POLYGON_MODE_POINT;
            case Pipeline::PolygonMode::Line:  return VK_POLYGON_MODE_LINE;
            case Pipeline::PolygonMode::Fill:  return VK_POLYGON_MODE_FILL;
            default: break;
        }
        throw Exception("Provided polygon mode is not supported by the Vulkan renderer.");
    }

    static VkFrontFace GetFrontFace(const Pipeline::FrontFace frontFace)
    {
        switch (frontFace)
        {
            case Pipeline::FrontFace::Clockwise:        return VK_FRONT_FACE_CLOCKWISE;
            case Pipeline::FrontFace::Counterclockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
            default: break;
        }
        throw Exception("Provided front face is not supported by the Vulkan renderer.");
    }

    static VkCullModeFlagBits GetCullMode(const Pipeline::CullMode cullMode)
    {
        switch (cullMode)
        {
            case Pipeline::CullMode::None:         return VK_CULL_MODE_NONE;
            case Pipeline::CullMode::Front:        return VK_CULL_MODE_FRONT_BIT;
            case Pipeline::CullMode::Back:         return VK_CULL_MODE_BACK_BIT;
            case Pipeline::CullMode::FrontAndBack: return VK_CULL_MODE_FRONT_AND_BACK;
            default: break;
        }
        throw Exception("Provided cull mode is not supported by the Vulkan renderer.");
    }


    // Vulkan renderer class implementations.
    RendererVulkan::RendererVulkan() :
        m_instance(VK_NULL_HANDLE),
        m_debugMessenger(),
        m_surface(VK_NULL_HANDLE),
        m_physicalDevice(),
        m_logicalDevice(VK_NULL_HANDLE),
        m_graphicsQueue(VK_NULL_HANDLE),
        m_presentQueue(VK_NULL_HANDLE),
        m_swapChain(VK_NULL_HANDLE),
        m_swapChainImageFormat(VK_FORMAT_UNDEFINED),
        m_swapChainExtent{0, 0}
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

    void RendererVulkan::Open(const WindowBase& window, const Version& version, DebugCallback debugCallback)
    {
        Close();
        LoadInstance(version, debugCallback);
        LoadSurface(window);
        LoadPhysicalDevice(); 
        LoadLogicalDevice();
        LoadSwapChain(window);
        LoadImageViews();
    }

    void RendererVulkan::Close()
    {
        if (m_logicalDevice)
        {
            for (auto& imageView : m_swapChainImageViews)
            {
                vkDestroyImageView(m_logicalDevice, imageView, nullptr);
            }
            m_swapChainImageViews.clear();

            vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
            vkDestroyDevice(m_logicalDevice, nullptr);
        }
        if (m_instance)
        {
            if (m_debugMessenger.messenger && m_debugMessenger.DestroyDebugUtilsMessengerEXT)
            {
                m_debugMessenger.DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger.messenger, nullptr);
            }    

            if (m_surface)
            {
                vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
                
            }

            vkDestroyInstance(m_instance, nullptr);
        }
        
        m_instance = VK_NULL_HANDLE;
        m_surface = VK_NULL_HANDLE;
        m_logicalDevice = VK_NULL_HANDLE;
        m_graphicsQueue = VK_NULL_HANDLE;
        m_presentQueue = VK_NULL_HANDLE;
        m_swapChain = VK_NULL_HANDLE;
        m_swapChainImageFormat = VK_FORMAT_UNDEFINED;
        m_swapChainExtent = { 0, 0 };
        m_swapChainImages.clear();

        m_physicalDevice.Clear();
        m_debugMessenger.Clear();
        m_validationLayers.clear();
        m_deviceExtensions.clear();
        
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

    }

    Shader* RendererVulkan::CreateShader(const ShaderDescriptor& descriptor)
    {
        const uint8_t* rawData = descriptor.data;
        size_t dataSize = descriptor.dataSize;

        std::vector<uint8_t> data;
        if (!rawData)
        {
            if (!descriptor.filename)
            {
                return nullptr;
            }

            data = FileSystem::ReadFile(descriptor.filename);
            if (!data.size())
            {
                return nullptr;
            }

            rawData = data.data();
            dataSize = data.size();
        }

        VkShaderModuleCreateInfo shaderModuleInfo = {};
        shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleInfo.codeSize = dataSize;
        shaderModuleInfo.pCode = reinterpret_cast<const uint32_t *>(rawData);

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_logicalDevice, &shaderModuleInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw Exception("Failed to create shader module.");
        }

        ShaderVulkan* shader = new ShaderVulkan;
        shader->resource = static_cast<Resource>(shaderModule);
        shader->type = descriptor.type;
        return shader;
    }

    void RendererVulkan::DestroyShader(Shader* shader)
    {
        ShaderVulkan* shaderVulkan = static_cast<ShaderVulkan*>(shader);
        vkDestroyShaderModule(m_logicalDevice, shaderVulkan->resource, nullptr);
        delete shaderVulkan;
    }

    Texture* RendererVulkan::CreateTexture()
    {
        return new TextureVulkan;
    }

    void RendererVulkan::DestroyTexture(Texture* texture)
    {
        delete static_cast<TextureVulkan*>(texture);
    }

    Pipeline* RendererVulkan::CreatePipeline(const PipelineDescriptor& descriptor)
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;

        VkRenderPass renderPass;
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass.");
        }


        std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos(descriptor.shaders.size(), VkPipelineShaderStageCreateInfo{});
        for (size_t i = 0; i < descriptor.shaders.size(); i++)
        {
            const ShaderVulkan* shader = static_cast<ShaderVulkan*>(descriptor.shaders[i]);
            VkPipelineShaderStageCreateInfo& createInfo = shaderStageInfos[i];
            createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            createInfo.stage = GetShaderStageFlag(shader->type);
            createInfo.module = shader->resource;
            createInfo.pName = "main";
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
        inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateInfo.topology = GetPrimitiveTopology(descriptor.topology);
        inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapChainExtent.width);
        viewport.height = static_cast<float>(m_swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 0.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = m_swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportStateInfo = {};
        viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.pViewports = &viewport;
        viewportStateInfo.scissorCount = 1;
        viewportStateInfo.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
        rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerInfo.depthClampEnable = VK_FALSE;
        rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerInfo.polygonMode = GetPrimitiveTopology(descriptor.polygonMode);
        rasterizerInfo.lineWidth = 1.0f;
        rasterizerInfo.cullMode = GetCullMode(descriptor.cullMode);
        rasterizerInfo.frontFace = GetFrontFace(descriptor.frontFace);
        rasterizerInfo.depthBiasEnable = VK_FALSE;
        rasterizerInfo.depthBiasConstantFactor = 0.0f;
        rasterizerInfo.depthBiasClamp = 0.0f;
        rasterizerInfo.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
        multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisamplingInfo.sampleShadingEnable = VK_FALSE;
        multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisamplingInfo.minSampleShading = 1.0f;
        multisamplingInfo.pSampleMask = nullptr;
        multisamplingInfo.alphaToOneEnable = VK_FALSE;
        multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
        
        //VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
        colorBlendAttachmentState.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.blendEnable = VK_FALSE;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &colorBlendAttachmentState;
        colorBlendInfo.blendConstants[0] = 0.0f;
        colorBlendInfo.blendConstants[1] = 0.0f;
        colorBlendInfo.blendConstants[2] = 0.0f;
        colorBlendInfo.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        VkPipelineLayout pipelineLayout;
        if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            vkDestroyRenderPass(m_logicalDevice, renderPass, nullptr);
            throw std::runtime_error("Failed to create pipeline layout.");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.size());
        pipelineInfo.pStages = shaderStageInfos.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;
        pipelineInfo.pViewportState = &viewportStateInfo;
        pipelineInfo.pRasterizationState = &rasterizerInfo;
        pipelineInfo.pMultisampleState = &multisamplingInfo;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        VkPipeline graphicsPipeline;
        if (vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            vkDestroyPipelineLayout(m_logicalDevice, pipelineLayout, nullptr);
            vkDestroyRenderPass(m_logicalDevice, renderPass, nullptr);
            throw std::runtime_error("Failed to create pipeline.");
        }

        PipelineVulkan* pipeline = new PipelineVulkan;
        pipeline->pipeline = graphicsPipeline;
        pipeline->renderPass = renderPass;
        pipeline->layout = pipelineLayout;
        return pipeline;
    }

    void RendererVulkan::DestroyPipeline(Pipeline* pipeline)
    {
        PipelineVulkan * pipelineVulkan = static_cast<PipelineVulkan*>(pipeline);

        if (pipelineVulkan->layout)
        {
            vkDestroyPipelineLayout(m_logicalDevice, pipelineVulkan->layout, nullptr);
        }

        if (pipelineVulkan->renderPass)
        {
            vkDestroyRenderPass(m_logicalDevice, pipelineVulkan->renderPass, nullptr);
        }
        
        delete pipelineVulkan;
    }


    RendererVulkan::DebugMessenger::DebugMessenger() :
        messenger(VK_NULL_HANDLE),
        CreateDebugUtilsMessengerEXT(nullptr),
        DestroyDebugUtilsMessengerEXT(nullptr),
        validationDebugger(false),
        callback(nullptr)
    { }

    void RendererVulkan::DebugMessenger::Clear()
    {
        messenger = VK_NULL_HANDLE;
        CreateDebugUtilsMessengerEXT = nullptr;
        DestroyDebugUtilsMessengerEXT = nullptr;
        validationDebugger = false;
        callback = nullptr;       
    }

    RendererVulkan::PhysicalDevice::PhysicalDevice() :
        device(VK_NULL_HANDLE),
        graphicsQueueIndex(0),
        presentQueueIndex(0)
    { }

    RendererVulkan::PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) :
        device(device),
        graphicsQueueIndex(0),
        presentQueueIndex(0)
    { }

    RendererVulkan::PhysicalDevice::PhysicalDevice(VkPhysicalDevice device, uint32_t graphicsQueueIndex, uint32_t presentQueueIndex) :
        device(device),
        graphicsQueueIndex(graphicsQueueIndex),
        presentQueueIndex(presentQueueIndex)
    { }

    void RendererVulkan::PhysicalDevice::Clear()
    {
        device = VK_NULL_HANDLE;
        graphicsQueueIndex = 0;
        presentQueueIndex = 0;
    }

    PFN_vkVoidFunction RendererVulkan::GetVulkanFunction(const char* functionName) const
    {
        return vkGetInstanceProcAddr(m_instance, functionName);
    }

    void RendererVulkan::LoadInstance(const Version& version, DebugCallback debugCallback)
    {
        auto newVersion = version;
        if (newVersion == Version::None)
        {
            newVersion = Version(1, 1);
        }
        auto curseVersion = CURSE_MAKE_VERSION;
        auto engineVersion = VK_MAKE_VERSION(curseVersion.Major, curseVersion.Minor, curseVersion.Patch);
        auto vulkanVersion = VK_MAKE_VERSION(newVersion.Major, newVersion.Minor, newVersion.Patch);

        // Get required extensions.
        std::vector<std::string> extensions;
        if (!GetRequiredExtensions(extensions, debugCallback != nullptr))
        {
            throw Exception("RendererVulkan: The required extensions are unavailable.");
        }
        std::vector<const char*> ptrExtensions(extensions.size());
        for (size_t i = 0; i < extensions.size(); i++)
        {
            ptrExtensions[i] = extensions[i].c_str();
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
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(ptrExtensions.size());
        instanceInfo.ppEnabledExtensionNames = ptrExtensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugMessageInfo = {};
        bool debuggerIsAvailable = LoadDebugger(instanceInfo, debugMessageInfo, debugCallback);
        
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
                return;
            }
            m_debugMessenger.validationDebugger = true;
        }

    }

    bool RendererVulkan::GetRequiredExtensions(std::vector<std::string>& out, const bool requestDebugger) const
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        if (!extensionCount)
        {
            return false;
        }
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        /*
        "VK_KHR_surface"
        "VK_KHR_win32_surface"
        "VK_KHR_xlib_surface"
        */
        
        out.clear();
        out.push_back("VK_KHR_surface");
        if (requestDebugger)
        {
            out.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        #if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
            out.push_back("VK_KHR_win32_surface");
        #elif CURSE_PLATFORM == CURSE_PLATFORM_LINUX
            out.push_back("VK_KHR_xlib_surface");
        #endif

        std::set<std::string> missingExtensions(out.begin(), out.end());
        for (const auto& extension : extensions)
        {
            missingExtensions.erase(extension.extensionName);
        }

        return missingExtensions.size() == 0;
    }

    bool RendererVulkan::LoadDebugger(VkInstanceCreateInfo& instanceInfo, VkDebugUtilsMessengerCreateInfoEXT& debugMessageInfo, DebugCallback debugCallback)
    {
        if (!debugCallback)
        {
            return false;
        }
     
        // Add more validations here if desired
        // ...
        m_validationLayers.push_back("VK_LAYER_KHRONOS_validation");
        // ...


        if (!m_validationLayers.size())
        {
            return false;
        }
        
        m_debugMessenger.callback = debugCallback;

        uint32_t availableLayersCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr);
        if (!availableLayersCount)
        {
            m_debugMessenger.callback("Failed to find any validation layers.");
            return false;
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
            m_debugMessenger.callback("Failed to find all requested validation layers.");
            return false;
        }       

        instanceInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        instanceInfo.ppEnabledLayerNames = m_validationLayers.data();

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
        
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessageInfo;

        return true;
    }

    void RendererVulkan::LoadSurface(const WindowBase& window)
    {       
    #if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.hwnd = window.GetWin32Window();
        surfaceInfo.hinstance = window.GetWin32Instance();

        if (vkCreateWin32SurfaceKHR(m_instance, &surfaceInfo, nullptr, &m_surface) != VK_SUCCESS)
        {
            throw Exception("Failed to create window surface.");
        }
    #else
        throw Exception("Vulkan surface loading is not supported for platform: " CURSE_PLATFORM_NAME);
    #endif
    }

    void RendererVulkan::LoadPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
        if (!deviceCount)
        {
            throw Exception("Failed to find any physical device supporting Vulkan.");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

        m_deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        std::multimap<uint32_t, PhysicalDevice> scoredDevices;
        for (auto& device : devices)
        {              
            PhysicalDevice physicalDevice(device);
            uint32_t score = 0;
            if (!ScorePhysicalDevice(physicalDevice, score))
            {
                continue;
            }
            scoredDevices.insert({ score, physicalDevice });
        }

        if (!scoredDevices.size())
        {
            throw Exception("Failed to find any physical device supporting the requirements.");
        }

        m_physicalDevice = scoredDevices.rbegin()->second;
    }

    bool RendererVulkan::ScorePhysicalDevice(PhysicalDevice& physicalDevice, uint32_t& score)
    {
        score = 0;
        VkPhysicalDevice device = physicalDevice.device;

        VkPhysicalDeviceProperties deviceProps;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProps);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        if (!deviceFeatures.geometryShader ||
            !CheckDeviceExtensionSupport(physicalDevice) ||
            !FetchSwapChainSupport(physicalDevice))
        {
            return false;
        }

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        if (!queueFamilyCount)
        {
            return false;
        }
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        uint32_t graphicsQueueIndex = 0;
        uint32_t presentQueueIndex = 0;;
        bool graphicsFamilySupport = false;
        bool presentFamilySupport = false;
        uint32_t queueIndex = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsFamilySupport = true;
                graphicsQueueIndex = queueIndex;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, queueIndex, m_surface, &presentSupport);
            if (presentSupport)
            {
                presentFamilySupport = true;
                presentQueueIndex = queueIndex;
            }

            if (graphicsFamilySupport && presentFamilySupport)
            {
                continue;
            }

            queueIndex++;
        }
        if (!graphicsFamilySupport || !presentFamilySupport)
        {
            return false;
        }

        
        if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 2000000;
        }
        else if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            score += 1000000;
        }

        score += (deviceProps.limits.maxFramebufferWidth / 1000) *
                       (deviceProps.limits.maxFramebufferHeight / 1000);

        //VkPhysicalDeviceMemoryProperties memoryProps;
        //vkGetPhysicalDeviceMemoryProperties(physicalDevice.device, &memoryProps);

        physicalDevice.graphicsQueueIndex = graphicsQueueIndex;
        physicalDevice.presentQueueIndex = presentQueueIndex;
        return true;
    }

    bool RendererVulkan::CheckDeviceExtensionSupport(PhysicalDevice& physicalDevice)
    {
        if (!m_deviceExtensions.size())
        {
            return true;
        }

        VkPhysicalDevice device = physicalDevice.device;

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        
        if (!extensionCount)
        {
            throw Exception("Failed to find any device extensions.");
        }

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> missingExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());
        for (auto& extension : availableExtensions)
        {
            missingExtensions.erase(extension.extensionName);
        }
  
        return missingExtensions.size() == 0;
    }

    bool RendererVulkan::FetchSwapChainSupport(PhysicalDevice& physicalDevice)
    {
        VkPhysicalDevice device = physicalDevice.device;
        SwapChainSupport& support = physicalDevice.swapChainSupport;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &support.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
        if (!formatCount)
        {
            return false;
        }

        support.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, support.formats.data());

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

        if (!presentModeCount)
        {
            return false;
        }
        
        support.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, support.presentModes.data());

        return true;
    }

    void RendererVulkan::LoadLogicalDevice()
    {
        std::vector<VkDeviceQueueCreateInfo> queueInfos;

        std::set<uint32_t> uniqueFamilies = { m_physicalDevice.graphicsQueueIndex, m_physicalDevice.presentQueueIndex };
        float queuePriority = 1.0f;

        for (auto family : uniqueFamilies)
        {
            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = family;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;
            queueInfos.push_back(queueInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        VkDeviceCreateInfo deviceInfo = {};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.pEnabledFeatures = &deviceFeatures;
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

        deviceInfo.enabledLayerCount = 0;
        if (m_debugMessenger.validationDebugger)
        {
            deviceInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            deviceInfo.ppEnabledLayerNames = m_validationLayers.data();
        }

        if (vkCreateDevice(m_physicalDevice.device, &deviceInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
        {
            throw Exception("Failed to create logical device.");
        }

        vkGetDeviceQueue(m_logicalDevice, m_physicalDevice.graphicsQueueIndex, 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_logicalDevice, m_physicalDevice.presentQueueIndex, 0, &m_presentQueue);
    }

    void RendererVulkan::LoadSwapChain(const WindowBase& window)
    {
        SwapChainSupport& swapChainSupport = m_physicalDevice.swapChainSupport;

        VkSurfaceFormatKHR surfaceFormat = {};
        bool foundSurfaceFormat = false;
        for (auto& format : swapChainSupport.formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                foundSurfaceFormat = true;
                surfaceFormat = format;
                m_swapChainImageFormat = format.format;
                break;
            }
        }
        if (!foundSurfaceFormat)
        {
            throw Exception("Failed find required surface format for the swap chain.");
        }

        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        for (auto& mode : swapChainSupport.presentModes)
        {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {  
                presentMode = mode;
                break;
            }
        }

        VkSurfaceCapabilitiesKHR& capabilities = swapChainSupport.capabilities;
        m_swapChainExtent = capabilities.currentExtent;
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            auto windowSize = window.GetInitialSize();
            m_swapChainExtent = {
                std::max(capabilities.currentExtent.width, std::min(windowSize.x, capabilities.maxImageExtent.width)),
                std::max(capabilities.currentExtent.height, std::min(windowSize.x, capabilities.maxImageExtent.height))
            };
        }

        uint32_t imageCount = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);
        if (!imageCount)
        {
            throw Exception("Failed to get correct image count: " + std::to_string(imageCount) + ".");
        }

        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
        swapchainInfo.surface = m_surface;
        swapchainInfo.presentMode = presentMode;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.minImageCount = imageCount;
        swapchainInfo.imageFormat = surfaceFormat.format;
        swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainInfo.imageExtent = m_swapChainExtent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.preTransform = capabilities.currentTransform;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        const uint32_t queueFamilies[2] = { m_physicalDevice.graphicsQueueIndex, m_physicalDevice.presentQueueIndex };
        if (queueFamilies[0] != queueFamilies[1])
        {
            swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainInfo.queueFamilyIndexCount = 2;
            swapchainInfo.pQueueFamilyIndices = queueFamilies;
        }
        else
        {
            swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainInfo.queueFamilyIndexCount = 0;
            swapchainInfo.pQueueFamilyIndices = nullptr;
        }

        if (vkCreateSwapchainKHR(m_logicalDevice, &swapchainInfo, nullptr, &m_swapChain) != VK_SUCCESS)
        {
            throw Exception("Failed create swap chain.");
        }

        uint32_t createdImageCount;
        vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &createdImageCount, nullptr);
        if (createdImageCount != imageCount)
        {
            throw Exception("Failed to create the requested number of swap chain images.");
        }

        m_swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &createdImageCount, m_swapChainImages.data());
    }

    void RendererVulkan::LoadImageViews()
    {
        m_swapChainImageViews.resize(m_swapChainImages.size());

        for (size_t i = 0; i < m_swapChainImages.size(); i++)
        {
            VkImageViewCreateInfo imageViewInfo = {};
            imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewInfo.image = m_swapChainImages[i];
            imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewInfo.format = m_swapChainImageFormat;
            imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewInfo.subresourceRange.baseMipLevel = 0;
            imageViewInfo.subresourceRange.levelCount = 1;
            imageViewInfo.subresourceRange.baseArrayLayer = 0;
            imageViewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_logicalDevice, &imageViewInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS)
            {
                throw Exception("Failed create swap chain.");
            }
        }
    }

}

#endif