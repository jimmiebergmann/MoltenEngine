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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_VULKANRENDERER_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_VULKANRENDERER_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Renderer/Vulkan/VulkanPipeline.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanMemoryAllocator.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanResourceDestroyer.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanInstance.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanSurface.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDeviceFeatures.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Renderer/Vulkan/Utility/VulkanSwapChain.hpp"
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{
    struct DeviceImage;
}

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
        ~VulkanRenderer() override; // Replace by noexcept version.

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

        /** Get supported capabilities and features of renderer. */
        const RendererCapabilities& GetCapabilities() const override;

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

        /** Create render pass object. */
        SharedRenderResource<RenderPass> CreateRenderPass(const RenderPassDescriptor& descriptor) override;

        /** Create sampler object. */
        /**@{*/
        SharedRenderResource<Sampler1D> CreateSampler(const SamplerDescriptor1D& descriptor) override;
        SharedRenderResource<Sampler2D> CreateSampler(const SamplerDescriptor2D& descriptor) override;
        SharedRenderResource<Sampler3D> CreateSampler(const SamplerDescriptor3D& descriptor) override;
        /**@}*/

        /** Create shader module object. */
        SharedRenderResource<ShaderProgram> CreateShaderProgram(const VisualShaderProgramDescriptor& descriptor) override;

        /** Create texture object. */
        /**@{*/
        SharedRenderResource<Texture1D> CreateTexture(const TextureDescriptor1D& descriptor) override;
        SharedRenderResource<Texture2D> CreateTexture(const TextureDescriptor2D& descriptor) override;
        SharedRenderResource<Texture3D> CreateTexture(const TextureDescriptor3D& descriptor) override;
        /**@}*/

        /** Create uniform buffer object. */
        RenderResource<UniformBuffer> CreateUniformBuffer(const UniformBufferDescriptor& descriptor) override;

        /** Create framed uniform buffer object. */
        RenderResource<FramedUniformBuffer> CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor) override;

        /** Create vertex buffer object. */
        RenderResource<VertexBuffer> CreateVertexBuffer(const VertexBufferDescriptor& descriptor) override;


        /** Update texture data. */
        /**@{*/
        bool UpdateTexture(Texture1D& texture1D, const TextureUpdateDescriptor1D& descriptor) override;
        bool UpdateTexture(Texture2D& texture2D, const TextureUpdateDescriptor2D& descriptor) override;
        bool UpdateTexture(Texture3D& texture3D, const TextureUpdateDescriptor3D& descriptor) override;
        /**@}*/


        /** Destroys render resource.
         *  Resources are not destroyed right away, but instead put in a cleanup queue in order to make sure that resources in use not are destroyed.
         */
        /**@{*/
        void Destroy(DescriptorSet& descriptorSet) override;
        void Destroy(FramedDescriptorSet& framedDescriptorSet) override;
        void Destroy(Framebuffer& framebuffer) override;
        void Destroy(IndexBuffer& indexBuffer) override;
        void Destroy(Pipeline& pipeline) override;
        void Destroy(RenderPass& renderPass) override;
        void Destroy(Sampler1D& sampler1D) override;
        void Destroy(Sampler2D& sampler2D) override;
        void Destroy(Sampler3D& sampler3D) override;
        void Destroy(ShaderProgram& shaderProgram) override;
        void Destroy(Texture1D& texture1D) override;
        void Destroy(Texture2D& texture2D) override;
        void Destroy(Texture3D& texture3D) override;
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




        void DrawFrame(const RenderPassGroup& renderPassGroup) override;



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
        void UpdateUniformBuffer(RenderResource<UniformBuffer>& uniformBuffer, const void* data, const size_t size, const size_t offset) override;

        /** Update uniform buffer data. */
        void UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& framedUniformBuffer, const void* data, const size_t size, const size_t offset) override;

    private:

        /** Renderer creation functions. */
        /**@{*/
        bool LoadRequirements();
        bool LoadInstance(const Version& version);      
        bool LoadSurface();
        bool LoadPhysicalDevice();
        bool LoadLogicalDevice();
        bool LoadMemoryAllocator();
        bool LoadRenderPass();
        bool LoadSwapChain();
        bool LoadCommandPool();
        /**@}*/

        /** Get next cleanup frame index. */
        uint32_t GetNextDestroyerFrameIndex() const;

        /** Texture creation/update helper functions.*/
        /**@{*/
        bool CreateTexture(
            Vulkan::DeviceImage& deviceImage,
            VkImageView& imageView,
            const Vector3ui32& dimensions,
            const void* data,
            const VkDeviceSize dataSize,
            const VkFormat imageFormat,
            const VkFormat internalImageFormat,
            const VkImageType imageType,
            const VkImageViewType imageViewType,
            const VkComponentMapping& componentMapping);

        bool UpdateTexture(
            Vulkan::DeviceImage& deviceImage,
            const uint8_t bytesPerPixel,
            const void* data,
            const Vector3ui32& destinationDimensions,
            const Vector3ui32& destinationOffset);
        /**@}*/

        /** Shader creation and manipulation functions. */
        /**@{*/
        bool CreateVertexInputAttributes(
            const Shader::Visual::InputInterface& inputs,
            std::vector<VkVertexInputAttributeDescription>& attributes, 
            uint32_t& stride) const;
        
        bool CreateDescriptorSetLayouts(
            Vulkan::DescriptorSetLayouts& setLayouts,
            const MappedDescriptorSets& mappedDescriptorSets);
        /**@}*/

        /** Renderer creation variables. */
        /**@{*/
        RenderTarget* m_renderTarget;
        Version m_version;
        RendererCapabilities m_capabilities;
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
        Vulkan::MemoryAllocator m_memoryAllocator;
        Vulkan::ResourceDestroyer m_resourceDestroyer;
        VkRenderPass m_renderPass;
        Vulkan::SwapChain m_swapChain;
        VkSurfaceFormatKHR m_surfaceFormat;
        VkPresentModeKHR m_presentMode;
        VkCommandPool m_commandPool;
        std::vector<VkCommandBuffer> m_commandBuffers;
        VkCommandBuffer* m_currentCommandBuffer;
        bool m_beginDraw;
        uint32_t m_endedDraws;
        VulkanPipeline* m_currentPipeline;
        Shader::GlslGenerator m_glslGenerator;
        /**@}*/
           
    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif