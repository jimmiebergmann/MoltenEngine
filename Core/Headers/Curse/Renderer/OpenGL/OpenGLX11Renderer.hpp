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

#ifndef CURSE_CORE_RENDERER_OPENGL_OPENGLX11RENDERER_HPP
#define CURSE_CORE_RENDERER_OPENGL_OPENGLX11RENDERER_HPP

#include "Curse/Core.hpp"

#if defined(CURSE_ENABLE_OPENGL)
#if CURSE_PLATFORM == CURSE_PLATFORM_LINUX

#include "Curse/Renderer/Renderer.hpp"


namespace Curse
{

    /**
    * @brief OpenGL renderer class for Win32.
    */
    class CURSE_API OpenGLX11Renderer : public Renderer
    {

    public:

        /**
        * @brief Constructor.
        */
        OpenGLX11Renderer();

        /**
        * @brief Constructs and creates renderer.
        *
        * @param window[in] Render target window.
        */
        OpenGLX11Renderer(const Window& window, const Version& version, Logger* logger = nullptr);

        /**
        * @brief Virtual destructor.
        */
        ~OpenGLX11Renderer();

        /**
        * @brief Opens renderer.
        *
        * @param window[in] Render target window.
        */
        virtual bool Open(const Window& window, const Version& version = Version::None, Logger* logger = nullptr) override;

        /**
        * @brief Closing renderer.
        */
        virtual void Close() override;

        /**
        * @brief Resize the framebuffers.
        *        Execute this function as soon as the render target's work area is resized.
        */
        virtual void Resize(const Vector2ui32& size) override;

        /**
        * @brief Get backend API type.
        */
        virtual BackendApi GetBackendApi() const override;

        /**
        * @brief Get renderer API version.
        */
        virtual Version GetVersion() const override;


        /**
        * @brief Create framebuffer object.
        */
        virtual Framebuffer* CreateFramebuffer(const FramebufferDescriptor& descriptor) override;

        /**
        * @brief Create index buffer object.
        */
        virtual IndexBuffer* CreateIndexBuffer(const IndexBufferDescriptor& descriptor) override;

        /**
        * @brief Create pipeline object.
        */
        virtual Pipeline* CreatePipeline(const PipelineDescriptor& descriptor) override;

        /**
        * @brief Create vertex shader stage object out of vertex script.
        */
        virtual Shader::VertexStage* CreateVertexShaderStage(const Shader::Visual::VertexScript& script) override;

        /**
        * @brief Create fragment shader stage object out of fragment script.
        */
        virtual Shader::FragmentStage* CreateFragmentShaderStage(const Shader::Visual::FragmentScript& script) override;

        /**
        * @brief Create texture object.
        */
        virtual Texture* CreateTexture() override;

        /**
        * @brief Create uniform buffer object.
        */
        virtual UniformBlock* CreateUniformBlock(const UniformBlockDescriptor& descriptor) override;

        /**
        * @brief Create uniform buffer object.
        */
        virtual UniformBuffer* CreateUniformBuffer(const UniformBufferDescriptor& descriptor) override;

        /**
        * @brief Create vertex buffer object.
        */
        virtual VertexBuffer* CreateVertexBuffer(const VertexBufferDescriptor& descriptor) override;


        /**
        * @brief Destroy framebuffer object.
        */
        virtual void DestroyFramebuffer(Framebuffer* framebuffer) override;

        /**
        * @brief Destroy index buffer object.
        */
        virtual void DestroyIndexBuffer(IndexBuffer* indexBuffer) override;

        /**
        * @brief Destroy pipeline object.
        */
        virtual void DestroyPipeline(Pipeline* pipeline) override;

        /**
        * @brief Destroy vertex shader stage object.
        */
        virtual void DestroyVertexShaderStage(Shader::VertexStage* shader) override;

        /**
        * @brief Destroy fragment shader stage object.
        */
        virtual void DestroyFragmentShaderStage(Shader::FragmentStage* shader) override;

        /**
        * @brief Destroy texture object.
        */
        virtual void DestroyTexture(Texture* texture) override;

        /**
        * @brief Destroy uniform block object.
        */
        virtual void DestroyUniformBlock(UniformBlock* uniformBlock) override;

        /**
        * @brief Destroy uniform buffer object.
        */
        virtual void DestroyUniformBuffer(UniformBuffer* uniformBuffer) override;

        /**
        * @brief Destroy vertex buffer object.
        */
        virtual void DestroyVertexBuffer(VertexBuffer* vertexBuffer) override;


        /**
        * @brief Bind pipeline to draw queue.
        */
        virtual void BindPipeline(Pipeline* pipeline) override;

        /**
        * @brief Bind pipeline to draw queue.
        */
        virtual void BindUniformBlock(UniformBlock* uniformBlock, const uint32_t offset = 0) override;


        /**
        * @brief Begin and initialize rendering to framebuffers.
        */
        virtual void BeginDraw() override;

        /**
        * @brief Draw vertex buffer, using the current bound pipeline.
        */
        virtual void DrawVertexBuffer(VertexBuffer* vertexBuffer) override;

        /**
        * @brief * @brief Draw indexed vertex buffer, using the current bound pipeline.
        */
        virtual void DrawVertexBuffer(IndexBuffer* indexBuffer, VertexBuffer* vertexBuffer) override;

        /**
        * @brie Push constant values to shader stage.
        */
        virtual void PushShaderConstants(Shader::Type stage, const uint32_t offset, const uint32_t size, const void* data) override;

        /**
        * @brief Finalize and present rendering.
        */
        virtual void EndDraw() override;


        /**
        * @brief Sleep until the graphical device is ready.
        */
        virtual void WaitForDevice() override;

        /**
        * @brief Update uniform buffer data.
        */
        virtual void UpdateUniformBuffer(UniformBuffer* uniformBuffer, const size_t offset, const size_t size, const void* data) override;

    private:

        /**
        * @brief Open opengl context by provided version.
        *
        * @param version[in] Opened version.
        *
        * @throw Exception If failed to open provided version.
        */
       // bool OpenVersion(HDC deviceContext, const Version& version);

        /**
        * @brief Open the best available opengl context.
        *
        * @param version[out] Opened version.
        *
        * @throw Exception If failed to open any opengl context.
        */
        //void OpenBestVersion(HDC deviceContext, Version& version);

        Version m_version;

    };

}

#endif

#endif

#endif
