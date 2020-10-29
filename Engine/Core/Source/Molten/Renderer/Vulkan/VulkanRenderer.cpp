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

#include "Molten/Renderer/Vulkan/VulkanFunctions.hpp"
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
#include "Molten/Renderer/Vulkan/Utility/VulkanResultLogger.hpp"
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
        }
        return false;
        MOLTEN_UNSCOPED_ENUM_END
    }

    static VkIndexType GetIndexBufferDataType(const IndexBuffer::DataType dataType)
    {
        MOLTEN_UNSCOPED_ENUM_BEGIN
        switch (dataType)
        {
            case IndexBuffer::DataType::Uint16: return VkIndexType::VK_INDEX_TYPE_UINT16;
            case IndexBuffer::DataType::Uint32: return VkIndexType::VK_INDEX_TYPE_UINT32;
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
        }

        throw Exception("Provided data type is not supported as index buffer data type by the Vulkan renderer.");
    }

    
    // Vulkan renderer class implementations.
    VulkanRenderer::VulkanRenderer() :
        m_renderTarget(nullptr),
        m_version(0, 0, 0),
        m_logger(nullptr), 
        m_requiredInstanceExtensions{},
        m_requiredInstanceLayers{},
        m_requiredDeviceExtensions{},
        m_requiredDeviceFeatures{},
        m_isOpen(false),
        m_enableDebugMessenger(false),
        m_debugInstanceExtensions{},
        m_debugInstanceLayers{},
        m_instance{},
        m_debugMessenger{},
        m_surface(VK_NULL_HANDLE),
        m_physicalDevice{},
        m_logicalDevice{},
        m_swapChain{},
        //  vvvvvvvv
        
        
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

    VulkanRenderer::VulkanRenderer(RenderTarget& renderTarget, const Version& version, Logger* logger) :
        VulkanRenderer()
    {
        Open(renderTarget, version, logger);
    }

    VulkanRenderer::~VulkanRenderer()
    {
        Close();
    }

    bool VulkanRenderer::Open(RenderTarget& renderTarget, const Version& version, Logger* logger)
    {
        Close();

        m_renderTarget = &renderTarget;
        m_logger = logger;

    #if MOLTEN_BUILD == MOLTEN_BUILD_DEBUG
        m_enableDebugMessenger = true;
    #endif

        bool loaded =
            LoadRequirements() &&
            LoadInstance(version) &&
            LoadSurface() &&
            LoadPhysicalDevice() &&

            LoadLogicalDevice() &&
            LoadSwapChain() &&
            LoadImageViews() &&
            LoadRenderPass() &&
            LoadPresentFramebuffer() &&
            LoadCommandPool() &&
            LoadSyncObjects();

        m_isOpen = loaded;
        return loaded;   
    }

    void VulkanRenderer::Close()
    {
        if (m_logicalDevice.handle)
        {
            vkDeviceWaitIdle(m_logicalDevice.handle);

            if (m_commandPool)
            {
                vkDestroyCommandPool(m_logicalDevice.handle, m_commandPool, nullptr);
            }

            if (m_renderPass)
            {
                vkDestroyRenderPass(m_logicalDevice.handle, m_renderPass, nullptr);
            }

            UnloadSwapchain();
            vkDestroyDevice(m_logicalDevice.handle, nullptr);
        }

        m_debugMessenger.Destroy();

        if (m_instance.GetHandle())
        {
            if (m_surface)
            {
                vkDestroySurfaceKHR(m_instance.GetHandle(), m_surface, nullptr);

            }

            vkDestroyInstance(m_instance.GetHandle(), nullptr);
        }

        m_renderTarget = nullptr;
        m_version = { 0, 0, 0 };
        m_logger = nullptr;
        m_requiredInstanceExtensions = {};
        m_requiredInstanceLayers = {};
        m_requiredDeviceExtensions = {};
        m_isOpen = false;
        m_debugInstanceExtensions = {};
        m_debugInstanceLayers = {};
        m_instance = {};
        m_surface = VK_NULL_HANDLE;
        m_physicalDevice = {};
        m_logicalDevice = {};
        m_swapChain = {};

        m_swapChainImageFormat = VK_FORMAT_UNDEFINED;
        m_swapChainExtent = { 0, 0 };
        m_swapChainImages.clear();
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

    bool VulkanRenderer::IsOpen() const
    {
        return m_isOpen;
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
            vkDestroyBuffer(m_logicalDevice.handle, stagingBuffer, nullptr);
            vkFreeMemory(m_logicalDevice.handle, stagingMemory, nullptr);
        };

        void* data;
        vkMapMemory(m_logicalDevice.handle, stagingMemory, 0, bufferSize, 0, &data);
        std::memcpy(data, descriptor.data, (size_t)bufferSize);
        vkUnmapMemory(m_logicalDevice.handle, stagingMemory);

        VkBuffer indexBuffer;
        VkDeviceMemory indexMemory;
        if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexMemory))
        {
            return nullptr;
        }

        CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

        auto* buffer = new VulkanIndexBuffer(indexBuffer, indexMemory, descriptor.indexCount, descriptor.dataType);
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
        Vulkan::ShaderModules shaderModules;
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

        Vulkan::DescriptorSetLayouts setLayouts;
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
        if (vkCreatePipelineLayout(m_logicalDevice.handle, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
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
        if (vkCreateGraphicsPipelines(m_logicalDevice.handle, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            vkDestroyPipelineLayout(m_logicalDevice.handle, pipelineLayout, nullptr);
            Logger::WriteError(m_logger, "Failed to create pipeline.");
            return nullptr;
        }

        return new VulkanPipeline(
            graphicsPipeline, 
            pipelineLayout, 
            std::move(setLayouts),
            std::move(pushConstantLocations), 
            std::move(pushConstantOffsets),
            std::move(shaderModules));
    }

    Texture* VulkanRenderer::CreateTexture(const TextureDescriptor& descriptor)
    {
        const auto bufferSize = 
            static_cast<VkDeviceSize>(descriptor.dimensions.x) * 
            static_cast<VkDeviceSize>(descriptor.dimensions.y) * 4;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory))
        {
            return nullptr;
        }

        SmartFunction stagingDestroyer = [&]()
        {
            vkDestroyBuffer(m_logicalDevice.handle, stagingBuffer, nullptr);
            vkFreeMemory(m_logicalDevice.handle, stagingMemory, nullptr);
        };

        void* data;
        vkMapMemory(m_logicalDevice.handle, stagingMemory, 0, bufferSize, 0, &data);
        std::memcpy(data, descriptor.data, (size_t)bufferSize);
        vkUnmapMemory(m_logicalDevice.handle, stagingMemory);

        VkImage image;
        VkDeviceMemory imageMemory;
        if (!CreateImage(descriptor.dimensions, VK_FORMAT_R8G8B8A8_SRGB, 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            image, imageMemory))
        {
            Logger::WriteError(m_logger, "Failed to create image.");
            return nullptr;
        }
        

        /*VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = descriptor.dimensions.x;
        imageInfo.extent.height = descriptor.dimensions.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;

        VkImage textureImage;
        if (vkCreateImage(m_logicalDevice.handle, &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to crteate vulkan image.");
            return nullptr;
        }

        VkDeviceMemory textureImageMemory;*/

        /*
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexMemory;
        if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexMemory))
        {
            return nullptr;
        }

        CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);*/


        auto* texture = new VulkanTexture(image, imageMemory);
        return texture;
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

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        SmartFunction descriptorPoolDestroyer = [&]()
        {
            if (descriptorPool)
            {
                vkDestroyDescriptorPool(m_logicalDevice.handle, descriptorPool, nullptr);
            }
        };

        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSize.descriptorCount = static_cast<uint32_t>(m_swapChainImages.size());

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(m_swapChainImages.size());
        poolInfo.flags = 0;

        if (vkCreateDescriptorPool(m_logicalDevice.handle, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor pool.");
            return nullptr;
        }
      
        std::vector<VkDescriptorSetLayout> layouts(m_swapChainImages.size(), vulkanPipeline->descriptionSetLayouts[descriptor.id]);
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(m_swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data(); 

        Vulkan::DescriptorSets descriptorSets(m_swapChainImages.size(), VK_NULL_HANDLE);
        if (vkAllocateDescriptorSets(m_logicalDevice.handle, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor sets.");
            return nullptr;
        }

        for (size_t i = 0; i < descriptorSets.size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = vulkanUniformBuffer->frames[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet descWrite = {};
            descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descWrite.dstSet = descriptorSets[i];
            descWrite.dstBinding = 0;//descriptor.binding;
            descWrite.dstArrayElement = 0;
            descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descWrite.descriptorCount = 1;
            descWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(m_logicalDevice.handle, 1, &descWrite, 0, nullptr);
        }

        descriptorPoolDestroyer.Release();

        auto* uniformBlock = new VulkanUniformBlock(
            vulkanPipeline->pipelineLayout, descriptorPool, std::move(descriptorSets), descriptor.id);
        return uniformBlock;
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
                    vkDestroyBuffer(m_logicalDevice.handle, frame.buffer, nullptr);
                }
                if ( frame.memory != VK_NULL_HANDLE)
                {
                    vkFreeMemory(m_logicalDevice.handle, frame.memory, nullptr);
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
            vkDestroyBuffer(m_logicalDevice.handle, stagingBuffer, nullptr);
            vkFreeMemory(m_logicalDevice.handle, stagingMemory, nullptr);
        };

        void* data;
        vkMapMemory(m_logicalDevice.handle, stagingMemory, 0, bufferSize, 0, &data);
        std::memcpy(data, descriptor.data, (size_t)bufferSize);
        vkUnmapMemory(m_logicalDevice.handle, stagingMemory);

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexMemory;
        if (!CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexMemory))
        {
            return nullptr;
        }

        CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        auto* buffer = new VulkanVertexBuffer(vertexBuffer, vertexMemory, descriptor.vertexCount, descriptor.vertexSize);
        return buffer;
    }

    void VulkanRenderer::DestroyFramebuffer(Framebuffer* framebuffer)
    {
        VulkanFramebuffer* vulkanFramebuffer = static_cast<VulkanFramebuffer*>(framebuffer);
        vkDestroyFramebuffer(m_logicalDevice.handle, vulkanFramebuffer->framebuffer, nullptr);
        delete vulkanFramebuffer;
    }

    void VulkanRenderer::DestroyIndexBuffer(IndexBuffer* indexBuffer)
    {
        VulkanIndexBuffer* vulkanIndexBuffer = static_cast<VulkanIndexBuffer*>(indexBuffer);
        vkDestroyBuffer(m_logicalDevice.handle, vulkanIndexBuffer->buffer, nullptr);
        vkFreeMemory(m_logicalDevice.handle, vulkanIndexBuffer->memory, nullptr);
        delete vulkanIndexBuffer;
    }

    void VulkanRenderer::DestroyPipeline(Pipeline* pipeline)
    {
        VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);

        if (vulkanPipeline->graphicsPipeline)
        {
            vkDeviceWaitIdle(m_logicalDevice.handle);
            vkDestroyPipeline(m_logicalDevice.handle, vulkanPipeline->graphicsPipeline, nullptr);
        }
        if (vulkanPipeline->pipelineLayout)
        {
            vkDestroyPipelineLayout(m_logicalDevice.handle, vulkanPipeline->pipelineLayout, nullptr);
        }
        for (auto& setLayout : vulkanPipeline->descriptionSetLayouts)
        {
            vkDestroyDescriptorSetLayout(m_logicalDevice.handle, setLayout, nullptr);
        }
        for (auto& shaderModule : vulkanPipeline->shaderModules)
        {
            vkDestroyShaderModule(m_logicalDevice.handle, shaderModule, nullptr);
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
            vkDestroyDescriptorPool(m_logicalDevice.handle, vulkanUniformBlock->descriptorPool, nullptr);
        }
        delete vulkanUniformBlock;
    }

    void VulkanRenderer::DestroyUniformBuffer(UniformBuffer* uniformBuffer)
    {
        VulkanUniformBuffer* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer);

        for (auto& frame : vulkanUniformBuffer->frames)
        {
            vkDestroyBuffer(m_logicalDevice.handle, frame.buffer, nullptr);
            vkFreeMemory(m_logicalDevice.handle, frame.memory, nullptr);
        }

        delete vulkanUniformBuffer;
    }

    void VulkanRenderer::DestroyVertexBuffer(VertexBuffer* vertexBuffer)
    {
        VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);
        vkDestroyBuffer(m_logicalDevice.handle, vulkanVertexBuffer->buffer, nullptr);
        vkFreeMemory(m_logicalDevice.handle, vulkanVertexBuffer->memory, nullptr);
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
      
        vkWaitForFences(m_logicalDevice.handle, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
       
        MOLTEN_UNSCOPED_ENUM_BEGIN
        VkResult result = vkAcquireNextImageKHR(m_logicalDevice.handle, m_swapChain.handle, std::numeric_limits<uint64_t>::max(), 
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
            vkWaitForFences(m_logicalDevice.handle, 1, &m_imagesInFlight[m_currentImageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
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

        vkResetFences(m_logicalDevice.handle, 1, &m_inFlightFences[m_currentFrame]);
        if (vkQueueSubmit(m_logicalDevice.graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to submit draw command buffer.");
            return;
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = renderSemaphores;

        VkSwapchainKHR swapChains[] = { m_swapChain.handle };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &m_currentImageIndex;

        vkQueuePresentKHR(m_logicalDevice.presentQueue, &presentInfo);

        m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
        m_beginDraw = false;
    }

    void VulkanRenderer::WaitForDevice()
    {
        vkDeviceWaitIdle(m_logicalDevice.handle);
    }

    void VulkanRenderer::UpdateUniformBuffer(UniformBuffer* uniformBuffer, const size_t offset, const size_t size, const void* data)
    {
        VulkanUniformBuffer* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer);

        auto& frame = vulkanUniformBuffer->frames[m_currentImageIndex];

        void* dataMap;
        vkMapMemory(m_logicalDevice.handle, frame.memory, offset, size, 0, &dataMap);
        std::memcpy(dataMap, data, size);
        vkUnmapMemory(m_logicalDevice.handle, frame.memory);
    }

    bool VulkanRenderer::LoadRequirements()
    {
        // Debug extensions and layers
        m_debugInstanceExtensions = { Vulkan::Extension{ VK_EXT_DEBUG_UTILS_EXTENSION_NAME } };
        m_debugInstanceLayers = { Vulkan::Layer{ "VK_LAYER_KHRONOS_validation" } };


        // Instance extensions.
        m_requiredInstanceExtensions = { Vulkan::Extension{ "VK_KHR_surface", 0 } };

        auto& platformSurfaceExtension = Vulkan::GetPlatformSurfaceExtensionName();
        if (platformSurfaceExtension.size() != 0)
        {
            m_requiredInstanceExtensions.push_back(Vulkan::Extension{ platformSurfaceExtension });
        }

        if (m_enableDebugMessenger)
        {
            m_requiredInstanceExtensions.insert(
                m_requiredInstanceExtensions.end(), 
                m_debugInstanceExtensions.begin(), 
                m_debugInstanceExtensions.end());
        }

       /* Vulkan::Extensions missingExtensions;
        if (!Vulkan::CheckRequiredExtensions(missingExtensions, m_requiredInstanceExtensions, m_instanceExtensions))
        {
            std::string errorMessage = "Missing instance extensions: ";
            if (missingExtensions.size())
            {
                auto it = missingExtensions.begin(), last = std::prev(missingExtensions.end());
                for (; it != last; it++)
                {
                    errorMessage += (*it).name + "(" + std::to_string((*it).version) + "), ";
                }
                errorMessage += (*it).name + "(" + std::to_string((*it).version) + ").";
            }

            Logger::WriteError(m_logger, errorMessage);
            return false;
        }*/

        // Instance Layers
        m_requiredInstanceLayers = {}; 
        if (m_enableDebugMessenger)
        {
            m_requiredInstanceLayers.insert(
                m_requiredInstanceLayers.end(),
                m_debugInstanceLayers.begin(),
                m_debugInstanceLayers.end());
        }

        // Device extensions.
        m_requiredDeviceExtensions = { Vulkan::Extension{ VK_KHR_SWAPCHAIN_EXTENSION_NAME } };

        // Physical device features
        m_requiredDeviceFeatures = {};
        m_requiredDeviceFeatures.fillModeNonSolid = true;
        m_requiredDeviceFeatures.geometryShader = true;

        return true;
    }

    bool VulkanRenderer::LoadInstance(const Version& version)
    {
        VkResult result = VkResult::VK_SUCCESS;

        // Prepare instance and prepare debugger if avilable.
        if ((result = Vulkan::PrepareInstance(m_instance)) != VK_SUCCESS)
        {
            Vulkan::LogError(m_logger, "Failed prepare instance", result);
            return false;
        }     

        Vulkan::DebugMessenger* debugMessengerPtr = nullptr;    
        if (m_enableDebugMessenger)
        {
            const uint32_t debugSeverityFlags = m_logger ? m_logger->GetSeverityFlags() : 0;
            if (debugSeverityFlags != 0)
            {
                auto debugCallback = [&](Logger::Severity severity, const char* message)
                {
                    Logger::Write(m_logger, severity, message);
                };

                auto debuggerResult = Vulkan::PrepareDebugMessengerResult::Successful;

                if ((debuggerResult = m_debugMessenger.Prepare(debugSeverityFlags, debugCallback)) == Vulkan::PrepareDebugMessengerResult::Successful)
                {
                    debugMessengerPtr = &m_debugMessenger;
                }
                else
                {
                    Logger::WriteWarning(m_logger, "Failed to prepare vulkan debugger messenger.");
                }
            }
        }

        if (!debugMessengerPtr)
        {
            Vulkan::RemoveExtensions(m_requiredInstanceExtensions, m_debugInstanceExtensions);
            Vulkan::RemoveLayers(m_requiredInstanceLayers, m_debugInstanceLayers);      
        }
        
        const Version vulkanVersion = version != Version::None ? version : Version(1, 1);
        if ((result = Vulkan::CreateInstance(
            m_instance,
            vulkanVersion,
            "Molten Engine",
            MOLTEN_VERSION,
            "Molten Engine Application",
            Version(1, 0, 0),
            m_requiredInstanceExtensions,
            m_requiredInstanceLayers,
            debugMessengerPtr)) != VK_SUCCESS)
        {
            Vulkan::LogError(m_logger, "Failed create instance(vulkan version " + vulkanVersion.AsString() + ")", result);
            return false;
        }

        if(debugMessengerPtr)
        {
            if (m_debugMessenger.Create(m_instance.GetHandle()) != Vulkan::CreateDebugMessengerResult::Successful)
            {
                Logger::WriteWarning(m_logger, "Failed to create vulkan debugger messenger.");
            }
        }

        return true;
    }

    bool VulkanRenderer::LoadSurface()
    {
        VkResult result = VkResult::VK_SUCCESS;

        if ((result = Vulkan::CreatePlatformSurface(m_surface, m_instance, *m_renderTarget)) != VkResult::VK_SUCCESS)
        {
            Vulkan::LogError(m_logger, "Failed create surface", result);
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadPhysicalDevice()
    {
        VkResult result = VkResult::VK_SUCCESS;

        Vulkan::PhysicalDevices physicalDevices;
        if ((result = Vulkan::FetchPhysicalDevices(physicalDevices, m_instance)) != VkResult::VK_SUCCESS)
        {
            Vulkan::LogError(m_logger, "Failed create surface", result);
            return false;
        }

        Vulkan::PhysicalDevices physicalDevicesWithCapabilities;
        std::copy(physicalDevices.begin(), physicalDevices.end(), std::back_inserter(physicalDevicesWithCapabilities));

        Vulkan::PhysicalDevices physicalDevicesCandidates;
        Vulkan::FilterPhysicalDevicesWithRenderCapabilities(
            physicalDevicesCandidates,
            physicalDevicesWithCapabilities,
            m_requiredDeviceExtensions,
            m_requiredDeviceFeatures,
            m_surface,
            m_logger);

        if(!physicalDevicesCandidates.size())
        {
            Logger::WriteError(m_logger, "Found no suitable physical devices.");
            return false;
        }

        physicalDevicesCandidates.push_back(physicalDevicesCandidates[0]);

        bool foundDevice = Vulkan::ScorePhysicalDevices(m_physicalDevice, physicalDevicesCandidates,
            [&](const Vulkan::PhysicalDevice& physicalDevice) -> int32_t
        {
            int32_t score = 0;
            auto& properties = physicalDevice.GetCapabilities().properties;

            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                score += 2000000;
            }
            else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
                score += 1000000;
            }

            score += (properties.limits.maxFramebufferWidth / 1000) *
                (properties.limits.maxFramebufferHeight / 1000);

            return score;
        });

        if (!foundDevice)
        {
            Logger::WriteError(m_logger, "Found no physical devices with suitable score.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadLogicalDevice()
    {

        VkResult result = VkResult::VK_SUCCESS;

        if ((result = Vulkan::CreateLogicalDevice(
            m_logicalDevice,
            m_physicalDevice,
            m_requiredInstanceLayers,
            m_requiredDeviceExtensions,
            m_requiredDeviceFeatures)) != VkResult::VK_SUCCESS)
        {
            Vulkan::LogError(m_logger, "Failed to create logical device", result);
            return false;
        }

        return true;
    }


    bool VulkanRenderer::LoadSwapChain()
    {
        VkResult result = VkResult::VK_SUCCESS;

        VkSurfaceFormatKHR surfaceFormat = {};
        bool foundSurfaceFormat = false;
        for (auto& format : m_physicalDevice.GetCapabilities().surfaceCapabilities.formats)
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
        for (auto& mode : m_physicalDevice.GetCapabilities().surfaceCapabilities.presentModes)
        {
            if (mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
            {  
                presentMode = mode;
                break;
            }
        }

        // We need to get the physical device surface capabilities once again, because the surface may have changed due to resize.
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice.GetHandle(), m_surface, &m_physicalDevice.GetCapabilities().surfaceCapabilities.capabilities);
        VkSurfaceCapabilitiesKHR& capabilities = m_physicalDevice.GetCapabilities().surfaceCapabilities.capabilities;
        
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            auto windowSize = m_renderTarget->GetSize();
            capabilities.currentExtent = {
                std::max(capabilities.currentExtent.width, std::min(windowSize.x, capabilities.maxImageExtent.width)),
                std::max(capabilities.currentExtent.height, std::min(windowSize.y, capabilities.maxImageExtent.height))
            };
        }
        m_swapChainExtent = capabilities.currentExtent;

        uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        result = Vulkan::CreateSwapChain(
            m_swapChain,
            m_logicalDevice,
            m_physicalDevice,
            m_surface, 
            surfaceFormat,
            presentMode, 
            capabilities, 
            imageCount);

        if(result != VkResult::VK_SUCCESS || m_swapChain.handle == VK_NULL_HANDLE)
        {
            Logger::WriteError(m_logger, "Failed create swap chain.");
            return false;
        }

        result = Vulkan::GetSwapchainImages(m_swapChainImages, m_logicalDevice.handle, m_swapChain.handle);
        if(result != VkResult::VK_SUCCESS || m_swapChainImages.size() != imageCount)
        {
            vkDestroySwapchainKHR(m_logicalDevice.handle, m_swapChain.handle, nullptr);
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

            if (vkCreateImageView(m_logicalDevice.handle, &imageViewInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS)
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

        if (vkCreateRenderPass(m_logicalDevice.handle, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
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
        if (vkCreateFramebuffer(m_logicalDevice.handle, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create framebuffer.");
            return nullptr;
        }

        auto* vulkanFramebuffer = new VulkanFramebuffer(framebuffer);
        return vulkanFramebuffer;
    }

    bool VulkanRenderer::LoadCommandPool()
    {
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = m_physicalDevice.GetDeviceQueues().graphicsQueue;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(m_logicalDevice.handle, &commandPoolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
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

        if (vkAllocateCommandBuffers(m_logicalDevice.handle, &commandBufferInfo, m_commandBuffers.data()) != VK_SUCCESS)
        {
            vkDestroyCommandPool(m_logicalDevice.handle, m_commandPool, nullptr);
            Logger::WriteError(m_logger, "Failed to allocate command buffers.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadSyncObjects()
    {
        m_imagesInFlight.resize(m_swapChainImages.size(), VK_NULL_HANDLE);

        if (Vulkan::CreateSemaphores(m_imageAvailableSemaphores, m_logicalDevice.handle, m_maxFramesInFlight) != VkResult::VK_SUCCESS ||
            Vulkan::CreateSemaphores(m_renderFinishedSemaphores, m_logicalDevice.handle, m_maxFramesInFlight) != VkResult::VK_SUCCESS ||
            Vulkan::CreateFences(m_inFlightFences, m_logicalDevice.handle, VK_FENCE_CREATE_SIGNALED_BIT, m_maxFramesInFlight) != VkResult::VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create semaphores and fences.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::RecreateSwapChain()
    {
        vkDeviceWaitIdle(m_logicalDevice.handle);

        if (m_swapChain.handle)
        {
            Vulkan::DestroySemaphores(m_logicalDevice.handle, m_imageAvailableSemaphores);
            Vulkan::DestroySemaphores(m_logicalDevice.handle, m_renderFinishedSemaphores);
            Vulkan::DestroyFences(m_logicalDevice.handle, m_inFlightFences);

            m_imagesInFlight.clear();

            for (auto& framebuffer : m_presentFramebuffers)
            {
                DestroyFramebuffer(framebuffer);
            }
            m_presentFramebuffers.clear();

            Vulkan::DestroyImageViews(m_logicalDevice.handle, m_swapChainImageViews);
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
        Vulkan::DestroySemaphores(m_logicalDevice.handle, m_imageAvailableSemaphores);
        Vulkan::DestroySemaphores(m_logicalDevice.handle, m_renderFinishedSemaphores);
        Vulkan::DestroyFences(m_logicalDevice.handle, m_inFlightFences);

        m_imagesInFlight.clear();

        for (auto& framebuffer : m_presentFramebuffers)
        {
            DestroyFramebuffer(framebuffer);
        }
        m_presentFramebuffers.clear();

        Vulkan::DestroyImageViews(m_logicalDevice.handle, m_swapChainImageViews);

        if (m_swapChain.handle)
        {
            vkDestroySwapchainKHR(m_logicalDevice.handle, m_swapChain.handle, nullptr);
            m_swapChain = {};
        }      
    }

    bool VulkanRenderer::FindPhysicalDeviceMemoryType(uint32_t& index, const uint32_t filter, const VkMemoryPropertyFlags properties)
    {
        /// < MOVE THIS CODE TO ScorePhysicalDevice....
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice.GetHandle(), &memProperties);
        /// < MOVE THIS CODE TO ScorePhysicalDevice....

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((filter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
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

        if (vkCreateBuffer(m_logicalDevice.handle, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create vertex buffer.");
            return false;
        }

        VkMemoryRequirements memoryReq;
        vkGetBufferMemoryRequirements(m_logicalDevice.handle, buffer, &memoryReq);

        uint32_t memoryTypeIndex = 0;
        if (!FindPhysicalDeviceMemoryType(memoryTypeIndex, memoryReq.memoryTypeBits, properties))
        {
            vkDestroyBuffer(m_logicalDevice.handle, buffer, nullptr);
            Logger::WriteError(m_logger, "Failed to find matching memory type for vertex buffer.");
            return false;
        }

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryReq.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(m_logicalDevice.handle, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS)
        {
            vkDestroyBuffer(m_logicalDevice.handle, buffer, nullptr);
            Logger::WriteError(m_logger, "Failed to allocate vertex buffer memory.");
            return false;
        }

        if (vkBindBufferMemory(m_logicalDevice.handle, buffer, memory, 0) != VK_SUCCESS)
        {
            vkDestroyBuffer(m_logicalDevice.handle, buffer, nullptr);
            vkFreeMemory(m_logicalDevice.handle, memory, nullptr);
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
        vkAllocateCommandBuffers(m_logicalDevice.handle, &commandBufferInfo, &commandBuffer);

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
        vkQueueSubmit(m_logicalDevice.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_logicalDevice.graphicsQueue);

        vkFreeCommandBuffers(m_logicalDevice.handle, m_commandPool, 1, &commandBuffer);
    }

    bool VulkanRenderer::CreateImage(const Vector2ui32& dimensions, VkFormat format, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory)
    {
        /*VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = dimensions.x;
        imageInfo.extent.height = dimensions.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;

        if (vkCreateImage(m_logicalDevice.device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            return nullptr;
        }

        VkMemoryRequirements memoryReq;
        vkGetBufferMemoryRequirements(m_logicalDevice.handle, image, &memoryReq);

        uint32_t memoryTypeIndex = 0;
        if (!FindPhysicalDeviceMemoryType(memoryTypeIndex, memoryReq.memoryTypeBits, properties))
        {
            return false;
        }

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryReq.size;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(m_logicalDevice.handle, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to allocate image memory.");
            return false;
        }

        if (vkBindImageMemory(m_logicalDevice.handle, image, memory, 0) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to bind memory to image.");
            return false;
        }*/

        return true;
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
        Vulkan::DescriptorSetLayouts& setLayouts)
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
            if (vkCreateDescriptorSetLayout(m_logicalDevice.handle, &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
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
        Vulkan::ShaderModules& shaderModules,
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
        if (vkCreateShaderModule(m_logicalDevice.handle, &shaderModuleInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create shader module.");
            return VK_NULL_HANDLE;
        }

        return shaderModule;
    }

}

#endif