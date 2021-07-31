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

#include "Molten/Renderer/Vulkan/VulkanRenderer.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Renderer/Vulkan/VulkanDescriptorSet.hpp"
#include "Molten/Renderer/Vulkan/VulkanFramebuffer.hpp"
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

    static VkFormat GetImageFormat(const ImageFormat imageFormat, uint8_t& bytesPerPixel)
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

            case ImageFormat::UBlue8Green8Red8: bytesPerPixel = 3; return VkFormat::VK_FORMAT_B8G8R8_UNORM;
            case ImageFormat::UBlue8Green8Red8Alpha8: bytesPerPixel = 4; return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
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
  
    static VkDescriptorType GetDescriptorType(const DescriptorBindingType bindingType)
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
        m_memoryAllocator{},
        m_resourceDestroyer(m_logicalDevice, m_memoryAllocator),
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
            LoadMemoryAllocator() &&
            LoadRenderPass() &&
            LoadSwapChain() &&
            LoadCommandPool();

        m_isOpen = loaded;
        return loaded;   
    }

    void VulkanRenderer::Close()
    {
        m_resourceDestroyer.ProcessAll();
        m_memoryAllocator.Destroy();

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
        auto& vulkanPipeline = static_cast<VulkanPipeline&>(pipeline);
        auto& locations = vulkanPipeline.shaderProgram->pushConstantLocations;
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
        }, RenderResourceDeleter<DescriptorSet>{ this });
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
                return {};
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
        allocInfo.descriptorSetCount = swapChainImageCount;
        allocInfo.pSetLayouts = setLayouts.data();

        Vulkan::DescriptorSets descriptorSets(swapChainImageCount, VK_NULL_HANDLE);
        if (vkAllocateDescriptorSets(m_logicalDevice.GetHandle(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            Logger::WriteError(m_logger, "Failed to create descriptor sets.");
            return {};
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
        VulkanFramebuffer::Frames frames(m_swapChain.GetImageCount());
        //Vulkan::DeviceImageAndViews deviceImageAndViews(m_swapChain.GetImageCount());

        const auto imageFormat = m_surfaceFormat.format;

        // TODO: Insufficient device image guarding.

        for(auto& frame : frames)
        {
            // Create image.
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.flags = 0;
            imageInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = descriptor.dimensions.x;
            imageInfo.extent.height = descriptor.dimensions.y;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = imageFormat;
            imageInfo.tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

            if (const auto result = m_memoryAllocator.CreateDeviceImage(
                frame.texture.deviceImage,
                imageInfo,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create device image for framebuffer");
                return {};
            }

            // Create image view.
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = frame.texture.deviceImage.image;
            viewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = imageFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;
            viewInfo.components = {};

            if (const auto result = vkCreateImageView(m_logicalDevice.GetHandle(), &viewInfo, nullptr, &frame.texture.imageView); result != VK_SUCCESS)
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create image view for framebuffer");
                return {};
            }

            // Create framebuffer.
            VkImageView framebufferAttachments[] = { frame.texture.imageView };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = framebufferAttachments;
            framebufferInfo.width = descriptor.dimensions.x;
            framebufferInfo.height = descriptor.dimensions.y;
            framebufferInfo.layers = 1;

            if (const auto result = vkCreateFramebuffer(m_logicalDevice.GetHandle(), &framebufferInfo, nullptr, &frame.framebuffer); result != VK_SUCCESS)
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to create framebuffer object for framebuffer");
                return {};
            }
        }

        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = m_physicalDevice.GetDeviceQueueIndices().graphicsQueue.value();
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkCommandPool commandPool = VK_NULL_HANDLE;
        if (const auto result = vkCreateCommandPool(m_logicalDevice.GetHandle(), &commandPoolInfo, nullptr, &commandPool); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create command pool for framebuffer.");
            return {};
        }

        std::vector<VkCommandBuffer> commandBuffers(frames.size());

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (const auto result = vkAllocateCommandBuffers(m_logicalDevice.GetHandle(), &commandBufferInfo, commandBuffers.data()); result != VK_SUCCESS)
        {
            vkDestroyCommandPool(m_logicalDevice.GetHandle(), commandPool, nullptr);
            Vulkan::Logger::WriteError(m_logger, result, "Failed to allocate command buffers for framebuffer.");
            return {};
        }

        return RenderResource<Framebuffer>(new VulkanFramebuffer{
            std::move(frames),
            descriptor.dimensions,
            commandPool,
            std::move(commandBuffers)
        }, RenderResourceDeleter<Framebuffer>{ this });
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
        }, RenderResourceDeleter<IndexBuffer>{ this });
    }

    RenderResource<Pipeline> VulkanRenderer::CreatePipeline(const PipelineDescriptor& descriptor)
    {
        auto* vulkanShaderProgram = static_cast<VulkanShaderProgram*>(descriptor.shaderProgram);
        if(!vulkanShaderProgram)
        {
            Logger::WriteError(m_logger, "Cannot create a pipeline without a shader program.");
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
        }, RenderResourceDeleter<Pipeline>{ this });
    }

    SharedRenderResource<RenderPass> VulkanRenderer::CreateRenderPass(const RenderPassDescriptor& descriptor)
    {
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;

        //auto& vulkanFramebuffer = static_cast<VulkanFramebuffer&>(descriptor.framebuffer);

        // Failure cleanup function.
        SmartFunction destroyer([&]()
        {
            if (commandPool != VK_NULL_HANDLE)
            {
                vkDestroyCommandPool(m_logicalDevice.GetHandle(), commandPool, nullptr);
            }
            if (renderPass != VK_NULL_HANDLE)
            {
                vkDestroyRenderPass(m_logicalDevice.GetHandle(), renderPass, nullptr);
            }
        });


        // Create command pool.
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = m_physicalDevice.GetDeviceQueueIndices().graphicsQueue.value();
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (const auto result = vkCreateCommandPool(m_logicalDevice.GetHandle(), &commandPoolInfo, nullptr, &commandPool); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create command pool");
            return {};
        }

        // Create command buffers.
        Vulkan::CommandBuffers commandBuffers(m_swapChain.GetImageCount(), VK_NULL_HANDLE);

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (const auto result = vkAllocateCommandBuffers(m_logicalDevice.GetHandle(), &commandBufferInfo, commandBuffers.data()); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to allocate command buffers");
            return {};
        }

        // Create render pass.
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

        if (const auto result = vkCreateRenderPass(m_logicalDevice.GetHandle(), &renderPassInfo, nullptr, &renderPass); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create internal render pass object");
            return {};
        }


        // Finalize
        destroyer.Release();

        Vulkan::Semaphores finishSemaphores;

        return SharedRenderResource<RenderPass>(new VulkanRenderPass{
            m_logger,
            //vulkanFramebuffer,
            commandPool,
            std::move(commandBuffers),
            std::move(finishSemaphores),
            renderPass,
            descriptor.recordFunction
        }, RenderResourceDeleter<RenderPass>{ this });
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

        return RenderResource<Sampler3D>(new VulkanSampler3D{
            std::move(imageSampler)
        }, RenderResourceDeleter<Sampler3D>{ this });
    }

    RenderResource<ShaderProgram> VulkanRenderer::CreateShaderProgram(const VisualShaderProgramDescriptor& descriptor)
    {
        if (descriptor.vertexScript == nullptr)
        {
            Logger::WriteError(m_logger, "Cannot create shader program without vertex script.");
            return {};
        }
        if (descriptor.fragmentScript == nullptr)
        {
            Logger::WriteError(m_logger, "Cannot create shader program without fragment script.");
            return {};
        }

        auto& vertexOutputs = descriptor.vertexScript->GetOutputInterface();
        auto& fragmentInputs = descriptor.fragmentScript->GetInputInterface();
        if (!vertexOutputs.CompareStructure(fragmentInputs))
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
                if(shaderModule)
                {
                    vkDestroyShaderModule(m_logicalDevice.GetHandle(), shaderModule, nullptr);
                }     
            }
        });

        std::vector<Shader::Visual::Script*> visualScripts = { descriptor.vertexScript, descriptor.fragmentScript };

        Shader::GlslGenerator::GlslTemplate glslTemplate;
        if (!Shader::GlslGenerator::GenerateGlslTemplate(glslTemplate, visualScripts, m_logger))
        {
            Logger::WriteError(m_logger, "Failed to generate glsl template.");
            return {};
        }

        for (size_t i = 0; i < visualScripts.size(); i++)
        {
            const auto& script = visualScripts[i];

            auto glslCode = m_glslGenerator.Generate(*script, Shader::GlslGenerator::Compability::SpirV, &glslTemplate, m_logger);
            if (glslCode.empty())
            {
                Logger::WriteError(m_logger, "Failed to generate GLSL code.");
                return {};
            }

            const auto scriptType = script->GetType();
            auto spirvCode = Shader::GlslGenerator::ConvertGlslToSpriV(glslCode, scriptType, m_logger);
            if (spirvCode.empty())
            {
                Logger::WriteError(m_logger, "Failed to convert GLSL to SPIR-V.");
                return {};
            }

            VkShaderModuleCreateInfo shaderModuleInfo = {};
            shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModuleInfo.codeSize = spirvCode.size();
            shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(spirvCode.data());

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
            stageCreateInfo.stage = GetShaderProgramStageFlag(scriptType);
        }

        // Push constant information.
        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
        pushConstantRange.offset = 0;
        pushConstantRange.size = glslTemplate.pushConstantBlockByteSize;

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
            vertexBindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }

        // Finalize.
        destroyer.Release();

        return RenderResource<ShaderProgram>(new VulkanShaderProgram{
            std::move(shaderModules),
            std::move(pipelineShaderStageCreateInfos),
            std::move(glslTemplate.mappedDescriptorSets),
            std::move(vertexInputAttributeDescriptions),
            std::move(vertexBindingDescription),
            std::move(glslTemplate.pushConstantLocations),
            pushConstantRange
        }, RenderResourceDeleter<ShaderProgram>{ this });
    }

    RenderResource<Texture1D> VulkanRenderer::CreateTexture(const TextureDescriptor1D& descriptor)
    {
        uint8_t bytesPerPixel = 0;
        uint8_t internalBytesPerPixel = 0;
        const auto imageFormat = GetImageFormat(descriptor.format, bytesPerPixel);
        const auto internalImageFormat = GetImageFormat(descriptor.internalFormat, internalBytesPerPixel);
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
            imageFormat,
            internalImageFormat,
            VulkanImageTypeTraits<1>::type,
            VulkanImageViewTypeTraits<1>::type,
            GetComponentMappings(descriptor.swizzleMapping)))
        {
            return {};
        }

        deviceImageGuard.Release();

        return RenderResource<Texture1D>(new VulkanTexture1D{
            std::move(deviceImage),
            imageView,
            bytesPerPixel
        }, RenderResourceDeleter<Texture1D>{ this });
    }

    RenderResource<Texture2D> VulkanRenderer::CreateTexture(const TextureDescriptor2D& descriptor)
    {
        uint8_t bytesPerPixel = 0;
        uint8_t internalBytesPerPixel = 0;
        const auto imageFormat = GetImageFormat(descriptor.format, bytesPerPixel);
        const auto internalImageFormat = GetImageFormat(descriptor.internalFormat, internalBytesPerPixel);
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
            imageFormat,
            internalImageFormat,
            VulkanImageTypeTraits<2>::type,
            VulkanImageViewTypeTraits<2>::type,
            GetComponentMappings(descriptor.swizzleMapping)))
        {
            return {};
        }

        deviceImageGuard.Release();

        return RenderResource<Texture2D>(new VulkanTexture2D{
            std::move(deviceImage),
            imageView,
            bytesPerPixel
        }, RenderResourceDeleter<Texture2D>{ this });
    }

    RenderResource<Texture3D> VulkanRenderer::CreateTexture(const TextureDescriptor3D& descriptor)
    {
        uint8_t bytesPerPixel = 0;
        uint8_t internalBytesPerPixel = 0;
        const auto imageFormat = GetImageFormat(descriptor.format, bytesPerPixel);
        const auto internalImageFormat = GetImageFormat(descriptor.internalFormat, internalBytesPerPixel);
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
            imageFormat,
            internalImageFormat,
            VulkanImageTypeTraits<3>::type,
            VulkanImageViewTypeTraits<3>::type,
            GetComponentMappings(descriptor.swizzleMapping)))
        {
            return {};
        }

        deviceImageGuard.Release();

        return RenderResource<Texture3D>(new VulkanTexture3D{
            std::move(deviceImage),
            imageView,
            bytesPerPixel
        }, RenderResourceDeleter<Texture3D>{ this });
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
        }, RenderResourceDeleter<UniformBuffer>{ this });
    }

    RenderResource<FramedUniformBuffer> VulkanRenderer::CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor)
    {
        std::vector<Vulkan::DeviceBuffer> deviceBuffers(m_swapChain.GetImageCount());

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
        }, RenderResourceDeleter<FramedUniformBuffer>{ this });
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
        }, RenderResourceDeleter<VertexBuffer>{ this });
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

    void VulkanRenderer::Destroy(Framebuffer& framebuffer)
    {
        auto& vulkanFramebuffer = static_cast<VulkanFramebuffer&>(framebuffer);
        m_resourceDestroyer.Add(GetNextDestroyerFrameIndex(), vulkanFramebuffer);
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

    void VulkanRenderer::DrawFrame(const RenderPassGroup& renderPassGroup)
    {
        // Begin frame.
        if (const auto result = m_swapChain.BeginDraw(); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to begin drawing of swap chain.");
            return;
        }

        const auto currentImageIndex = m_swapChain.GetCurrentImageIndex();

        const auto swapChainExtent = m_swapChain.GetExtent(); /// TODO: Store default values for viewport and scissor.
        const auto viewportScissorBounds = Bounds2i32{ 0, 0, static_cast<int32_t>(swapChainExtent.width), static_cast<int32_t>(swapChainExtent.height) };

        // Draw render passes. Recording could be multi-threaded in the future, so recording and submits are separate.
        for(auto& renderPass : renderPassGroup)
        {
            auto& vulkanRenderPass = static_cast<VulkanRenderPass&>(*renderPass);
            if(auto result = vulkanRenderPass.Record(currentImageIndex, viewportScissorBounds, viewportScissorBounds); !result.IsSuccessful())
            {
                Vulkan::Logger::WriteError(m_logger, result, "Failed to record command buffer");
                return;
            }
            vulkanRenderPass.Submit();
        }

        // End frame.
        if (const auto result = m_swapChain.EndDraw(*m_currentCommandBuffer); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to end swap chain");
            return;
        }

        // Process destroyed resources.
        m_resourceDestroyer.Process(m_endedDraws);
        ++m_endedDraws;
    }

    void VulkanRenderer::BeginDraw()
    {
        if (m_beginDraw)
        {
            Logger::WriteError(m_logger, "Calling BeginDraw twice, without any previous call to EndDraw.");
            return;
        }

        if (const auto result = m_swapChain.BeginDraw(); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to begin drawing of swap chain.");
            return;
        }

        const auto currentImageIndex = m_swapChain.GetCurrentImageIndex();

        m_currentCommandBuffer = &m_commandBuffers[currentImageIndex];

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (const auto result = vkBeginCommandBuffer(*m_currentCommandBuffer, &commandBufferBeginInfo); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to begin recording command buffer.");
            return;
        }

        const auto currentFramebuffer = m_swapChain.GetCurrentFramebuffer();
        const auto swapChainExtent = m_swapChain.GetExtent();

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = currentFramebuffer;
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = swapChainExtent;

        const VkClearValue clearColor = { 0.3f, 0.0f, 0.0f, 0.0f }; // TODO: Invalid initialization.
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

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer.deviceBuffer.buffer };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(*m_currentCommandBuffer, static_cast<uint32_t>(vulkanVertexBuffer.vertexCount), 1, 0, 0);
    }

    void VulkanRenderer::DrawVertexBuffer(IndexBuffer& indexBuffer, VertexBuffer& vertexBuffer)
    {
        auto& vulkanIndexBuffer = static_cast<VulkanIndexBuffer&>(indexBuffer);
        auto& vulkanVertexBuffer = static_cast<VulkanVertexBuffer&>(vertexBuffer);

        VkBuffer vertexBuffers[] = { vulkanVertexBuffer.deviceBuffer.buffer };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(*m_currentCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(*m_currentCommandBuffer, vulkanIndexBuffer.deviceBuffer.buffer, 0, GetIndexBufferDataType(vulkanIndexBuffer.dataType));
        vkCmdDrawIndexed(*m_currentCommandBuffer, static_cast<uint32_t>(vulkanIndexBuffer.indexCount), 1, 0, 0, 0);
    }

    void VulkanRenderer::PushConstant(const uint32_t location, const bool& value)
    {
        vkCmdPushConstants(
            *m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const int32_t& value)
    {
        vkCmdPushConstants(
            *m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const float& value)
    {
        vkCmdPushConstants(
            *m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const Vector2f32& value)
    {
        vkCmdPushConstants(
            *m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const Vector3f32& value)
    {
        vkCmdPushConstants(
            *m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const Vector4f32& value)
    {
        vkCmdPushConstants(
            *m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);
    }
    void VulkanRenderer::PushConstant(const uint32_t location, const Matrix4x4f32& value)
    {
        vkCmdPushConstants(
            *m_currentCommandBuffer, m_currentPipeline->pipelineLayout,
            VK_SHADER_STAGE_ALL, location, sizeof(value), &value);
    }

    void VulkanRenderer::EndDraw()
    {
        if (!m_beginDraw)
        {
            Logger::WriteError(m_logger, "Calling EndDraw, without any previous call to BeginDraw.");
            return;
        }

        vkCmdEndRenderPass(*m_currentCommandBuffer);
        if (const auto result = vkEndCommandBuffer(*m_currentCommandBuffer); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to end command buffer");
            return;
        }

        if (const auto result = m_swapChain.EndDraw(*m_currentCommandBuffer); !result.IsSuccessful())
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
        auto& deviceBuffer = vulkanFramedUniformBuffer->deviceBuffers[m_swapChain.GetCurrentImageIndex()];

        if (const auto result = Vulkan::MapMemory(m_logicalDevice, deviceBuffer, data, size, offset); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to map memory of framed uniform buffer");
        }
    }

    void VulkanRenderer::BeginFramebufferDraw(Framebuffer& framebuffer)
    {
        auto& vulkanFramebuffer = static_cast<VulkanFramebuffer&>(framebuffer);
        auto& frames = vulkanFramebuffer.frames;
        auto& frame = frames[0];
        const auto extent = VkExtent2D{ vulkanFramebuffer.dimensions.x, vulkanFramebuffer.dimensions.y };

        const auto currentImageIndex = m_swapChain.GetCurrentImageIndex();
        m_currentCommandBuffer = &vulkanFramebuffer.commandBuffers[currentImageIndex];

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;
   
        if (const auto result = vkBeginCommandBuffer(*m_currentCommandBuffer, &commandBufferBeginInfo); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to begin recording command buffer for framebuffer.");
            return;
        }

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = frame.framebuffer; // TODO: TEST
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = extent; // TODO: TEST

        const VkClearValue clearColor = { 1.0f, 1.0f, 0.0f, 1.0f };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 0.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;

        vkCmdBeginRenderPass(*m_currentCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdSetViewport(*m_currentCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(*m_currentCommandBuffer, 0, 1, &scissor);
    }

    void VulkanRenderer::EndFramebufferDraw(Framebuffer& framebuffer)
    {
        // auto& vulkanFramebuffer = static_cast<VulkanFramebuffer&>(framebuffer);

        vkCmdEndRenderPass(*m_currentCommandBuffer);
        if (const auto result = vkEndCommandBuffer(*m_currentCommandBuffer); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to end command buffer for framebuffer");
            return;
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        //VkSemaphore imageSemaphores[] = { m_imageAvailableSemaphores[m_currentFrameIndex] };
        VkPipelineStageFlags pipelineWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr; //imageSemaphores;
        submitInfo.pWaitDstStageMask = pipelineWaitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = m_currentCommandBuffer;

        //VkSemaphore renderSemaphores[] = { m_renderFinishedSemaphores[m_currentFrameIndex] };
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;//renderSemaphores;

        /*if (!(result = vkResetFences(m_logicalDevice.GetHandle(), 1, &m_inFlightFences[m_currentFrameIndex])))
        {
            return result;
        }*/

        auto& deviceQueues = m_logicalDevice.GetDeviceQueues();
        if (const auto result = vkQueueSubmit(deviceQueues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed submit queue for framebuffer");
            return;
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
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create instance(vulkan version " + vulkanVersion.AsString() + ")");
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

        if (const auto result = vkCreateRenderPass(m_logicalDevice.GetHandle(), &renderPassInfo, nullptr, &m_renderPass); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create render pass.");
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

        // Create swap chain.
        if (const auto result = m_swapChain.Create(
            m_logicalDevice,
            m_renderPass,
            m_surfaceFormat,
            m_presentMode,
            imageCount); !result.IsSuccessful())
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

        if (const auto result = vkCreateCommandPool(m_logicalDevice.GetHandle(), &commandPoolInfo, nullptr, &m_commandPool); result != VK_SUCCESS)
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to create command pool.");
            return false;
        }

        m_commandBuffers.resize(m_swapChain.GetImageCount());

        VkCommandBufferAllocateInfo commandBufferInfo = {};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = m_commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        if (const auto result = vkAllocateCommandBuffers(m_logicalDevice.GetHandle(), &commandBufferInfo, m_commandBuffers.data()); result != VK_SUCCESS)
        {
            vkDestroyCommandPool(m_logicalDevice.GetHandle(), m_commandPool, nullptr);
            Vulkan::Logger::WriteError(m_logger, result, "Failed to allocate command buffers.");
            return false;
        }

        return true;
    }

    uint32_t VulkanRenderer::GetNextDestroyerFrameIndex() const
    {
        return m_endedDraws + m_swapChain.GetMaxFramesInFlight() + 2;
    }

    bool VulkanRenderer::CreateTexture(
        Vulkan::DeviceImage& deviceImage,
        VkImageView& imageView,
        const Vector3ui32& dimensions,
        const void* data,
        const VkDeviceSize dataSize,
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
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
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

        if(const auto result = Vulkan::CopyDeviceBufferToDeviceImage(
            stagingBuffer,
            deviceImage,
            m_logicalDevice,
            m_commandPool,
            copyRegion,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to copy staging buffer to image");
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

        if (const auto result = Vulkan::CopyDeviceBufferToDeviceImage(
            stagingBuffer,
            deviceImage,
            m_logicalDevice,
            m_commandPool,
            copyRegion,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); !result.IsSuccessful())
        {
            Vulkan::Logger::WriteError(m_logger, result, "Failed to copy staging buffer to image for textute update");
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