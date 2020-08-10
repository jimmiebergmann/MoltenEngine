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
#include "Molten/Renderer/Vulkan/Vulkan.hpp"
#include "Molten/Renderer/Vulkan/VulkanPipeline.hpp"

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

        VulkanRenderer();

        /**
         * Constructs and creates renderer.
         *
         * @param window[in] Render target window.
         */
        VulkanRenderer(const Window& window, const Version& version, Logger* logger = nullptr);

        ~VulkanRenderer();

        /**
         * Opens renderer by loading and attaching renderer to provided window.
         *
         * @param window Render target window.
         */
        virtual bool Open(const Window& window, const Version& version = Version::None, Logger* logger = nullptr) override;

        /**  Closing renderer. */
        virtual void Close() override;

        /**
         * Resize the framebuffers.
         * Execute this function as soon as the render target's work area is resized.
         */
        virtual void Resize(const Vector2ui32& size) override;

        /** Get backend API type. */
        virtual BackendApi GetBackendApi() const override;

        /** Get renderer API version. */
        virtual Version GetVersion() const override;

        /** Get location of pipeline push constant by id. Id is set in shader script. */
        virtual uint32_t GetPushConstantLocation(Pipeline * pipeline, const uint32_t id) override;


        /** Create framebuffer object. */
        virtual Framebuffer* CreateFramebuffer(const FramebufferDescriptor& descriptor) override;

        /**  Create index buffer object. */
        virtual IndexBuffer* CreateIndexBuffer(const IndexBufferDescriptor& descriptor) override;

        /** Create pipeline object. */
        virtual Pipeline* CreatePipeline(const PipelineDescriptor& descriptor) override;

        /** Create vertex shader stage object out of vertex script. */
        virtual Shader::VertexStage* CreateVertexShaderStage(const Shader::Visual::VertexScript& script) override;

        /** Create fragment shader stage object out of fragment script. */
        virtual Shader::FragmentStage* CreateFragmentShaderStage(const Shader::Visual::FragmentScript& script) override;

        /** Create texture object. */
        virtual Texture* CreateTexture() override;

        /** Create uniform buffer object. */
        virtual UniformBlock* CreateUniformBlock(const UniformBlockDescriptor& descriptor) override;

        /** Create uniform buffer object. */
        virtual UniformBuffer* CreateUniformBuffer(const UniformBufferDescriptor& descriptor) override;

        /** Create vertex buffer object. */
        virtual VertexBuffer* CreateVertexBuffer(const VertexBufferDescriptor& descriptor) override;


        /** Destroy framebuffer object. */
        virtual void DestroyFramebuffer(Framebuffer* framebuffer) override;

        /** Destroy index buffer object. */
        virtual void DestroyIndexBuffer(IndexBuffer* indexBuffer) override;

        /** Destroy pipeline object. */
        virtual void DestroyPipeline(Pipeline* pipeline) override;

        /** Destroy vertex shader stage object. */
        virtual void DestroyVertexShaderStage(Shader::VertexStage* shader) override;

        /** Destroy fragment shader stage object. */
        virtual void DestroyFragmentShaderStage(Shader::FragmentStage* shader) override;

        /** Destroy texture object. */
        virtual void DestroyTexture(Texture* texture) override;

        /** Destroy uniform block object. */
        virtual void DestroyUniformBlock(UniformBlock* uniformBlock) override;

        /** Destroy uniform buffer object. */
        virtual void DestroyUniformBuffer(UniformBuffer* uniformBuffer) override;

        /** Destroy vertex buffer object. */
        virtual void DestroyVertexBuffer(VertexBuffer* vertexBuffer) override;


        /** Bind pipeline to draw queue. */
        virtual void BindPipeline(Pipeline* pipeline) override;

        /** Bind pipeline to draw queue. */
        virtual void BindUniformBlock(UniformBlock* uniformBlock, const uint32_t offset = 0) override;


        /** Begin and initialize rendering to framebuffers. */
        virtual void BeginDraw() override;

        /** Draw vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(VertexBuffer* vertexBuffer) override;

        /** Draw indexed vertex buffer, using the current bound pipeline. */
        virtual void DrawVertexBuffer(IndexBuffer* indexBuffer, VertexBuffer* vertexBuffer) override;

        /**
         * Push constant values to shader stage.
         * This function call has no effect if provided id argument is greater than the number of push constants in pipeline.
         *
         * @param id Id of push constant to update. This id can be shared between multiple shader stages.
         */
         /**@{*/
        virtual void PushConstant(const uint32_t location, const bool& value) override;
        virtual void PushConstant(const uint32_t location, const int32_t& value) override;
        virtual void PushConstant(const uint32_t location, const float& value) override;
        virtual void PushConstant(const uint32_t location, const Vector2f32& value) override;
        virtual void PushConstant(const uint32_t location, const Vector3f32& value) override;
        virtual void PushConstant(const uint32_t location, const Vector4f32& value) override;
        virtual void PushConstant(const uint32_t location, const Matrix4x4f32& value) override;
        /**@}*/


        /** Finalize and present rendering. */
        virtual void EndDraw() override;


        /** Sleep until the graphical device is ready. */
        virtual void WaitForDevice() override;

        /** Update uniform buffer data. */
        virtual void UpdateUniformBuffer(UniformBuffer* uniformBuffer, const size_t offset, const size_t size, const void* data) override;

    private:

        struct DebugMessenger
        {
            DebugMessenger();
            void Clear();

            VkDebugUtilsMessengerEXT messenger;
            PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
            PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
        };

        struct SwapChainSupport
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        struct PhysicalDevice
        {
            PhysicalDevice();
            explicit PhysicalDevice(VkPhysicalDevice device);
            PhysicalDevice(VkPhysicalDevice device, uint32_t graphicsQueueIndex, uint32_t presentQueueIndex);
            void Clear();

            VkPhysicalDevice device;
            uint32_t graphicsQueueIndex;
            uint32_t presentQueueIndex;
            SwapChainSupport swapChainSupport;
        };

        PFN_vkVoidFunction GetVulkanFunction(const char* functionName) const;
        bool LoadInstance(const Version& version);
        bool GetRequiredExtensions(std::vector<std::string>& extensions, const bool requestDebugger) const;
        bool LoadDebugger(VkInstanceCreateInfo& instanceInfo, VkDebugUtilsMessengerCreateInfoEXT& debugMessageInfo);
        bool LoadSurface();
        bool LoadPhysicalDevice();
        bool ScorePhysicalDevice(PhysicalDevice& physicalDevice, uint32_t & score);
        bool CheckDeviceExtensionSupport(PhysicalDevice & physicalDevice);
        bool FetchSwapChainSupport(PhysicalDevice& physicalDevice);
        bool LoadLogicalDevice();
        bool LoadSwapChain();
        bool LoadImageViews();
        bool LoadRenderPass();
        bool LoadPresentFramebuffer();
        Framebuffer* CreateFramebuffer(const VkImageView& imageView, const Vector2ui32 size);
        bool LoadCommandPool();
        bool LoadSyncObjects();
        bool RecreateSwapChain();
        void UnloadSwapchain();
        bool FindPhysicalDeviceMemoryType(uint32_t& index, const uint32_t filter, const VkMemoryPropertyFlags properties);
        bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory);
        void CopyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size);
        bool CreateVertexInputAttributes(const Shader::Visual::InputStructure& inputs, std::vector<VkVertexInputAttributeDescription>& attributes, uint32_t& stride);
        bool CreateDescriptorSetLayouts(const Shader::Visual::VertexScript& vertexScript, const Shader::Visual::FragmentScript& fragmentScript, std::vector<VkDescriptorSetLayout>& setLayouts);
        bool CreatePushConstants(
            const Shader::Visual::VertexScript& vertexScript, 
            const Shader::Visual::FragmentScript& fragmentScript, 
            std::vector<VkPushConstantRange>& pushConstants, 
            VulkanPipeline::PushConstantLocations& pipelinePushConstantLocations,
            VulkanPipeline::PushConstants& pipelinePushConstants);

        template<typename T>
        void InternalPushConstant(const uint32_t location, const T& value);

        Logger* m_logger;
        Version m_version;
        const Window* m_renderTarget;
        VkInstance m_instance;
        std::vector<const char*> m_validationLayers;
        std::vector<const char*> m_deviceExtensions;
        DebugMessenger m_debugMessenger;
        VkSurfaceKHR m_surface;
        PhysicalDevice m_physicalDevice;
        VkDevice m_logicalDevice;
        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;       
        VkSwapchainKHR m_swapChain;
        VkFormat m_swapChainImageFormat;
        VkExtent2D m_swapChainExtent;
        std::vector<VkImage> m_swapChainImages;
        std::vector<VkImageView> m_swapChainImageViews;
        VkRenderPass m_renderPass;
        std::vector<VulkanFramebuffer*> m_presentFramebuffers;
        VkCommandPool m_commandPool;
        std::vector<VkCommandBuffer> m_commandBuffers;
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;
        std::vector<VkFence> m_imagesInFlight;
        size_t m_maxFramesInFlight;
        size_t m_currentFrame;

        bool m_resized;
        bool m_beginDraw;
        uint32_t m_currentImageIndex;
        VkCommandBuffer* m_currentCommandBuffer;
        VkFramebuffer m_currentFramebuffer;
        VulkanPipeline * m_currentPipeline;
        
        
    };

}

#include "Molten/Renderer/Vulkan/VulkanRenderer.inl"

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif