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


#include "Molten/Renderer/Vulkan/VulkanRenderer.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Renderer/Vulkan/VulkanFramebuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanIndexBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Molten/Renderer/Vulkan/VulkanSampler.hpp"
#include "Molten/Renderer/Vulkan/VulkanTexture.hpp"
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
#include <algorithm>
#include <limits>
#include <memory>

namespace Molten
{

    // Static helper functions and traits.
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

    static VkFormat GetImageFormatAndPixelSize(const ImageFormat imageFormat, uint32_t& pixelSize)
    {
        switch (imageFormat)
        {
            case ImageFormat::URed8: pixelSize = 1; return VkFormat::VK_FORMAT_R8_UNORM;
            case ImageFormat::URed8Green8: pixelSize = 2; return VkFormat::VK_FORMAT_R8G8_UNORM;
            case ImageFormat::URed8Green8Blue8: pixelSize = 3; return VkFormat::VK_FORMAT_R8G8B8_UNORM;
            case ImageFormat::URed8Green8Blue8Alpha8: pixelSize = 4; return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;

            case ImageFormat::SrgbRed8Green8Blue8: pixelSize = 3; return VkFormat::VK_FORMAT_R8G8B8_SRGB;
            case ImageFormat::SrgbRed8Green8Blue8Alpha8: pixelSize = 4; return VkFormat::VK_FORMAT_R8G8B8A8_SRGB;

            case ImageFormat::SRed8: pixelSize = 1; return VkFormat::VK_FORMAT_R8_SNORM;
            case ImageFormat::SRed8Green8: pixelSize = 2; return VkFormat::VK_FORMAT_R8G8_SNORM;
            case ImageFormat::SRed8Green8Blue8: pixelSize = 3; return VkFormat::VK_FORMAT_R8G8B8_SNORM;
            case ImageFormat::SRed8Green8Blue8Alpha8: pixelSize = 4; return VkFormat::VK_FORMAT_R8G8B8A8_SNORM;

            case ImageFormat::UBlue8Green8Red8: pixelSize = 3; return VkFormat::VK_FORMAT_B8G8R8_UNORM;
            case ImageFormat::UBlue8Green8Red8Alpha8: pixelSize = 4; return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
        }
        throw Exception("Provided image format is not supported by the Vulkan renderer.");
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
            case Shader::BindingType::UniformBuffer: return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        throw Exception("Provided binding type is not handled.");
    }

    static VkFilter GetSamplerFilter(const SamplerFilter samplerFilter)
    {
        switch (samplerFilter)
        {
            case SamplerFilter::Nearest: return VkFilter::VK_FILTER_NEAREST;
            case SamplerFilter::Linear: return VkFilter::VK_FILTER_NEAREST;
        }
        throw Exception("Provided sampler filter is not handled.");
    }

    static VkSamplerAddressMode GetSamplerAddressMode(const SamplerWrapMode samplerWrapMode)
    {
        switch (samplerWrapMode)
        {
            case SamplerWrapMode::Repeat: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case SamplerWrapMode::RepeatMirror: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case SamplerWrapMode::Clamp: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; // Or to border??
        }
        throw Exception("Provided sampler wrap mode is not handled.");
    }

    static VkBlendFactor GetBlendFactor(const Pipeline::BlendFunction blendFunction)
    {
        switch (blendFunction)
        {
            case Pipeline::BlendFunction::Zero: return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
            case Pipeline::BlendFunction::One: return VkBlendFactor::VK_BLEND_FACTOR_ONE;
            case Pipeline::BlendFunction::SourceColor: return VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;
            case Pipeline::BlendFunction::SourceAlpha: return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
            case Pipeline::BlendFunction::DestinationColor: return VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;
            case Pipeline::BlendFunction::DestinationAlpha: return VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;
            case Pipeline::BlendFunction::OneMinusSourceColor: return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
            case Pipeline::BlendFunction::OneMinusSourceAlpha: return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            case Pipeline::BlendFunction::OneMinusDestinationColor: return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            case Pipeline::BlendFunction::OneMinusDestinationAlpha: return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        }

        throw Exception("Provided blend function is not handled.");
    }

    static VkBlendOp GetBlendOperator(const Pipeline::BlendOperator blendOperator)
    {
        switch (blendOperator)
        {
            case Pipeline::BlendOperator::Add: return VkBlendOp::VK_BLEND_OP_ADD;
            case Pipeline::BlendOperator::Subtract: return VkBlendOp::VK_BLEND_OP_SUBTRACT;
            case Pipeline::BlendOperator::ReverseSubtract: return VkBlendOp::VK_BLEND_OP_REVERSE_SUBTRACT;
            case Pipeline::BlendOperator::Min: return VkBlendOp::VK_BLEND_OP_MIN;
            case Pipeline::BlendOperator::Max: return VkBlendOp::VK_BLEND_OP_MAX;
        }

        throw Exception("Provided blend operator is not handled.");
    }

    static VkComponentSwizzle GetComponentSwizzle(const ImageComponentSwizzle componentswizzle)
    {
        switch (componentswizzle)
        {
            case ImageComponentSwizzle::Identity: return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
            case ImageComponentSwizzle::Red: return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
            case ImageComponentSwizzle::Green: return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
            case ImageComponentSwizzle::Blue: return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
            case ImageComponentSwizzle::Alpha: return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;
            case ImageComponentSwizzle::Zero: return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ZERO;
            case ImageComponentSwizzle::One: return VkComponentSwizzle::VK_COMPONENT_SWIZZLE_ONE;
        }

        throw Exception("Provided texture component swizzle is not handled.");
    }

    static VkComponentMapping GetComponentMappings(const ImageSwizzleMapping swizzleMapping)
    {
        return {
            GetComponentSwizzle(swizzleMapping.red),
            GetComponentSwizzle(swizzleMapping.green),
            GetComponentSwizzle(swizzleMapping.blue),
            GetComponentSwizzle(swizzleMapping.alpha)
        };
    }

    template<size_t VDimensions>
    struct VulkanImageTypeTraits { };
    template<>
    struct VulkanImageTypeTraits<1>
    {
        inline static constexpr VkImageType type = VkImageType::VK_IMAGE_TYPE_1D;
    };
    template<>
    struct VulkanImageTypeTraits<2>
    {
        inline static constexpr VkImageType type = VkImageType::VK_IMAGE_TYPE_2D;
    };
    template<>
    struct VulkanImageTypeTraits<3>
    {
        inline static constexpr VkImageType type = VkImageType::VK_IMAGE_TYPE_3D;
    };

    template<size_t VDimensions>
    struct VulkanImageViewTypeTraits { };
    template<>
    struct VulkanImageViewTypeTraits<1>
    {
        inline static constexpr VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;
    };
    template<>
    struct VulkanImageViewTypeTraits<2>
    {
        inline static constexpr VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
    };
    template<>
    struct VulkanImageViewTypeTraits<3>
    {
        inline static constexpr VkImageViewType type = VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;
    };

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
        m_capabilities{},
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
        m_resourceDestroyer(m_logicalDevice),
        m_renderPass(VK_NULL_HANDLE),
        m_swapChain{},
        m_surfaceFormat{},
        m_presentMode(VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR),
        m_commandPool(VK_NULL_HANDLE),
        m_commandBuffers{},
        m_currentCommandBuffer(nullptr),
        m_beginDraw(false),
        m_currentPipeline(nullptr),
        m_endedDraws(0)
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
        m_resourceDestroyer.ProcessAll();

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
        m_endedDraws = 0;
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

    const RendererCapabilities& VulkanRenderer::GetCapabilities() const
    {
        static RendererCapabilities tmpCapabilities = {};
        return tmpCapabilities;
    }

    uint32_t VulkanRenderer::GetPushConstantLocation(Pipeline& pipeline, const uint32_t id)
    {
        auto& locations = static_cast<VulkanPipeline&>(pipeline).pushConstantLocations;
        const auto it = locations.find(id);
        if (it == locations.end())
        {
            return PushConstantLocation::UnknownLocation;
        }

        return it->second.location;
    }

    RenderResource<DescriptorSet> VulkanRenderer::CreateDescriptorSet(const DescriptorSetDescriptor& descriptor)
    {
        auto* vulkanPipeline = static_cast<VulkanPipeline*>(descriptor.pipeline->get());

        const auto& mappedSets = vulkanPipeline->mappedDescriptorSets;
        auto mappedSetIt = mappedSets.find(descriptor.id);
        if(mappedSetIt == mappedSets.end())
        {
            Logger::WriteError(m_logger, "Failed to find mapped descriptor set of id " + std::to_string(descriptor.id) + ".");
            return { };
        }

        const auto& mappedSet = mappedSetIt->second;
        const uint32_t setIndex = mappedSet.index;

        std::vector<std::unique_ptr<VkDescriptorBufferInfo>> bufferInfos;
        std::vector<std::unique_ptr<VkDescriptorImageInfo>> imageInfos;
        std::vector<VkDescriptorPoolSize> poolSizes = {};

        auto addBindingToPool = [&](VkDescriptorType descriptorType)
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
        
        auto writeBufferInfo = [&](VkWriteDescriptorSet& write, VkBuffer buffer)
        {
            addBindingToPool(VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

            auto bufferInfo = std::make_unique<VkDescriptorBufferInfo>();
            bufferInfo->buffer = buffer;
            bufferInfo->offset = 0;
            bufferInfo->range = VK_WHOLE_SIZE;

            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.pBufferInfo = bufferInfo.get();

            bufferInfos.push_back(std::move(bufferInfo));
        };
        auto writeImageInfo = [&](VkWriteDescriptorSet& write, VkSampler sampler, VkImageView imageView)
        {
            addBindingToPool(VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

            auto imageInfo = std::make_unique<VkDescriptorImageInfo>();
            imageInfo->sampler = sampler;
            imageInfo->imageView = imageView;
            imageInfo->imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.descriptorCount = 1;
            write.pImageInfo = imageInfo.get();

            imageInfos.push_back(std::move(imageInfo));      
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
                return { };
            }
            const auto& mappedBinding = mappedBindingIt->second;

            auto& write = writes[writeIndex];
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = VK_NULL_HANDLE;
            write.dstBinding = mappedBinding.index;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;

            std::visit([&](auto& bindingData) 
            {
                using T = std::decay_t<decltype(bindingData)>;
                if constexpr (std::is_same_v<T, RenderResource<UniformBuffer>*>)
                {
                    const auto bufferHandle = static_cast<VulkanUniformBuffer*>(bindingData->get())->deviceBuffer.GetHandle();
                    writeBufferInfo(write, bufferHandle);
                }
                else if constexpr (std::is_same_v<T, CombinedTextureSampler1D>)
                {
                    const auto imageView = static_cast<VulkanTexture1D*>(bindingData.texture->get())->imageView;
                    const auto sampler = static_cast<VulkanSampler1D*>(bindingData.sampler)->imageSampler.GetHandle();
                    writeImageInfo(write, sampler, imageView);
                }
                else if constexpr (std::is_same_v<T, CombinedTextureSampler2D>)
                {
                    const auto imageView = static_cast<VulkanTexture2D*>(bindingData.texture->get())->imageView;
                    const auto sampler = static_cast<VulkanSampler2D*>(bindingData.sampler)->imageSampler.GetHandle();
                    writeImageInfo(write, sampler, imageView);
                }
                else if constexpr (std::is_same_v<T, CombinedTextureSampler3D>)
                {
                    const auto imageView = static_cast<VulkanTexture3D*>(bindingData.texture->get())->imageView;
                    const auto sampler = static_cast<VulkanSampler3D*>(bindingData.sampler)->imageSampler.GetHandle();
                    writeImageInfo(write, sampler, imageView);
                }
                else
                {
                    static_assert(false, "Unknown binding type." );
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
        SmartFunction destroyer = [&]()
        {
            if(descriptorPool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(m_logicalDevice.GetHandle(), descriptorPool, nullptr);
            }
        };
 
        if (vkCreateDescriptorPool(m_logicalDevice.GetHandle(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor pool.");
            return { };
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
            return { };
        }

        for(auto& write : writes)
        {
            write.dstSet = descriptorSet;
            vkUpdateDescriptorSets(m_logicalDevice.GetHandle(), 1, &write, 0, nullptr);
        }

        destroyer.Release();

        return RenderResource<DescriptorSet>(new VulkanDescriptorSet{
            setIndex,
            descriptorSet,
            descriptorPool
        }, RenderResourceDeleter<DescriptorSet>{ this });
    }

    RenderResource<FramedDescriptorSet> VulkanRenderer::CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& descriptor)
    {
        auto* vulkanPipeline = static_cast<VulkanPipeline*>(descriptor.pipeline->get());

        const auto& mappedSets = vulkanPipeline->mappedDescriptorSets;
        auto mappedSetIt = mappedSets.find(descriptor.id);
        if (mappedSetIt == mappedSets.end())
        {
            Logger::WriteError(m_logger, "Failed to find mapped descriptor set of id " + std::to_string(descriptor.id) + ".");
            return { };
        }

        const auto& mappedSet = mappedSetIt->second;
        const uint32_t setIndex = mappedSet.index;

        std::vector<std::unique_ptr<VkDescriptorBufferInfo[]>> bufferInfos;
        std::vector<std::unique_ptr<VkDescriptorImageInfo[]>> imageInfos;
        std::vector<VkDescriptorPoolSize> poolSizes;
        const uint32_t swapChainImageCount = m_swapChain.GetImageCount();  
    
        const auto& bindings = descriptor.bindings;
        auto setWrites = std::make_unique<std::vector<VkWriteDescriptorSet>[]>(swapChainImageCount);
        for (uint32_t i = 0; i < swapChainImageCount; i++)
        {
            setWrites[i].resize(bindings.size());
        }

        using SetWritesType = std::unique_ptr<std::vector<VkWriteDescriptorSet>[]>;

        auto AddBindingToPool = [&](VkDescriptorType descriptorType, const uint32_t descriptorCount)
        {
            auto it = std::find_if(poolSizes.begin(), poolSizes.end(), [&](VkDescriptorPoolSize& poolSize)
            {
                return poolSize.type == descriptorType;
            });

            if (it != poolSizes.end())
            {
                auto& poolSize = *it;
                poolSize.descriptorCount += descriptorCount;
                return;
            }

            VkDescriptorPoolSize poolSize = {};
            poolSize.type = descriptorType;
            poolSize.descriptorCount = descriptorCount;
            poolSizes.push_back(poolSize);
        };

        auto writeBufferInfos = [&](
            SetWritesType& writes,
            const size_t writesIndex,
            const uint32_t bindingIndex,
            const std::vector<Vulkan::DeviceBuffer>& buffers)
        {
           auto bufferInfo = std::make_unique<VkDescriptorBufferInfo[]>(swapChainImageCount);
           const VkDescriptorType descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            for(size_t i = 0; i < swapChainImageCount; i++)
            {
                bufferInfo[i].buffer = buffers[i].GetHandle();
                bufferInfo[i].offset = 0;
                bufferInfo[i].range = VK_WHOLE_SIZE;

                auto& write = writes[i][writesIndex];
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = VK_NULL_HANDLE;
                write.dstBinding = bindingIndex;
                write.dstArrayElement = 0;
                write.descriptorCount = 1;
                write.descriptorType = descriptorType;
                write.pBufferInfo = &bufferInfo[i];
            }

            bufferInfos.push_back(std::move(bufferInfo));
            AddBindingToPool(descriptorType, swapChainImageCount);
        };

        auto writeImageInfos = [&](
            SetWritesType& writes,
            const size_t writesIndex,
            const uint32_t bindingIndex,
            VkSampler sampler,
            VkImageView imageView)
        {
            auto imageInfo = std::make_unique<VkDescriptorImageInfo[]>(swapChainImageCount);
            const VkDescriptorType descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

            for (size_t i = 0; i < swapChainImageCount; i++)
            {
                imageInfo[i].sampler = sampler;
                imageInfo[i].imageView = imageView;
                imageInfo[i].imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                auto& write = writes[i][writesIndex];
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = VK_NULL_HANDLE;
                write.dstBinding = bindingIndex;
                write.dstArrayElement = 0;
                write.descriptorCount = 1;
                write.descriptorType = descriptorType;
                write.pImageInfo = &imageInfo[i];
            }

            imageInfos.push_back(std::move(imageInfo));
            AddBindingToPool(descriptorType, swapChainImageCount);
        };

        size_t writeIndex = 0;
        const auto& mappedBindings = mappedSet.bindings;
        for (const auto& binding : bindings)
        {
            auto mappedBindingIt = mappedBindings.find(binding.id);
            if (mappedBindingIt == mappedBindings.end())
            {
                Logger::WriteError(m_logger, "Failed to find mapped descriptor binding of id " + std::to_string(binding.id) + ".");
                return { };
            }
            const auto& mappedBinding = mappedBindingIt->second;
            const uint32_t bindingIndex = mappedBinding.index;

            bool successfulVisit = true;
            std::visit([&](auto& bindingData)
            {
                using T = std::decay_t<decltype(bindingData)>;
                if constexpr (std::is_same_v<T, RenderResource<FramedUniformBuffer>*>)
                {
                    const auto& deviceBuffers = static_cast<VulkanFramedUniformBuffer*>(bindingData->get())->deviceBuffers;
                    if (deviceBuffers.size() != swapChainImageCount)
                    {
                        Logger::WriteError(m_logger, "Buffer count(" + std::to_string(deviceBuffers.size()) +
                            ") of framed buffer is mismatching with number of swap chain images(" + std::to_string(swapChainImageCount) + ").");
                        successfulVisit = false;
                        return;
                    }

                    writeBufferInfos(setWrites, writeIndex, bindingIndex, deviceBuffers);
                }
                else if constexpr (std::is_same_v<T, CombinedTextureSampler1D>)
                {
                    const auto imageView = static_cast<VulkanTexture1D*>(bindingData.texture->get())->imageView;
                    const auto sampler = static_cast<VulkanSampler1D*>(bindingData.sampler)->imageSampler.GetHandle();
                    writeImageInfos(setWrites, writeIndex, bindingIndex, sampler, imageView);
                }
                else if constexpr (std::is_same_v<T, CombinedTextureSampler2D>)
                {
                    const auto imageView = static_cast<VulkanTexture2D*>(bindingData.texture->get())->imageView;
                    const auto sampler = static_cast<VulkanSampler2D*>(bindingData.sampler)->imageSampler.GetHandle();
                    writeImageInfos(setWrites, writeIndex, bindingIndex, sampler, imageView);
                }
                else if constexpr (std::is_same_v<T, CombinedTextureSampler3D>)
                {
                    const auto imageView = static_cast<VulkanTexture3D*>(bindingData.texture->get())->imageView;
                    const auto sampler = static_cast<VulkanSampler3D*>(bindingData.sampler)->imageSampler.GetHandle();
                    writeImageInfos(setWrites, writeIndex, bindingIndex, sampler, imageView);
                }
                else
                {
                    static_assert(false, "Unknown binding type.");
                }

            }, binding.binding);

            if(!successfulVisit)
            {
                return { };
            }

            writeIndex++;
        }

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = swapChainImageCount;
        poolInfo.flags = 0;

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        SmartFunction destroyer = [&]()
        {
            if (descriptorPool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(m_logicalDevice.GetHandle(), descriptorPool, nullptr);
            }
        };

        if (vkCreateDescriptorPool(m_logicalDevice.GetHandle(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor pool.");
            return { };
        }

        std::vector<VkDescriptorSetLayout> setLayouts(swapChainImageCount, vulkanPipeline->descriptionSetLayouts[setIndex]);
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = swapChainImageCount;
        allocInfo.pSetLayouts = setLayouts.data();

        std::vector<VkDescriptorSet> descriptorSets(swapChainImageCount, VK_NULL_HANDLE);
        if (vkAllocateDescriptorSets(m_logicalDevice.GetHandle(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor sets.");
            return { };
        }

        for(uint32_t i = 0; i < swapChainImageCount; i++)
        {
            const auto descriptorSet = descriptorSets[i];
            for (auto& write : setWrites[i])
            {
                write.dstSet = descriptorSet;
                vkUpdateDescriptorSets(m_logicalDevice.GetHandle(), 1, &write, 0, nullptr);
            }
        }

        destroyer.Release();

        return RenderResource<FramedDescriptorSet>(new VulkanFramedDescriptorSet{
            setIndex,
            std::move(descriptorSets),
            descriptorPool
        }, RenderResourceDeleter<FramedDescriptorSet>{ this });
    }

    RenderResource<Framebuffer> VulkanRenderer::CreateFramebuffer(const FramebufferDescriptor& descriptor)
    {
        return { };
    }

    RenderResource<IndexBuffer> VulkanRenderer::CreateIndexBuffer(const IndexBufferDescriptor& descriptor)
    {
        const auto bufferSize = static_cast<VkDeviceSize>(descriptor.indexCount) * GetIndexBufferDataTypeSize(descriptor.dataType);

        Vulkan::DeviceBuffer stagingBuffer;

        Vulkan::Result<> result;
        if (!(result = stagingBuffer.Create(m_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_memoryTypesHostVisibleOrCoherent)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for index buffer");
            return { };
        }

        if (!(result = stagingBuffer.MapMemory(0, bufferSize, descriptor.data)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for index buffer");
            return { };
        }

        Vulkan::DeviceBuffer indexBuffer;
        if (!(result = indexBuffer.Create(m_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_memoryTypesDeviceLocal)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create index buffer");
            return { };
        }

        if (!(result = indexBuffer.CopyFromBuffer(m_commandPool, stagingBuffer, bufferSize)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to copy from staging buffer to index buffer");
            return { };
        }

        return RenderResource<IndexBuffer>(new VulkanIndexBuffer{
            std::move(indexBuffer),
            descriptor.indexCount,
            descriptor.dataType
        }, RenderResourceDeleter<IndexBuffer>{ this });
    }

    RenderResource<Pipeline> VulkanRenderer::CreatePipeline(const PipelineDescriptor& descriptor)
    {
        if (descriptor.vertexScript == nullptr)
        {
            Logger::WriteError(m_logger, "Vertex script is missing for pipeline. (vertexScript == nullptr).");
            return { };
        }
        if (descriptor.fragmentScript == nullptr)
        {
            Logger::WriteError(m_logger, "Fragment script is missing for pipeline. (fragmentScript == nullptr).");
            return { };
        }

        auto& vertexScript = *descriptor.vertexScript;
        auto& fragmentScript = *descriptor.fragmentScript;
        auto& vertexOutputs = vertexScript.GetOutputInterface();
        auto& fragmentInputs = fragmentScript.GetInputInterface();

        if (!vertexOutputs.CompareStructure(fragmentInputs))
        {
            Logger::WriteError(m_logger, "Vertex output structure is not compatible with fragment input structure.");
            return { };
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
            return { };
        }

        std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
        uint32_t vertexBindingStride = 0;
        if (descriptor.vertexScript)
        {
            auto& vertexInputs = descriptor.vertexScript->GetInputInterface();
            if (!CreateVertexInputAttributes(vertexInputs, vertexInputAttributes, vertexBindingStride))
            {
                return { };
            }
        }

        Vulkan::DescriptorSetLayouts setLayouts;

        SmartFunction destroyer = [&]()
        {
            for(auto& setLayout : setLayouts)
            {
                vkDestroyDescriptorSetLayout(m_logicalDevice.GetHandle(), setLayout, nullptr);
            }
        };

        if (!CreateDescriptorSetLayouts(setLayouts, mappedDescriptorSets))
        {
            return { };
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

        auto& blendingDescriptor = descriptor.blending;
        const auto blendOperator = GetBlendOperator(blendingDescriptor.blendOperator);

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
        colorBlendAttachmentState.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.blendEnable = VK_TRUE;
        colorBlendAttachmentState.srcColorBlendFactor = GetBlendFactor(blendingDescriptor.sourceColor);
        colorBlendAttachmentState.dstColorBlendFactor = GetBlendFactor(blendingDescriptor.destinationColor);
        colorBlendAttachmentState.colorBlendOp = blendOperator;
        colorBlendAttachmentState.srcAlphaBlendFactor = GetBlendFactor(blendingDescriptor.sourceAlpha);
        colorBlendAttachmentState.dstAlphaBlendFactor = GetBlendFactor(blendingDescriptor.destinationAlpha);
        colorBlendAttachmentState.alphaBlendOp = blendOperator;

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
            return { };
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
            return { };
        }

        destroyer.Release();

        return RenderResource<Pipeline>(new VulkanPipeline{
            graphicsPipeline,
            pipelineLayout,
            std::move(setLayouts),
            std::move(pushConstantLocations),
            std::move(shaderModules),
            std::move(mappedDescriptorSets)
        }, RenderResourceDeleter<Pipeline>{ this });
    }

    RenderResource<Sampler1D> VulkanRenderer::CreateSampler(const SamplerDescriptor1D& descriptor)
    {
        const auto magFilter = GetSamplerFilter(descriptor.magFilter);
        const auto minFilter = GetSamplerFilter(descriptor.minFilter);
        const auto addressModeU = GetSamplerAddressMode(descriptor.wrapModes.c[0]);
        const auto addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        const auto addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        const bool anisotropyEnable = descriptor.maxAnisotropy > 1;
        const auto maxAnisotropy = static_cast<float>(descriptor.maxAnisotropy);

        Vulkan::Result<> result;
        Vulkan::ImageSampler imageSampler;
        if (!(result = imageSampler.Create(
            m_logicalDevice,
            magFilter,
            minFilter,
            addressModeU,
            addressModeV,
            addressModeW,
            anisotropyEnable,
            maxAnisotropy)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create 1D image sampler");
            return { };
        }

        return RenderResource<Sampler1D>(new VulkanSampler1D{
            std::move(imageSampler)
         }, RenderResourceDeleter<Sampler1D>{ this });
    }

    RenderResource<Sampler2D> VulkanRenderer::CreateSampler(const SamplerDescriptor2D& descriptor)
    {
        const auto magFilter = GetSamplerFilter(descriptor.magFilter);
        const auto minFilter = GetSamplerFilter(descriptor.minFilter);
        const auto addressModeU = GetSamplerAddressMode(descriptor.wrapModes.x);
        const auto addressModeV = GetSamplerAddressMode(descriptor.wrapModes.y);
        const auto addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        const bool anisotropyEnable = descriptor.maxAnisotropy > 1;
        const auto maxAnisotropy = static_cast<float>(descriptor.maxAnisotropy);

        Vulkan::Result<> result;
        Vulkan::ImageSampler imageSampler;
        if (!(result = imageSampler.Create(
            m_logicalDevice,
            magFilter,
            minFilter,
            addressModeU,
            addressModeV,
            addressModeW,
            anisotropyEnable,
            maxAnisotropy)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create 2D image sampler");
            return { };
        }

        return RenderResource<Sampler2D>(new VulkanSampler2D{
            std::move(imageSampler)
        }, RenderResourceDeleter<Sampler2D>{ this });
    }

    RenderResource<Sampler3D> VulkanRenderer::CreateSampler(const SamplerDescriptor3D& descriptor)
    {
        const auto magFilter = GetSamplerFilter(descriptor.magFilter);
        const auto minFilter = GetSamplerFilter(descriptor.minFilter);
        const auto addressModeU = GetSamplerAddressMode(descriptor.wrapModes.x);
        const auto addressModeV = GetSamplerAddressMode(descriptor.wrapModes.y);
        const auto addressModeW = GetSamplerAddressMode(descriptor.wrapModes.x);
        const bool anisotropyEnable = descriptor.maxAnisotropy > 1;
        const auto maxAnisotropy = static_cast<float>(descriptor.maxAnisotropy);

        Vulkan::Result<> result;
        Vulkan::ImageSampler imageSampler;
        if (!(result = imageSampler.Create(
            m_logicalDevice,
            magFilter,
            minFilter,
            addressModeU,
            addressModeV,
            addressModeW,
            anisotropyEnable,
            maxAnisotropy)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create 3D image sampler");
            return { };
        }

        return RenderResource<Sampler3D>(new VulkanSampler3D{
            std::move(imageSampler)
        }, RenderResourceDeleter<Sampler3D>{ this });
    }

    RenderResource<Texture1D> VulkanRenderer::CreateTexture(const TextureDescriptor1D& descriptor)
    {
        uint32_t formatPixelSize = 0;
        uint32_t internalFormatPixelSize = 0;
        const auto imageFormat = GetImageFormatAndPixelSize(descriptor.format, formatPixelSize);
        const auto internalImageFormat = GetImageFormatAndPixelSize(descriptor.internalFormat, internalFormatPixelSize);
        if(formatPixelSize != internalFormatPixelSize)
        {
            Logger::WriteError(m_logger, "Format and internal format of texture mismatches number of bytes per pixel: " + 
                std::to_string(formatPixelSize) + " : " + std::to_string(internalFormatPixelSize));
            return { };
        }

        const auto dataSize = static_cast<VkDeviceSize>(descriptor.dimensions.c[0]) * static_cast<VkDeviceSize>(formatPixelSize);
        const auto dimensions = Vector3ui32{ descriptor.dimensions.c[0], 1, 1 };

        return CreateTexture<1>(dimensions, dataSize, descriptor.data, imageFormat, internalImageFormat, GetComponentMappings(descriptor.swizzleMapping));
    }

    RenderResource<Texture2D> VulkanRenderer::CreateTexture(const TextureDescriptor2D& descriptor)
    {
        uint32_t formatPixelSize = 0;
        uint32_t internalFormatPixelSize = 0;
        const auto imageFormat = GetImageFormatAndPixelSize(descriptor.format, formatPixelSize);
        const auto internalImageFormat = GetImageFormatAndPixelSize(descriptor.internalFormat, internalFormatPixelSize);
        if (formatPixelSize != internalFormatPixelSize)
        {
            Logger::WriteError(m_logger, "Format and internal format of texture mismatches number of bytes per pixel: " +
                std::to_string(formatPixelSize) + " : " + std::to_string(internalFormatPixelSize));
            return { };
        }

        const auto dataSize =
            static_cast<VkDeviceSize>(descriptor.dimensions.x) *
            static_cast<VkDeviceSize>(descriptor.dimensions.y) * static_cast<VkDeviceSize>(formatPixelSize);
        const auto dimensions = Vector3ui32{ descriptor.dimensions.x, descriptor.dimensions.y, 1 };

        return CreateTexture<2>(dimensions, dataSize, descriptor.data, imageFormat, internalImageFormat, GetComponentMappings(descriptor.swizzleMapping));
    }

    RenderResource<Texture3D> VulkanRenderer::CreateTexture(const TextureDescriptor3D& descriptor)
    {
        uint32_t formatPixelSize = 0;
        uint32_t internalFormatPixelSize = 0;
        const auto imageFormat = GetImageFormatAndPixelSize(descriptor.format, formatPixelSize);
        const auto internalImageFormat = GetImageFormatAndPixelSize(descriptor.internalFormat, internalFormatPixelSize);
        if (formatPixelSize != internalFormatPixelSize)
        {
            Logger::WriteError(m_logger, "Format and internal format of texture mismatches number of bytes per pixel: " +
                std::to_string(formatPixelSize) + " : " + std::to_string(internalFormatPixelSize));
            return { };
        }

        const auto dataSize =
            static_cast<VkDeviceSize>(descriptor.dimensions.x) *
            static_cast<VkDeviceSize>(descriptor.dimensions.y) *
            static_cast<VkDeviceSize>(descriptor.dimensions.z) * static_cast<VkDeviceSize>(formatPixelSize);
        const auto dimensions = Vector3ui32{ descriptor.dimensions.x, descriptor.dimensions.y, descriptor.dimensions.z };

        return CreateTexture<3>(dimensions, dataSize, descriptor.data, imageFormat, internalImageFormat, GetComponentMappings(descriptor.swizzleMapping));
    }

    RenderResource<UniformBuffer> VulkanRenderer::CreateUniformBuffer(const UniformBufferDescriptor& descriptor)
    {
        Vulkan::Result<> result;
        Vulkan::DeviceBuffer deviceBuffer;
        if (!(result = deviceBuffer.Create(m_logicalDevice, descriptor.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_memoryTypesHostVisibleOrCoherent)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create device buffer for uniform buffer");
            return { };
        }

        return RenderResource<UniformBuffer>(new VulkanUniformBuffer{
            std::move(deviceBuffer)
        }, RenderResourceDeleter<UniformBuffer>{ this });
    }

    RenderResource<FramedUniformBuffer> VulkanRenderer::CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor)
    {
        std::vector<Vulkan::DeviceBuffer> deviceBuffers(m_swapChain.GetImageCount());
        Vulkan::Result<> result;
        for(auto& deviceBuffer : deviceBuffers)
        {
            if (!(result = deviceBuffer.Create(m_logicalDevice, descriptor.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_memoryTypesHostVisibleOrCoherent)))
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create device buffer for framed uniform buffer");
                return { };
            }
        }

        return RenderResource<FramedUniformBuffer>(new VulkanFramedUniformBuffer{
            std::move(deviceBuffers)
        }, RenderResourceDeleter<FramedUniformBuffer>{ this });
    }

    RenderResource<VertexBuffer> VulkanRenderer::CreateVertexBuffer(const VertexBufferDescriptor& descriptor)
    {
        const size_t bufferSize = 
            static_cast<VkDeviceSize>(static_cast<VkDeviceSize>(descriptor.vertexCount) *
            static_cast<VkDeviceSize>(descriptor.vertexSize));

        Vulkan::DeviceBuffer stagingBuffer;

        Vulkan::Result<> result;
        if (!(result = stagingBuffer.Create(m_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_memoryTypesHostVisibleOrCoherent)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for vertex buffer");
            return { };
        }

        if (!(result = stagingBuffer.MapMemory(0, bufferSize, descriptor.data)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for vertex buffer");
            return { };
        }

        Vulkan::DeviceBuffer vertexBuffer;
        if (!(result = vertexBuffer.Create(m_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_memoryTypesDeviceLocal)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create vertex buffer");
            return { };
        }

        if (!(result = vertexBuffer.CopyFromBuffer(m_commandPool, stagingBuffer, bufferSize)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to copy from staging buffer to vertex buffer");
            return { };
        }

        return RenderResource<VertexBuffer>(new VulkanVertexBuffer{
            std::move(vertexBuffer),
            descriptor.vertexCount,
            descriptor.vertexSize
        }, RenderResourceDeleter<VertexBuffer>{ this });
    }

    bool VulkanRenderer::UpdateTexture(Texture1D& texture1D, const TextureUpdateDescriptor1D& descriptor)
    {
        auto& vulkanTexture = static_cast<VulkanTexture1D&>(texture1D);

        const size_t dataSize = descriptor.destinationDimensions.c[0];
        const Vector3ui32 destinationDimensions = {
            descriptor.destinationDimensions.c[0], 1, 1
        };
        const Vector3ui32 destinationOffset = {
            descriptor.destinationOffset.c[0], 0, 0
        };

        return UpdateTexture<1>(
            vulkanTexture.image,
            descriptor.data,
            dataSize,
            destinationDimensions,
            destinationOffset);
    }

    bool VulkanRenderer::UpdateTexture(Texture2D& texture2D, const TextureUpdateDescriptor2D& descriptor)
    {
        auto& vulkanTexture = static_cast<VulkanTexture2D&>(texture2D);

        const size_t dataSize = descriptor.destinationDimensions.x * descriptor.destinationDimensions.y;
        const Vector3ui32 destinationDimensions = {
            descriptor.destinationDimensions.x, descriptor.destinationDimensions.y, 1
        };
        const Vector3ui32 destinationOffset = {
            descriptor.destinationOffset.x, descriptor.destinationOffset.y, 0
        };

        return UpdateTexture<2>(
            vulkanTexture.image,
            descriptor.data,
            dataSize,
            destinationDimensions,
            destinationOffset);
    }

    bool VulkanRenderer::UpdateTexture(Texture3D& texture3D, const TextureUpdateDescriptor3D& descriptor)
    {
        auto& vulkanTexture = static_cast<VulkanTexture3D&>(texture3D);

        const size_t dataSize = descriptor.destinationDimensions.x * descriptor.destinationDimensions.y * descriptor.destinationDimensions.z;

        return UpdateTexture<3>(
            vulkanTexture.image,
            descriptor.data,
            dataSize,
            descriptor.destinationDimensions,
            descriptor.destinationOffset);
    }

    void VulkanRenderer::Destroy(DescriptorSet& descriptorSet)
    {
        auto& vulkanDescriptorSet = static_cast<VulkanDescriptorSet&>(descriptorSet);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanDescriptorSet);
    }

    void VulkanRenderer::Destroy(FramedDescriptorSet& framedDescriptorSet)
    {
        auto& vulkanFramedDescriptorSet = static_cast<VulkanFramedDescriptorSet&>(framedDescriptorSet);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanFramedDescriptorSet);
    }

    void VulkanRenderer::Destroy(Framebuffer& /*framebuffer*/)
    {
        // Not implemented yet, due to missing creation implementations.
        //
        //auto& vulkanFramebuffer = static_cast<VulkanFramebuffer&>(framebuffer);
        //m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanFramebuffer);
    }

    void VulkanRenderer::Destroy(IndexBuffer& indexBuffer)
    {
        auto& vulkanIndexBuffer = static_cast<VulkanIndexBuffer&>(indexBuffer);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanIndexBuffer);
    }

    void VulkanRenderer::Destroy(Pipeline& pipeline)
    {
        auto& vulkanPipeline = static_cast<VulkanPipeline&>(pipeline);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanPipeline);
    }

    void VulkanRenderer::Destroy(Sampler1D& sampler1D)
    {
        auto& vulkanSampler = static_cast<VulkanSampler1D&>(sampler1D);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanSampler);
    }

    void VulkanRenderer::Destroy(Sampler2D& sampler2D)
    {
        auto& vulkanSampler = static_cast<VulkanSampler2D&>(sampler2D);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanSampler);
    }

    void VulkanRenderer::Destroy(Sampler3D& sampler3D)
    {
        auto& vulkanSampler = static_cast<VulkanSampler3D&>(sampler3D);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanSampler);
    }

    void VulkanRenderer::Destroy(Texture1D& texture1D)
    {
        auto& vulkanTexture = static_cast<VulkanTexture1D&>(texture1D);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanTexture);
    }

    void VulkanRenderer::Destroy(Texture2D& texture2D)
    {
        auto& vulkanTexture = static_cast<VulkanTexture2D&>(texture2D);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanTexture);
    }

    void VulkanRenderer::Destroy(Texture3D& texture3D)
    {
        auto& vulkanTexture = static_cast<VulkanTexture3D&>(texture3D);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanTexture);
    }

    void VulkanRenderer::Destroy(UniformBuffer& uniformBuffer)
    {
        auto& vulkanUniformBuffer = static_cast<VulkanUniformBuffer&>(uniformBuffer);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanUniformBuffer);
    }

    void VulkanRenderer::Destroy(FramedUniformBuffer& framedUniformBuffer)
    {
        auto& vulkanFramedUniformBuffer = static_cast<VulkanFramedUniformBuffer&>(framedUniformBuffer);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanFramedUniformBuffer);
    }

    void VulkanRenderer::Destroy(VertexBuffer& vertexBuffer)
    {
        auto& vulkanVertexBuffer = static_cast<VulkanVertexBuffer&>(vertexBuffer);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanVertexBuffer);
    }

    void VulkanRenderer::BindDescriptorSet(DescriptorSet& descriptorSet)
    {
        auto& vulkanDescriptorSet = static_cast<VulkanDescriptorSet&>(descriptorSet);

        vkCmdBindDescriptorSets(
            *m_currentCommandBuffer, 
            VK_PIPELINE_BIND_POINT_GRAPHICS, 
            m_currentPipeline->pipelineLayout,
            vulkanDescriptorSet.index,
            1,
            &vulkanDescriptorSet.descriptorSet,
            0,
            nullptr);
    }

    void VulkanRenderer::BindFramedDescriptorSet(FramedDescriptorSet& framedDescriptorSet)
    {
        auto& vulkanFramedDescriptorSet = static_cast<VulkanFramedDescriptorSet&>(framedDescriptorSet);

        vkCmdBindDescriptorSets(
            *m_currentCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_currentPipeline->pipelineLayout,
            vulkanFramedDescriptorSet.index,
            1,
            &vulkanFramedDescriptorSet.descriptorSets[m_swapChain.GetCurrentImageIndex()],
            0,
            nullptr);
    }

    void VulkanRenderer::BindPipeline(Pipeline& pipeline)
    {
        auto& vulkanPipeline = static_cast<VulkanPipeline&>(pipeline);
        vkCmdBindPipeline(*m_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline.graphicsPipeline);
        m_currentPipeline = &vulkanPipeline;
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

    void VulkanRenderer::DrawVertexBuffer(VertexBuffer& vertexBuffer)
    {
        auto& vulkanVertexBuffer = static_cast<VulkanVertexBuffer&>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer.deviceBuffer.GetHandle() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(*m_currentCommandBuffer, static_cast<uint32_t>(vulkanVertexBuffer.vertexCount), 1, 0, 0);
    }

    void VulkanRenderer::DrawVertexBuffer(IndexBuffer& indexBuffer, VertexBuffer& vertexBuffer)
    {
        auto& vulkanIndexBuffer = static_cast<VulkanIndexBuffer&>(indexBuffer);
        auto& vulkanVertexBuffer = static_cast<VulkanVertexBuffer&>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer.deviceBuffer.GetHandle() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(*m_currentCommandBuffer, vulkanIndexBuffer.deviceBuffer.GetHandle(), 0, GetIndexBufferDataType(vulkanIndexBuffer.dataType));
        vkCmdDrawIndexed(*m_currentCommandBuffer, static_cast<uint32_t>(vulkanIndexBuffer.indexCount), 1, 0, 0, 0);
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

        m_resourceDestroyer.Process(m_endedDraws);

        ++m_endedDraws;
    }

    void VulkanRenderer::WaitForDevice()
    {
        m_logicalDevice.WaitIdle();
    }

    void VulkanRenderer::UpdateUniformBuffer(RenderResource<UniformBuffer>& uniformBuffer, const size_t offset, const size_t size, const void* data)
    {
        auto* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer.get());      

        Vulkan::Result<> result;
        if (!(result = vulkanUniformBuffer->deviceBuffer.MapMemory(offset, size, data, 0)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map memory of uniform buffer");
        }
    }

    void VulkanRenderer::UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& framedUniformBuffer, const size_t offset, const size_t size, const void* data)
    {
        auto* vulkanFramedUniformBuffer = static_cast<VulkanFramedUniformBuffer*>(framedUniformBuffer.get());

        auto& deviceBuffer = vulkanFramedUniformBuffer->deviceBuffers[m_swapChain.GetCurrentImageIndex()];

        Vulkan::Result<> result;
        if (!(result = deviceBuffer.MapMemory(offset, size, data, 0)))
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map memory of framed uniform buffer");
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

        // Set renderer capabilities.
        m_capabilities.textureSwizzle = true;

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

    uint32_t VulkanRenderer::GetNextDestroyerFrameIndex() const
    {
        return m_endedDraws + m_swapChain.GetMaxFramesInFlight() + 2;
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