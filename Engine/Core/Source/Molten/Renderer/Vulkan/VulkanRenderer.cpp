/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

#include "Molten/Renderer/Vulkan/VulkanRenderer.hpp"
#include "Molten/Renderer/Vulkan/VulkanDescriptorSet.hpp"
#include "Molten/Renderer/Vulkan/VulkanIndexBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Molten/Renderer/Vulkan/VulkanRenderPass.hpp"
#include "Molten/Renderer/Vulkan/VulkanSampler.hpp"
#include "Molten/Renderer/Vulkan/VulkanShaderProgram.hpp"
#include "Molten/Renderer/Vulkan/VulkanTexture.hpp"
#include "Molten/Renderer/Vulkan/VulkanUniformBuffer.hpp"
#include "Molten/Renderer/Vulkan/VulkanVertexBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanFunctions.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResultLogger.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceBuffer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanDeviceImage.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include "Molten/Renderer/Shader/Generator/SpirvShaderGenerator.hpp"
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
    MOLTEN_UNSCOPED_ENUM_BEGIN
    static VkShaderStageFlagBits GetVulkanShaderProgramStageFlag(const Shader::Type type)
    {
        switch (type)
        {
            case Shader::Type::Vertex:   return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
            case Shader::Type::Fragment: return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        throw Exception("Provided shader type is not supported by the Vulkan renderer.");
    }
 
    static VkPrimitiveTopology GetVulkanPrimitiveTopology(const Pipeline::Topology topology)
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

    static VkPolygonMode GetVulkanPolygonMode(const Pipeline::PolygonMode polygonMode)
    {
        switch (polygonMode)
        {
            case Pipeline::PolygonMode::Point: return VkPolygonMode::VK_POLYGON_MODE_POINT;
            case Pipeline::PolygonMode::Line:  return VkPolygonMode::VK_POLYGON_MODE_LINE;
            case Pipeline::PolygonMode::Fill:  return VkPolygonMode::VK_POLYGON_MODE_FILL;
        }
        throw Exception("Provided polygon mode is not supported by the Vulkan renderer.");

    }
    
    static VkFrontFace GetVulkanFrontFace(const Pipeline::FrontFace frontFace)
    {
        switch (frontFace)
        {
            case Pipeline::FrontFace::Clockwise:        return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
            case Pipeline::FrontFace::Counterclockwise: return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
        throw Exception("Provided front face is not supported by the Vulkan renderer.");
    }

    static VkCullModeFlagBits GetVulkanCullMode(const Pipeline::CullMode cullMode)
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

    static VkFormat GetVulkanImageFormat(const ImageFormat imageFormat, uint8_t& bytesPerPixel)
    {
        switch (imageFormat)
        {
            case ImageFormat::URed8: bytesPerPixel = 1; return VkFormat::VK_FORMAT_R8_UNORM;
            case ImageFormat::URed8Green8: bytesPerPixel = 2; return VkFormat::VK_FORMAT_R8G8_UNORM;
            case ImageFormat::URed8Green8Blue8: bytesPerPixel = 3; return VkFormat::VK_FORMAT_R8G8B8_UNORM;
            case ImageFormat::URed8Green8Blue8Alpha8: bytesPerPixel = 4; return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;

            case ImageFormat::SrgbRed8Green8Blue8: bytesPerPixel = 3; return VkFormat::VK_FORMAT_R8G8B8_SRGB;
            case ImageFormat::SrgbRed8Green8Blue8Alpha8: bytesPerPixel = 4; return VkFormat::VK_FORMAT_R8G8B8A8_SRGB;

            case ImageFormat::SRed8: bytesPerPixel = 1; return VkFormat::VK_FORMAT_R8_SNORM;
            case ImageFormat::SRed8Green8: bytesPerPixel = 2; return VkFormat::VK_FORMAT_R8G8_SNORM;
            case ImageFormat::SRed8Green8Blue8: bytesPerPixel = 3; return VkFormat::VK_FORMAT_R8G8B8_SNORM;
            case ImageFormat::SRed8Green8Blue8Alpha8: bytesPerPixel = 4; return VkFormat::VK_FORMAT_R8G8B8A8_SNORM;
            case ImageFormat::SDepthFloat24StencilUint8: bytesPerPixel = 4; return VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;

            case ImageFormat::UBlue8Green8Red8: bytesPerPixel = 3; return VkFormat::VK_FORMAT_B8G8R8_UNORM;
            case ImageFormat::UBlue8Green8Red8Alpha8: bytesPerPixel = 4; return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
        }
        throw Exception("Provided image format is not supported by the Vulkan renderer.");
    }

    static ImageFormat GetImageFormat(const VkFormat imageFormat)
    {
        switch (imageFormat)
        {
            case VkFormat::VK_FORMAT_R8_UNORM: return ImageFormat::URed8;
            case VkFormat::VK_FORMAT_R8G8_UNORM: return ImageFormat::URed8Green8;
            case VkFormat::VK_FORMAT_R8G8B8_UNORM: return ImageFormat::URed8Green8Blue8;
            case VkFormat::VK_FORMAT_R8G8B8A8_UNORM: return ImageFormat::URed8Green8Blue8Alpha8;

            case VkFormat::VK_FORMAT_R8G8B8_SRGB: return ImageFormat::SrgbRed8Green8Blue8;
            case VkFormat::VK_FORMAT_R8G8B8A8_SRGB: return ImageFormat::SrgbRed8Green8Blue8Alpha8;

            case VkFormat::VK_FORMAT_R8_SNORM: return ImageFormat::SRed8;
            case VkFormat::VK_FORMAT_R8G8_SNORM: return ImageFormat::SRed8Green8;
            case VkFormat::VK_FORMAT_R8G8B8_SNORM: return ImageFormat::SRed8Green8Blue8;
            case VkFormat::VK_FORMAT_R8G8B8A8_SNORM: return ImageFormat::SRed8Green8Blue8Alpha8;
            case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT: return ImageFormat::SDepthFloat24StencilUint8;

            case VkFormat::VK_FORMAT_B8G8R8_UNORM: return ImageFormat::UBlue8Green8Red8;
            case VkFormat::VK_FORMAT_B8G8R8A8_UNORM: return ImageFormat::UBlue8Green8Red8Alpha8;

            default: break;
        }
        throw Exception("Provided Culkan image format is not supported by the Vulkan renderer.");
    }

    static VkImageLayout GetVulkanImageLayout(const TextureType type, const TextureUsage usage)
    {
        switch (type)
        {
            case TextureType::Color:
            {
                switch (usage)
                {
                    case TextureUsage::ReadOnly: return VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    case TextureUsage::Attachment: return VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                }
            }
            case TextureType::DepthStencil:
            {
                switch (usage)
                {
                    case TextureUsage::ReadOnly: return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                    case TextureUsage::Attachment: return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                }
            }
        }

        return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
    }

    static bool GetVulkanVertexAttributeFormat(const Shader::VariableDataType format, VkFormat & vulkanFormat, uint32_t & formatSize)
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

    /*static VkIndexType GetIndexBufferDataType(const IndexBuffer::DataType dataType)
    {
        switch (dataType)
        {
            case IndexBuffer::DataType::Uint16: return VkIndexType::VK_INDEX_TYPE_UINT16;
            case IndexBuffer::DataType::Uint32: return VkIndexType::VK_INDEX_TYPE_UINT32;
        }
        
        throw Exception("Provided data type is not supported as index buffer data type by the Vulkan renderer.");
    }*/
  
    static VkDescriptorType GetVulkanDescriptorType(const DescriptorBindingType bindingType)
    {
        switch(bindingType)
        {
            case DescriptorBindingType::Sampler1D:
            case DescriptorBindingType::Sampler2D:
            case DescriptorBindingType::Sampler3D: return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case DescriptorBindingType::UniformBuffer: return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
        throw Exception("Provided binding type is not handled.");
    }

    static VkFilter GetVulkanSamplerFilter(const SamplerFilter samplerFilter)
    {
        switch (samplerFilter)
        {
            case SamplerFilter::Nearest: return VkFilter::VK_FILTER_NEAREST;
            case SamplerFilter::Linear: return VkFilter::VK_FILTER_NEAREST;
        }
        throw Exception("Provided sampler filter is not handled.");
    }

    static VkSamplerAddressMode GetVulkanSamplerAddressMode(const SamplerWrapMode samplerWrapMode)
    {
        switch (samplerWrapMode)
        {
            case SamplerWrapMode::Repeat: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case SamplerWrapMode::RepeatMirror: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case SamplerWrapMode::Clamp: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE; // Or to border??
        }
        throw Exception("Provided sampler wrap mode is not handled.");
    }

    static VkBlendFactor GetVulkanBlendFactor(const Pipeline::BlendFunction blendFunction)
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

    static VkBlendOp GetVulkanBlendOperator(const Pipeline::BlendOperator blendOperator)
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

    static VkComponentSwizzle GetVulkanComponentSwizzle(const ImageComponentSwizzle componentswizzle)
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

    static VkComponentMapping GetVulkanComponentMappings(const ImageSwizzleMapping swizzleMapping)
    {
        return {
            GetVulkanComponentSwizzle(swizzleMapping.red),
            GetVulkanComponentSwizzle(swizzleMapping.green),
            GetVulkanComponentSwizzle(swizzleMapping.blue),
            GetVulkanComponentSwizzle(swizzleMapping.alpha)
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
        m_memoryAllocator{},
        m_resourceDestroyer(m_logicalDevice, m_memoryAllocator),
        m_swapChain{},
        m_swapChainRenderPass{},
        m_surfaceFormat{},
        m_presentMode(VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR),
        m_commandPool(VK_NULL_HANDLE),
        m_drawFrameCount(0),
        m_drawingFrame(false),
        m_recordedRenderPasses{}
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
            LoadMemoryAllocator() &&
            //LoadRenderPass() &&
            LoadSwapChain() &&
            //LoadSwapChainRenderPass() &&
            LoadCommandPool();

        m_isOpen = loaded;
        return loaded;   
    }

    void VulkanRenderer::Close()
    {
        if(m_swapChainRenderPass)
        {
            if(!m_swapChainRenderPass->m_attachments.empty())
            {
                auto& attachment = m_swapChainRenderPass->m_attachments[0];
                auto& texture = attachment.texture;

                for(auto& textureFrame : texture->frames)
                {
                    textureFrame.deviceImage.image = VK_NULL_HANDLE;
                }

                m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), *texture);

                m_swapChainRenderPass->m_attachments.clear();
            }

            m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), *m_swapChainRenderPass);
            m_swapChainRenderPass.reset();
        }

        m_resourceDestroyer.ProcessAll();
        m_memoryAllocator.Destroy();

        if (m_logicalDevice.IsCreated())
        {
            m_logicalDevice.WaitIdle();

            if (m_commandPool)
            {
                vkDestroyCommandPool(m_logicalDevice.GetHandle(), m_commandPool, nullptr);
                m_commandPool = VK_NULL_HANDLE;
            }

            m_swapChain.Destroy();  
        }

        m_logicalDevice.Destroy();
        m_surface.Destroy();  
        m_instance.Destroy();

        m_renderTarget = nullptr;
        m_version = {};
        m_logger = nullptr;
        m_requiredInstanceExtensions = {};
        m_requiredInstanceLayers = {};
        m_requiredDeviceExtensions = {};
        m_isOpen = false;
        m_debugInstanceLayers = {};
        m_physicalDevice = {};
        m_logicalDevice = {};
        m_swapChain = {};
        m_drawFrameCount = 0;
        m_drawingFrame = false;
    }

    bool VulkanRenderer::IsOpen() const
    {
        return m_isOpen;
    }

    void VulkanRenderer::Resize(const Vector2ui32& /*size*/)
    {
        /*if (m_drawingFrame)
        {
            Logger::WriteError(m_logger, "Cannot resize renderer while drawing frame.");
            return;
        }

        auto swapChainExtent = m_swapChain.GetExtent();
        if (size == Vector2ui32{ swapChainExtent.width, swapChainExtent.height })
        {
            return;
        }

        swapChainExtent.width = size.x;
        swapChainExtent.height = size.y;
        m_swapChain.SetExtent(swapChainExtent);*/
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
        auto& vulkanPipeline = static_cast<VulkanPipeline&>(pipeline);
        auto& locations = vulkanPipeline.shaderProgram->pushConstantLocations;
        const auto it = locations.find(id);
        if (it == locations.end())
        {
            return PushConstantLocation::UnknownLocation;
        }

        return it->second.location;
    }

    SharedRenderResource<RenderPass> VulkanRenderer::GetSwapChainRenderPass()
    {
        return m_swapChainRenderPass;
    }

    RenderResource<DescriptorSet> VulkanRenderer::CreateDescriptorSet(const DescriptorSetDescriptor& descriptor)
    {
        auto* vulkanPipeline = static_cast<VulkanPipeline*>(descriptor.pipeline->get());

        const auto& mappedSets = vulkanPipeline->shaderProgram->mappedDescriptorSets;
        auto mappedSetIt = mappedSets.find(descriptor.id);
        if(mappedSetIt == mappedSets.end())
        {
            Logger::WriteError(m_logger, "Failed to find mapped descriptor set of id " + std::to_string(descriptor.id) + ".");
            return {};
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
                return {};
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
                    const auto bufferHandle = static_cast<VulkanUniformBuffer*>(bindingData->get())->deviceBuffer.buffer;
                    writeBufferInfo(write, bufferHandle);
                }
                else if constexpr (std::is_same_v<T, CombinedTextureSampler1D>)
                {
                    const auto imageView = static_cast<VulkanTexture1D*>(bindingData.texture.get())->imageView;
                    const auto sampler = static_cast<VulkanSampler1D*>(bindingData.sampler.get())->imageSampler.GetHandle();
                    writeImageInfo(write, sampler, imageView);
                }
                else if constexpr (std::is_same_v<T, CombinedTextureSampler2D>)
                {
                    const auto imageView = static_cast<VulkanTexture2D*>(bindingData.texture.get())->imageView;
                    const auto sampler = static_cast<VulkanSampler2D*>(bindingData.sampler.get())->imageSampler.GetHandle();
                    writeImageInfo(write, sampler, imageView);
                }
                else if constexpr (std::is_same_v<T, CombinedTextureSampler3D>)
                {
                    const auto imageView = static_cast<VulkanTexture3D*>(bindingData.texture.get())->imageView;
                    const auto sampler = static_cast<VulkanSampler3D*>(bindingData.sampler.get())->imageSampler.GetHandle();
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
        SmartFunction destroyer([&]()
        {
            if(descriptorPool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(m_logicalDevice.GetHandle(), descriptorPool, nullptr);
            }
        });
 
        if (vkCreateDescriptorPool(m_logicalDevice.GetHandle(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor pool.");
            return {};
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
            return {};
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
        }, RenderResourceDeleter{ this });
    }

    RenderResource<FramedDescriptorSet> VulkanRenderer::CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& descriptor)
    {
        auto* vulkanPipeline = static_cast<VulkanPipeline*>(descriptor.pipeline->get());

        const auto& mappedSets = vulkanPipeline->shaderProgram->mappedDescriptorSets;
        auto mappedSetIt = mappedSets.find(descriptor.id);
        if (mappedSetIt == mappedSets.end())
        {
            Logger::WriteError(m_logger, "Failed to find mapped descriptor set of id " + std::to_string(descriptor.id) + ".");
            return {};
        }

        const auto& mappedSet = mappedSetIt->second;
        const uint32_t setIndex = mappedSet.index;

        std::vector<std::unique_ptr<VkDescriptorBufferInfo[]>> bufferInfos;
        std::vector<std::unique_ptr<VkDescriptorImageInfo[]>> imageInfos;
        std::vector<VkDescriptorPoolSize> poolSizes;
        const size_t swapChainImageCount = m_swapChain.GetImages().size();  
    
        const auto& bindings = descriptor.bindings;
        auto setWrites = std::make_unique<std::vector<VkWriteDescriptorSet>[]>(swapChainImageCount);
        for (size_t i = 0; i < swapChainImageCount; i++)
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
                bufferInfo[i].buffer = buffers[i].buffer;
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
            AddBindingToPool(descriptorType, static_cast<uint32_t>(swapChainImageCount));
        };

        auto writeImageInfos = [&](
            SetWritesType& writes,
            const size_t writesIndex,
            const uint32_t bindingIndex,
            VkSampler sampler,
            const VulkanTextureFrames& textureFrames)
        {
            auto imageInfo = std::make_unique<VkDescriptorImageInfo[]>(swapChainImageCount);
            const VkDescriptorType descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

            for (size_t i = 0; i < swapChainImageCount; i++)
            {
                imageInfo[i].sampler = sampler;
                imageInfo[i].imageView = textureFrames[i].imageView;
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
            AddBindingToPool(descriptorType, static_cast<uint32_t>(swapChainImageCount));
        };

        size_t writeIndex = 0;
        const auto& mappedBindings = mappedSet.bindings;
        for (const auto& binding : bindings)
        {
            auto mappedBindingIt = mappedBindings.find(binding.id);
            if (mappedBindingIt == mappedBindings.end())
            {
                Logger::WriteError(m_logger, "Failed to find mapped descriptor binding of id " + std::to_string(binding.id) + ".");
                return {};
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
                else if constexpr (std::is_same_v<T, CombinedFramedTextureSampler1D>)
                {
                    const auto texture = static_cast<VulkanFramedTexture1D*>(bindingData.framedTexture.get());
                    const auto sampler = static_cast<VulkanSampler1D*>(bindingData.sampler.get())->imageSampler.GetHandle();
                    writeImageInfos(setWrites, writeIndex, bindingIndex, sampler, texture->frames);
                }
                else if constexpr (std::is_same_v<T, CombinedFramedTextureSampler2D>)
                {
                    const auto texture = static_cast<VulkanFramedTexture2D*>(bindingData.framedTexture.get());
                    const auto sampler = static_cast<VulkanSampler2D*>(bindingData.sampler.get())->imageSampler.GetHandle();
                    writeImageInfos(setWrites, writeIndex, bindingIndex, sampler, texture->frames);
                }
                else if constexpr (std::is_same_v<T, CombinedFramedTextureSampler3D>)
                {
                    const auto texture = static_cast<VulkanFramedTexture3D*>(bindingData.framedTexture.get());
                    const auto sampler = static_cast<VulkanSampler3D*>(bindingData.sampler.get())->imageSampler.GetHandle();
                    writeImageInfos(setWrites, writeIndex, bindingIndex, sampler, texture->frames);
                }
                else
                {
                    static_assert(false, "Unknown binding type.");
                }

            }, binding.binding);

            if(!successfulVisit)
            {
                return {};
            }

            writeIndex++;
        }

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImageCount);
        poolInfo.flags = 0;

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        SmartFunction destroyer([&]()
        {
            if (descriptorPool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(m_logicalDevice.GetHandle(), descriptorPool, nullptr);
            }
        });

        if (vkCreateDescriptorPool(m_logicalDevice.GetHandle(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor pool.");
            return {};
        }

        Vulkan::DescriptorSetLayouts setLayouts(swapChainImageCount, vulkanPipeline->descriptionSetLayouts[setIndex]);
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImageCount);
        allocInfo.pSetLayouts = setLayouts.data();

        Vulkan::DescriptorSets descriptorSets(swapChainImageCount, VK_NULL_HANDLE);
        if (vkAllocateDescriptorSets(m_logicalDevice.GetHandle(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor sets.");
            return {};
        }

        for(size_t i = 0; i < swapChainImageCount; i++)
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
        }, RenderResourceDeleter{ this });
    }

    RenderResource<IndexBuffer> VulkanRenderer::CreateIndexBuffer(const IndexBufferDescriptor& descriptor)
    {
        const auto bufferSize = static_cast<VkDeviceSize>(descriptor.indexCount) * GetIndexBufferDataTypeSize(descriptor.dataType);

        Vulkan::DeviceBuffer stagingBuffer;
        Vulkan::DeviceBufferGuard stagingBufferGuard(m_memoryAllocator, stagingBuffer);

        if (const auto result = m_memoryAllocator.CreateDeviceBuffer(
            stagingBuffer,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for index buffer");
            return {};
        }

        if (const auto result = Vulkan::MapMemory(m_logicalDevice, stagingBuffer, descriptor.data, bufferSize, 0); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for index buffer");
            return {};
        }

        Vulkan::DeviceBuffer indexBuffer;
        Vulkan::DeviceBufferGuard indexBufferGuard(m_memoryAllocator, indexBuffer);

        if (const auto result = m_memoryAllocator.CreateDeviceBuffer(
            indexBuffer,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create index buffer");
            return {};
        }

        if (const auto result = Vulkan::CopyMemory(m_logicalDevice, m_commandPool, stagingBuffer, indexBuffer, bufferSize); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to copy from staging buffer to index buffer");
            return {};
        }

        indexBufferGuard.Release();

        return RenderResource<IndexBuffer>(new VulkanIndexBuffer{
            std::move(indexBuffer),
            descriptor.indexCount,
            descriptor.dataType
        }, RenderResourceDeleter{ this });
    }

    RenderResource<Pipeline> VulkanRenderer::CreatePipeline(const PipelineDescriptor& descriptor)
    {
        auto vulkanShaderProgram = std::static_pointer_cast<VulkanShaderProgram>(descriptor.shaderProgram);
        if(!vulkanShaderProgram)
        {
            Logger::WriteError(m_logger, "Cannot create a pipeline without a shader program.");
            return {};
        }

        auto vulkanRenderPass = std::static_pointer_cast<VulkanRenderPass>(descriptor.renderPass);
        if (!vulkanRenderPass)
        {
            Logger::WriteError(m_logger, "Cannot create a pipeline without a render pass.");
            return {};
        }

        Vulkan::DescriptorSetLayouts setLayouts;

        SmartFunction destroyer([&]()
        {
            for(auto& setLayout : setLayouts)
            {
                vkDestroyDescriptorSetLayout(m_logicalDevice.GetHandle(), setLayout, nullptr);
            }
        });

        if (!CreateDescriptorSetLayouts(setLayouts, vulkanShaderProgram->mappedDescriptorSets))
        {
            return {};
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = vulkanShaderProgram->vertexBindingDescription.get();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vulkanShaderProgram->vertexInputAttributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = vulkanShaderProgram->vertexInputAttributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
        inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateInfo.topology = GetVulkanPrimitiveTopology(descriptor.topology);
        inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportStateInfo = {};
        viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.pViewports = nullptr; // Using dynamic state, set in VulkanRenderPass::Record(...).
        viewportStateInfo.scissorCount = 1;
        viewportStateInfo.pScissors = nullptr; // Using dynamic state, set in VulkanRenderPass::Record(...).

        VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
        rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerInfo.depthClampEnable = VK_FALSE;
        rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerInfo.polygonMode = GetVulkanPolygonMode(descriptor.polygonMode);
        rasterizerInfo.lineWidth = 1.0f;
        rasterizerInfo.cullMode = GetVulkanCullMode(descriptor.cullMode);
        rasterizerInfo.frontFace = GetVulkanFrontFace(descriptor.frontFace);
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
                
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
        if(vulkanRenderPass->m_hasDepthStencilAttachment)
        {
            depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilInfo.depthTestEnable = VK_TRUE;
            depthStencilInfo.depthWriteEnable = VK_TRUE;
            depthStencilInfo.depthCompareOp = VkCompareOp::VK_COMPARE_OP_LESS;
            depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
            depthStencilInfo.stencilTestEnable = VK_FALSE;
        }

        auto& blendingDescriptor = descriptor.blending;
        const auto blendOperator = GetVulkanBlendOperator(blendingDescriptor.blendOperator);

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
        colorBlendAttachmentState.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.blendEnable = VK_TRUE;
        colorBlendAttachmentState.srcColorBlendFactor = GetVulkanBlendFactor(blendingDescriptor.sourceColor);
        colorBlendAttachmentState.dstColorBlendFactor = GetVulkanBlendFactor(blendingDescriptor.destinationColor);
        colorBlendAttachmentState.colorBlendOp = blendOperator;
        colorBlendAttachmentState.srcAlphaBlendFactor = GetVulkanBlendFactor(blendingDescriptor.sourceAlpha);
        colorBlendAttachmentState.dstAlphaBlendFactor = GetVulkanBlendFactor(blendingDescriptor.destinationAlpha);
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
        pipelineLayoutInfo.pushConstantRangeCount = (vulkanShaderProgram->pushConstantRange.size > 0) ? 1 : 0;
        pipelineLayoutInfo.pPushConstantRanges = (vulkanShaderProgram->pushConstantRange.size > 0) ? &vulkanShaderProgram->pushConstantRange : nullptr;

        VkPipelineLayout pipelineLayout;
        if (const auto result = vkCreatePipelineLayout(m_logicalDevice.GetHandle(), &pipelineLayoutInfo, nullptr, &pipelineLayout); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create pipeline layout.");
            return {};
        }

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(vulkanShaderProgram->pipelineShaderStageCreateInfos.size());
        pipelineInfo.pStages = vulkanShaderProgram->pipelineShaderStageCreateInfos.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;
        pipelineInfo.pViewportState = &viewportStateInfo;
        pipelineInfo.pRasterizationState = &rasterizerInfo;
        pipelineInfo.pMultisampleState = &multisamplingInfo;
        pipelineInfo.pDepthStencilState = vulkanRenderPass->m_hasDepthStencilAttachment ? &depthStencilInfo : nullptr;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = vulkanRenderPass->m_renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        VkPipeline graphicsPipeline;
        if (const auto result = vkCreateGraphicsPipelines(
            m_logicalDevice.GetHandle(),
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &graphicsPipeline); result != VK_SUCCESS)
        {
            vkDestroyPipelineLayout(m_logicalDevice.GetHandle(), pipelineLayout, nullptr);
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create pipeline.");
            return {};
        }

        destroyer.Release();

        return RenderResource<Pipeline>(new VulkanPipeline{
            graphicsPipeline,
            pipelineLayout,
            std::move(setLayouts),
            vulkanShaderProgram
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<RenderPass> VulkanRenderer::CreateRenderPass(const RenderPassDescriptor& descriptor)
    {
        const auto logicalDevice = m_logicalDevice.GetHandle();
        const auto& swapChainImages = m_swapChain.GetImages();

        auto commandPool = VkCommandPool{ VK_NULL_HANDLE };
        auto renderPass = VkRenderPass{ VK_NULL_HANDLE };
        auto renderPassFrames = VulkanRenderPassFrames(swapChainImages.size());

        auto vulkanAttachments = VulkanRenderPassAttachments{};
        vulkanAttachments.reserve(descriptor.attachments.size());

        SmartFunction destroyer([&]()
        {
            for (auto& renderPassFrame : renderPassFrames)
            {
                if (renderPassFrame.framebuffer)
                {
                    vkDestroyFramebuffer(logicalDevice, renderPassFrame.framebuffer, nullptr);
                }

                if (renderPassFrame.finishSemaphore)
                {
                    vkDestroySemaphore(logicalDevice, renderPassFrame.finishSemaphore, nullptr);
                }
            }

            if (commandPool)
            {
                vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
            }

            if (renderPass)
            {
                vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
            }
        });

        // Create command pool.
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = m_physicalDevice.GetDeviceQueueIndices().graphicsQueue.value();
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (const auto result = vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &commandPool); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create command pool");
            return {};
        }

        // Create command buffers.
        Vulkan::CommandBuffers commandBuffers(renderPassFrames.size(), nullptr);

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (const auto result = vkAllocateCommandBuffers(logicalDevice, &commandBufferInfo, commandBuffers.data()); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to allocate command buffers");
            return {};
        }

        for (size_t i = 0; i < renderPassFrames.size(); i++)
        {
            renderPassFrames[i].commandBuffer = commandBuffers[i];
        }

        // Create finish semaphores
        Vulkan::Semaphores semaphores;
        Vulkan::CreateSemaphores(semaphores, m_logicalDevice.GetHandle(), renderPassFrames.size());

        for (size_t i = 0; i < renderPassFrames.size(); i++)
        {
            renderPassFrames[i].finishSemaphore = semaphores[i];
        }

        // Construct attachment descriptors.
        std::vector<VkAttachmentDescription> attachmentDescriptors(descriptor.attachments.size(), VkAttachmentDescription{});
        std::vector<VkAttachmentReference> colorAttachmentReferences;
        std::vector<VkAttachmentReference> depthStencilAttachmentReferences;
        std::vector<std::vector<VkImageView>> framedAttachmentImageViews(swapChainImages.size());
        std::vector<VkClearValue> clearValues(descriptor.attachments.size(), VkClearValue{});

        for(size_t i = 0; i < descriptor.attachments.size(); i++)
        {
            auto& attachment = descriptor.attachments[i];
            const auto& vulkanTexture = std::dynamic_pointer_cast<VulkanFramedTexture2D>(attachment.texture);
            const auto imageFormat = vulkanTexture->GetFormat();

            const auto initialLayout = GetVulkanImageLayout(attachment.initialType, attachment.initialUsage);
            const auto finalTypeValue = attachment.finalType.value_or(attachment.initialType);
            const auto finalUsageValue = attachment.finalUsage.value_or(attachment.initialUsage);
            const auto finalLayout = GetVulkanImageLayout(finalTypeValue, finalUsageValue);

            vulkanAttachments.emplace_back(vulkanTexture, attachment.initialType, initialLayout, finalLayout);

            for (size_t j = 0; j < framedAttachmentImageViews.size(); j++)
            {
                auto& attachmentImageViews = framedAttachmentImageViews[j];
                auto& vulkanTextureFrame = vulkanTexture->frames[j];
                attachmentImageViews.push_back(vulkanTextureFrame.imageView);
            }

            uint8_t bytesPerPixel = 0;
            const auto vulkanImageFormat = GetVulkanImageFormat(imageFormat, bytesPerPixel);

    
            VkAttachmentLoadOp loadOp;
            if(attachment.clearValue.index() != 0)
            {
                loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

                if(attachment.initialType == TextureType::Color)
                {
                    if(const auto clearValueIndex = attachment.clearValue.index(); clearValueIndex != 1)
                    {
                        Logger::WriteError(m_logger, "Clear value for render pass attachment is of type " + 
                            std::to_string(clearValueIndex) + " , but expects 1");
                        return {};
                    }

                    const auto& clearValue = std::get<1>(attachment.clearValue);
                    clearValues[i].color = { { clearValue.color.x, clearValue.color.y, clearValue.color.z, clearValue.color.w } };
                }
                else
                {
                    if(const auto clearValueIndex = attachment.clearValue.index(); clearValueIndex != 2)
                    {
                        Logger::WriteError(m_logger, "Clear value for render pass attachment is of type " +
                            std::to_string(clearValueIndex) + " , but expects 2");
                        return {};
                    }

                    const auto& clearValue = std::get<2>(attachment.clearValue);
                    clearValues[i].depthStencil = { clearValue.depth, clearValue.stencil };
                }
            }
            else
            {
                loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            }

            auto& attachmentDescriptor = attachmentDescriptors[i];
            attachmentDescriptor.format = vulkanImageFormat;
            attachmentDescriptor.samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescriptor.loadOp = loadOp;
            attachmentDescriptor.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // TODO: Need custom check here?
            attachmentDescriptor.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // TODO: Need custom check here?
            attachmentDescriptor.initialLayout = initialLayout;

            if (attachment.initialType == TextureType::Color)
            {
                attachmentDescriptor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachmentDescriptor.finalLayout = finalLayout;

                colorAttachmentReferences.emplace_back();
                auto& colorAttachmentReference = colorAttachmentReferences.back();
                colorAttachmentReference.attachment = static_cast<uint32_t>(i);
                colorAttachmentReference.layout = initialLayout;
            }
            else if (attachment.initialType == TextureType::DepthStencil)
            {
                if(!depthStencilAttachmentReferences.empty())
                {
                    Logger::WriteError(m_logger, "Multiple depth stencil attachments(" + std::to_string(i) + ") is not supported.");
                    return {};
                }

                attachmentDescriptor.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDescriptor.finalLayout = finalLayout;

                depthStencilAttachmentReferences.emplace_back();
                auto& depthStencilAttachmentReference = depthStencilAttachmentReferences.back();
                depthStencilAttachmentReference.attachment = static_cast<uint32_t>(i);
                depthStencilAttachmentReference.layout = initialLayout;
            }
            else
            {
                Logger::WriteError(m_logger, "Invalid attachment initial type(" + 
                    std::to_string(static_cast<size_t>(attachment.initialType)) + ") provided to render pass creation.");
                return {};
            }
        }

        // Constructor subpass.
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
        subpass.pColorAttachments = !colorAttachmentReferences.empty() ? colorAttachmentReferences.data() : nullptr;
        subpass.pDepthStencilAttachment = !depthStencilAttachmentReferences.empty() ? depthStencilAttachmentReferences.data() : nullptr;

        VkSubpassDependency subpassDependency = {};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        if(!depthStencilAttachmentReferences.empty())
        {
            subpassDependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            subpassDependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            subpassDependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }

        // Create render pass.
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptors.size());
        renderPassInfo.pAttachments = attachmentDescriptors.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &subpassDependency;

        if (const auto result = vkCreateRenderPass(m_logicalDevice.GetHandle(), &renderPassInfo, nullptr, &renderPass); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create internal render pass object");
            return {};
        }

        // Create framebuffers
        for (size_t i = 0; i < renderPassFrames.size(); i++)
        {
            auto& renderPassFrame = renderPassFrames[i];
            auto& attachmentImageViews = framedAttachmentImageViews[i];

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachmentImageViews.size());
            framebufferInfo.pAttachments = attachmentImageViews.data();
            framebufferInfo.width = descriptor.dimensions.x;
            framebufferInfo.height = descriptor.dimensions.y;
            framebufferInfo.layers = 1;

            if (const auto result = vkCreateFramebuffer(m_logicalDevice.GetHandle(), &framebufferInfo, nullptr, &renderPassFrame.framebuffer); result != VK_SUCCESS)
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create framebuffer object for render pass");
                return {};
            }
        }

        // Finalize.
        destroyer.Release();

        return SharedRenderResource<VulkanRenderPass>(new VulkanRenderPass{
            m_logger,
            renderPass,
            commandPool,
            descriptor.dimensions,
            std::move(renderPassFrames),
            std::move(vulkanAttachments),
            !depthStencilAttachmentReferences.empty(),
            std::move(clearValues),
            nullptr
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<Sampler1D> VulkanRenderer::CreateSampler(const SamplerDescriptor1D& descriptor)
    {
        const auto magFilter = GetVulkanSamplerFilter(descriptor.magFilter);
        const auto minFilter = GetVulkanSamplerFilter(descriptor.minFilter);
        const auto addressModeU = GetVulkanSamplerAddressMode(descriptor.wrapModes.c[0]);
        const auto addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        const auto addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        const bool anisotropyEnable = descriptor.maxAnisotropy > 1;
        const auto maxAnisotropy = static_cast<float>(descriptor.maxAnisotropy);

        Vulkan::ImageSampler imageSampler;
        if (const auto result = imageSampler.Create(
            m_logicalDevice,
            magFilter,
            minFilter,
            addressModeU,
            addressModeV,
            addressModeW,
            anisotropyEnable,
            maxAnisotropy); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create 1D image sampler");
            return {};
        }

        return SharedRenderResource<Sampler1D>(new VulkanSampler1D{
            std::move(imageSampler)
         }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<Sampler2D> VulkanRenderer::CreateSampler(const SamplerDescriptor2D& descriptor)
    {
        const auto magFilter = GetVulkanSamplerFilter(descriptor.magFilter);
        const auto minFilter = GetVulkanSamplerFilter(descriptor.minFilter);
        const auto addressModeU = GetVulkanSamplerAddressMode(descriptor.wrapModes.x);
        const auto addressModeV = GetVulkanSamplerAddressMode(descriptor.wrapModes.y);
        const auto addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        const bool anisotropyEnable = descriptor.maxAnisotropy > 1;
        const auto maxAnisotropy = static_cast<float>(descriptor.maxAnisotropy);

        Vulkan::ImageSampler imageSampler;
        if (const auto result = imageSampler.Create(
            m_logicalDevice,
            magFilter,
            minFilter,
            addressModeU,
            addressModeV,
            addressModeW,
            anisotropyEnable,
            maxAnisotropy); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create 2D image sampler");
            return {};
        }

        return SharedRenderResource<Sampler2D>(new VulkanSampler2D{
            std::move(imageSampler)
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<Sampler3D> VulkanRenderer::CreateSampler(const SamplerDescriptor3D& descriptor)
    {
        const auto magFilter = GetVulkanSamplerFilter(descriptor.magFilter);
        const auto minFilter = GetVulkanSamplerFilter(descriptor.minFilter);
        const auto addressModeU = GetVulkanSamplerAddressMode(descriptor.wrapModes.x);
        const auto addressModeV = GetVulkanSamplerAddressMode(descriptor.wrapModes.y);
        const auto addressModeW = GetVulkanSamplerAddressMode(descriptor.wrapModes.x);
        const bool anisotropyEnable = descriptor.maxAnisotropy > 1;
        const auto maxAnisotropy = static_cast<float>(descriptor.maxAnisotropy);

        Vulkan::ImageSampler imageSampler;
        if (const auto result = imageSampler.Create(
            m_logicalDevice,
            magFilter,
            minFilter,
            addressModeU,
            addressModeV,
            addressModeW,
            anisotropyEnable,
            maxAnisotropy); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create 3D image sampler");
            return {};
        }

        return SharedRenderResource<Sampler3D>(new VulkanSampler3D{
            std::move(imageSampler)
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<ShaderProgram> VulkanRenderer::CreateShaderProgram(const VisualShaderProgramDescriptor& descriptor)
    {
        const auto* vertexScript = descriptor.vertexScript;
        if (vertexScript == nullptr)
        {
            Logger::WriteError(m_logger, "Cannot create shader program without vertex script.");
            return {};
        }

        const auto* fragmentScript = descriptor.fragmentScript;
        if (fragmentScript == nullptr)
        {
            Logger::WriteError(m_logger, "Cannot create shader program without fragment script.");
            return {};
        }

        if (!vertexScript->GetOutputInterface().CompareStructure(fragmentScript->GetInputInterface()))
        {
            Logger::WriteError(m_logger, "Vertex output structure is not compatible with fragment input structure.");
            return {};
        }

        Vulkan::ShaderModules shaderModules(2, VK_NULL_HANDLE);
        Vulkan::PipelineShaderStageCreateInfos pipelineShaderStageCreateInfos(2);

        SmartFunction destroyer([&]()
        {
            for (auto& shaderModule : shaderModules)
            {
                if (shaderModule)
                {
                    vkDestroyShaderModule(m_logicalDevice.GetHandle(), shaderModule, nullptr);
                }
            }
        });

        Shader::SpirvCombinedShadersTemplate spirvTemplate = {};
        if(!Shader::SpirvGenerator::CreateCombinedShaderTemplate(spirvTemplate, { descriptor.fragmentScript, descriptor.vertexScript }, m_logger))
        {
            return {};
        }

        Shader::SpirvGenerator spirvGenerator(m_logger);
        std::vector<std::pair<Shader::Type, Shader::SpirvGeneratoResult>> shaderSources;  

        auto generateSource = [&](const auto* script, const std::vector<size_t>& ignoredOutputIndices, const bool ignoreUnusedInputs)
        {
            const Shader::SpirvGeneratorDescriptor spirvDescriptor = { script, ignoredOutputIndices, &spirvTemplate, ignoreUnusedInputs, true };
            auto spirvResult = spirvGenerator.Generate(spirvDescriptor);

            if (spirvResult.source.empty())
            {
                return false;
            }

            shaderSources.emplace_back(script->GetType(), std::move(spirvResult));
            return true;
        };

        if (!generateSource(fragmentScript, {}, true))
        {
            Logger::WriteError(m_logger, "Failed to generate SPIR-V code for Vertex stage. ");
            return {};
        }

        if (!generateSource(vertexScript, shaderSources.back().second.ignoredInputIndices, false))
        {
            Logger::WriteError(m_logger, "Failed to generate SPIR-V code for Vertex stage. ");
            return {};
        }

        for (size_t i = 0; i < shaderSources.size(); i++)
        {
            const auto shaderStage = shaderSources[i].first;
            auto& spirvResult = shaderSources[i].second;

            VkShaderModuleCreateInfo shaderModuleInfo = {};
            shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModuleInfo.codeSize = spirvResult.source.size() * sizeof(Shader::Spirv::Word);
            shaderModuleInfo.pCode = spirvResult.source.data();

            auto& shaderModule = shaderModules[i];
            if (const auto result = vkCreateShaderModule(m_logicalDevice.GetHandle(), &shaderModuleInfo, nullptr, &shaderModule); result != VK_SUCCESS)
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed create shader module");
                return {};
            }

            auto& stageCreateInfo = pipelineShaderStageCreateInfos[i];
            stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stageCreateInfo.pName = "main";
            stageCreateInfo.module = shaderModule;
            stageCreateInfo.stage = GetVulkanShaderProgramStageFlag(shaderStage);
        }

        // Push constant information.
        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
        pushConstantRange.offset = 0;
        pushConstantRange.size = spirvTemplate.pushConstantBlockByteCount;

        // Vertex input information.
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
        auto vertexBindingDescription = std::make_unique<VkVertexInputBindingDescription>();
        if (descriptor.vertexScript)
        {
            auto& vertexInputs = descriptor.vertexScript->GetInputInterface();
            uint32_t vertexBindingStride = 0;
            if (!CreateVertexInputAttributes(vertexInputs, vertexInputAttributeDescriptions, vertexBindingStride))
            {
                return {};
            }

            vertexBindingDescription->binding = 0;
            vertexBindingDescription->stride = vertexBindingStride;
            vertexBindingDescription->inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
        }

        // Finalize.
        destroyer.Release();

        return SharedRenderResource<ShaderProgram>(new VulkanShaderProgram{
            std::move(shaderModules),
            std::move(pipelineShaderStageCreateInfos),
            std::move(spirvTemplate.mappedDescriptorSets),
            std::move(vertexInputAttributeDescriptions),
            std::move(vertexBindingDescription),
            std::move(spirvTemplate.pushConstantLocations),
            pushConstantRange
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<Texture1D> VulkanRenderer::CreateTexture(const TextureDescriptor1D& descriptor)
    {
        uint8_t bytesPerPixel = 0;
        uint8_t internalBytesPerPixel = 0;
        const auto imageFormat = GetVulkanImageFormat(descriptor.format, bytesPerPixel);
        const auto internalImageFormat = GetVulkanImageFormat(descriptor.internalFormat, internalBytesPerPixel);
        if (bytesPerPixel != internalBytesPerPixel)
        {
            Logger::WriteError(m_logger, "Format and internal format of texture mismatches number of bytes per pixel: " +
                std::to_string(bytesPerPixel) + " : " + std::to_string(internalBytesPerPixel));
            return {};
        }

        const auto dimensions = Vector3ui32{ descriptor.dimensions.c[0], 1, 1 };
        const auto dataSize =
            static_cast<VkDeviceSize>(descriptor.dimensions.c[0]) * static_cast<VkDeviceSize>(bytesPerPixel);

        Vulkan::DeviceImage deviceImage;
        Vulkan::DeviceImageGuard deviceImageGuard(m_memoryAllocator, deviceImage);
        VkImageView imageView = VK_NULL_HANDLE;

        if (!CreateTexture(
            deviceImage,
            imageView,
            dimensions,
            descriptor.data,
            dataSize,
            VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
            imageFormat,
            internalImageFormat,
            VulkanImageTypeTraits<1>::type,
            VulkanImageViewTypeTraits<1>::type,
            GetVulkanComponentMappings(descriptor.swizzleMapping)))
        {
            return {};
        }

        deviceImageGuard.Release();

        return SharedRenderResource<Texture1D>(new VulkanTexture1D{
            std::move(deviceImage),
            imageView,
            descriptor.format,
            descriptor.dimensions,
            bytesPerPixel
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<Texture2D> VulkanRenderer::CreateTexture(const TextureDescriptor2D& descriptor)
    {
        uint8_t bytesPerPixel = 0;
        uint8_t internalBytesPerPixel = 0;
        const auto imageFormat = GetVulkanImageFormat(descriptor.format, bytesPerPixel);
        const auto internalImageFormat = GetVulkanImageFormat(descriptor.internalFormat, internalBytesPerPixel);
        if (bytesPerPixel != internalBytesPerPixel)
        {
            Logger::WriteError(m_logger, "Format and internal format of texture mismatches number of bytes per pixel: " +
                std::to_string(bytesPerPixel) + " : " + std::to_string(internalBytesPerPixel));
            return {};
        }

        const auto dimensions = Vector3ui32{ descriptor.dimensions.x, descriptor.dimensions.y, 1 };
        const auto dataSize =
            static_cast<VkDeviceSize>(descriptor.dimensions.x) *
            static_cast<VkDeviceSize>(descriptor.dimensions.y) * static_cast<VkDeviceSize>(bytesPerPixel);

        Vulkan::DeviceImage deviceImage;
        Vulkan::DeviceImageGuard deviceImageGuard(m_memoryAllocator, deviceImage);
        VkImageView imageView = VK_NULL_HANDLE;

        if(!CreateTexture(
            deviceImage,
            imageView,
            dimensions,
            descriptor.data,
            dataSize,
            VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
            imageFormat,
            internalImageFormat,
            VulkanImageTypeTraits<2>::type,
            VulkanImageViewTypeTraits<2>::type,
            GetVulkanComponentMappings(descriptor.swizzleMapping)))
        {
            return {};
        }

        deviceImageGuard.Release();

        return SharedRenderResource<Texture2D>(new VulkanTexture2D{
            std::move(deviceImage),
            imageView,
            descriptor.format,
            descriptor.dimensions,
            bytesPerPixel
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<Texture3D> VulkanRenderer::CreateTexture(const TextureDescriptor3D& descriptor)
    {
        uint8_t bytesPerPixel = 0;
        uint8_t internalBytesPerPixel = 0;
        const auto imageFormat = GetVulkanImageFormat(descriptor.format, bytesPerPixel);
        const auto internalImageFormat = GetVulkanImageFormat(descriptor.internalFormat, internalBytesPerPixel);
        if (bytesPerPixel != internalBytesPerPixel)
        {
            Logger::WriteError(m_logger, "Format and internal format of texture mismatches number of bytes per pixel: " +
                std::to_string(bytesPerPixel) + " : " + std::to_string(internalBytesPerPixel));
            return {};
        }

        const auto dimensions = descriptor.dimensions;
        const auto dataSize =
            static_cast<VkDeviceSize>(descriptor.dimensions.x) *
            static_cast<VkDeviceSize>(descriptor.dimensions.y) *
            static_cast<VkDeviceSize>(descriptor.dimensions.z) * static_cast<VkDeviceSize>(bytesPerPixel);

        Vulkan::DeviceImage deviceImage;
        Vulkan::DeviceImageGuard deviceImageGuard(m_memoryAllocator, deviceImage);
        VkImageView imageView = VK_NULL_HANDLE;

        if (!CreateTexture(
            deviceImage,
            imageView,
            dimensions,
            descriptor.data,
            dataSize,
            VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
            imageFormat,
            internalImageFormat,
            VulkanImageTypeTraits<3>::type,
            VulkanImageViewTypeTraits<3>::type,
            GetVulkanComponentMappings(descriptor.swizzleMapping)))
        {
            return {};
        }

        deviceImageGuard.Release();

        return SharedRenderResource<Texture3D>(new VulkanTexture3D{
            std::move(deviceImage),
            imageView,
            descriptor.format,
            descriptor.dimensions,
            bytesPerPixel
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<FramedTexture1D> VulkanRenderer::CreateFramedTexture(const TextureDescriptor1D& descriptor)
    {
        uint8_t bytesPerPixel = 0;
        uint8_t internalBytesPerPixel = 0;
        const auto imageFormat = GetVulkanImageFormat(descriptor.format, bytesPerPixel);
        const auto internalImageFormat = GetVulkanImageFormat(descriptor.internalFormat, internalBytesPerPixel);
        if (bytesPerPixel != internalBytesPerPixel)
        {
            Logger::WriteError(m_logger, "Format and internal format of texture mismatches number of bytes per pixel: " +
                std::to_string(bytesPerPixel) + " : " + std::to_string(internalBytesPerPixel));
            return {};
        }

        const VkImageLayout initialLayout = GetVulkanImageLayout(descriptor.type, descriptor.initialUsage);
        if (initialLayout == VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED)
        {
            Logger::WriteError(m_logger, "Failed to find VkImageLayout for texture - initial usage: " +
                std::to_string(static_cast<size_t>(descriptor.initialUsage)) + ", type: " + std::to_string(static_cast<size_t>(descriptor.type)));
            return {};
        }

        const auto dimensions = Vector3ui32{ descriptor.dimensions.c[0], 1, 1 };
        const auto dataSize =
            static_cast<VkDeviceSize>(descriptor.dimensions.c[0]) * static_cast<VkDeviceSize>(bytesPerPixel);

        VulkanTextureFrames frames;

        if (!CreateFramedTexture(
            frames,
            dimensions,
            descriptor.data,
            dataSize,
            descriptor.type,
            initialLayout,
            imageFormat,
            internalImageFormat,
            VulkanImageTypeTraits<1>::type,
            VulkanImageViewTypeTraits<1>::type,
            GetVulkanComponentMappings(descriptor.swizzleMapping)))
        {
            return {};
        }

        return SharedRenderResource<FramedTexture1D>(new VulkanFramedTexture1D{
            std::move(frames),
            descriptor.format,
            descriptor.dimensions,
            bytesPerPixel
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<FramedTexture2D> VulkanRenderer::CreateFramedTexture(const TextureDescriptor2D& descriptor)
    {
        uint8_t bytesPerPixel = 0;
        uint8_t internalBytesPerPixel = 0;
        const auto imageFormat = GetVulkanImageFormat(descriptor.format, bytesPerPixel);
        const auto internalImageFormat = GetVulkanImageFormat(descriptor.internalFormat, internalBytesPerPixel);
        if (bytesPerPixel != internalBytesPerPixel)
        {
            Logger::WriteError(m_logger, "Format and internal format of texture mismatches number of bytes per pixel: " +
                std::to_string(bytesPerPixel) + " : " + std::to_string(internalBytesPerPixel));
            return {};
        }

        const VkImageLayout initialLayout = GetVulkanImageLayout(descriptor.type, descriptor.initialUsage);
        if (initialLayout == VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED)
        {
            Logger::WriteError(m_logger, "Failed to find VkImageLayout for texture - initial usage: " +
                std::to_string(static_cast<size_t>(descriptor.initialUsage)) + ", type: " + std::to_string(static_cast<size_t>(descriptor.type)));
            return {};
        }

        const auto dimensions = Vector3ui32{ descriptor.dimensions.x, descriptor.dimensions.y, 1 };
        const auto dataSize =
            static_cast<VkDeviceSize>(descriptor.dimensions.x) *
            static_cast<VkDeviceSize>(descriptor.dimensions.y) * static_cast<VkDeviceSize>(bytesPerPixel);

        VulkanTextureFrames frames;

        if (!CreateFramedTexture(
            frames,
            dimensions,
            descriptor.data,
            dataSize,
            descriptor.type,
            initialLayout,
            imageFormat,
            internalImageFormat,
            VulkanImageTypeTraits<2>::type,
            VulkanImageViewTypeTraits<2>::type,
            GetVulkanComponentMappings(descriptor.swizzleMapping)))
        {
            return {};
        }

        return SharedRenderResource<FramedTexture2D>(new VulkanFramedTexture2D{
            std::move(frames),
            descriptor.format,
            descriptor.dimensions,
            bytesPerPixel
        }, RenderResourceDeleter{ this });
    }

    SharedRenderResource<FramedTexture3D> VulkanRenderer::CreateFramedTexture(const TextureDescriptor3D& descriptor)
    {
        uint8_t bytesPerPixel = 0;
        uint8_t internalBytesPerPixel = 0;
        const auto imageFormat = GetVulkanImageFormat(descriptor.format, bytesPerPixel);
        const auto internalImageFormat = GetVulkanImageFormat(descriptor.internalFormat, internalBytesPerPixel);
        if (bytesPerPixel != internalBytesPerPixel)
        {
            Logger::WriteError(m_logger, "Format and internal format of texture mismatches number of bytes per pixel: " +
                std::to_string(bytesPerPixel) + " : " + std::to_string(internalBytesPerPixel));
            return {};
        }

        const VkImageLayout initialLayout = GetVulkanImageLayout(descriptor.type, descriptor.initialUsage);
        if(initialLayout == VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED)
        {
            Logger::WriteError(m_logger, "Failed to find VkImageLayout for texture - initial usage: " +
                std::to_string(static_cast<size_t>(descriptor.initialUsage)) + ", type: " + std::to_string(static_cast<size_t>(descriptor.type)));
            return {};
        }

        const auto dimensions = descriptor.dimensions;
        const auto dataSize =
            static_cast<VkDeviceSize>(descriptor.dimensions.x) *
            static_cast<VkDeviceSize>(descriptor.dimensions.y) *
            static_cast<VkDeviceSize>(descriptor.dimensions.z) * static_cast<VkDeviceSize>(bytesPerPixel);

        VulkanTextureFrames frames;

        if (!CreateFramedTexture(
            frames,
            dimensions,
            descriptor.data,
            dataSize,
            descriptor.type,
            initialLayout,
            imageFormat,
            internalImageFormat,
            VulkanImageTypeTraits<3>::type,
            VulkanImageViewTypeTraits<3>::type,
            GetVulkanComponentMappings(descriptor.swizzleMapping)))
        {
            return {};
        }

        return SharedRenderResource<FramedTexture3D>(new VulkanFramedTexture3D{
            std::move(frames),
            descriptor.format,
            descriptor.dimensions,
            bytesPerPixel
        }, RenderResourceDeleter{ this });
    }

    RenderResource<UniformBuffer> VulkanRenderer::CreateUniformBuffer(const UniformBufferDescriptor& descriptor)
    { 
        Vulkan::DeviceBuffer deviceBuffer;

        if (const auto result = m_memoryAllocator.CreateDeviceBuffer(
            deviceBuffer,
            descriptor.size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create device buffer for uniform buffer");
            return {};
        }

        return RenderResource<UniformBuffer>(new VulkanUniformBuffer{
            std::move(deviceBuffer)
        }, RenderResourceDeleter{ this });
    }

    RenderResource<FramedUniformBuffer> VulkanRenderer::CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor)
    {
        std::vector<Vulkan::DeviceBuffer> deviceBuffers(m_swapChain.GetImages().size());

        SmartFunction destroyer([&]()
        {
            for(auto& deviceBuffer : deviceBuffers)
            {
                if(deviceBuffer.IsEmpty())
                {
                    return;
                }

                m_memoryAllocator.FreeDeviceBuffer(deviceBuffer);
            }
        });

        for(auto& deviceBuffer : deviceBuffers)
        {
            if (const auto result = m_memoryAllocator.CreateDeviceBuffer(
                deviceBuffer,
                descriptor.size,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create device buffer for framed uniform buffer");
                return {};
            }
        }

        destroyer.Release();

        return RenderResource<FramedUniformBuffer>(new VulkanFramedUniformBuffer{
            std::move(deviceBuffers)
        }, RenderResourceDeleter{ this });
    }

    RenderResource<VertexBuffer> VulkanRenderer::CreateVertexBuffer(const VertexBufferDescriptor& descriptor)
    {
        const auto bufferSize = 
            static_cast<VkDeviceSize>(descriptor.vertexCount) *
            static_cast<VkDeviceSize>(descriptor.vertexSize);

        Vulkan::DeviceBuffer stagingBuffer;
        Vulkan::DeviceBufferGuard stagingBufferGuard(m_memoryAllocator, stagingBuffer);

        if(const auto result = m_memoryAllocator.CreateDeviceBuffer(
            stagingBuffer,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for vertex buffer");
            return {};
        }

        if (const auto result = Vulkan::MapMemory(m_logicalDevice, stagingBuffer, descriptor.data, bufferSize, 0); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for vertex buffer");
            return {};
        }

        Vulkan::DeviceBuffer vertexBuffer;
        Vulkan::DeviceBufferGuard vertexBufferGuard(m_memoryAllocator, stagingBuffer);

        if (const auto result = m_memoryAllocator.CreateDeviceBuffer(
            vertexBuffer,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for vertex buffer");
            return {};
        }

        if (const auto result = Vulkan::CopyMemory(m_logicalDevice, m_commandPool, stagingBuffer, vertexBuffer, bufferSize); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to copy from staging buffer to vertex buffer");
            return {};
        }

        vertexBufferGuard.Release();

        return RenderResource<VertexBuffer>(new VulkanVertexBuffer{
            std::move(vertexBuffer),
            descriptor.vertexCount,
            descriptor.vertexSize
        }, RenderResourceDeleter{ this });
    }

    bool VulkanRenderer::UpdateRenderPass(RenderPass& renderPass, const RenderPassUpdateDescriptor& descriptor)
    {
        auto& vulkanRenderPass = static_cast<VulkanRenderPass&>(renderPass);

        if(descriptor.attachments.size() != vulkanRenderPass.m_attachments.size())
        {
            Logger::WriteError(m_logger, "Cannot update render pass with mismatching attachment count: " +
                std::to_string(descriptor.attachments.size()) + " != " + std::to_string(vulkanRenderPass.m_attachments.size()) + ".");
            return false;
        }

        const auto& swapChainImages = m_swapChain.GetImages();
        std::vector<std::vector<VkImageView>> framedAttachmentImageViews(swapChainImages.size());

        for(size_t i = 0; i < vulkanRenderPass.m_attachments.size(); i++)
        {
            auto& oldAttachment = vulkanRenderPass.m_attachments[i];
            const auto& newAttachment = descriptor.attachments[i];

            const auto initialLayout = GetVulkanImageLayout(newAttachment.initialType, newAttachment.initialUsage);

            const auto finalTypeValue = newAttachment.finalType.value_or(newAttachment.initialType);
            const auto finalUsageValue = newAttachment.finalUsage.value_or(newAttachment.initialUsage);
            const auto finalLayout = GetVulkanImageLayout(finalTypeValue, finalUsageValue);

            if (initialLayout != oldAttachment.initialLayout)
            {
                Logger::WriteError(m_logger, "Initial layout of attachment(" + std::to_string(i) + ") is mismatching  in update: " +
                    std::to_string(initialLayout) + " != " + std::to_string(oldAttachment.initialLayout) + ".");
                return false;
            }

            if (finalLayout != oldAttachment.finalLayout)
            {
                Logger::WriteError(m_logger, "Final layout of attachment(" + std::to_string(i) + ") is mismatching in update: " +
                    std::to_string(finalLayout) + " != " + std::to_string(oldAttachment.finalLayout) + ".");
                return false;
            }

            if (newAttachment.initialType != oldAttachment.initialType)
            {
                Logger::WriteError(m_logger, "Initial type of attachment(" + std::to_string(i) + ") is mismatching in update: " +
                    std::to_string(static_cast<size_t>(newAttachment.initialType)) + " != " + std::to_string(static_cast<size_t>(oldAttachment.initialType)) + ".");
                return false;
            }

            const auto vulkanFramedTexture = std::dynamic_pointer_cast<VulkanFramedTexture2D>(newAttachment.texture);
            oldAttachment.texture = vulkanFramedTexture;

            for (size_t j = 0; j < framedAttachmentImageViews.size(); j++)
            {
                auto& attachmentImageViews = framedAttachmentImageViews[j];
                auto& vulkanTextureFrame = vulkanFramedTexture->frames[j];
                attachmentImageViews.push_back(vulkanTextureFrame.imageView);
            }
        }

        // Create framebuffers
        for (size_t i = 0; i < vulkanRenderPass.m_frames.size(); i++)
        {
            auto& renderPassFrame = vulkanRenderPass.m_frames[i];
            auto& attachmentImageViews = framedAttachmentImageViews[i];

            m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), renderPassFrame.framebuffer);

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = vulkanRenderPass.m_renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachmentImageViews.size());
            framebufferInfo.pAttachments = attachmentImageViews.data();
            framebufferInfo.width = descriptor.dimensions.x;
            framebufferInfo.height = descriptor.dimensions.y;
            framebufferInfo.layers = 1;

            if (const auto result = vkCreateFramebuffer(m_logicalDevice.GetHandle(), &framebufferInfo, nullptr, &renderPassFrame.framebuffer); result != VK_SUCCESS)
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create framebuffer object for render pass");
                return {};
            }
        }

        vulkanRenderPass.m_dimensions = descriptor.dimensions;

        return true;
    }

    bool VulkanRenderer::UpdateTexture(Texture1D& texture1D, const TextureUpdateDescriptor1D& descriptor)
    {
        auto& vulkanTexture = static_cast<VulkanTexture1D&>(texture1D);

        const auto destinationDimensions = Vector3ui32{
            descriptor.destinationDimensions.c[0], 1, 1
        };
        const auto destinationOffset = Vector3ui32{
            descriptor.destinationOffset.c[0], 0, 0
        };

        return UpdateTexture(
            vulkanTexture.deviceImage,
            vulkanTexture.bytesPerPixel,
            descriptor.data,
            destinationDimensions,
            destinationOffset);
    }

    bool VulkanRenderer::UpdateTexture(Texture2D& texture2D, const TextureUpdateDescriptor2D& descriptor)
    {
        auto& vulkanTexture = static_cast<VulkanTexture2D&>(texture2D);

        const auto destinationDimensions = Vector3ui32{
            descriptor.destinationDimensions.x, descriptor.destinationDimensions.y, 1
        };
        const auto destinationOffset = Vector3ui32{
            descriptor.destinationOffset.x, descriptor.destinationOffset.y, 0
        };

        return UpdateTexture(
            vulkanTexture.deviceImage,
            vulkanTexture.bytesPerPixel,
            descriptor.data,
            destinationDimensions,
            destinationOffset);
    }

    bool VulkanRenderer::UpdateTexture(Texture3D& texture3D, const TextureUpdateDescriptor3D& descriptor)
    {
        auto& vulkanTexture = static_cast<VulkanTexture3D&>(texture3D);

        return UpdateTexture(
            vulkanTexture.deviceImage,
            vulkanTexture.bytesPerPixel,
            descriptor.data,
            descriptor.destinationDimensions,
            descriptor.destinationOffset);
    }

    void VulkanRenderer::UpdateUniformBuffer(RenderResource<UniformBuffer>& uniformBuffer, const void* data, const size_t size, const size_t offset)
    {
        auto* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer.get());
        auto& deviceBuffer = vulkanUniformBuffer->deviceBuffer;

        if (const auto result = Vulkan::MapMemory(m_logicalDevice, deviceBuffer, data, size, offset); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map memory of uniform buffer");
        }
    }

    void VulkanRenderer::UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& framedUniformBuffer, const void* data, const size_t size, const size_t offset)
    {
        auto* vulkanFramedUniformBuffer = static_cast<VulkanFramedUniformBuffer*>(framedUniformBuffer.get());
        const auto currentImageIndex = m_swapChain.GetCurrentImageIndex();
        auto& deviceBuffer = vulkanFramedUniformBuffer->deviceBuffers[currentImageIndex];

        if (const auto result = Vulkan::MapMemory(m_logicalDevice, deviceBuffer, data, size, offset); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map memory of framed uniform buffer");
        }
    }

    bool VulkanRenderer::DrawFrame(const RenderPasses& renderPasses)
    {
        // Begin frame.
        if (m_drawingFrame)
        {
            Logger::WriteError(m_logger, "DrawFrame(...) failed, calling DrawFrame while current frame is being rendered is not permitted.");
            return false;
        }

        SmartFunction destroyer([&]()
        {
            m_drawingFrame = false;
        });
        m_drawingFrame = true;

        if (auto result = m_swapChain.AcquireNextImage(); !result.IsSuccessful())
        {
            if (result == VkResult::VK_ERROR_OUT_OF_DATE_KHR || result == VkResult::VK_SUBOPTIMAL_KHR) // TODO: Invalid compare here?
            {
                if (result = m_logicalDevice.GetPhysicalDevice().ReloadCapabilities(); !result.IsSuccessful())
                {
                    Vulkan::Logger::WriteError(m_logger, result, "Failed to reload physical device capabilities.");
                    return false;
                }

                if (!ReloadSwapChain())
                {
                    return false;
                }

                if (result = m_swapChain.AcquireNextImage(); !result.IsSuccessful())
                {
                    Vulkan::Logger::WriteError(m_logger, result, "Failed to acquire next image of swap chain after recreation.");
                    return false;
                }
            }
            else
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to acquire next image of swap chain.");
                return false;
            }
        }

        ++m_drawFrameCount;
        const auto currentImageIndex = m_swapChain.GetCurrentImageIndex();

        // Record render passes.
        // Multi-threaded recording is currently not supported, because attachment layouts are changed in record function,
        // this must be taken into consideration when multi-thread support is implemented.
        m_recordedRenderPasses.clear();
        for (auto& renderPass : renderPasses)
        {
            auto* vulkanRenderPass = static_cast<VulkanRenderPass*>(renderPass.get());

            if (auto result = vulkanRenderPass->Record(currentImageIndex); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to record command buffer");
                return false;
            }

            if (vulkanRenderPass->GetCommandCount() == 0)
            {
                continue;
            }

            m_recordedRenderPasses.push_back(vulkanRenderPass);
        }

        // Submit all render passes except the last one.
        auto waitSemaphore = m_swapChain.GetCurrentImageAvailableSemaphore();
        const auto renderPassSubmitCount = static_cast<int32_t>(m_recordedRenderPasses.size());

        for(int32_t i = 0; i < renderPassSubmitCount - 1; i++)
        {
            auto* vulkanRenderPass = m_recordedRenderPasses[i];

            if (auto result = vulkanRenderPass->Submit(m_logicalDevice, waitSemaphore, VK_NULL_HANDLE); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to submit command buffer");
                return false;
            }

            waitSemaphore = vulkanRenderPass->GetCurrentFrame().finishSemaphore;
        }

        // Handle last render pass and present swap chain image
        if(!m_recordedRenderPasses.empty())
        {
            auto* lastVulkanRenderPass = m_recordedRenderPasses.back();

            auto submitFence = m_swapChain.GetCurrentFrameFence();
            if (auto result = lastVulkanRenderPass->Submit(m_logicalDevice, waitSemaphore, submitFence); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to submit command buffer");
                return false;
            }

            waitSemaphore = lastVulkanRenderPass->GetCurrentFrame().finishSemaphore;

            // Present swap chain.
            if (const auto result = m_swapChain.PresentImage(waitSemaphore); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteDebug(m_logger, result, "Failed to present next image of swap chain.");
            }
        }

        // Process destroyed resources.
        m_resourceDestroyer.Process(m_drawFrameCount);

        return true;
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

    void VulkanRenderer::Destroy(RenderPass& renderPass)
    {
        auto& vulkanRenderPass = static_cast<VulkanRenderPass&>(renderPass);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanRenderPass);
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

    void VulkanRenderer::Destroy(ShaderProgram& shaderProgram)
    {
        auto& vulkanShaderProgram = static_cast<VulkanShaderProgram&>(shaderProgram);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanShaderProgram);
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

    void VulkanRenderer::Destroy(FramedTexture1D& framedTexture1D)
    {
        auto& vulkanTexture = static_cast<VulkanFramedTexture1D&>(framedTexture1D);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanTexture);
    }

    void VulkanRenderer::Destroy(FramedTexture2D& framedTexture2D)
    {
        auto& vulkanTexture = static_cast<VulkanFramedTexture2D&>(framedTexture2D);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanTexture);
    }

    void VulkanRenderer::Destroy(FramedTexture3D& framedTexture3D)
    {
        auto& vulkanTexture = static_cast<VulkanFramedTexture3D&>(framedTexture3D);
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

    void VulkanRenderer::WaitForDevice()
    {
        m_logicalDevice.WaitIdle();
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

        if (const auto result = m_instance.Create(
            vulkanVersion,
            "Molten Engine",
            MOLTEN_VERSION,
            "Molten Engine Application",
            Version(1, 0, 0),
            m_requiredInstanceExtensions,
            m_requiredInstanceLayers,
            debugCallbackDesc); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create instance(vulkan version " + ToString(vulkanVersion) + ")");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadSurface()
    {
        if (const auto result = m_surface.Create(m_instance, *m_renderTarget); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create surface");
            return false;
        }

        m_surfaceFormat = {};
        m_surfaceFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
        m_surfaceFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        return true;
    }

    bool VulkanRenderer::LoadPhysicalDevice()
    {
        Vulkan::PhysicalDevices physicalDevices;
        const auto result = Vulkan::FetchAndCreatePhysicalDevices(physicalDevices, m_instance, m_surface, {
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

        if (!result.IsSuccessful())
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
        VkPhysicalDeviceFeatures enabledDeviceFeatures = {};
        Vulkan::EnablePhysicalDeviceFeatures(enabledDeviceFeatures, m_requiredDeviceFeatures);

        auto& availableDeviceFeatures = m_physicalDevice.GetCapabilities().features;
        Vulkan::EnableOptionalPhysicalDeviceFeatures(enabledDeviceFeatures, availableDeviceFeatures, m_optionalDeviceFeatures);

        if (const auto result = m_logicalDevice.Create(
            m_physicalDevice,
            m_requiredInstanceLayers,
            m_requiredDeviceExtensions,
            enabledDeviceFeatures); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create logical device");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadMemoryAllocator()
    {
        if (const auto result = m_memoryAllocator.Load(
            m_logicalDevice,
            1024 * 1024 * 256,
            m_logger); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to load memory allocator");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::LoadSwapChain()
    {
        auto& surfaceCapabilities = m_physicalDevice.GetCapabilities().surfaceCapabilities;

        // Get image count.
        uint32_t imageCount = surfaceCapabilities.capabilities.minImageCount + 1;
        if (surfaceCapabilities.capabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.capabilities.maxImageCount)
        {
            imageCount = surfaceCapabilities.capabilities.maxImageCount;
        }

        if (const auto result = m_swapChain.Create(
            m_logicalDevice,
            m_surfaceFormat,
            m_presentMode,
            imageCount); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create swap chain");
            return false;
        }

        if (const auto result = LoadSwapChainRenderPass(); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to load swap chain render pass.");
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

        if (const auto result = vkCreateCommandPool(m_logicalDevice.GetHandle(), &commandPoolInfo, nullptr, &m_commandPool); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create command pool.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::ReloadSwapChain()
    {
        if(const auto result = m_swapChain.Recreate(); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to recreate swap chain.");
            return false;
        }

        if (const auto result = ReloadSwapChainRenderPass(); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to reload swap chain render pass.");
            return false;
        }

        return true;
    }

    Vulkan::Result<> VulkanRenderer::LoadSwapChainRenderPass()
    {
        const auto logicalDevice = m_logicalDevice.GetHandle();
        const auto swapChainExtent = m_swapChain.GetExtent();
        const auto& swapChainImages = m_swapChain.GetImages();

        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        VulkanTextureFrames colorAttachmentFrames(swapChainImages.size());
        VulkanRenderPassFrames renderPassFrames(swapChainImages.size());

        SmartFunction destroyer([&]()
        {
            for(auto& renderPassFrame : renderPassFrames)
            {
                if (renderPassFrame.framebuffer)
                {
                    vkDestroyFramebuffer(logicalDevice, renderPassFrame.framebuffer, nullptr);
                }
 
                if(renderPassFrame.finishSemaphore)
                {
                    vkDestroySemaphore(logicalDevice, renderPassFrame.finishSemaphore, nullptr);
                }
            }

            if(commandPool)
            {
                vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
            }

            if(renderPass)
            {
                vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
            }

            for(auto& colorAttachmentFrame : colorAttachmentFrames)
            {
                if(colorAttachmentFrame.imageView)
                {
                    vkDestroyImageView(logicalDevice, colorAttachmentFrame.imageView, nullptr);
                }
            }
        });

        const auto vulkanImageFormat = m_swapChain.GetSurfaceFormat().format;
        const auto imageFormat = GetImageFormat(vulkanImageFormat);

        // Create framed color texture attachment.    
        for (size_t i = 0; i < swapChainImages.size(); i++)
        {
            const auto& swapChainImage = swapChainImages[i];
            auto& colorAttachmentFrame = colorAttachmentFrames[i];

            colorAttachmentFrame.deviceImage.image = swapChainImage;

            VkImageViewCreateInfo imageViewInfo = {};
            imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewInfo.image = swapChainImage;
            imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewInfo.format = vulkanImageFormat;
            imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewInfo.subresourceRange.baseMipLevel = 0;
            imageViewInfo.subresourceRange.levelCount = 1;
            imageViewInfo.subresourceRange.baseArrayLayer = 0;
            imageViewInfo.subresourceRange.layerCount = 1;

            if (const auto result = vkCreateImageView(logicalDevice, &imageViewInfo, nullptr, &colorAttachmentFrame.imageView); result != VK_SUCCESS)
            {
                return result;
            }
        }

        auto imageDimensions = Vector2ui32{ swapChainExtent.width, swapChainExtent.height };

        auto colorAttachmentTexture = std::make_shared<VulkanFramedTexture<2>>(
            std::move(colorAttachmentFrames),
            imageFormat,
            imageDimensions,
            uint8_t{ 0 }  // TODO: Change 0 bytes per pixel.
        );

        auto attachments = VulkanRenderPassAttachments{
            {
                colorAttachmentTexture,
                TextureType::Color,
                VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            }
        };

        std::vector<VkClearValue> clearValues(1);
        clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };

        // Create command pool.
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = m_physicalDevice.GetDeviceQueueIndices().graphicsQueue.value();
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (const auto result = vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &commandPool); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create command pool");
            return {};
        }        

        // Create command buffers.
        Vulkan::CommandBuffers commandBuffers(renderPassFrames.size(), nullptr);

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (const auto result = vkAllocateCommandBuffers(logicalDevice, &commandBufferInfo, commandBuffers.data()); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to allocate command buffers");
            return {};
        }

        for (size_t i = 0; i < renderPassFrames.size(); i++)
        {
            renderPassFrames[i].commandBuffer = commandBuffers[i];
        }

        // Create finish semaphores
        Vulkan::Semaphores semaphores;
        Vulkan::CreateSemaphores(semaphores, m_logicalDevice.GetHandle(), renderPassFrames.size());

        for (size_t i = 0; i < renderPassFrames.size(); i++)
        {
            renderPassFrames[i].finishSemaphore = semaphores[i];
        }

        // Create render pass.
        VkAttachmentDescription renderPassColorAttachmentDesc = {};
        renderPassColorAttachmentDesc.format = m_surfaceFormat.format;
        renderPassColorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        renderPassColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        renderPassColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        renderPassColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        renderPassColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        renderPassColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        renderPassColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference renderPassColorAttachmentRef = {};
        renderPassColorAttachmentRef.attachment = 0;
        renderPassColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &renderPassColorAttachmentRef;

        VkSubpassDependency subpassDependency = {};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &renderPassColorAttachmentDesc;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &subpassDependency;

        if (const auto result = vkCreateRenderPass(m_logicalDevice.GetHandle(), &renderPassInfo, nullptr, &renderPass); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create internal render pass object");
            return {};
        }

        // Create framebuffers
        for(size_t i = 0; i < renderPassFrames.size(); i++)
        {
            auto& renderPassFrame = renderPassFrames[i];
            auto& colorAttachmentFrame = colorAttachmentTexture->frames[i];

            VkImageView framebufferAttachments[] = { colorAttachmentFrame.imageView };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = framebufferAttachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (const auto result = vkCreateFramebuffer(m_logicalDevice.GetHandle(), &framebufferInfo, nullptr, &renderPassFrame.framebuffer); result != VK_SUCCESS)
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create framebuffer object for render pass");
                return {};
            }      
        }

        // Finalize.
        destroyer.Release();

        m_swapChainRenderPass = SharedRenderResource<VulkanRenderPass>(new VulkanRenderPass{
            m_logger,
            renderPass,
            commandPool,
            imageDimensions,
            std::move(renderPassFrames),
            std::move(attachments),
            false,
            std::move(clearValues),
            nullptr
            }, RenderResourceDeleter{ this });

        return {};
    }

    Vulkan::Result<> VulkanRenderer::ReloadSwapChainRenderPass()
    {
        const auto logicalDevice = m_logicalDevice.GetHandle();
        const auto swapChainExtent = m_swapChain.GetExtent();
        const auto& swapChainImages = m_swapChain.GetImages();

        auto& colorAttachment = m_swapChainRenderPass->m_attachments[0];
        auto& colorAttachmentTexture = colorAttachment.texture;

        // Update color attachment.

        for(size_t i = 0; i < swapChainImages.size(); i++)
        {
            const auto swapChainImage = swapChainImages[i];
            auto& colorAttachmentTextureFrame = colorAttachmentTexture->frames[i];

            // Set new image from swapchain.
            colorAttachmentTextureFrame.deviceImage.image = swapChainImage;
            colorAttachmentTextureFrame.deviceImage.layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;

            // Destroy old image view.
            auto& imageView = colorAttachmentTextureFrame.imageView;
            if(imageView)
            {
                vkDestroyImageView(logicalDevice, imageView, nullptr);
                imageView = VK_NULL_HANDLE;
            }

            // Create new image view.
            VkImageViewCreateInfo imageViewInfo = {};
            imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewInfo.image = swapChainImage;
            imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewInfo.format = m_swapChain.GetSurfaceFormat().format;
            imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewInfo.subresourceRange.baseMipLevel = 0;
            imageViewInfo.subresourceRange.levelCount = 1;
            imageViewInfo.subresourceRange.baseArrayLayer = 0;
            imageViewInfo.subresourceRange.layerCount = 1;

            if (const auto result = vkCreateImageView(logicalDevice, &imageViewInfo, nullptr, &imageView); result != VK_SUCCESS)
            {
                return result;
            }
        }

        // Update framebuffers
        auto& renderPassFrames = m_swapChainRenderPass->m_frames;

        for(size_t i = 0; i < renderPassFrames.size(); i++)
        {
            auto& renderPassFrame = renderPassFrames[i];

            m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), renderPassFrame.framebuffer);

            const auto& colorAttachmentFrame = colorAttachmentTexture->frames[i];
            const VkImageView framebufferAttachments[] = { colorAttachmentFrame.imageView };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_swapChainRenderPass->m_renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = framebufferAttachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (const auto result = vkCreateFramebuffer(m_logicalDevice.GetHandle(), &framebufferInfo, nullptr, &renderPassFrame.framebuffer); result != VK_SUCCESS)
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to re-create framebuffer object for swap chain render pass");
                return {};
            }
        }

        m_swapChainRenderPass->SetDimensions({ swapChainExtent.width, swapChainExtent.height });

        return {};
    }

    uint32_t VulkanRenderer::GetNextDestroyerFrameIndex() const
    {
        return m_drawFrameCount + static_cast<uint32_t>(m_swapChain.GetImages().size()) + 2;
    }

    bool VulkanRenderer::CreateTexture(
        Vulkan::DeviceImage& deviceImage,
        VkImageView& imageView,
        const Vector3ui32& dimensions,
        const void* data,
        const VkDeviceSize dataSize,
        const VkImageLayout /*layout*/, // TODO: Use this!
        const VkFormat imageFormat,
        const VkFormat internalImageFormat,
        const VkImageType imageType,
        const VkImageViewType imageViewType,
        const VkComponentMapping& componentMapping)
    {
        Vulkan::DeviceBuffer stagingBuffer;
        Vulkan::DeviceBufferGuard stagingBufferGuard(m_memoryAllocator, stagingBuffer);

        // Create staging buffer.
        if (const auto result = m_memoryAllocator.CreateDeviceBuffer(
            stagingBuffer,
            dataSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for texture");
            return false;
        }

        if (const auto result = Vulkan::MapMemory(m_logicalDevice, stagingBuffer, data, dataSize, 0); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for vertex buffer");
            return false;
        }


        // Create image.
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.flags = 0;
        imageInfo.imageType = imageType;
        imageInfo.extent.width = dimensions.x;
        imageInfo.extent.height = dimensions.y;
        imageInfo.extent.depth = dimensions.z;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = imageFormat;
        imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; // TODO: Need check here!
        imageInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

        if (const auto result = m_memoryAllocator.CreateDeviceImage(
            deviceImage,
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create device image from memory allocator");
            return false;
        }

        // Transfer staging buffer to image memory.
        VkBufferImageCopy copyRegion = {};
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageExtent = { dimensions.x, dimensions.y, dimensions.z };

        VkCommandBuffer commandBuffer = nullptr;
        if (const auto result = Vulkan::BeginSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool); !result.IsSuccessful())
        {
            Logger::WriteError(m_logger, "Failed to begin single time command for texture creations.");
            return false;
        }

        if(!Vulkan::CopyDeviceBufferToDeviceImage(
            stagingBuffer,
            deviceImage,
            commandBuffer,
            copyRegion,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
        {
            Logger::WriteError(m_logger, "Failed to copy staging buffer to image for texture creation.");
            return false;
        }

        if (const auto result = Vulkan::EndSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool); !result.IsSuccessful())
        {
            Logger::WriteError(m_logger, "Failed to end single time command for texture creations.");
            return false;
        }

        // Create image view.
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = deviceImage.image;
        viewInfo.viewType = imageViewType;
        viewInfo.format = internalImageFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.components = componentMapping;

        if (const auto result = vkCreateImageView(m_logicalDevice.GetHandle(), &viewInfo, nullptr, &imageView); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create image view for texture");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::CreateFramedTexture(
        VulkanTextureFrames& frames,
        const Vector3ui32& dimensions,
        const void* data,
        const VkDeviceSize dataSize,
        const TextureType textureType,
        const VkImageLayout layout,
        const VkFormat imageFormat,
        const VkFormat internalImageFormat,
        const VkImageType imageType,
        const VkImageViewType imageViewType,
        const VkComponentMapping& componentMapping)
    {
        frames.resize(m_swapChain.GetImages().size());

        VkFlags attachmentUsage = textureType == TextureType::Color ?
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VkFlags aspectMask = textureType == TextureType::Color ?
            VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT : (VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT);

        for (auto& frame : frames)
        {
            // Create image.
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.flags = 0;
            imageInfo.imageType = imageType;
            imageInfo.extent.width = dimensions.x;
            imageInfo.extent.height = dimensions.y;
            imageInfo.extent.depth = dimensions.z;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = imageFormat;
            imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | attachmentUsage; // TODO: Need check here!
            imageInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

            if (const auto result = m_memoryAllocator.CreateDeviceImage(
                frame.deviceImage,
                imageInfo,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create device image from memory allocator");
                return false;
            }

            // Create image view.
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = frame.deviceImage.image;
            viewInfo.viewType = imageViewType;
            viewInfo.format = internalImageFormat;
            viewInfo.subresourceRange.aspectMask = aspectMask;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;
            viewInfo.components = componentMapping;

            if (const auto result = vkCreateImageView(m_logicalDevice.GetHandle(), &viewInfo, nullptr, &frame.imageView); result != VK_SUCCESS)
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create image view for texture");
                return false;
            }
        }

        VkCommandBuffer commandBuffer = nullptr;
        if (const auto result = Vulkan::BeginSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool); !result.IsSuccessful())
        {
            Logger::WriteError(m_logger, "Failed to begin single time command for framed texture creations.");
            return false;
        }

        Vulkan::DeviceBuffer stagingBuffer;
        Vulkan::DeviceBufferGuard stagingBufferGuard(m_memoryAllocator, stagingBuffer);

        if(data)
        {
            // Create staging buffer.
            if (const auto result = m_memoryAllocator.CreateDeviceBuffer(
                stagingBuffer,
                dataSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for texture");
                return false;
            }

            if (const auto result = Vulkan::MapMemory(m_logicalDevice, stagingBuffer, data, dataSize, 0); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for vertex buffer");
                return false;
            }

            VkBufferImageCopy copyRegion = {};
            copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageExtent = { dimensions.x, dimensions.y, dimensions.z };

            for (auto& frame : frames)
            {
                if(!Vulkan::CopyDeviceBufferToDeviceImage(
                    stagingBuffer,
                    frame.deviceImage,
                    commandBuffer,
                    copyRegion,
                    layout /*VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL*/))
                {
                    Logger::WriteError(m_logger, "Failed to copy staging buffer to image for framed texture creation.");
                    return false;
                }
            }
        }
        else
        {
            for (auto& frame : frames)
            {
                if (!Vulkan::TransitionImageLayout(commandBuffer, frame.deviceImage, layout))
                {
                    Logger::WriteError(m_logger, "Failed to transition image layout of framed texture to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL");
                    return false;
                }
            }
        }

        if (const auto result = Vulkan::EndSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool); !result.IsSuccessful())
        {
            Logger::WriteError(m_logger, "Failed to end single time command for framed texture creations.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::UpdateTexture(
        Vulkan::DeviceImage& deviceImage,
        const uint8_t bytesPerPixel,
        const void* data,
        const Vector3ui32& destinationDimensions,
        const Vector3ui32& destinationOffset)
    {
        const auto dataSize = 
            static_cast<VkDeviceSize>(destinationDimensions.x) *
            static_cast<VkDeviceSize>(destinationDimensions.y) *
            static_cast<VkDeviceSize>(destinationDimensions.z) *
            static_cast<VkDeviceSize>(bytesPerPixel);

        Vulkan::DeviceBuffer stagingBuffer;
        Vulkan::DeviceBufferGuard stagingBufferGuard(m_memoryAllocator, stagingBuffer);

        if (const auto result = m_memoryAllocator.CreateDeviceBuffer(
            stagingBuffer,
            dataSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create staging buffer for texture update");
            return false;
        }

        if (const auto result = Vulkan::MapMemory(m_logicalDevice, stagingBuffer, data, dataSize, 0); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map staging buffer for texture update");
            return false;
        }

        // Transfer staging buffer to image memory.
        VkBufferImageCopy copyRegion = {};
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageOffset = { static_cast<int32_t>(destinationOffset.x), static_cast<int32_t>(destinationOffset.y), static_cast<int32_t>(destinationOffset.z) };
        copyRegion.imageExtent = { destinationDimensions.x, destinationDimensions.y, destinationDimensions.z };

        VkCommandBuffer commandBuffer = nullptr;
        if (const auto result = Vulkan::BeginSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool); !result.IsSuccessful())
        {
            Logger::WriteError(m_logger, "Failed to begin single time command for texture update.");
            return false;
        }

        if (!Vulkan::CopyDeviceBufferToDeviceImage(
            stagingBuffer,
            deviceImage,
            commandBuffer,
            copyRegion,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
        {
            Logger::WriteError(m_logger, "Failed to copy staging buffer to image for texture update.");
            return false;
        }

        if (const auto result = Vulkan::EndSingleTimeCommands(commandBuffer, m_logicalDevice, m_commandPool); !result.IsSuccessful())
        {
            Logger::WriteError(m_logger, "Failed to end single time command for texture update.");
            return false;
        }

        return true;
    }

    bool VulkanRenderer::CreateVertexInputAttributes(
        const Shader::Visual::InputInterface& inputs,
        std::vector<VkVertexInputAttributeDescription>& attributes, 
        uint32_t& stride) const
    {
        attributes.resize(inputs.GetMemberCount());
        size_t index = 0;
        uint32_t location = 0;

        const auto outputPins = inputs.GetOutputPins();
        for (auto* pins : outputPins)
        {
            VkFormat format;
            uint32_t formatSize;
            if (!GetVulkanVertexAttributeFormat(pins->GetDataType(), format, formatSize))
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
                binding.descriptorType = GetVulkanDescriptorType(mappedDescriptorBinding.bindingType);
                binding.descriptorCount = 1;
                binding.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
                binding.pImmutableSamplers = nullptr;
            }

            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
            descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            descriptorSetLayoutInfo.pBindings = bindings.data();

            VkDescriptorSetLayout descriptorSetLayout;
            if (const auto result = vkCreateDescriptorSetLayout(m_logicalDevice.GetHandle(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout); result != VK_SUCCESS)
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create descriptor set layout.");
                return false;
            }

            setLayouts.push_back(descriptorSetLayout);
        }

       return true;
    }

}

#endif