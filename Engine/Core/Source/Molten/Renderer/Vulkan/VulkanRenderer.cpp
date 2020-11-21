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
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Renderer/Vulkan/VulkanFramebuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanIndexBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Molten/Renderer/Vulkan/VulkanTexture.hpp"
#include "Molten/Renderer/Vulkan/VulkanUniformBlock.hpp"
#include "Molten/Renderer/Vulkan/VulkanUniformBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanVertexBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResultLogger.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanImage.hpp"
#include "Molten/Window/Window.hpp"
#include "Molten/Logger.hpp"
#include "Molten/System/Exception.hpp"
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

    MOLTEN_UNSCOPED_ENUM_BEGIN
    static VkPrimitiveTopology GetPrimitiveTopology(const Pipeline::Topology topology)
    {     
        switch (topology)
        {
            case Pipeline::Topology::PointList:     return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case Pipeline::Topology::LineList:      return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case Pipeline::Topology::LineStrip:     return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case Pipeline::Topology::TriangleList:  return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case Pipeline::Topology::TriangleStrip: return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        }
        throw Exception("Provided primitive topology is not supported by the Vulkan renderer.");      
    }

    static VkPolygonMode GetPolygonMode(const Pipeline::PolygonMode polygonMode)
    {
        switch (polygonMode)
        {
            case Pipeline::PolygonMode::Point: return VkPolygonMode::VK_POLYGON_MODE_POINT;
            case Pipeline::PolygonMode::Line:  return VkPolygonMode::VK_POLYGON_MODE_LINE;
            case Pipeline::PolygonMode::Fill:  return VkPolygonMode::VK_POLYGON_MODE_FILL;
        }
        throw Exception("Provided polygon mode is not supported by the Vulkan renderer.");

    }
    
    static VkFrontFace GetFrontFace(const Pipeline::FrontFace frontFace)
    {
        switch (frontFace)
        {
            case Pipeline::FrontFace::Clockwise:        return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
            case Pipeline::FrontFace::Counterclockwise: return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
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
        }
        throw Exception("Provided cull mode is not supported by the Vulkan renderer.");
    }
    
    static bool GetVertexAttributeFormatAndSize(const Shader::VariableDataType format, VkFormat & vulkanFormat, uint32_t & formatSize)
    {       
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
    }

    static VkIndexType GetIndexBufferDataType(const IndexBuffer::DataType dataType)
    {
        switch (dataType)
        {
            case IndexBuffer::DataType::Uint16: return VkIndexType::VK_INDEX_TYPE_UINT16;
            case IndexBuffer::DataType::Uint32: return VkIndexType::VK_INDEX_TYPE_UINT32;
        }
        
        throw Exception("Provided data type is not supported as index buffer data type by the Vulkan renderer.");
    }
  
    static VkDescriptorType GetDescriptorType(const Shader::BindingType bindingType)
    {
        switch(bindingType)
        {
            case Shader::BindingType::Sampler1D:
            case Shader::BindingType::Sampler2D:
            case Shader::BindingType::Sampler3D: return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case Shader::BindingType::UniformBuffer: return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        }
        throw Exception("Provided provided binding type is not handled.");
    }

    MOLTEN_UNSCOPED_ENUM_END

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
        m_optionalDeviceFeatures{},
        m_isOpen(false),
        m_enableDebugMessenger(false),
        m_debugInstanceLayers{},
        m_instance{},
        m_surface{},
        m_physicalDevice{},
        m_logicalDevice{},
        m_renderPass(VK_NULL_HANDLE),
        m_swapChain{},
        m_surfaceFormat{},
        m_presentMode(VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR),
        m_commandPool(VK_NULL_HANDLE),
        m_commandBuffers{},
        m_currentCommandBuffer(nullptr),
        m_beginDraw(false),
        m_currentPipeline(nullptr)
    {}

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
            LoadMemoryTypes() &&
            LoadRenderPass() &&
            LoadSwapChain() &&
            LoadCommandPool();

        m_isOpen = loaded;
        return loaded;   
    }

    void VulkanRenderer::Close()
    {
        if (m_logicalDevice.IsCreated())
        {
            m_logicalDevice.WaitIdle();

            if (m_commandPool)
            {
                vkDestroyCommandPool(m_logicalDevice.GetHandle(), m_commandPool, nullptr);
                m_commandPool = VK_NULL_HANDLE;
                m_commandBuffers = {};
                m_currentCommandBuffer = nullptr;
            }

            m_swapChain.Destroy();  

            if (m_renderPass)
            {
                vkDestroyRenderPass(m_logicalDevice.GetHandle(), m_renderPass, nullptr);
                m_renderPass = VK_NULL_HANDLE;
            }
        }

        m_logicalDevice.Destroy();
        m_surface.Destroy();  
        m_instance.Destroy();

        m_renderTarget = nullptr;
        m_version = { 0, 0, 0 };
        m_logger = nullptr;
        m_requiredInstanceExtensions = {};
        m_requiredInstanceLayers = {};
        m_requiredDeviceExtensions = {};
        m_isOpen = false;
        m_debugInstanceLayers = {};
        m_physicalDevice = {};
        m_logicalDevice = {};
        m_swapChain = {};
        m_commandBuffers.clear();
        m_currentCommandBuffer = nullptr;
        m_beginDraw = false;      
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

        auto swapChainExtent = m_swapChain.GetExtent();
        if (size == Vector2ui32{ swapChainExtent.width, swapChainExtent.height })
        {
            return;
        }

        swapChainExtent.width = size.x;
        swapChainExtent.height = size.y;
        m_swapChain.SetExtent(swapChainExtent);
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
            return std::numeric_limits<uint32_t>::max();
        }

        return it->second.location;
    }

    DescriptorSet* VulkanRenderer::CreateDescriptorSet(const DescriptorSetDescriptor& descriptor)
    {
        auto* vulkanPipeline = static_cast<VulkanPipeline*>(descriptor.pipeline);

        const auto& mappedSets = vulkanPipeline->mappedDescriptorSets;
        auto mappedSetIt = mappedSets.find(descriptor.id);
        if(mappedSetIt == mappedSets.end())
        {
            Logger::WriteError(m_logger, "Failed to find mapped descriptor set of id " + std::to_string(descriptor.id) + ".");
            return nullptr;
        }

        const auto& mappedSet = mappedSetIt->second;
        const uint32_t setIndex = mappedSet.index;

        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<VkDescriptorPoolSize> poolSizes = {};

        auto AddBindingToPool = [&](VkDescriptorType descriptorType)
        {
            auto it = std::find_if(poolSizes.begin(), poolSizes.end(), [&](VkDescriptorPoolSize& poolSize)
            {
                return poolSize.type == descriptorType;
            });

            if(it != poolSizes.end())
            {
                auto& poolSize = *it;
                poolSize.descriptorCount++;
                return;
            }

            VkDescriptorPoolSize poolSize = {};
            poolSize.type = descriptorType;
            poolSize.descriptorCount = 1;
            poolSizes.push_back(poolSize);
        };
        
        auto CreateBufferInfo = [&](VkBuffer buffer) -> VkDescriptorBufferInfo&
        {
            AddBindingToPool(VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;
            bufferInfos.push_back(bufferInfo);
            return bufferInfos.back();
        };
        auto CreateImageInfo = [&](VkSampler sampler, VkImageView imageView) -> VkDescriptorImageInfo&
        {
            AddBindingToPool(VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

            VkDescriptorImageInfo imageInfo = {};
            imageInfo.sampler = sampler;
            imageInfo.imageView = imageView;
            imageInfo.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos.push_back(imageInfo);
            return imageInfos.back();
        };

        const auto bindings = descriptor.bindings;
        const auto& mappedBindings = mappedSet.bindings;
        std::vector<VkWriteDescriptorSet> writes(bindings.size());
        size_t writeIndex = 0;
        for (const auto& binding : bindings)
        {
            auto mappedBindingIt = mappedBindings.find(binding.id);
            if (mappedBindingIt == mappedBindings.end())
            {
                Logger::WriteError(m_logger, "Failed to find mapped descriptor binding of id " + std::to_string(binding.id) + ".");
                return nullptr;
            }
            const auto& mappedBinding = mappedBindingIt->second;

            auto& write = writes[writeIndex];
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = VK_NULL_HANDLE;
            write.dstBinding = mappedBinding.index;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;

            std::visit([&](auto& bindingData) {
                using T = std::decay_t<decltype(bindingData)>;
                if constexpr (std::is_same_v<T, UniformBufferDescriptorBinding>)
                {
                    auto* vulkanBuffer = static_cast<VulkanUniformBuffer*>(bindingData.uniformBuffer);
                    const auto bufferHandle = vulkanBuffer->frames[0].buffer.GetHandle();
                    auto& bufferInfo = CreateBufferInfo(bufferHandle);

                    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; 
                    write.pBufferInfo = &bufferInfo;
                }
                else if constexpr (std::is_same_v<T, SampledTextureDescriptorBinding>)
                {
                    auto* vulkanTexture = static_cast<VulkanTexture*>(bindingData.texture);
                    const auto samplerHandle = vulkanTexture->imageSampler.GetHandle();
                    auto& imageInfo = CreateImageInfo(samplerHandle, VK_NULL_HANDLE);

                    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    write.descriptorCount = 1;
                    write.pImageInfo = &imageInfo;
                }   
                else
                {
                    static_assert(false, "Unknown binding type.");
                }
                    
            }, binding.binding);

            ++writeIndex;
        }

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 1;
        poolInfo.flags = 0;

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        if (vkCreateDescriptorPool(m_logicalDevice.GetHandle(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor pool.");
            return nullptr;
        }

        auto& setLayouts = vulkanPipeline->descriptionSetLayouts;
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &setLayouts[setIndex];

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        if (vkAllocateDescriptorSets(m_logicalDevice.GetHandle(), &allocInfo, &descriptorSet) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor sets.");
            return nullptr;
        }

        for(auto& write : writes)
        {
            write.dstSet = descriptorSet;
            vkUpdateDescriptorSets(m_logicalDevice.GetHandle(), 1, &write, 0, nullptr);
        }

        return new VulkanDescriptorSet(
            setIndex,
            descriptorSet,
            descriptorPool);
    }

    Framebuffer* VulkanRenderer::CreateFramebuffer(const FramebufferDescriptor& descriptor)
    {
        return nullptr;
    }

    IndexBuffer* VulkanRenderer::CreateIndexBuffer(const IndexBufferDescriptor& descriptor)
    {
        const auto bufferSize = static_cast<VkDeviceSize>(descriptor.indexCount) * GetIndexBufferDataTypeSize(descriptor.dataType);

        Vulkan::DeviceBuffer stagingBuffer;

        Vulkan::Result<> result;
        if (!(result = stagingBuffer.Create(m_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_memoryTypesHostVisibleOrCoherent)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for index buffer");
            return nullptr;
        }

        if (!(result = stagingBuffer.MapMemory(0, bufferSize, descriptor.data)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for index buffer");
            return nullptr;
        }

        Vulkan::DeviceBuffer indexBuffer;
        if (!(result = indexBuffer.Create(m_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_memoryTypesDeviceLocal)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create index buffer");
            return nullptr;
        }

        if (!(result = indexBuffer.CopyFromBuffer(m_commandPool, stagingBuffer, bufferSize)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to copy from staging buffer to index buffer");
            return nullptr;
        }

        auto* buffer = new VulkanIndexBuffer(std::move(indexBuffer), descriptor.indexCount, descriptor.dataType);
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

        if (!vertexOutputs.CompareStructure(fragmentInputs))
        {
            Logger::WriteError(m_logger, "Vertex output structure is not compatible with fragment input structure.");
            return nullptr;
        }
        
        const std::vector<Shader::Visual::Script*> shaderScripts =
        {
            descriptor.vertexScript, descriptor.fragmentScript
        };
       
        Vulkan::ShaderModules shaderModules;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
        PushConstantLocations pushConstantLocations;
        VkPushConstantRange pushConstantRange = {};
        MappedDescriptorSets mappedDescriptorSets;
        if (!LoadShaderModules(
            shaderModules,
            shaderStageCreateInfos,
            pushConstantLocations,
            pushConstantRange,
            mappedDescriptorSets,
            shaderScripts))
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

        Vulkan::DescriptorSetLayouts setLayouts;
        if (!CreateDescriptorSetLayouts(setLayouts, mappedDescriptorSets))
        {
            return nullptr;
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
        rasterizerInfo.polygonMode = GetPolygonMode(descriptor.polygonMode);
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
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = (pushConstantRange.size > 0) ? 1 : 0;
        pipelineLayoutInfo.pPushConstantRanges = (pushConstantRange.size > 0) ? &pushConstantRange : nullptr;

        VkPipelineLayout pipelineLayout;
        if (vkCreatePipelineLayout(m_logicalDevice.GetHandle(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
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
        if (vkCreateGraphicsPipelines(m_logicalDevice.GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            vkDestroyPipelineLayout(m_logicalDevice.GetHandle(), pipelineLayout, nullptr);
            Logger::WriteError(m_logger, "Failed to create pipeline.");
            return nullptr;
        }

        return new VulkanPipeline(
            graphicsPipeline, 
            pipelineLayout, 
            std::move(setLayouts),
            std::move(pushConstantLocations), 
            std::move(shaderModules),
            std::move(mappedDescriptorSets));
    }

    Texture* VulkanRenderer::CreateTexture(const TextureDescriptor& descriptor)
    {
        const auto bufferSize = 
            static_cast<VkDeviceSize>(descriptor.dimensions.x) * 
            static_cast<VkDeviceSize>(descriptor.dimensions.y) * 4;

        Vulkan::DeviceBuffer stagingBuffer;

        Vulkan::Result<> result;
        if (!(result = stagingBuffer.Create(m_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_memoryTypesHostVisibleOrCoherent)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for index buffer");
            return nullptr;
        }

        if (!(result = stagingBuffer.MapMemory(0, bufferSize, descriptor.data)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for index buffer");
            return nullptr;
        }

        Vulkan::Image image;
        if (!(result = image.Create(
            m_logicalDevice,
            stagingBuffer,
            m_commandPool,
            descriptor.dimensions,
            VkFormat::VK_FORMAT_R8G8B8A8_SRGB,
            m_memoryTypesDeviceLocal)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create image");
            return nullptr;
        }

        if (!(result = image.TransitionToLayout(
            m_commandPool,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed transition image to read only optional layout");
            return nullptr;
        }

        Vulkan::ImageSampler sampler;
        if (!(result = sampler.Create(m_logicalDevice)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create image sampler");
            return nullptr;
        }

        auto* texture = new VulkanTexture(std::move(image), std::move(sampler));
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
                vkDestroyDescriptorPool(m_logicalDevice.GetHandle(), descriptorPool, nullptr);
            }
        };

        const uint32_t swapChainImageCount = m_swapChain.GetImageCount();

        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSize.descriptorCount = swapChainImageCount;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = swapChainImageCount;
        poolInfo.flags = 0;

        if (vkCreateDescriptorPool(m_logicalDevice.GetHandle(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor pool.");
            return nullptr;
        }
      
        std::vector<VkDescriptorSetLayout> layouts(swapChainImageCount, vulkanPipeline->descriptionSetLayouts[descriptor.id]);
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = swapChainImageCount;
        allocInfo.pSetLayouts = layouts.data(); 

        Vulkan::DescriptorSets descriptorSets(swapChainImageCount, VK_NULL_HANDLE);
        if (vkAllocateDescriptorSets(m_logicalDevice.GetHandle(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor sets.");
            return nullptr;
        }

        for (size_t i = 0; i < descriptorSets.size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = vulkanUniformBuffer->frames[i].buffer.GetHandle();
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet descWrite = {};
            descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descWrite.dstSet = descriptorSets[i];
            descWrite.dstBinding = 0;
            descWrite.dstArrayElement = 0;
            descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descWrite.descriptorCount = 1;
            descWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(m_logicalDevice.GetHandle(), 1, &descWrite, 0, nullptr);
        }

        descriptorPoolDestroyer.Release();

        auto* uniformBlock = new VulkanUniformBlock(
            vulkanPipeline->pipelineLayout, descriptorPool, std::move(descriptorSets), descriptor.id);
        return uniformBlock;
    }

    UniformBuffer* VulkanRenderer::CreateUniformBuffer(const UniformBufferDescriptor& descriptor)
    {
        std::vector<VulkanUniformBuffer::Frame> frames;
        frames.resize(m_swapChain.GetImageCount());

        for (auto& frame : frames)
        {
            Vulkan::Result<> result;
            if (!(result = frame.buffer.Create(m_logicalDevice, descriptor.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_memoryTypesHostVisibleOrCoherent)))
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create frame buffer for uniform buffer");
                return nullptr;
            }          
        }

        auto vulkanUniformBuffer = new VulkanUniformBuffer;
        vulkanUniformBuffer->frames = std::move(frames);
        return vulkanUniformBuffer;
    }

    VertexBuffer* VulkanRenderer::CreateVertexBuffer(const VertexBufferDescriptor& descriptor)
    {
        const size_t bufferSize = 
            static_cast<VkDeviceSize>(static_cast<VkDeviceSize>(descriptor.vertexCount) *
            static_cast<VkDeviceSize>(descriptor.vertexSize));

        Vulkan::DeviceBuffer stagingBuffer;

        Vulkan::Result<> result;
        if (!(result = stagingBuffer.Create(m_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_memoryTypesHostVisibleOrCoherent)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for vertex buffer");
            return nullptr;
        }

        if (!(result = stagingBuffer.MapMemory(0, bufferSize, descriptor.data)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for vertex buffer");
            return nullptr;
        }

        Vulkan::DeviceBuffer vertexBuffer;
        if (!(result = vertexBuffer.Create(m_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_memoryTypesDeviceLocal)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create vertex buffer");
            return nullptr;
        }

        if (!(result = vertexBuffer.CopyFromBuffer(m_commandPool, stagingBuffer, bufferSize)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to copy from staging buffer to vertex buffer");
            return nullptr;
        }

        auto* buffer = new VulkanVertexBuffer(std::move(vertexBuffer), descriptor.vertexCount, descriptor.vertexSize);
        return buffer;
    }

    void VulkanRenderer::DestroyFramebuffer(Framebuffer* framebuffer)
    {
        VulkanFramebuffer* vulkanFramebuffer = static_cast<VulkanFramebuffer*>(framebuffer);
        vkDestroyFramebuffer(m_logicalDevice.GetHandle(), vulkanFramebuffer->framebuffer, nullptr);
        delete vulkanFramebuffer;
    }

    void VulkanRenderer::DestroyIndexBuffer(IndexBuffer* indexBuffer)
    {
        VulkanIndexBuffer* vulkanIndexBuffer = static_cast<VulkanIndexBuffer*>(indexBuffer);
        delete vulkanIndexBuffer;
    }

    void VulkanRenderer::DestroyPipeline(Pipeline* pipeline)
    {
        VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);

        if (vulkanPipeline->graphicsPipeline)
        {
            vkDeviceWaitIdle(m_logicalDevice.GetHandle());
            vkDestroyPipeline(m_logicalDevice.GetHandle(), vulkanPipeline->graphicsPipeline, nullptr);
        }
        if (vulkanPipeline->pipelineLayout)
        {
            vkDestroyPipelineLayout(m_logicalDevice.GetHandle(), vulkanPipeline->pipelineLayout, nullptr);
        }
        for (auto& setLayout : vulkanPipeline->descriptionSetLayouts)
        {
            vkDestroyDescriptorSetLayout(m_logicalDevice.GetHandle(), setLayout, nullptr);
        }
        for (auto& shaderModule : vulkanPipeline->shaderModules)
        {
            shaderModule.Destroy();
        }

        delete vulkanPipeline;
    }

    void VulkanRenderer::DestroyTexture(Texture* texture)
    {
        VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(texture);
        delete vulkanTexture;
    }

    void VulkanRenderer::DestroyUniformBlock(UniformBlock* uniformBlock)
    {
        VulkanUniformBlock* vulkanUniformBlock = static_cast<VulkanUniformBlock*>(uniformBlock);
        if(vulkanUniformBlock->descriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(m_logicalDevice.GetHandle(), vulkanUniformBlock->descriptorPool, nullptr);
        }
        delete vulkanUniformBlock;
    }

    void VulkanRenderer::DestroyUniformBuffer(UniformBuffer* uniformBuffer)
    {
        VulkanUniformBuffer* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer);
        delete vulkanUniformBuffer;
    }

    void VulkanRenderer::DestroyVertexBuffer(VertexBuffer* vertexBuffer)
    {
        VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);
        delete vulkanVertexBuffer;
    }

    void VulkanRenderer::BindDescriptorSet(DescriptorSet* descriptorSet)
    {
        VulkanDescriptorSet* vulkanDescriptorSet = static_cast<VulkanDescriptorSet*>(descriptorSet);

        uint32_t offset = 0;

        vkCmdBindDescriptorSets(
            *m_currentCommandBuffer, 
            VK_PIPELINE_BIND_POINT_GRAPHICS, 
            m_currentPipeline->pipelineLayout,
            vulkanDescriptorSet->index,
            1,
            &vulkanDescriptorSet->descriptorSet,
            1,
            &offset);
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
            &vulkanUniformBlock->descriptorSets[m_swapChain.GetCurrentImageIndex()], 1, &offset);
    }

    void VulkanRenderer::BeginDraw()
    {
        if (m_beginDraw)
        {
            Logger::WriteError(m_logger, "Calling BeginDraw twice, without any previous call to EndDraw.");
            return;
        }

        Vulkan::Result<> result;

        if (!(result = m_swapChain.BeginDraw()))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to begin drawing of swap chain.");
            return;
        }

        auto currentImageIndex = m_swapChain.GetCurrentImageIndex();

        m_currentCommandBuffer = &m_commandBuffers[currentImageIndex];

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (!(result = vkBeginCommandBuffer(*m_currentCommandBuffer, &commandBufferBeginInfo)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to begin recording command buffer.");
            return;
        }

        auto currentFramebuffer = m_swapChain.GetCurrentFramebuffer();
        auto swapChainExtent = m_swapChain.GetExtent();

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = currentFramebuffer;
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = swapChainExtent;

        const VkClearValue clearColor = { 0.3f, 0.0f, 0.0f, 0.0f };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChainExtent.width);
        viewport.height = static_cast<float>(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 0.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;

        vkCmdBeginRenderPass(*m_currentCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdSetViewport(*m_currentCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(*m_currentCommandBuffer, 0, 1, &scissor);

        m_beginDraw = true;
    }

    void VulkanRenderer::DrawVertexBuffer(VertexBuffer* vertexBuffer)
    {
        VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer->buffer.GetHandle() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(*m_currentCommandBuffer, static_cast<uint32_t>(vulkanVertexBuffer->vertexCount), 1, 0, 0);
    }

    void VulkanRenderer::DrawVertexBuffer(IndexBuffer* indexBuffer, VertexBuffer* vertexBuffer)
    {
        VulkanIndexBuffer* vulkanIndexBuffer = static_cast<VulkanIndexBuffer*>(indexBuffer);
        VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer->buffer.GetHandle() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(*m_currentCommandBuffer, vulkanIndexBuffer->buffer.GetHandle(), 0, GetIndexBufferDataType(vulkanIndexBuffer->dataType));
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

        Vulkan::Result<> result;

        vkCmdEndRenderPass(*m_currentCommandBuffer);
        if (!(result = vkEndCommandBuffer(*m_currentCommandBuffer)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to end command buffer");
            return;
        }

        if (!(result = m_swapChain.EndDraw(*m_currentCommandBuffer)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to end swap chain");
            return;
        }

        m_beginDraw = false;
    }

    void VulkanRenderer::WaitForDevice()
    {
        m_logicalDevice.WaitIdle();
    }

    void VulkanRenderer::UpdateUniformBuffer(UniformBuffer* uniformBuffer, const size_t offset, const size_t size, const void* data)
    {
        VulkanUniformBuffer* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer);

        auto& frame = vulkanUniformBuffer->frames[m_swapChain.GetCurrentImageIndex()];

        Vulkan::Result<> result;
        if (!(result = frame.buffer.MapMemory(offset, size, data, 0)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to update uniform buffer");
        }
    }

    bool VulkanRenderer::LoadRequirements()
    {
        // Instance extensions.
        m_requiredInstanceExtensions = { 
            Vulkan::Extension{ "VK_KHR_surface", 0 },
            Vulkan::Surface::GetPlatformExtension()
        };

        // Instance Layers
        m_requiredInstanceLayers = {};

        // Device extensions.
        m_requiredDeviceExtensions = { Vulkan::Extension{ VK_KHR_SWAPCHAIN_EXTENSION_NAME } };

        // Physical device features
        m_requiredDeviceFeatures = {
            &VkPhysicalDeviceFeatures::fillModeNonSolid,
            &VkPhysicalDeviceFeatures::geometryShader
        };

        m_optionalDeviceFeatures = {
            &VkPhysicalDeviceFeatures::samplerAnisotropy
        };

        // Debug extensions and layers
        if (m_enableDebugMessenger)
        {
            m_debugInstanceLayers = { Vulkan::Layer{ "VK_LAYER_KHRONOS_validation" } };

            m_requiredInstanceLayers.insert(
                m_requiredInstanceLayers.end(),
                m_debugInstanceLayers.begin(),
                m_debugInstanceLayers.end());
        }

        return true;
    }

    bool VulkanRenderer::LoadInstance(const Version& version)
    {
        Vulkan::DebugCallbackDescriptor debugCallbackDesc =
        {
            m_logger ? m_logger->GetSeverityFlags() : 0,
            [&](Logger::Severity severity, const char* message)
            {
                Logger::Write(m_logger, severity, message);
            }
        };
     
        const Version vulkanVersion = version != Version::None ? version : Version(1, 1);

        Vulkan::Result<> result;
        if (!(result = m_instance.Create(
            vulkanVersion,
            "Molten Engine",
            MOLTEN_VERSION,
            "Molten Engine Application",
            Version(1, 0, 0),
            m_requiredInstanceExtensions,
            m_requiredInstanceLayers,
            debugCallbackDesc)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create instance(vulkan version " + vulkanVersion.AsString() + ")");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadSurface()
    {
        Vulkan::Result<> result;
        if (!(result = m_surface.Create(m_instance, *m_renderTarget)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create surace");
            return false;
        }

        m_surfaceFormat = {};
        m_surfaceFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
        m_surfaceFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        return true;
    }

    bool VulkanRenderer::LoadPhysicalDevice()
    {
        Vulkan::Result<> result;

        Vulkan::PhysicalDevices physicalDevices;
        result = Vulkan::FetchAndCreatePhysicalDevices(physicalDevices, m_instance, m_surface, {
            [&](const Vulkan::PhysicalDevice& physicalDevice)
            {
                auto& capabilities = physicalDevice.GetCapabilities();
                if (!capabilities.surfaceCapabilities.formats.size() ||
                    !capabilities.surfaceCapabilities.presentModes.size())
                {
                    return false;
                }

                auto& queueIndices = physicalDevice.GetDeviceQueueIndices();
                if (!queueIndices.graphicsQueue.has_value() ||
                    !queueIndices.presentQueue.has_value())
                {
                    return false;
                }

                auto& surfaceCapabilities = capabilities.surfaceCapabilities;
                auto& surfaceFormats = surfaceCapabilities.formats;
                if (std::find_if(surfaceFormats.begin(), surfaceFormats.end(), 
                    [&](auto format)
                    {
                        return format.format == m_surfaceFormat.format && format.colorSpace == m_surfaceFormat.colorSpace;
                    }) == surfaceFormats.end())
                {
                    return false;
                }

                Vulkan::PhysicalDeviceFeaturesWithName missingFeatures;
                if (!Vulkan::CheckRequiredPhysicalDeviceFeatures(missingFeatures, capabilities.features, m_requiredDeviceFeatures))
                {
                    return false;
                }

                return true;
            }
        });

        if (!result)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to fetch or create physical device");
            return false;
        }

        bool foundDevice = Vulkan::ScorePhysicalDevices(m_physicalDevice, physicalDevices,
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

                score += 
                    (properties.limits.maxFramebufferWidth / 1000) *
                    (properties.limits.maxFramebufferHeight / 1000);

                return score;
            }
        );

        if (!foundDevice)
        {
            Logger::WriteError(m_logger, 
                "Found no physical device, out of " + 
                std::to_string(physicalDevices.size()) + 
                " possible, with suitable render capabilities.");
            return false;
        }

        // Get present mode.
        m_presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
        for (auto& mode : m_physicalDevice.GetCapabilities().surfaceCapabilities.presentModes)
        {
            if (mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
            {
                m_presentMode = mode;
                break;
            }
        }

        return true;
    }

    bool VulkanRenderer::LoadLogicalDevice()
    {
        Vulkan::Result<> result;

        VkPhysicalDeviceFeatures enabledDeviceFeatures = {};
        Vulkan::EnablePhysicalDeviceFeatures(enabledDeviceFeatures, m_requiredDeviceFeatures);

        auto& availableDeviceFeatures = m_physicalDevice.GetCapabilities().features;
        Vulkan::EnableOptionalPhysicalDeviceFeatures(enabledDeviceFeatures, availableDeviceFeatures, m_optionalDeviceFeatures);

        if (!(result = m_logicalDevice.Create(
            m_physicalDevice,
            m_requiredInstanceLayers,
            m_requiredDeviceExtensions,
            enabledDeviceFeatures)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create logical device");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadMemoryTypes()
    {
        m_memoryTypesHostVisibleOrCoherent.clear();
        m_memoryTypesDeviceLocal.clear();

        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperies;
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice.GetHandle(), &physicalDeviceMemoryProperies);

        Vulkan::FilterMemoryTypesByPropertyFlags(
            m_memoryTypesHostVisibleOrCoherent,
            physicalDeviceMemoryProperies,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (!m_memoryTypesHostVisibleOrCoherent.size())
        {
            Logger::WriteError(m_logger, "Failed to find filtered memory types of host visible or coherent.");
            return false;
        }

        Vulkan::FilterMemoryTypesByPropertyFlags(
            m_memoryTypesDeviceLocal,
            physicalDeviceMemoryProperies,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (!m_memoryTypesDeviceLocal.size())
        {
            Logger::WriteError(m_logger, "Failed to find filtered memory types of device local.");
            return false;
        }
        
        return true;
    }

    bool VulkanRenderer::LoadRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_surfaceFormat.format;
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

        if (vkCreateRenderPass(m_logicalDevice.GetHandle(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create render pass.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadSwapChain()
    {
        Vulkan::Result<> result;

        auto& surfaceCapabilities = m_physicalDevice.GetCapabilities().surfaceCapabilities;

        // Get image count.
        uint32_t imageCount = surfaceCapabilities.capabilities.minImageCount + 1;
        if (surfaceCapabilities.capabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.capabilities.maxImageCount)
        {
            imageCount = surfaceCapabilities.capabilities.maxImageCount;
        }

        // Create swap chain.
        if (!(result = m_swapChain.Create(
            m_logicalDevice,
            m_renderPass,
            m_surfaceFormat,
            m_presentMode,
            imageCount)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create swap chain");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadCommandPool()
    {
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = m_physicalDevice.GetDeviceQueueIndices().graphicsQueue.value();
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(m_logicalDevice.GetHandle(), &commandPoolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create command pool.");
            return false;
        }

        m_commandBuffers.resize(m_swapChain.GetImageCount());

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = m_commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        if (vkAllocateCommandBuffers(m_logicalDevice.GetHandle(), &commandBufferInfo, m_commandBuffers.data()) != VK_SUCCESS)
        {
            vkDestroyCommandPool(m_logicalDevice.GetHandle(), m_commandPool, nullptr);
            Logger::WriteError(m_logger, "Failed to allocate command buffers.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::CreateVertexInputAttributes(
        const Shader::Visual::InputInterface& inputs,
        std::vector<VkVertexInputAttributeDescription>& attributes, 
        uint32_t& stride)
    {
        attributes.resize(inputs.GetMemberCount());
        size_t index = 0;
        uint32_t location = 0;

        const auto outputPins = inputs.GetOutputPins();
        for (auto* pins : outputPins)
        {
            VkFormat format;
            uint32_t formatSize;
            if (!GetVertexAttributeFormatAndSize(pins->GetDataType(), format, formatSize))
            {
                Logger::WriteError(m_logger, "Failed to find vertex input attribute format.");
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
        return true;
    }

    bool VulkanRenderer::CreateDescriptorSetLayouts(
        Vulkan::DescriptorSetLayouts& setLayouts,
        const MappedDescriptorSets& mappedDescriptorSets)
    {
        for(const auto& mappedDescriptorSetPair : mappedDescriptorSets)
        {
            const auto& mappedDescriptorSet = mappedDescriptorSetPair.second;
            const auto mappedDescriptorBindings = mappedDescriptorSet.bindings;

            std::vector<VkDescriptorSetLayoutBinding> bindings(mappedDescriptorBindings.size());
            uint32_t bindingIndex = 0;
            for(const auto& mappedDescriptorBindingPair : mappedDescriptorBindings)
            {
                const auto& mappedDescriptorBinding = mappedDescriptorBindingPair.second;

                auto& binding = bindings[bindingIndex++];
                binding.binding = mappedDescriptorBinding.index;
                binding.descriptorType = GetDescriptorType(mappedDescriptorBinding.bindingType);
                binding.descriptorCount = 1;
                binding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
                binding.pImmutableSamplers = nullptr;
            }

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
            descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            descriptorSetLayoutInfo.pBindings = bindings.data();

            VkDescriptorSetLayout descriptorSetLayout;
            if (vkCreateDescriptorSetLayout(m_logicalDevice.GetHandle(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            {
                Logger::WriteError(m_logger, "Failed to create descriptor set layout.");
                return false;
            }

            setLayouts.push_back(descriptorSetLayout);
        }

       return true;
    }

    bool VulkanRenderer::LoadShaderModules(
        Vulkan::ShaderModules& shaderModules,
        std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
        PushConstantLocations& pushConstantLocations,
        VkPushConstantRange& pushConstantRange,
        MappedDescriptorSets& mappedDescriptorSets,
        const std::vector<Shader::Visual::Script*>& visualScripts
    )
    {
        shaderModules.resize(visualScripts.size());

        if(visualScripts.empty())
        {
            Logger::WriteError(m_logger, "Cannot load shader module with 0 scripts.");
            return false;
        }

        Shader::GlslGenerator::GlslTemplate glslTemplate;
        if(!Shader::GlslGenerator::GenerateGlslTemplate(glslTemplate, visualScripts, m_logger))
        {
            return false;
        }

        for(size_t i = 0; i < visualScripts.size(); i++)
        {
            const auto& script = visualScripts[i];

            auto glslCode = m_glslGenerator.Generate(*script, Shader::GlslGenerator::Compability::SpirV, &glslTemplate, m_logger);
            if (glslCode.empty())
            {
                Logger::WriteError(m_logger, "Failed to generate GLSL code.");
                return false;
            }

            const auto scriptType = script->GetType();
            auto sprivCode = Shader::GlslGenerator::ConvertGlslToSpriV(glslCode, scriptType, m_logger);
            if (sprivCode.empty())
            {
                Logger::WriteError(m_logger, "Failed to convert GLSL to SPIR-V.");
                return false;
            }

            Vulkan::Result<> result;
            auto& shaderModule = shaderModules[i];
            if (!(result = shaderModule.Create(m_logicalDevice, sprivCode)))
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create shader module");
                return false;
            }

            VkPipelineShaderStageCreateInfo stageCreateInfo = {};
            stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageCreateInfo.pName = "main";
            stageCreateInfo.module = shaderModule.GetHandle();
            stageCreateInfo.stage = GetShaderProgramStageFlag(scriptType);
            shaderStageCreateInfos.push_back(stageCreateInfo);
        }

        pushConstantLocations = std::move(glslTemplate.pushConstantLocations);

        pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
        pushConstantRange.offset = 0;
        pushConstantRange.size = glslTemplate.pushConstantBlockByteSize;

        mappedDescriptorSets = std::move(glslTemplate.mappedDescriptorSets);

        return true;
    }

}

#endif