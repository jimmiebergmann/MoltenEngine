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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_VULKANRENDERER_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_VULKANRENDERER_HPP

#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include "Molten/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanInstance.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanSurface.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDeviceFeatures.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanSwapChain.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryType.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanShaderModule.hpp"
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten
{

    class VulkanFramebuffer;

    /**
    * @brief Vulkan renderer class.
    */
    class MOLTEN_API VulkanRenderer : public Renderer
    {

    public:

        /** Constructor. */
        VulkanRenderer();

        /** Constructs and creates renderer. */
        VulkanRenderer(RenderTarget& renderTarget, const Version& version, Logger* logger = nullptr);

        /** Virtual destructor. */
        ~VulkanRenderer() override;

        /** Opens renderer by loading and attaching renderer to provided window. */
        bool Open(RenderTarget& renderTarget, const Version& version = Version::None, Logger* logger = nullptr) override;

        /** Closing renderer. */
        void Close() override;

        /** Checks if renderer is open Same value returned as last call to Open. */
        bool IsOpen() const override;

        /** Resize the framebuffers.
         *  Execute this function as soon as the render target's work area is resized.
         */
        void Resize(const Vector2ui32& size) override;

        /** Get backend API type. */
        BackendApi GetBackendApi() const override;

        /** Get renderer API version. */
        Version GetVersion() const override;

        /** Get location of pipeline push constant by id. Id is set in shader script. */
        uint32_t GetPushConstantLocation(Pipeline& pipeline, const uint32_t id) override;


        /** Create descriptor set object. */
        RenderResource<DescriptorSet> CreateDescriptorSet(const DescriptorSetDescriptor& descriptor) override;

        /** Create framed descriptor set object. */
        RenderResource<FramedDescriptorSet> CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& descriptor) override;

        /** Create framebuffer object. */
        RenderResource<Framebuffer> CreateFramebuffer(const FramebufferDescriptor& descriptor) override;

        /**  Create index buffer object. */
        RenderResource<IndexBuffer> CreateIndexBuffer(const IndexBufferDescriptor& descriptor) override;

        /** Create pipeline object. */
        RenderResource<Pipeline> CreatePipeline(const PipelineDescriptor& descriptor) override;

        /** Create texture object. */
        RenderResource<Texture> CreateTexture(const TextureDescriptor& descriptor) override;

        /** Create uniform buffer object. */
        RenderResource<UniformBuffer> CreateUniformBuffer(const UniformBufferDescriptor& descriptor) override;

        /** Create framed uniform buffer object. */
        RenderResource<FramedUniformBuffer> CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor) override;

        /** Create vertex buffer object. */
        RenderResource<VertexBuffer> CreateVertexBuffer(const VertexBufferDescriptor& descriptor) override;


        /** Destroys render resource.
         *  Resources are not destroyed right away, but instead put in a cleanup queue in order to make sure that resources in use not are destroyed.
         */
        /**@{*/
        void Destroy(DescriptorSet& descriptorSet) override;
        void Destroy(FramedDescriptorSet& framedDescriptorSet) override;
        void Destroy(Framebuffer& framebuffer) override;
        void Destroy(IndexBuffer& indexBuffer) override;
        void Destroy(Pipeline& pipeline) override;
        void Destroy(Texture& texture) override;
        void Destroy(UniformBuffer& uniformBuffer) override;
        void Destroy(FramedUniformBuffer& framedUniformBuffer) override;
        void Destroy(VertexBuffer& vertexBuffer) override;
        /**@}*/


        /** Bind descriptor set to draw queue. */
        void BindDescriptorSet(DescriptorSet& descriptorSet) override;

        /** Bind framed descriptor set to draw queue. */
        void BindFramedDescriptorSet(FramedDescriptorSet& framedDescriptorSet) override;

        /** Bind pipeline to draw queue. */
        void BindPipeline(Pipeline& pipeline) override;


        /** Begin and initialize rendering to framebuffers. */
        void BeginDraw() override;

        /** Draw vertex buffer, using the current bound pipeline. */
        void DrawVertexBuffer(VertexBuffer& vertexBuffer) override;

        /** Draw indexed vertex buffer, using the current bound pipeline. */
        void DrawVertexBuffer(IndexBuffer& indexBuffer, VertexBuffer& vertexBuffer) override;

        /** Push constant values to shader stage.
         *  This function call has no effect if provided id argument is greater than the number of push constants in pipeline.
         *
         * @param location Id of push constant to update. This id can be shared between multiple shader stages.
         */
        /**@{*/
        void PushConstant(const uint32_t location, const bool& value) override;
        void PushConstant(const uint32_t location, const int32_t& value) override;
        void PushConstant(const uint32_t location, const float& value) override;
        void PushConstant(const uint32_t location, const Vector2f32& value) override;
        void PushConstant(const uint32_t location, const Vector3f32& value) override;
        void PushConstant(const uint32_t location, const Vector4f32& value) override;
        void PushConstant(const uint32_t location, const Matrix4x4f32& value) override;
        /**@}*/


        /** Finalize and present rendering. */
        void EndDraw() override;


        /** Sleep until the graphical device is ready. */
        void WaitForDevice() override;

        /** Update uniform buffer data. */
        void UpdateUniformBuffer(RenderResource<UniformBuffer>& uniformBuffer, const size_t offset, const size_t size, const void* data) override;

        /** Update uniform buffer data. */
        void UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& framedUniformBuffer, const size_t offset, const size_t size, const void* data) override;

    private:

        /** Renderer creation functions. */
        /**@{*/
        bool LoadRequirements();
        bool LoadInstance(const Version& version);      
        bool LoadSurface();
        bool LoadPhysicalDevice();
        bool LoadLogicalDevice();
        bool LoadMemoryTypes();
        bool LoadRenderPass();
        bool LoadSwapChain();
        bool LoadCommandPool();
        /**@}*/

        /** Shader creation and manipulation functions. */
        /**@{*/
        bool CreateVertexInputAttributes(
            const Shader::Visual::InputInterface& inputs,
            std::vector<VkVertexInputAttributeDescription>& attributes, 
            uint32_t& stride);
        
        bool CreateDescriptorSetLayouts(
            Vulkan::DescriptorSetLayouts& setLayouts,
            const MappedDescriptorSets& mappedDescriptorSets);

        bool LoadShaderModules(
            Vulkan::ShaderModules& shaderModules,
            std::vector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
            PushConstantLocations& pushConstantLocations,
            VkPushConstantRange& pushConstantRange,
            MappedDescriptorSets& mappedDescriptorSets,
            const std::vector<Shader::Visual::Script*>& visualScripts);


        template<typename T>
        void InternalPushConstant(const uint32_t location, const T& value);
        /**@}*/

        /** Renderer creation variables. */
        /**@{*/
        RenderTarget* m_renderTarget;
        Version m_version;
        Logger* m_logger;     
        /**@}*/

        /** Requirements variables. */
        /**@{*/
        Vulkan::Extensions m_requiredInstanceExtensions;
        Vulkan::Layers m_requiredInstanceLayers;      
        Vulkan::Extensions m_requiredDeviceExtensions;
        Vulkan::PhysicalDeviceFeaturePointers m_requiredDeviceFeatures;
        Vulkan::PhysicalDeviceFeaturePointers m_optionalDeviceFeatures;
        /**@}*/

        /** Vulkan context variables. */
        /**@{*/
        bool m_isOpen;
        bool m_enableDebugMessenger;
        Vulkan::Layers m_debugInstanceLayers;
        Vulkan::Instance m_instance;
        Vulkan::Surface m_surface;
        Vulkan::PhysicalDevice m_physicalDevice;
        Vulkan::LogicalDevice m_logicalDevice;
        VkRenderPass m_renderPass;
        Vulkan::SwapChain m_swapChain;
        VkSurfaceFormatKHR m_surfaceFormat;
        VkPresentModeKHR m_presentMode;
        VkCommandPool m_commandPool;
        std::vector<VkCommandBuffer> m_commandBuffers;
        VkCommandBuffer* m_currentCommandBuffer;
        bool m_beginDraw;
        VulkanPipeline* m_currentPipeline;
        Shader::GlslGenerator m_glslGenerator;
        /**@}*/

        /** Pre-filtered memory types. */
        /**@{*/
        Vulkan::FilteredMemoryTypes m_memoryTypesHostVisibleOrCoherent;
        Vulkan::FilteredMemoryTypes m_memoryTypesDeviceLocal;
        /**@}*/
           
    };

}

#include "Molten/Renderer/Vulkan/VulkanRenderer.inl"

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif