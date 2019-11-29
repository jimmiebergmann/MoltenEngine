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
#include "Curse/Renderer/Vulkan/FramebufferVulkan.hpp"
#include "Curse/Renderer/Vulkan/VertexArrayVulkan.hpp"
#include "Curse/Renderer/Vulkan/VertexBufferVulkan.hpp"
#include "Curse/Window/WindowBase.hpp"
#include "Curse/Logger.hpp"
#include "Curse/System/Exception.hpp"
#include "Curse/System/FileSystem.hpp"
#include <map>
#include <set>
#include <algorithm>
#include <limits>

#define CURSE_RENDERER_LOG(severity, message) if(m_logger){ m_logger->Write(severity, message); }

namespace Curse
{

    // Static helper functions.
    static VkShaderStageFlagBits GetShaderStageFlag(const Shader::Type type)
    {
        switch (type)
        {
            case Shader::Type::Vertex:   return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
            case Shader::Type::Fragment: return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
            default: break;
        }

        throw Exception("Provided shader type is not supported by the Vulkan renderer.");
    }

    static VkPrimitiveTopology GetPrimitiveTopology(const Pipeline::Topology topology)
    {
        switch (topology)
        {
            case Pipeline::Topology::PointList:     return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case Pipeline::Topology::LineList:      return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case Pipeline::Topology::LineStrip:     return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case Pipeline::Topology::TriangleList:  return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case Pipeline::Topology::TriangleStrip: return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            default: break;
        }
        throw Exception("Provided primitive topology is not supported by the Vulkan renderer.");
    }

    static VkPolygonMode GetPrimitiveTopology(const Pipeline::PolygonMode polygonMode)
    {
        switch (polygonMode)
        {
            case Pipeline::PolygonMode::Point: return VkPolygonMode::VK_POLYGON_MODE_POINT;
            case Pipeline::PolygonMode::Line:  return VkPolygonMode::VK_POLYGON_MODE_LINE;
            case Pipeline::PolygonMode::Fill:  return VkPolygonMode::VK_POLYGON_MODE_FILL;
            default: break;
        }
        throw Exception("Provided polygon mode is not supported by the Vulkan renderer.");
    }

    static VkFrontFace GetFrontFace(const Pipeline::FrontFace frontFace)
    {
        switch (frontFace)
        {
            case Pipeline::FrontFace::Clockwise:        return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
            case Pipeline::FrontFace::Counterclockwise: return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
            default: break;
        }
        throw Exception("Provided front face is not supported by the Vulkan renderer.");
    }

    static VkCullModeFlagBits GetCullMode(const Pipeline::CullMode cullMode)
    {
        switch (cullMode)
        {
            case Pipeline::CullMode::None:         return VkCullModeFlagBits::VK_CULL_MODE_NONE;
            case Pipeline::CullMode::Front:        return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
            case Pipeline::CullMode::Back:         return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
            case Pipeline::CullMode::FrontAndBack: return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
            default: break;
        }
        throw Exception("Provided cull mode is not supported by the Vulkan renderer.");
    }

    static VkFormat GetVertexAttributeFormat(const Pipeline::AttributeFormat format)
    {
        switch (format)
        {
            case Pipeline::AttributeFormat::R32_Float:             return VkFormat::VK_FORMAT_R32_SFLOAT;
            case Pipeline::AttributeFormat::R32_G32_Float:         return VkFormat::VK_FORMAT_R32G32_SFLOAT;
            case Pipeline::AttributeFormat::R32_G32_B32_Float:     return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
            case Pipeline::AttributeFormat::R32_G32_B32_A32_Float: return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
            default: break;
        }
        throw Exception("Provided attribute format is not supported by the Vulkan renderer.");
    }

    static VkDebugUtilsMessageSeverityFlagsEXT GetVulkanLoggerSeverityFlags(const uint32_t severityFlags)
    {
        VkDebugUtilsMessageSeverityFlagsEXT flags = 0;

        flags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Info)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT : 0;
        flags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Warning)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT : 0;
        flags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Error)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT : 0;

        return flags;
    }

    static Logger::Severity GetCurseLoggerSeverityFlag(const VkDebugUtilsMessageSeverityFlagsEXT severityFlags)
    {
        switch (severityFlags)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: return Logger::Severity::Info;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return Logger::Severity::Warning;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: return Logger::Severity::Error;
            default: break;
        }

        return Logger::Severity::Info;
    }


    // Vulkan renderer class implementations.
    RendererVulkan::RendererVulkan() :
        m_logger(nullptr),
        m_version(0, 0, 0),
        m_renderTarget(nullptr),
        m_instance(VK_NULL_HANDLE),
        m_debugMessenger(),
        m_surface(VK_NULL_HANDLE),
        m_physicalDevice(),
        m_logicalDevice(VK_NULL_HANDLE),
        m_graphicsQueue(VK_NULL_HANDLE),
        m_presentQueue(VK_NULL_HANDLE),
        m_swapChain(VK_NULL_HANDLE),
        m_swapChainImageFormat(VK_FORMAT_UNDEFINED),
        m_swapChainExtent{0, 0},
        m_renderPass(VK_NULL_HANDLE),
        m_commandPool(VK_NULL_HANDLE),
        m_maxFramesInFlight(0),
        m_currentFrame(0),
        m_resized(false),
        m_beginDraw(false),
        m_currentCommandBuffer(nullptr),
        m_currentFramebuffer(nullptr)  
    {
    }

    RendererVulkan::RendererVulkan(const WindowBase& window, const Version& version, Logger* logger) :
        RendererVulkan()
    {
        Open(window, version, logger);
    }

    RendererVulkan::~RendererVulkan()
    {
        Close();
    }

    void RendererVulkan::Open(const WindowBase& window, const Version& version, Logger* logger)
    {
        Close();

        m_renderTarget = &window;
        m_logger = logger;

        try
        {
            LoadInstance(version);
            LoadSurface();
            LoadPhysicalDevice();
            LoadLogicalDevice();
            LoadSwapChain();
            LoadImageViews();
            LoadRenderPass();
            LoadPresentFramebuffer();
            LoadCommandPool();
            LoadSyncObjects();
        }
        catch (Exception & e)
        {
            CURSE_RENDERER_LOG(Logger::Severity::Error, e.GetMessage());
        }       
    }

    void RendererVulkan::Close()
    {   
        if (m_logicalDevice)
        {
            vkDeviceWaitIdle(m_logicalDevice);
          
            for (auto& fence : m_inFlightFences)
            {
                vkDestroyFence(m_logicalDevice, fence, nullptr);
            }

            if (m_commandPool)
            {
                vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);
            }

            for (auto& framebuffer : m_presentFramebuffers)
            {
                DestroyFramebuffer(framebuffer);
            }
            m_presentFramebuffers.clear();

            if (m_renderPass)
            {
                vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr);
            }

            for (auto& imageView : m_swapChainImageViews)
            {
                vkDestroyImageView(m_logicalDevice, imageView, nullptr);
            }

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
        
        m_version = { 0, 0, 0 };
        m_renderTarget = nullptr;
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
        m_swapChainImageViews.clear();
        m_renderPass = VK_NULL_HANDLE;
        m_commandPool = VK_NULL_HANDLE;
        m_imageAvailableSemaphores.clear();
        m_renderFinishedSemaphores.clear();
        m_inFlightFences.clear();
        m_maxFramesInFlight = 0;
        m_currentFrame = 0;

        m_resized = false;
        m_beginDraw = false;    
        m_currentCommandBuffer = nullptr;
        m_currentFramebuffer = nullptr;
    }

    void RendererVulkan::Resize(const Vector2ui32& size)
    {
        if (m_beginDraw)
        {
            throw Exception("Cannot resize renderer while drawing.");
        }

        const Vector2ui32 extent(m_swapChainExtent.width, m_swapChainExtent.height);
        if (extent == size)
        {
            return;
        }

        m_swapChainExtent.width = size.x;
        m_swapChainExtent.height = size.y;
        m_resized = true;
    }

    Renderer::BackendApi RendererVulkan::GetBackendApi() const
    {
        return Renderer::BackendApi::Vulkan;
    }

    Version RendererVulkan::GetVersion() const
    {
        return m_version;
    }

    std::vector<uint8_t> RendererVulkan::CompileShader(const Shader::Format inputFormat, const Shader::Type inputType,
                                                       const std::vector<uint8_t>& inputData, const Shader::Format outputFormat)
    {
        std::string errorMessage;
        auto output = Shader::Compile(inputFormat, inputType, inputData, outputFormat, errorMessage);
        if (errorMessage.size())
        {
            CURSE_RENDERER_LOG(Logger::Severity::Error, errorMessage);
        }

        return std::move(output);
    }

    Framebuffer* RendererVulkan::CreateFramebuffer(const FramebufferDescriptor& descriptor)
    {
        VkImageView attachments[] =
        {
            descriptor.image
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = descriptor.size.x;
        framebufferInfo.height = descriptor.size.y;
        framebufferInfo.layers = 1;

        VkFramebuffer framebuffer;
        if (vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
        {
            throw Exception("Failed to create framebuffer.");
        }

        FramebufferVulkan* framebufferVulkan = new FramebufferVulkan;
        framebufferVulkan->resource = framebuffer;
        return framebufferVulkan;
    }

    Pipeline* RendererVulkan::CreatePipeline(const PipelineDescriptor& descriptor)
    {
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

        std::vector<VkVertexInputBindingDescription> bindings(descriptor.vertexBindings.size());
        size_t vertexAttributeCount = 0;
        for(size_t i = 0; i < descriptor.vertexBindings.size(); i++)
        {
            auto& binding = bindings[i];
            auto& descBinding = descriptor.vertexBindings[i];
            binding.binding = descBinding.binding;
            binding.stride = descBinding.stride;
            binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            vertexAttributeCount += descBinding.attributes.size();
        }

        std::vector<VkVertexInputAttributeDescription> vertexInputAttribute(vertexAttributeCount);
        size_t vertexAttributeIndex = 0;
        for (size_t i = 0; i < descriptor.vertexBindings.size(); i++)
        {
            auto& descBinding = descriptor.vertexBindings[i];
            uint32_t bindingId = descBinding.binding;

            for (size_t attIndex = 0; attIndex < descBinding.attributes.size(); attIndex++)
            {
                auto& attribute = vertexInputAttribute[vertexAttributeIndex];
                auto& attributeDesc = descBinding.attributes[attIndex];
                
                attribute.binding = bindingId;
                attribute.location = attributeDesc.location;
                attribute.offset = attributeDesc.offset;
                attribute.format = GetVertexAttributeFormat(attributeDesc.format);
                vertexAttributeIndex++;
            }           
        }      

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
        vertexInputInfo.pVertexBindingDescriptions = bindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttribute.size());
        vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttribute.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
        inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateInfo.topology = GetPrimitiveTopology(descriptor.topology);
        inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportStateInfo = {};
        viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.pViewports = nullptr; // Set view dynamic state in BeginDraw().
        viewportStateInfo.scissorCount = 1;
        viewportStateInfo.pScissors = nullptr; // Set view dynamic state in BeginDraw().

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

        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        const VkDynamicState dynamicStateEnables[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        dynamicStateInfo.pDynamicStates = dynamicStateEnables;
        dynamicStateInfo.dynamicStateCount = 2;
        dynamicStateInfo.flags = 0;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        VkPipelineLayout pipelineLayout;
        if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw Exception("Failed to create pipeline layout.");
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
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;


        VkPipeline graphicsPipeline;
        if (vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            vkDestroyPipelineLayout(m_logicalDevice, pipelineLayout, nullptr);
            throw Exception("Failed to create pipeline.");
        }

        PipelineVulkan* pipeline = new PipelineVulkan;
        pipeline->resource = graphicsPipeline;
        pipeline->layout = pipelineLayout;
        return pipeline;
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
        shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(rawData);

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

    Texture* RendererVulkan::CreateTexture()
    {
        return new TextureVulkan;
    }

    VertexArray* RendererVulkan::CreateVertexArray()
    {
        return nullptr;
    }

    VertexBuffer* RendererVulkan::CreateVertexBuffer(const VertexBufferDescriptor& descriptor)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = descriptor.size;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkBuffer vertexBuffer;
        if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
        {
            throw Exception("Failed to create vertex buffer.");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_logicalDevice, vertexBuffer, &memRequirements);

        uint32_t memoryTypeIndex = 0;
        if (!FindPhysicalDeviceMemoryType(memoryTypeIndex, memRequirements.memoryTypeBits,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            throw Exception("Failed to find matching memory type for vertex buffer.");
        }

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        VkDeviceMemory memory;
        if (vkAllocateMemory(m_logicalDevice, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS)
        {
            throw Exception("Failed to allocate vertex buffer memory.");
        }

        if (vkBindBufferMemory(m_logicalDevice, vertexBuffer, memory, 0) != VK_SUCCESS)
        {
            throw Exception("Failed to bind memory to vertex buffer.");
        }

        void* data;
        vkMapMemory(m_logicalDevice, memory, 0, bufferInfo.size, 0, &data);
        memcpy(data, descriptor.data, (size_t)bufferInfo.size);
        vkUnmapMemory(m_logicalDevice, memory);


        VertexBufferVulkan* buffer = new VertexBufferVulkan;
        buffer->resource = vertexBuffer;
        buffer->memory = memory;
        return buffer;
    }

    void RendererVulkan::DestroyFramebuffer(Framebuffer* framebuffer)
    {
        FramebufferVulkan* framebufferVulkan = static_cast<FramebufferVulkan*>(framebuffer);
        vkDestroyFramebuffer(m_logicalDevice, framebufferVulkan->resource, nullptr);
        delete framebufferVulkan;
    }

    void RendererVulkan::DestroyPipeline(Pipeline* pipeline)
    {
        PipelineVulkan* pipelineVulkan = static_cast<PipelineVulkan*>(pipeline);

        if (pipelineVulkan->resource)
        {
            vkDeviceWaitIdle(m_logicalDevice);
            vkDestroyPipeline(m_logicalDevice, pipelineVulkan->resource, nullptr);
        }
        if (pipelineVulkan->layout)
        {
            vkDestroyPipelineLayout(m_logicalDevice, pipelineVulkan->layout, nullptr);
        }

        delete pipelineVulkan;
    }

    void RendererVulkan::DestroyShader(Shader* shader)
    {
        ShaderVulkan* shaderVulkan = static_cast<ShaderVulkan*>(shader);
        vkDestroyShaderModule(m_logicalDevice, shaderVulkan->resource, nullptr);
        delete shaderVulkan;
    }

    void RendererVulkan::DestroyTexture(Texture* texture)
    {
        delete static_cast<TextureVulkan*>(texture);
    }

    void RendererVulkan::DestroyVertexArray(VertexArray* /*vertexArray*/)
    {
    }

    void RendererVulkan::DestroyVertexBuffer(VertexBuffer* vertexBuffer)
    {
        VertexBufferVulkan* shaderVulkan = static_cast<VertexBufferVulkan*>(vertexBuffer);
        vkDestroyBuffer(m_logicalDevice, shaderVulkan->resource, nullptr);
        vkFreeMemory(m_logicalDevice, shaderVulkan->memory, nullptr);
        delete shaderVulkan;
    }

    void RendererVulkan::BindPipeline(Pipeline* pipeline)
    {
        PipelineVulkan* pipelineVulkan = static_cast<PipelineVulkan*>(pipeline);
        vkCmdBindPipeline(*m_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineVulkan->resource);
    }

    void RendererVulkan::BeginDraw()
    {
        if (m_beginDraw)
        {
            throw Exception("Drawing has already begun.");
        }
      
        vkWaitForFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
        VkResult result = vkAcquireNextImageKHR(m_logicalDevice, m_swapChain, std::numeric_limits<uint64_t>::max(), 
            m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_resized)
        {  
            m_resized = false;
            RecreateSwapChain();
            BeginDraw();
            return;
        }
        else if (result != VK_SUCCESS)
        {
            throw Exception("Failed to acquire the next swap chain image.");
        }

        if (m_imagesInFlight[m_currentImageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(m_logicalDevice, 1, &m_imagesInFlight[m_currentImageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
        }
        m_imagesInFlight[m_currentImageIndex] = m_inFlightFences[m_currentFrame];

        if (m_currentImageIndex >= static_cast<uint32_t>(m_commandBuffers.size()))
        {
            throw Exception("Received invalid image index.");
        }

        m_currentCommandBuffer = &m_commandBuffers[m_currentImageIndex];
        m_currentFramebuffer = &m_presentFramebuffers[m_currentImageIndex]->resource;

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(*m_currentCommandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
            throw Exception("Failed to begin recording command buffer.");
        }

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = *m_currentFramebuffer;
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = m_swapChainExtent;

        const VkClearValue clearColor = { 0.3f, 0.0f, 0.0f, 1.0f };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

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

        vkCmdBeginRenderPass(*m_currentCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdSetViewport(*m_currentCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(*m_currentCommandBuffer, 0, 1, &scissor);

        m_beginDraw = true;
    }

    void RendererVulkan::DrawVertexArray(VertexArray* /*vertexArray*/)
    {
        vkCmdDraw(*m_currentCommandBuffer, 3, 1, 0, 0);
    }

    void RendererVulkan::DrawVertexBuffer(VertexBuffer* vertexBuffer)
    {
        VertexBufferVulkan* vertexBufferVulkan = static_cast<VertexBufferVulkan*>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vertexBufferVulkan->resource };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(*m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdDraw(*m_currentCommandBuffer, static_cast<uint32_t>(3), 1, 0, 0);
    }

    void RendererVulkan::EndDraw()
    {
        if (!m_beginDraw)
        {
            throw Exception("Drawing has not yet been started.");
        }

        vkCmdEndRenderPass(*m_currentCommandBuffer);
        if (vkEndCommandBuffer(*m_currentCommandBuffer) != VK_SUCCESS)
        {
            throw Exception("Failed to record command buffer.");
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore imageSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
        VkPipelineStageFlags pipelineWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = imageSemaphores;
        submitInfo.pWaitDstStageMask = pipelineWaitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = m_currentCommandBuffer;

        VkSemaphore renderSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = renderSemaphores;

        vkResetFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame]);
        if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
        {
            throw Exception("Failed to submit draw command buffer.");
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = renderSemaphores;

        VkSwapchainKHR swapChains[] = { m_swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &m_currentImageIndex;

        vkQueuePresentKHR(m_presentQueue, &presentInfo);

        m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
        m_beginDraw = false;
    }

    void RendererVulkan::WaitForDevice()
    {
        vkDeviceWaitIdle(m_logicalDevice);
    }


    // DebugMessenger implementations.
    RendererVulkan::DebugMessenger::DebugMessenger() :
        messenger(VK_NULL_HANDLE),
        CreateDebugUtilsMessengerEXT(nullptr),
        DestroyDebugUtilsMessengerEXT(nullptr)
    { }

    void RendererVulkan::DebugMessenger::Clear()
    {
        messenger = VK_NULL_HANDLE;
        CreateDebugUtilsMessengerEXT = nullptr;
        DestroyDebugUtilsMessengerEXT = nullptr;      
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

    void RendererVulkan::LoadInstance(const Version& version)
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
        if (!GetRequiredExtensions(extensions, m_logger != nullptr))
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
        bool debuggerIsAvailable = LoadDebugger(instanceInfo, debugMessageInfo);
        
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
                CURSE_RENDERER_LOG(Logger::Severity::Error, "Failed to get function pointer for \"vkCreateDebugUtilsMessengerEXT\".");
                return;
            }
            else if (!m_debugMessenger.DestroyDebugUtilsMessengerEXT)
            {
                CURSE_RENDERER_LOG(Logger::Severity::Error, "Failed to get function pointer for \"vkDestroyDebugUtilsMessengerEXT\".");
                return;
            }

            if (m_debugMessenger.CreateDebugUtilsMessengerEXT(m_instance, &debugMessageInfo, nullptr, &m_debugMessenger.messenger) != VK_SUCCESS)
            {
                CURSE_RENDERER_LOG(Logger::Severity::Error, "Failed to set up debug messenger.");
                return;
            }     
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

    bool RendererVulkan::LoadDebugger(VkInstanceCreateInfo& instanceInfo, VkDebugUtilsMessengerCreateInfoEXT& debugMessageInfo)
    {
        if (!m_logger)
        {
            return false;
        }
     
        // Add more validation layers here if desired.
        // ...
        m_validationLayers.push_back("VK_LAYER_KHRONOS_validation");
        // ...


        if (!m_validationLayers.size())
        {
            return false;
        }

        uint32_t availableLayersCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr);
        if (!availableLayersCount)
        {
            CURSE_RENDERER_LOG(Logger::Severity::Error, "Failed to find any validation layers.");
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
            CURSE_RENDERER_LOG(Logger::Severity::Error, "Failed to find all requested validation layers.");
            return false;
        }       

        instanceInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        instanceInfo.ppEnabledLayerNames = m_validationLayers.data();

        debugMessageInfo = {};
        debugMessageInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessageInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                           GetVulkanLoggerSeverityFlags(m_logger->GetSeverityFlags());
        debugMessageInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessageInfo.pUserData = m_logger;
        debugMessageInfo.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                              VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                              void* pUserData) -> VkBool32
        {
            Logger* logger = static_cast<Logger*>(pUserData);
            logger->Write(GetCurseLoggerSeverityFlag(messageSeverity), pCallbackData->pMessage);
            return VK_FALSE;
        };
        
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessageInfo;

        return true;
    }

    void RendererVulkan::LoadSurface()
    {       
    #if CURSE_PLATFORM == CURSE_PLATFORM_WINDOWS
        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.hwnd = m_renderTarget->GetWin32Window();
        surfaceInfo.hinstance = m_renderTarget->GetWin32Instance();

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

        if (!deviceFeatures.fillModeNonSolid ||
            !deviceFeatures.geometryShader ||
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
        deviceFeatures.fillModeNonSolid = VK_TRUE;

        VkDeviceCreateInfo deviceInfo = {};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.pEnabledFeatures = &deviceFeatures;
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
        deviceInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

        deviceInfo.enabledLayerCount = 0;
        if (m_debugMessenger.messenger != VK_NULL_HANDLE)
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

    void RendererVulkan::LoadSwapChain()
    {
        SwapChainSupport& swapChainSupport = m_physicalDevice.swapChainSupport;

        VkSurfaceFormatKHR surfaceFormat = {};
        bool foundSurfaceFormat = false;
        for (auto& format : swapChainSupport.formats)
        {
            if (format.format == VkFormat::VK_FORMAT_B8G8R8A8_UNORM &&
                format.colorSpace == VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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

        VkPresentModeKHR presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
        for (auto& mode : swapChainSupport.presentModes)
        {
            if (mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
            {  
                presentMode = mode;
                break;
            }
        }

        FetchSwapChainSupport(m_physicalDevice);

        VkSurfaceCapabilitiesKHR& capabilities = swapChainSupport.capabilities;
        m_swapChainExtent = capabilities.currentExtent;
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            auto windowSize = m_renderTarget->GetCurrentSize();
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

    void RendererVulkan::LoadRenderPass()
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

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        {
            throw Exception("Failed to create render pass.");
        }
    }

    void RendererVulkan::LoadPresentFramebuffer()
    {
        for (auto& imageView : m_swapChainImageViews)
        {
            FramebufferDescriptor descriptor;
            descriptor.size.x = m_swapChainExtent.width;
            descriptor.size.y = m_swapChainExtent.height;
            descriptor.image = imageView;
            FramebufferVulkan* framebuffer = static_cast<FramebufferVulkan*>(CreateFramebuffer(descriptor));
            m_presentFramebuffers.push_back(framebuffer);
        }

        if (!m_presentFramebuffers.size())
        {
            throw Exception("No framebuffers are available.");
        }

        m_maxFramesInFlight = m_presentFramebuffers.size() - 1;
    }

    void RendererVulkan::LoadCommandPool()
    {
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = m_physicalDevice.graphicsQueueIndex;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(m_logicalDevice, &commandPoolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw Exception("Failed to create command pool.");
        }

        m_commandBuffers.resize(m_presentFramebuffers.size());

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = m_commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        if (vkAllocateCommandBuffers(m_logicalDevice, &commandBufferInfo, m_commandBuffers.data()) != VK_SUCCESS)
        {
            throw Exception("Failed to allocate command buffers.");
        }
    }

    void RendererVulkan::LoadSyncObjects()
    {
        m_imageAvailableSemaphores.resize(m_maxFramesInFlight);
        m_renderFinishedSemaphores.resize(m_maxFramesInFlight);
        m_inFlightFences.resize(m_maxFramesInFlight);
        m_imagesInFlight.resize(m_swapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < m_maxFramesInFlight; i++)
        {
            if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
            {

                throw Exception("Failed to create semaphores and fences.");
            }
        }
    }

    void RendererVulkan::RecreateSwapChain()
    {
        vkDeviceWaitIdle(m_logicalDevice);
        //vkQueueWaitIdle(m_graphicsQueue);
        //vkQueueWaitIdle(m_presentQueue);       

        UnloadSwapchain();
        LoadSwapChain();
        LoadImageViews();
        LoadPresentFramebuffer();
        LoadSyncObjects();

    }

    void RendererVulkan::UnloadSwapchain()
    {
        for (auto& semaphore : m_imageAvailableSemaphores)
        {
            vkDestroySemaphore(m_logicalDevice, semaphore, nullptr);
        }
        for (auto& semaphore : m_renderFinishedSemaphores)
        {
            vkDestroySemaphore(m_logicalDevice, semaphore, nullptr);
        }

        for (auto& framebuffer : m_presentFramebuffers)
        {
            DestroyFramebuffer(framebuffer);
        }
        m_presentFramebuffers.clear();

        for (auto& imageView : m_swapChainImageViews)
        {
            vkDestroyImageView(m_logicalDevice, imageView, nullptr);
        }
        m_swapChainImageViews.clear();

        if (m_swapChain)
        {
            vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
            m_swapChain = VK_NULL_HANDLE;
        }      
    }

    bool RendererVulkan::FindPhysicalDeviceMemoryType(uint32_t& index, const uint32_t filter, const VkMemoryPropertyFlags properties)
    {
        /// < MOVE THIS CODE TO ScorePhysicalDevice....
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice.device, &memProperties);
        /// < MOVE THIS CODE TO ScorePhysicalDevice....

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((filter & (i << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                index = i;
                return true;
            }
        }

        return false;
    }
}

#endif