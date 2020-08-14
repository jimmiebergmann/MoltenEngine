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


#include "Molten/Renderer/Vulkan/VulkanRenderer.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include "Molten/Renderer/Shader/Generator/VulkanShaderGenerator.hpp"
#include "Molten/Renderer/Vulkan/VulkanFramebuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanIndexBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Molten/Renderer/Vulkan/VulkanTexture.hpp"
#include "Molten/Renderer/Vulkan/VulkanUniformBlock.hpp"
#include "Molten/Renderer/Vulkan/VulkanUniformBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanVertexBuffer.hpp"
#include "Molten/Window/Window.hpp"
#include "Molten/Logger.hpp"
#include "Molten/System/Exception.hpp"
#include "Molten/System/FileSystem.hpp"
#include "Molten/Utility/SmartFunction.hpp"
#include <map>
#include <set>
#include <algorithm>
#include <limits>
#include <memory>
#include <array>

namespace Molten
{

    // Static helper functions.
    static VkShaderStageFlagBits GetShaderProgramStageFlag(const Shader::Type type)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
        switch (type)
        {
            case Shader::Type::Vertex:   return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
            case Shader::Type::Fragment: return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
            default: break;
        }

        throw Exception("Provided shader type is not supported by the Vulkan renderer.");
        MOLTEN_UNSCOPED_ENUM_END
    }

    static VkPrimitiveTopology GetPrimitiveTopology(const Pipeline::Topology topology)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
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
        MOLTEN_UNSCOPED_ENUM_END
    }

    static VkPolygonMode GetPrimitiveTopology(const Pipeline::PolygonMode polygonMode)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
        switch (polygonMode)
        {
            case Pipeline::PolygonMode::Point: return VkPolygonMode::VK_POLYGON_MODE_POINT;
            case Pipeline::PolygonMode::Line:  return VkPolygonMode::VK_POLYGON_MODE_LINE;
            case Pipeline::PolygonMode::Fill:  return VkPolygonMode::VK_POLYGON_MODE_FILL;
            default: break;
        }
        throw Exception("Provided polygon mode is not supported by the Vulkan renderer.");
        MOLTEN_UNSCOPED_ENUM_END
    }

    static VkFrontFace GetFrontFace(const Pipeline::FrontFace frontFace)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
        switch (frontFace)
        {
            case Pipeline::FrontFace::Clockwise:        return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
            case Pipeline::FrontFace::Counterclockwise: return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
            default: break;
        }
        throw Exception("Provided front face is not supported by the Vulkan renderer.");
        MOLTEN_UNSCOPED_ENUM_END
    }

    static VkCullModeFlagBits GetCullMode(const Pipeline::CullMode cullMode)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
        switch (cullMode)
        {
            case Pipeline::CullMode::None:         return VkCullModeFlagBits::VK_CULL_MODE_NONE;
            case Pipeline::CullMode::Front:        return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
            case Pipeline::CullMode::Back:         return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
            case Pipeline::CullMode::FrontAndBack: return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
            default: break;
        }
        throw Exception("Provided cull mode is not supported by the Vulkan renderer.");
        MOLTEN_UNSCOPED_ENUM_END
    }

    static bool GetVertexAttributeFormatAndSize(const Shader::VariableDataType format, VkFormat & vulkanFormat, uint32_t & formatSize)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
        switch (format)
        {
            case Shader::VariableDataType::Bool:
            {
                vulkanFormat = VkFormat::VK_FORMAT_R8_UINT;
                formatSize = 1;
                return true;
            }
            case Shader::VariableDataType::Int32:
            {
                vulkanFormat = VkFormat::VK_FORMAT_R32_SINT;
                formatSize = 4;
                return true;
            }
            case Shader::VariableDataType::Float32:
            {
                vulkanFormat = VkFormat::VK_FORMAT_R32_SFLOAT;
                formatSize = 4;
                return true;
            }
            case Shader::VariableDataType::Vector2f32:
            {
                vulkanFormat = VkFormat::VK_FORMAT_R32G32_SFLOAT;
                formatSize = 8;
                return true;
            }
            case Shader::VariableDataType::Vector3f32:
            {
                vulkanFormat = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
                formatSize = 12;
                return true;
            }
            case Shader::VariableDataType::Vector4f32:
            {
                vulkanFormat = VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
                formatSize = 16;
                return true;
            }
            case Shader::VariableDataType::Matrix4x4f32:
            {
                vulkanFormat = VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
                formatSize = 64;
                return true;
            }

            default: break;
        }
        return false;
        MOLTEN_UNSCOPED_ENUM_END
    }

    static VkDebugUtilsMessageSeverityFlagsEXT GetVulkanLoggerSeverityFlags(const uint32_t severityFlags)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
        VkDebugUtilsMessageSeverityFlagsEXT flags = 0;

        flags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Info)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT : 0;
        flags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Warning)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT : 0;
        flags |= (severityFlags & static_cast<uint32_t>(Logger::Severity::Error)) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT : 0;

        return flags;
        MOLTEN_UNSCOPED_ENUM_END
    }

    static Logger::Severity GetMoltenLoggerSeverityFlag(const VkDebugUtilsMessageSeverityFlagsEXT severityFlags)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
        switch (severityFlags)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: return Logger::Severity::Info;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return Logger::Severity::Warning;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: return Logger::Severity::Error;
            default: break;
        }

        return Logger::Severity::Info;
        MOLTEN_UNSCOPED_ENUM_END
    }

    static VkIndexType GetIndexBufferDataType(const IndexBuffer::DataType dataType)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
        switch (dataType)
        {
            case IndexBuffer::DataType::Uint16: return VkIndexType::VK_INDEX_TYPE_UINT16;
            case IndexBuffer::DataType::Uint32: return VkIndexType::VK_INDEX_TYPE_UINT32;
            default: break;
        }
        MOLTEN_UNSCOPED_ENUM_END

        throw Exception("Provided data type is not supported as index buffer data type by the Vulkan renderer.");
    }

    static VkDeviceSize GetIndexBufferDataTypeSize(const IndexBuffer::DataType dataType)
    {
        switch (dataType)
        {
            case IndexBuffer::DataType::Uint16: return sizeof(uint16_t);
            case IndexBuffer::DataType::Uint32: return sizeof(uint32_t);
            default: break;
        }

        throw Exception("Provided data type is not supported as index buffer data type by the Vulkan renderer.");
    }

    


    // Vulkan renderer class implementations.
    VulkanRenderer::VulkanRenderer() :
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
        m_currentImageIndex(0),
        m_currentCommandBuffer(nullptr),
        m_currentFramebuffer(VK_NULL_HANDLE),
        m_currentPipeline(nullptr)
    {
    }

    VulkanRenderer::VulkanRenderer(const Window& window, const Version& version, Logger* logger) :
        VulkanRenderer()
    {
        Open(window, version, logger);
    }

    VulkanRenderer::~VulkanRenderer()
    {
        Close();
    }

    bool VulkanRenderer::Open(const Window& window, const Version& version, Logger* logger)
    {
        Close();

        m_renderTarget = &window;
        m_logger = logger;

        bool loaded =
            LoadInstance(version) &&
            LoadSurface() &&
            LoadPhysicalDevice() &&
            LoadLogicalDevice() &&
            FetchSwapChainSupport(m_physicalDevice) &&
            LoadSwapChain() &&
            LoadImageViews() &&
            LoadRenderPass() &&
            LoadPresentFramebuffer() &&
            LoadCommandPool() &&
            LoadSyncObjects();

        return loaded;   
    }

    void VulkanRenderer::Close()
    {   
        if (m_logicalDevice)
        {
            vkDeviceWaitIdle(m_logicalDevice);  

            if (m_commandPool)
            {
                vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);
            }
  
            if (m_renderPass)
            {
                vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr);
            }

            UnloadSwapchain();
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
        
        m_logger = nullptr;
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
        m_presentFramebuffers.clear();
        m_commandPool = VK_NULL_HANDLE;
        m_imageAvailableSemaphores.clear();
        m_renderFinishedSemaphores.clear();
        m_inFlightFences.clear();
        m_imagesInFlight.clear();
        m_maxFramesInFlight = 0;
        m_currentFrame = 0;

        m_resized = false;
        m_beginDraw = false;    
        m_currentCommandBuffer = nullptr;
        m_currentFramebuffer = VK_NULL_HANDLE;
        m_currentPipeline = nullptr;
    }

    void VulkanRenderer::Resize(const Vector2ui32& size)
    {
        if (m_beginDraw)
        {
            Logger::WriteError(m_logger, "Cannot resize renderer while drawing.");
            return;
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

    Renderer::BackendApi VulkanRenderer::GetBackendApi() const
    {
        return Renderer::BackendApi::Vulkan;
    }

    Version VulkanRenderer::GetVersion() const
    {
        return m_version;
    }

    uint32_t VulkanRenderer::GetPushConstantLocation(Pipeline* pipeline, const uint32_t id)
    {
        auto& locations = static_cast<VulkanPipeline*>(pipeline)->pushConstantLocations;
        auto it = locations.find(id);
        if (it == locations.end())
        {
            return 10000000;
        }

        return it->second;
    }

    Framebuffer* VulkanRenderer::CreateFramebuffer(const FramebufferDescriptor& descriptor)
    {
        return nullptr;
    }

    IndexBuffer* VulkanRenderer::CreateIndexBuffer(const IndexBufferDescriptor& descriptor)
    {
        const auto bufferSize = static_cast<VkDeviceSize>(descriptor.indexCount) * GetIndexBufferDataTypeSize(descriptor.dataType);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;        

        if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory))
        {
            return nullptr;
        }

        SmartFunction stagingDestroyer = [&]()
        {
            vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
            vkFreeMemory(m_logicalDevice, stagingMemory, nullptr);
        };

        void* data;
        vkMapMemory(m_logicalDevice, stagingMemory, 0, bufferSize, 0, &data);
        memcpy(data, descriptor.data, (size_t)bufferSize);
        vkUnmapMemory(m_logicalDevice, stagingMemory);

        VkBuffer indexBuffer;
        VkDeviceMemory indexMemory;
        if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexMemory))
        {
            return nullptr;
        }

        CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

        VulkanIndexBuffer* buffer = new VulkanIndexBuffer;
        buffer->buffer = indexBuffer;
        buffer->memory = indexMemory;
        buffer->indexCount = descriptor.indexCount;
        buffer->dataType = descriptor.dataType;
        return buffer;
    }

    Pipeline* VulkanRenderer::CreatePipeline(const PipelineDescriptor& descriptor)
    {
        if (descriptor.vertexScript == nullptr)
        {
            Logger::WriteError(m_logger, "Vertex script is missing for pipeline. (vertexScript == nullptr).");
            return nullptr;
        }
        if (descriptor.fragmentScript == nullptr)
        {
            Logger::WriteError(m_logger, "Fragment script is missing for pipeline. (fragmentScript == nullptr).");
            return nullptr;
        }

        auto& vertexScript = *descriptor.vertexScript;
        auto& fragmentScript = *descriptor.fragmentScript;
        auto& vertexOutputs = vertexScript.GetOutputInterface();
        auto& fragmentInputs = fragmentScript.GetInputInterface();

        if (!vertexOutputs.CheckCompability(fragmentInputs))
        {
            Logger::WriteError(m_logger, "Vertex output structure is not compatible with fragment input structure.");
            return nullptr;
        }

        const std::vector<Shader::Visual::Script*> shaderScripts =
        {
            descriptor.vertexScript, descriptor.fragmentScript
        };

        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
        std::vector<VkShaderModule> shaderModules;
        PushConstantLocations pushConstantLocations;
        PushConstantOffsets pushConstantOffsets;
        VkPushConstantRange pushConstantRange;

        if (!LoadShaderStages(
                shaderScripts,
                shaderModules,
                shaderStageCreateInfos,
                pushConstantLocations,
                pushConstantOffsets,
                pushConstantRange))
        {
            return nullptr;
        }

        std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
        uint32_t vertexBindingStride = 0;
        if (descriptor.vertexScript)
        {
            auto& vertexInputs = descriptor.vertexScript->GetInputInterface();
            if (!CreateVertexInputAttributes(vertexInputs, vertexInputAttributes, vertexBindingStride))
            {
                return nullptr;
            }
        }

        VkVertexInputBindingDescription vertexBinding;
        vertexBinding.binding = 0;
        vertexBinding.stride = vertexBindingStride;
        vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &vertexBinding;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributes.data();        

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

        std::vector<VkDescriptorSetLayout> setLayouts;
        if (!CreateDescriptorSetLayouts(shaderScripts, setLayouts))
        {
            return nullptr;
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = (pushConstantRange.size > 0) ? 1 : 0;
        pipelineLayoutInfo.pPushConstantRanges = (pushConstantRange.size > 0) ? &pushConstantRange : nullptr;

        VkPipelineLayout pipelineLayout;
        if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create pipeline layout.");
            return nullptr;
        }

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
        pipelineInfo.pStages = shaderStageCreateInfos.data();
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
            Logger::WriteError(m_logger, "Failed to create pipeline.");
            return nullptr;
        }

        return new VulkanPipeline(
            graphicsPipeline, 
            pipelineLayout, 
            setLayouts, 
            std::move(pushConstantLocations), 
            std::move(pushConstantOffsets),
            std::move(shaderModules));
    }

    Texture* VulkanRenderer::CreateTexture()
    {
        return new VulkanTexture;
    }

    UniformBlock* VulkanRenderer::CreateUniformBlock(const UniformBlockDescriptor& descriptor)
    {
        VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(descriptor.pipeline);
        VulkanUniformBuffer* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(descriptor.buffer);      

        if (descriptor.id >= vulkanPipeline->descriptionSetLayouts.size())
        {
            Logger::WriteError(m_logger, "Id of uniform descriptor block is too large.");
            return nullptr;
        }

        std::unique_ptr<VulkanUniformBlock, std::function<void(VulkanUniformBlock*)> > vulkanUniformBlock(new VulkanUniformBlock,
            [&](VulkanUniformBlock* uniformBlock)
        {
            DestroyUniformBlock(uniformBlock);
        });

        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSize.descriptorCount = static_cast<uint32_t>(m_swapChainImages.size());

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(m_swapChainImages.size());
        poolInfo.flags = 0;

        if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &vulkanUniformBlock->descriptorPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor pool.");
            return nullptr;
        }
      
        std::vector<VkDescriptorSetLayout> layouts(m_swapChainImages.size(), vulkanPipeline->descriptionSetLayouts[descriptor.id]);
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = vulkanUniformBlock->descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(m_swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data(); 

        vulkanUniformBlock->descriptorSets.resize(m_swapChainImages.size());
        if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, vulkanUniformBlock->descriptorSets.data()) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor sets.");
            return nullptr;
        }

        for (size_t i = 0; i < m_swapChainImages.size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = vulkanUniformBuffer->frames[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet descWrite = {};
            descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descWrite.dstSet = vulkanUniformBlock->descriptorSets[i];
            descWrite.dstBinding = 0;//descriptor.binding;
            descWrite.dstArrayElement = 0;
            descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descWrite.descriptorCount = 1;
            descWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(m_logicalDevice, 1, &descWrite, 0, nullptr);
        }

        vulkanUniformBlock->pipelineLayout = vulkanPipeline->pipelineLayout;
        vulkanUniformBlock->set = descriptor.id;

        return vulkanUniformBlock.release();
    }

    UniformBuffer* VulkanRenderer::CreateUniformBuffer(const UniformBufferDescriptor& descriptor)
    {
        std::vector<VulkanUniformBuffer::Frame> frames;
        frames.resize(m_swapChainImages.size());

        auto destroyBuffers = [&]()
        {
            for (auto& frame : frames)
            {
                if (frame.buffer != VK_NULL_HANDLE)
                {
                    vkDestroyBuffer(m_logicalDevice, frame.buffer, nullptr);
                }
                if ( frame.memory != VK_NULL_HANDLE)
                {
                    vkFreeMemory(m_logicalDevice, frame.memory, nullptr);
                }
            }    
        };

        for (auto& frame : frames)
        {
            if (!CreateBuffer(descriptor.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, frame.buffer, frame.memory))
            {
                destroyBuffers();
                return nullptr;
            }             
        }

        auto vulkanUniformBuffer = new VulkanUniformBuffer;
        vulkanUniformBuffer->frames = frames;
        return vulkanUniformBuffer;
    }

    VertexBuffer* VulkanRenderer::CreateVertexBuffer(const VertexBufferDescriptor& descriptor)
    {
        const size_t bufferSize = 
            static_cast<VkDeviceSize>(static_cast<VkDeviceSize>(descriptor.vertexCount) *
            static_cast<VkDeviceSize>(descriptor.vertexSize));

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory))
        {
            return nullptr;
        }

        SmartFunction stagingDestroyer = [&]()
        {
            vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
            vkFreeMemory(m_logicalDevice, stagingMemory, nullptr);
        };

        void* data;
        vkMapMemory(m_logicalDevice, stagingMemory, 0, bufferSize, 0, &data);
        memcpy(data, descriptor.data, (size_t)bufferSize);
        vkUnmapMemory(m_logicalDevice, stagingMemory);

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexMemory;
        if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexMemory))
        {
            return nullptr;
        }

        CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        VulkanVertexBuffer* buffer = new VulkanVertexBuffer;
        buffer->buffer = vertexBuffer;
        buffer->memory = vertexMemory;
        buffer->vertexCount = descriptor.vertexCount;
        buffer->vertexSize = descriptor.vertexSize;
        return buffer;
    }

    void VulkanRenderer::DestroyFramebuffer(Framebuffer* framebuffer)
    {
        VulkanFramebuffer* vulkanFramebuffer = static_cast<VulkanFramebuffer*>(framebuffer);
        vkDestroyFramebuffer(m_logicalDevice, vulkanFramebuffer->framebuffer, nullptr);
        delete vulkanFramebuffer;
    }

    void VulkanRenderer::DestroyIndexBuffer(IndexBuffer* indexBuffer)
    {
        VulkanIndexBuffer* vulkanIndexBuffer = static_cast<VulkanIndexBuffer*>(indexBuffer);
        vkDestroyBuffer(m_logicalDevice, vulkanIndexBuffer->buffer, nullptr);
        vkFreeMemory(m_logicalDevice, vulkanIndexBuffer->memory, nullptr);
        delete vulkanIndexBuffer;
    }

    void VulkanRenderer::DestroyPipeline(Pipeline* pipeline)
    {
        VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);

        if (vulkanPipeline->graphicsPipeline)
        {
            vkDeviceWaitIdle(m_logicalDevice);
            vkDestroyPipeline(m_logicalDevice, vulkanPipeline->graphicsPipeline, nullptr);
        }
        if (vulkanPipeline->pipelineLayout)
        {
            vkDestroyPipelineLayout(m_logicalDevice, vulkanPipeline->pipelineLayout, nullptr);
        }
        for (auto& setLayout : vulkanPipeline->descriptionSetLayouts)
        {
            vkDestroyDescriptorSetLayout(m_logicalDevice, setLayout, nullptr);
        }
        for (auto& shaderModule : vulkanPipeline->shaderModules)
        {
            vkDestroyShaderModule(m_logicalDevice, shaderModule, nullptr);
        }

        delete vulkanPipeline;
    }

    void VulkanRenderer::DestroyTexture(Texture* texture)
    {
        delete static_cast<VulkanTexture*>(texture);
    }

    void VulkanRenderer::DestroyUniformBlock(UniformBlock* uniformBlock)
    {
        VulkanUniformBlock* vulkanUniformBlock = static_cast<VulkanUniformBlock*>(uniformBlock);
        if(vulkanUniformBlock->descriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(m_logicalDevice, vulkanUniformBlock->descriptorPool, nullptr);
        }
        delete vulkanUniformBlock;
    }

    void VulkanRenderer::DestroyUniformBuffer(UniformBuffer* uniformBuffer)
    {
        VulkanUniformBuffer* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer);

        for (auto& frame : vulkanUniformBuffer->frames)
        {
            vkDestroyBuffer(m_logicalDevice, frame.buffer, nullptr);
            vkFreeMemory(m_logicalDevice, frame.memory, nullptr);
        }

        delete vulkanUniformBuffer;
    }

    void VulkanRenderer::DestroyVertexBuffer(VertexBuffer* vertexBuffer)
    {
        VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);
        vkDestroyBuffer(m_logicalDevice, vulkanVertexBuffer->buffer, nullptr);
        vkFreeMemory(m_logicalDevice, vulkanVertexBuffer->memory, nullptr);
        delete vulkanVertexBuffer;
    }

    void VulkanRenderer::BindPipeline(Pipeline* pipeline)
    {
        VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);
        vkCmdBindPipeline(*m_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->graphicsPipeline);
        m_currentPipeline = vulkanPipeline;
    }

    void VulkanRenderer::BindUniformBlock(UniformBlock* uniformBlock, const uint32_t offset)
    {
        VulkanUniformBlock* vulkanUniformBlock = static_cast<VulkanUniformBlock*>(uniformBlock);
        vkCmdBindDescriptorSets(*m_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanUniformBlock->pipelineLayout, vulkanUniformBlock->set, 1,
            &vulkanUniformBlock->descriptorSets[m_currentImageIndex], 1, &offset);
    }

    void VulkanRenderer::BeginDraw()
    {
        if (m_beginDraw)
        {
            Logger::WriteError(m_logger, "Calling BeginDraw twice, without any previous call to EndDraw.");
            return;
        }
      
        vkWaitForFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
       
        MOLTEN_UNSCOPED_ENUM_BEGIN
        VkResult result = vkAcquireNextImageKHR(m_logicalDevice, m_swapChain, std::numeric_limits<uint64_t>::max(), 
            m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_currentImageIndex);
        MOLTEN_UNSCOPED_ENUM_END

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_resized)
        {    
            m_resized = false;
            RecreateSwapChain();
            BeginDraw();
            return;
        }
        else if (result != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to acquire the next swap chain image.");
            return;
        }

        if (m_imagesInFlight[m_currentImageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(m_logicalDevice, 1, &m_imagesInFlight[m_currentImageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
        }
        m_imagesInFlight[m_currentImageIndex] = m_inFlightFences[m_currentFrame];

        if (m_currentImageIndex >= static_cast<uint32_t>(m_commandBuffers.size()))
        {
            Logger::WriteError(m_logger, "Received invalid image index.");
            return;
        }

        m_currentCommandBuffer = &m_commandBuffers[m_currentImageIndex];
        m_currentFramebuffer = m_presentFramebuffers[m_currentImageIndex]->framebuffer;

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(*m_currentCommandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to begin recording command buffer.");
            return;
        }

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = m_currentFramebuffer;
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = m_swapChainExtent;

        const VkClearValue clearColor = { 0.3f, 0.0f, 0.0f, 0.0f };
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

    void VulkanRenderer::DrawVertexBuffer(VertexBuffer* vertexBuffer)
    {
        VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer->buffer };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(*m_currentCommandBuffer, static_cast<uint32_t>(vulkanVertexBuffer->vertexCount), 1, 0, 0);
    }

    void VulkanRenderer::DrawVertexBuffer(IndexBuffer* indexBuffer, VertexBuffer* vertexBuffer)
    {
        VulkanIndexBuffer* vulkanIndexBuffer = static_cast<VulkanIndexBuffer*>(indexBuffer);
        VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer->buffer };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(*m_currentCommandBuffer, vulkanIndexBuffer->buffer, 0, GetIndexBufferDataType(vulkanIndexBuffer->dataType));
        vkCmdDrawIndexed(*m_currentCommandBuffer, static_cast<uint32_t>(vulkanIndexBuffer->indexCount), 1, 0, 0, 0);
    }

    void VulkanRenderer::PushConstant(const uint32_t location, const bool& value)
    {
        InternalPushConstant(location, value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const int32_t& value)
    {
        InternalPushConstant(location, value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const float& value)
    {
        InternalPushConstant(location, value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const Vector2f32& value)
    {
        InternalPushConstant(location, value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const Vector3f32& value)
    {
        InternalPushConstant(location, value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const Vector4f32& value)
    {
        InternalPushConstant(location, value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const Matrix4x4f32& value)
    {
        InternalPushConstant(location, value);
    }

    void VulkanRenderer::EndDraw()
    {
        if (!m_beginDraw)
        {
            Logger::WriteError(m_logger, "Calling EndDraw, without any previous call to BeginDraw.");
            return;
        }

        vkCmdEndRenderPass(*m_currentCommandBuffer);
        if (vkEndCommandBuffer(*m_currentCommandBuffer) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to record command buffer.");
            return;
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
            Logger::WriteError(m_logger, "Failed to submit draw command buffer.");
            return;
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

    void VulkanRenderer::WaitForDevice()
    {
        vkDeviceWaitIdle(m_logicalDevice);
    }

    void VulkanRenderer::UpdateUniformBuffer(UniformBuffer* uniformBuffer, const size_t offset, const size_t size, const void* data)
    {
        VulkanUniformBuffer* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer);

        auto& frame = vulkanUniformBuffer->frames[m_currentImageIndex];

        void* dataMap;
        vkMapMemory(m_logicalDevice, frame.memory, offset, size, 0, &dataMap);
        memcpy(dataMap, data, size);
        vkUnmapMemory(m_logicalDevice, frame.memory);
    }


    // DebugMessenger implementations.
    VulkanRenderer::DebugMessenger::DebugMessenger() :
        messenger(VK_NULL_HANDLE),
        CreateDebugUtilsMessengerEXT(nullptr),
        DestroyDebugUtilsMessengerEXT(nullptr)
    { }

    void VulkanRenderer::DebugMessenger::Clear()
    {
        messenger = VK_NULL_HANDLE;
        CreateDebugUtilsMessengerEXT = nullptr;
        DestroyDebugUtilsMessengerEXT = nullptr;      
    }

    VulkanRenderer::PhysicalDevice::PhysicalDevice() :
        device(VK_NULL_HANDLE),
        graphicsQueueIndex(0),
        presentQueueIndex(0)
    { }

    VulkanRenderer::PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) :
        device(device),
        graphicsQueueIndex(0),
        presentQueueIndex(0)
    { }

    VulkanRenderer::PhysicalDevice::PhysicalDevice(VkPhysicalDevice device, uint32_t graphicsQueueIndex, uint32_t presentQueueIndex) :
        device(device),
        graphicsQueueIndex(graphicsQueueIndex),
        presentQueueIndex(presentQueueIndex)
    { }

    void VulkanRenderer::PhysicalDevice::Clear()
    {
        device = VK_NULL_HANDLE;
        graphicsQueueIndex = 0;
        presentQueueIndex = 0;
    }

    PFN_vkVoidFunction VulkanRenderer::GetVulkanFunction(const char* functionName) const
    {
        return vkGetInstanceProcAddr(m_instance, functionName);
    }

    bool VulkanRenderer::LoadInstance(const Version& version)
    {
        auto newVersion = version;
        if (newVersion == Version::None)
        {
            newVersion = Version(1, 1);
        }
        const auto moltenVersion = MOLTEN_VERSION;
        auto engineVersion = VK_MAKE_VERSION(moltenVersion.Major, moltenVersion.Minor, moltenVersion.Patch);
        auto vulkanVersion = VK_MAKE_VERSION(newVersion.Major, newVersion.Minor, newVersion.Patch);

        // Get required extensions.
        std::vector<std::string> extensions;
        if (!GetRequiredExtensions(extensions, m_logger != nullptr))
        {
            Logger::WriteError(m_logger, "The required Vulkan extensions are unavailable.");
            return false;
        }
        std::vector<const char*> ptrExtensions(extensions.size());
        for (size_t i = 0; i < extensions.size(); i++)
        {
            ptrExtensions[i] = extensions[i].c_str();
        }

        // Create instance.
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Molten Engine Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Molten Engine";
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
                {
                    Logger::WriteError(m_logger, "VulkanRenderer: Driver for version " + version.AsString() + " of Vulkan is unavailable.");
                    return false;
                }
                break;
                default:
                    break;
            }

            Logger::WriteError(m_logger, "Failed to create Vulkan instance.");
            return false;
        }

        if (debuggerIsAvailable)
        {
            m_debugMessenger.CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)GetVulkanFunction("vkCreateDebugUtilsMessengerEXT");
            m_debugMessenger.DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)GetVulkanFunction("vkDestroyDebugUtilsMessengerEXT");

            if (!m_debugMessenger.CreateDebugUtilsMessengerEXT)
            {
                Logger::WriteError(m_logger, "Failed to get function pointer for \"vkCreateDebugUtilsMessengerEXT\".");
                return true;
            }
            else if (!m_debugMessenger.DestroyDebugUtilsMessengerEXT)
            {
                Logger::WriteError(m_logger, "Failed to get function pointer for \"vkDestroyDebugUtilsMessengerEXT\".");
                return true;
            }

            if (m_debugMessenger.CreateDebugUtilsMessengerEXT(m_instance, &debugMessageInfo, nullptr, &m_debugMessenger.messenger) != VK_SUCCESS)
            {
                Logger::WriteError(m_logger, "Failed to set up debug messenger.");
                return true;
            }     
        }

        return true;
    }

    bool VulkanRenderer::GetRequiredExtensions(std::vector<std::string>& out, const bool requestDebugger) const
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
        #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
            out.push_back("VK_KHR_win32_surface");
        #elif MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX
            out.push_back("VK_KHR_xlib_surface");
        #endif

        std::set<std::string> missingExtensions(out.begin(), out.end());
        for (const auto& extension : extensions)
        {
            missingExtensions.erase(extension.extensionName);
        }

        return missingExtensions.size() == 0;
    }

    bool VulkanRenderer::LoadDebugger(VkInstanceCreateInfo& instanceInfo, VkDebugUtilsMessengerCreateInfoEXT& debugMessageInfo)
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
            Logger::WriteError(m_logger, "Failed to find any validation layers.");
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
            Logger::WriteError(m_logger, "Failed to find all requested validation layers.");
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

        MOLTEN_UNSCOPED_ENUM_BEGIN
        debugMessageInfo.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                              VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                              void* pUserData) -> VkBool32
        {
            Logger* logger = static_cast<Logger*>(pUserData);
            logger->Write(GetMoltenLoggerSeverityFlag(messageSeverity), pCallbackData->pMessage);
            return VK_FALSE;
        };
        MOLTEN_UNSCOPED_ENUM_END
        
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessageInfo;

        return true;
    }

    bool VulkanRenderer::LoadSurface()
    {       
    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
        
        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.hwnd = m_renderTarget->GetWin32Window();
        surfaceInfo.hinstance = m_renderTarget->GetWin32Instance();

        if (vkCreateWin32SurfaceKHR(m_instance, &surfaceInfo, nullptr, &m_surface) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create window surface.");
            return false;
        }
        return true;

    #else

        Logger::WriteError(m_logger, "Vulkan surface loading is not supported for platform: " MOLTEN_PLATFORM_NAME);
        return false;

    #endif
    }

    bool VulkanRenderer::LoadPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
        if (!deviceCount)
        {
            Logger::WriteError(m_logger, "Failed to find any physical device supporting Vulkan.");
            return false;
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
            Logger::WriteError(m_logger, "Failed to find any physical device supporting the requirements.");
            return false;
        }

        m_physicalDevice = scoredDevices.rbegin()->second;
        return true;
    }

    bool VulkanRenderer::ScorePhysicalDevice(PhysicalDevice& physicalDevice, uint32_t& score)
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

    bool VulkanRenderer::CheckDeviceExtensionSupport(PhysicalDevice& physicalDevice)
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
            Logger::WriteError(m_logger, "Failed to find any device extensions.");
            return false;
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

    bool VulkanRenderer::FetchSwapChainSupport(PhysicalDevice& physicalDevice)
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

    bool VulkanRenderer::LoadLogicalDevice()
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
            Logger::WriteError(m_logger, "Failed to create logical device.");
            return false;
        }

        vkGetDeviceQueue(m_logicalDevice, m_physicalDevice.graphicsQueueIndex, 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_logicalDevice, m_physicalDevice.presentQueueIndex, 0, &m_presentQueue);
        return true;
    }

    bool VulkanRenderer::LoadSwapChain()
    {
        VkSurfaceFormatKHR surfaceFormat = {};
        bool foundSurfaceFormat = false;
        for (auto& format : m_physicalDevice.swapChainSupport.formats)
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
            Logger::WriteError(m_logger, "Failed find required surface format for the swap chain.");
            return false;
        }

        MOLTEN_UNSCOPED_ENUM_BEGIN
        VkPresentModeKHR presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
        MOLTEN_UNSCOPED_ENUM_END
        for (auto& mode : m_physicalDevice.swapChainSupport.presentModes)
        {
            if (mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
            {  
                presentMode = mode;
                break;
            }
        }

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice.device, m_surface, &m_physicalDevice.swapChainSupport.capabilities);
        VkSurfaceCapabilitiesKHR& capabilities = m_physicalDevice.swapChainSupport.capabilities;
        
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            auto windowSize = m_renderTarget->GetSize();
            capabilities.currentExtent = {
                std::max(capabilities.currentExtent.width, std::min(windowSize.x, capabilities.maxImageExtent.width)),
                std::max(capabilities.currentExtent.height, std::min(windowSize.y, capabilities.maxImageExtent.height))
            };
        }
        m_swapChainExtent = capabilities.currentExtent;

        const uint32_t imageCount = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);

        m_swapChain = Vulkan::CreateSwapchain(
            m_physicalDevice.device, m_logicalDevice, m_surface, surfaceFormat,
            presentMode, capabilities, imageCount, m_physicalDevice.graphicsQueueIndex, m_physicalDevice.presentQueueIndex, m_swapChain);

        if(m_swapChain == VK_NULL_HANDLE)
        {
            Logger::WriteError(m_logger, "Failed create swap chain.");
            return false;
        }

        Vulkan::GetSwapchainImages(m_logicalDevice, m_swapChain, m_swapChainImages);
        if(m_swapChainImages.size() != imageCount)
        {
            vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
            Logger::WriteError(m_logger, "Failed to create the requested number of swap chain images.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadImageViews()
    {
        m_swapChainImageViews.resize(m_swapChainImages.size(), VK_NULL_HANDLE);

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
                Logger::WriteError(m_logger, "Failed create image view " + std::to_string(i) + ".");
                return false;
            }
        }

        return true;
    }

    bool VulkanRenderer::LoadRenderPass()
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
            Logger::WriteError(m_logger, "Failed to create render pass.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadPresentFramebuffer()
    {
        for (auto& imageView : m_swapChainImageViews)
        {
            VulkanFramebuffer* framebuffer = static_cast<VulkanFramebuffer*>(
                CreateFramebuffer(imageView, { m_swapChainExtent.width, m_swapChainExtent.height }));
            m_presentFramebuffers.push_back(framebuffer);
        }

        if (!m_presentFramebuffers.size())
        {
            Logger::WriteError(m_logger, "No framebuffers are available.");
            return false;
        }

        m_maxFramesInFlight = m_presentFramebuffers.size() - 1;

        return true;
    }

    Framebuffer* VulkanRenderer::CreateFramebuffer(const VkImageView& imageView, const Vector2ui32 size)
    {
        VkImageView attachments[] = { imageView };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = size.x;
        framebufferInfo.height = size.y;
        framebufferInfo.layers = 1;

        VkFramebuffer framebuffer;
        if (vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create framebuffer.");
            return nullptr;
        }

        VulkanFramebuffer* vulkanFramebuffer = new VulkanFramebuffer;
        vulkanFramebuffer->framebuffer = framebuffer;
        return vulkanFramebuffer;
    }

    bool VulkanRenderer::LoadCommandPool()
    {
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = m_physicalDevice.graphicsQueueIndex;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(m_logicalDevice, &commandPoolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create command pool.");
            return false;
        }

        m_commandBuffers.resize(m_presentFramebuffers.size());

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = m_commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        if (vkAllocateCommandBuffers(m_logicalDevice, &commandBufferInfo, m_commandBuffers.data()) != VK_SUCCESS)
        {
            vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);
            Logger::WriteError(m_logger, "Failed to allocate command buffers.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadSyncObjects()
    {
        m_imageAvailableSemaphores.resize(m_maxFramesInFlight);
        m_renderFinishedSemaphores.resize(m_maxFramesInFlight);
        m_inFlightFences.resize(m_maxFramesInFlight, VK_NULL_HANDLE);
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
                Logger::WriteError(m_logger, "Failed to create semaphores and fences.");
                return false;
            }
        }

        return true;
    }

    bool VulkanRenderer::RecreateSwapChain()
    {
        vkDeviceWaitIdle(m_logicalDevice);

        if (m_swapChain)
        {
            Vulkan::DestroySemaphores(m_logicalDevice, m_imageAvailableSemaphores);
            Vulkan::DestroySemaphores(m_logicalDevice, m_renderFinishedSemaphores);
            Vulkan::DestroyFences(m_logicalDevice, m_inFlightFences);

            m_imagesInFlight.clear();

            for (auto& framebuffer : m_presentFramebuffers)
            {
                DestroyFramebuffer(framebuffer);
            }
            m_presentFramebuffers.clear();

            Vulkan::DestroyImageViews(m_logicalDevice, m_swapChainImageViews);
        }


        bool loaded =
            LoadSwapChain() &&
            LoadImageViews() &&
            LoadPresentFramebuffer()&&
            LoadSyncObjects();

        return loaded;
    }

    void VulkanRenderer::UnloadSwapchain()
    {
        Vulkan::DestroySemaphores(m_logicalDevice, m_imageAvailableSemaphores);
        Vulkan::DestroySemaphores(m_logicalDevice, m_renderFinishedSemaphores);
        Vulkan::DestroyFences(m_logicalDevice, m_inFlightFences);

        m_imagesInFlight.clear();

        for (auto& framebuffer : m_presentFramebuffers)
        {
            DestroyFramebuffer(framebuffer);
        }
        m_presentFramebuffers.clear();

        Vulkan::DestroyImageViews(m_logicalDevice, m_swapChainImageViews);

        if (m_swapChain)
        {
            vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
            m_swapChain = VK_NULL_HANDLE;
        }      
    }

    bool VulkanRenderer::FindPhysicalDeviceMemoryType(uint32_t& index, const uint32_t filter, const VkMemoryPropertyFlags properties)
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

    bool VulkanRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create vertex buffer.");
            return false;
        }

        VkMemoryRequirements memoryReq;
        vkGetBufferMemoryRequirements(m_logicalDevice, buffer, &memoryReq);

        uint32_t memoryTypeIndex = 0;
        if (!FindPhysicalDeviceMemoryType(memoryTypeIndex, memoryReq.memoryTypeBits, properties))
        {
            vkDestroyBuffer(m_logicalDevice, buffer, nullptr);
            Logger::WriteError(m_logger, "Failed to find matching memory type for vertex buffer.");
            return false;
        }

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryReq.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(m_logicalDevice, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS)
        {
            vkDestroyBuffer(m_logicalDevice, buffer, nullptr);
            Logger::WriteError(m_logger, "Failed to allocate vertex buffer memory.");
            return false;
        }

        if (vkBindBufferMemory(m_logicalDevice, buffer, memory, 0) != VK_SUCCESS)
        {
            vkDestroyBuffer(m_logicalDevice, buffer, nullptr);
            vkFreeMemory(m_logicalDevice, memory, nullptr);
            Logger::WriteError(m_logger, "Failed to bind memory to vertex buffer.");
            return false;
        }

        return true;
    }

    void VulkanRenderer::CopyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size)
    {
        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandPool = m_commandPool;
        commandBufferInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_logicalDevice, &commandBufferInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        VkBufferCopy copy = {};
        copy.srcOffset = 0;
        copy.dstOffset = 0;
        copy.size = size;
        vkCmdCopyBuffer(commandBuffer, source, destination, 1, &copy);
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        // USE FENCE?
        vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_graphicsQueue);

        vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBuffer);
    }

    bool VulkanRenderer::CreateVertexInputAttributes(
        const Shader::Visual::InputStructure& inputs,
        std::vector<VkVertexInputAttributeDescription>& attributes, 
        uint32_t& stride)
    {
        attributes.resize(inputs.GetMemberCount());
        size_t index = 0;
        uint32_t location = 0;

        for (auto* inputNode : inputs.GetMembers())
        {
            for (auto* outputPin : inputNode->GetOutputPins())
            {
                VkFormat format;
                uint32_t formatSize;
                if (!GetVertexAttributeFormatAndSize(outputPin->GetDataType(), format, formatSize))
                {
                    Logger::WriteError(m_logger, "Failed to find attribute format.");
                    return false;
                }

                auto& attribute = attributes[index++];
                attribute.binding = 0;
                attribute.location = location;
                attribute.offset = stride;
                attribute.format = format;

                location++;
                stride += formatSize;
            }
        }
        return true;
    }

    bool VulkanRenderer::CreateDescriptorSetLayouts(
        const std::vector<Shader::Visual::Script*>& visualScripts, 
        std::vector<VkDescriptorSetLayout>& setLayouts)
    {
        std::map<uint32_t, VkShaderStageFlags> uniformShaderStageFlags;
        uint32_t highestSetId = 0;
        for (auto script : visualScripts)
        {
            auto& uniformInterfaces = script->GetUniformInterfaces();

            for (auto* uniformInterface : uniformInterfaces)
            {
                uint32_t setId = uniformInterface->GetId();
                highestSetId = setId > highestSetId ? setId : highestSetId;

                auto it = uniformShaderStageFlags.find(setId);
                if (it != uniformShaderStageFlags.end())
                {
                    it->second |= GetShaderProgramStageFlag(script->GetType());
                    continue;
                }

                uniformShaderStageFlags.insert({ setId , GetShaderProgramStageFlag(script->GetType()) });
            }
        }

        if (highestSetId > static_cast<uint32_t>(uniformShaderStageFlags.size()))
        {
            Logger::WriteError(m_logger, "Uniform interface id is out of bound, expected " + 
                std::to_string(uniformShaderStageFlags.size() - 1) + " to be the highest.");
            return nullptr;
        }

        setLayouts.clear();
        for (auto& pair : uniformShaderStageFlags)
        {
            auto stageFlags = pair.second;

            VkDescriptorSetLayoutBinding binding;
            binding.binding = 0;
            binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            binding.descriptorCount = 1;
            binding.stageFlags = stageFlags;
            binding.pImmutableSamplers = nullptr;

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
            descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutInfo.bindingCount = 1;
            descriptorSetLayoutInfo.pBindings = &binding;

            VkDescriptorSetLayout descriptorSetLayout;
            if (vkCreateDescriptorSetLayout(m_logicalDevice, &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            {
                Logger::WriteError(m_logger, "Failed to create descriptor set layout.");
                return false;
            }

            setLayouts.push_back(descriptorSetLayout);
        }
        return true;
    }

    bool VulkanRenderer::LoadShaderStages(
        const std::vector<Shader::Visual::Script*>& visualScripts,
        std::vector<VkShaderModule>& shaderModules,
        std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
        PushConstantLocations& pushConstantLocations,
        PushConstantOffsets& pushConstantOffsets,
        VkPushConstantRange& pushConstantRange)
    {
        pushConstantRange.size = 0;

        Shader::VulkanGenerator::GlslTemplates glslTemplates;
        if (!Shader::VulkanGenerator::GenerateGlslTemplate(glslTemplates, visualScripts, m_logger))
        {
            return false;
        }

        shaderStageCreateInfos.reserve(visualScripts.size());
        shaderModules.reserve(visualScripts.size());

        auto& pushConstantTemplate = glslTemplates.pushConstantTemplate;

        for(size_t i = 0; i < visualScripts.size(); i++)
        {
            auto* script = visualScripts[i];
            auto scriptType = script->GetType();

            Shader::VulkanGenerator::GlslStageTemplates stageTemplate;
            stageTemplate.pushConstantTemplate.blockSource = &pushConstantTemplate.blockSource;
            stageTemplate.pushConstantTemplate.offsets = &pushConstantTemplate.stageOffsets[i];

            auto glslCode = Shader::VulkanGenerator::GenerateGlsl(*script, &stageTemplate, m_logger);
            if (!glslCode.size())
            {
                Logger::WriteError(m_logger, "Failed to generate GLSL code.");
                return nullptr;
            }

            auto sprivCode = Shader::VulkanGenerator::ConvertGlslToSpriV(glslCode, scriptType, m_logger);
            if (!sprivCode.size())
            {
                Logger::WriteError(m_logger, "Failed to convert GLSL to SPIR-V.");
                return nullptr;
            }

            auto shaderModule = CreateShaderModule(sprivCode);
            if (shaderModule == VK_NULL_HANDLE)
            {
                Logger::WriteError(m_logger, "Failed to create shader module.");
                return nullptr;
            }
            shaderModules.push_back(shaderModule);

            VkPipelineShaderStageCreateInfo stageCreateInfo = {};
            stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageCreateInfo.pName = "main";
            stageCreateInfo.module = shaderModule;
            stageCreateInfo.stage = GetShaderProgramStageFlag(scriptType);
            shaderStageCreateInfos.push_back(stageCreateInfo);
        }

        if (pushConstantTemplate.blockByteCount)
        {
            pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
            pushConstantRange.offset = 0;
            pushConstantRange.size = pushConstantTemplate.blockByteCount;
        }

        pushConstantLocations = std::move(pushConstantTemplate.locations);
        pushConstantOffsets = std::move(pushConstantTemplate.offsets);

        return true;
    }

    VkShaderModule VulkanRenderer::CreateShaderModule(const std::vector<uint8_t>& spirvCode)
    {
        VkShaderModuleCreateInfo shaderModuleInfo = {};
        shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleInfo.codeSize = spirvCode.size();
        shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(spirvCode.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_logicalDevice, &shaderModuleInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create shader module.");
            return VK_NULL_HANDLE;
        }

        return shaderModule;
    }

}

#endif