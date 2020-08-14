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

#ifndef MOLTEN_CORE_RENDERER_OPENGL_OPENGLWIN32RENDERER_HPP
#define MOLTEN_CORE_RENDERER_OPENGL_OPENGLWIN32RENDERER_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_OPENGL)
#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Platform/Win32Headers.hpp"

namespace Molten
{

    /**
    * @brief OpenGL renderer class for Win32.
    */
    class MOLTEN_API OpenGLWin32Renderer : public Renderer
    {

    public:

        OpenGLWin32Renderer();

        /**
         * Constructs and creates renderer.
         *
         * @param window[in] Render target window.
         */
        OpenGLWin32Renderer(const Window& window, const Version& version, Logger* logger = nullptr);

        ~OpenGLWin32Renderer();

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
        virtual uint32_t GetPushConstantLocation(Pipeline* pipeline, const uint32_t id) override;


        /** Create framebuffer object. */
        virtual Framebuffer* CreateFramebuffer(const FramebufferDescriptor& descriptor) override;

        /**  Create index buffer object. */
        virtual IndexBuffer* CreateIndexBuffer(const IndexBufferDescriptor& descriptor) override;

        /** Create pipeline object. */
        virtual Pipeline* CreatePipeline(const PipelineDescriptor& descriptor) override;

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

        /**
        * @brief Open opengl context by provided version.
        *
        * @param version[in] Opened version.
        *
        * @throw Exception If failed to open provided version.
        */
        bool OpenVersion(HDC deviceContext, const Version& version);

        /**
        * @brief Open the best available opengl context.
        *
        * @param version[out] Opened version.
        *
        * @throw Exception If failed to open any opengl context.
        */
        void OpenBestVersion(HDC deviceContext, Version& version);

        Version m_version;
        HDC m_deviceContext;
        HGLRC m_context;

    };

}

#endif

#endif

#endif
