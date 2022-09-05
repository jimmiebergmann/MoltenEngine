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

#ifndef MOLTEN_GRAPHICS_VULKAN_VULKANRENDERER_HPP
#define MOLTEN_GRAPHICS_VULKAN_VULKANRENDERER_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Graphics/Renderer.hpp"
#include "Molten/Graphics/Vulkan/VulkanPipeline.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanMemoryAllocator.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanResourceDestroyer.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanInstance.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanSurface.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanPhysicalDevice.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanPhysicalDeviceFeatures.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanLogicalDevice.hpp"
#include "Molten/Graphics/Vulkan/Utility/VulkanSwapChain.hpp"
#include "Molten/Shader/Visual/VisualShaderScript.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{
    struct DeviceImage;
}

namespace Molten
{
    class VulkanRenderPass;

    /**
    * @brief Vulkan renderer class.
    */
    class MOLTEN_GRAPHICS_API VulkanRenderer final : public Renderer
    {

    public:

        /** Constructor. */
        VulkanRenderer();

        /** Virtual destructor. */
        ~VulkanRenderer() override; // Replace by noexcept version.

        /** Opens renderer by loading and attaching renderer to provided window. */
        bool Open(const RendererDescriptor& descriptor) override;

        /** Closing renderer. */
        void Close() override;

        /** Checks if renderer is open Same value returned as last call to Open. */
        [[nodiscard]] bool IsOpen() const override;

        /** Resize the framebuffers.
         *  Execute this function as soon as the render target's work area is resized.
         */
        void Resize(const Vector2ui32& size) override;

        /** Get backend API type. */
        [[nodiscard]] BackendApi GetBackendApi() const override;

        /** Get renderer API version. */
        [[nodiscard]] Version GetVersion() const override;

        /** Get supported capabilities and features of renderer. */
        [[nodiscard]] const RendererCapabilities& GetCapabilities() const override;

        /** Get location of pipeline push constant by id. Id is set in shader script. */
        [[nodiscard]] uint32_t GetPushConstantLocation(Pipeline& pipeline, const uint32_t id) override;

        [[nodiscard]] SharedRenderResource<RenderPass> GetSwapChainRenderPass() override;


        /** Create descriptor set object. */
        [[nodiscard]] RenderResource<DescriptorSet> CreateDescriptorSet(const DescriptorSetDescriptor& descriptor) override;

        /** Create framed descriptor set object. */
        [[nodiscard]] RenderResource<FramedDescriptorSet> CreateFramedDescriptorSet(const FramedDescriptorSetDescriptor& descriptor) override;

        /**  Create index buffer object. */
        [[nodiscard]] RenderResource<IndexBuffer> CreateIndexBuffer(const IndexBufferDescriptor& descriptor) override;

        /** Create pipeline object. */
        [[nodiscard]] RenderResource<Pipeline> CreatePipeline(const PipelineDescriptor& descriptor) override;

        /** Create render pass object. */
        [[nodiscard]] SharedRenderResource<RenderPass> CreateRenderPass(const RenderPassDescriptor& descriptor) override;

        /** Create sampler object. */
        /**@{*/
        [[nodiscard]] SharedRenderResource<Sampler1D> CreateSampler(const SamplerDescriptor1D& descriptor) override;
        [[nodiscard]] SharedRenderResource<Sampler2D> CreateSampler(const SamplerDescriptor2D& descriptor) override;
        [[nodiscard]] SharedRenderResource<Sampler3D> CreateSampler(const SamplerDescriptor3D& descriptor) override;
        /**@}*/

        /** Create shader module object. */
        [[nodiscard]] SharedRenderResource<ShaderProgram> CreateShaderProgram(const VisualShaderProgramDescriptor& descriptor) override;

        /** Create texture object. */
        /**@{*/
        [[nodiscard]] SharedRenderResource<Texture1D> CreateTexture(const TextureDescriptor1D& descriptor) override;
        [[nodiscard]] SharedRenderResource<Texture2D> CreateTexture(const TextureDescriptor2D& descriptor) override;
        [[nodiscard]] SharedRenderResource<Texture3D> CreateTexture(const TextureDescriptor3D& descriptor) override;
        /**@}*/

        /** Create framed texture object. */
        /**@{*/
        [[nodiscard]] SharedRenderResource<FramedTexture1D> CreateFramedTexture(const TextureDescriptor1D& descriptor) override;
        [[nodiscard]] SharedRenderResource<FramedTexture2D> CreateFramedTexture(const TextureDescriptor2D& descriptor) override;
        [[nodiscard]] SharedRenderResource<FramedTexture3D> CreateFramedTexture(const TextureDescriptor3D& descriptor) override;
        /**@}*/

        /** Create uniform buffer object. */
        [[nodiscard]] RenderResource<UniformBuffer> CreateUniformBuffer(const UniformBufferDescriptor& descriptor) override;

        /** Create framed uniform buffer object. */
        [[nodiscard]] RenderResource<FramedUniformBuffer> CreateFramedUniformBuffer(const FramedUniformBufferDescriptor& descriptor) override;

        /** Create vertex buffer object. */
        [[nodiscard]] RenderResource<VertexBuffer> CreateVertexBuffer(const VertexBufferDescriptor& descriptor) override;


        /** Update render pass object. */
        [[nodiscard]] bool UpdateRenderPass(RenderPass& renderPass, const RenderPassUpdateDescriptor& descriptor) override;

        /** Update texture data. */
        /**@{*/
        bool UpdateTexture(Texture1D& texture1D, const TextureUpdateDescriptor1D& descriptor) override;
        bool UpdateTexture(Texture2D& texture2D, const TextureUpdateDescriptor2D& descriptor) override;
        bool UpdateTexture(Texture3D& texture3D, const TextureUpdateDescriptor3D& descriptor) override;
        /**@}*/

        /** Update uniform buffer data. */
        void UpdateUniformBuffer(RenderResource<UniformBuffer>& uniformBuffer, const void* data, const size_t size, const size_t offset) override;

        /** Update uniform buffer data. */
        void UpdateFramedUniformBuffer(RenderResource<FramedUniformBuffer>& framedUniformBuffer, const void* data, const size_t size, const size_t offset) override;


        /** Draw next frame by one or multiple render passes. */
        bool DrawFrame(const RenderPasses& renderPasses) override;


        /** Destroys render resource.
         *  Resources are not destroyed right away, but instead put in a cleanup queue in order to make sure that resources in use not are destroyed.
         */
        /**@{*/
        void Destroy(DescriptorSet& descriptorSet) override;
        void Destroy(FramedDescriptorSet& framedDescriptorSet) override;
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
        void Destroy(FramedTexture1D& framedTexture1D) override;
        void Destroy(FramedTexture2D& framedTexture2D) override;
        void Destroy(FramedTexture3D& framedTexture3D) override;
        void Destroy(UniformBuffer& uniformBuffer) override;
        void Destroy(FramedUniformBuffer& framedUniformBuffer) override;
        void Destroy(VertexBuffer& vertexBuffer) override;
        /**@}*/


        /** Sleep until the graphical device is ready. */
        void WaitForDevice() override;

    private:

        /** Renderer creation functions. */
        /**@{*/
        [[nodiscard]] bool LoadRequirements();
        [[nodiscard]] bool LoadInstance(const Version& version);
        [[nodiscard]] bool LoadSurface();
        [[nodiscard]] bool LoadPhysicalDevice();
        [[nodiscard]] bool LoadLogicalDevice();
        [[nodiscard]] bool LoadMemoryAllocator();
        [[nodiscard]] bool LoadSwapChain();
        [[nodiscard]] bool LoadCommandPool();
        /**@}*/

        /** Renderer recreation functions and load functions with vulkan result. */
        /**@{*/
        [[nodiscard]] bool ReloadSwapChain();
        Vulkan::Result<> LoadSwapChainRenderPass();
        Vulkan::Result<> ReloadSwapChainRenderPass();
        /** @}*/

        /** Get next cleanup frame index. */
        [[nodiscard]] uint32_t GetNextDestroyerFrameIndex() const;

        /** Texture creation/update helper functions.*/
        /**@{*/
        [[nodiscard]] bool CreateTexture(
            Vulkan::DeviceImage& deviceImage,
            VkImageView& imageView,
            const Vector3ui32& dimensions,
            const void* data,
            const VkDeviceSize dataSize,
            const VkImageLayout layout,
            const VkFormat imageFormat,
            const VkFormat internalImageFormat,
            const VkImageType imageType,
            const VkImageViewType imageViewType,
            const VkComponentMapping& componentMapping);

        [[nodiscard]] bool CreateFramedTexture(
            VulkanTextureFrames & frames,
            const Vector3ui32& dimensions,
            const void* data,
            const VkDeviceSize dataSize,
            const TextureType textureType,
            const VkImageLayout layout,
            const VkFormat imageFormat,
            const VkFormat internalImageFormat,
            const VkImageType imageType,
            const VkImageViewType imageViewType,
            const VkComponentMapping& componentMapping);

        [[nodiscard]] bool UpdateTexture(
            Vulkan::DeviceImage& deviceImage,
            const uint8_t bytesPerPixel,
            const void* data,
            const Vector3ui32& destinationDimensions,
            const Vector3ui32& destinationOffset);
        /**@}*/

        /** Shader creation and manipulation functions. */
        /**@{*/
        [[nodiscard]] bool CreateVertexInputAttributes(
            const Shader::Visual::InputInterface& inputs,
            std::vector<VkVertexInputAttributeDescription>& attributes, 
            uint32_t& stride) const;
        
        [[nodiscard]] bool CreateDescriptorSetLayouts(
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
        Vulkan::SwapChain m_swapChain;
        SharedRenderResource<VulkanRenderPass> m_swapChainRenderPass;
        VkSurfaceFormatKHR m_surfaceFormat;
        VkPresentModeKHR m_presentMode;
        VkCommandPool m_commandPool;
        uint32_t m_drawFrameCount;
        bool m_drawingFrame;
        std::vector<VulkanRenderPass*> m_recordedRenderPasses;
           
    };

}

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif