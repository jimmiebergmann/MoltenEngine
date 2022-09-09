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

#ifndef MOLTEN_EDITOR_VIEWPORT_SCENEVIEWPORT_HPP
#define MOLTEN_EDITOR_VIEWPORT_SCENEVIEWPORT_HPP

#include "Molten/Editor/Gui/Themes/EditorTheme.hpp"
#include "Molten/Utility/BufferCapacityPolicy.hpp"
#include "Molten/Graphics/Renderer.hpp"
#include "Molten/System/Time.hpp"
#include <array>
#include <optional>


namespace Molten::Editor
{

    struct SceneViewportDescriptor
    {
        Renderer& renderer;
        Gui::Viewport<Gui::EditorTheme>& viewportWidget;
        Logger* logger = nullptr;
    };

    class SceneViewport
    {

    public:

        static inline const auto defaultBufferCapacityPolicy = std::array<BufferCapacityPolicy, 2>{
            BufferCapacityPolicy{ BufferCapacityScalarPolicy{ 100 } },
            BufferCapacityPolicy{ BufferCapacityScalarPolicy{ 100 } }
        };

        static std::unique_ptr<SceneViewport> Create(const SceneViewportDescriptor& descriptor);

        ~SceneViewport() = default;

        SceneViewport(const SceneViewport&) = delete;
        SceneViewport(SceneViewport&&)  noexcept = delete;
        SceneViewport& operator =(const SceneViewport&) = delete;       
        SceneViewport& operator =(SceneViewport&&) noexcept = delete;

        bool Resize(const Vector2ui32 size);

        SharedRenderResource<FramedTexture2D> colorTexture = {};
        SharedRenderResource<FramedTexture2D> depthTexture = {};
        std::array<BufferCapacityPolicy, 2> bufferCapacityPolicy = defaultBufferCapacityPolicy;
        RenderPassUpdateDescriptor renderPassUpdateDesc = {};
        SharedRenderResource<RenderPass> renderPass = {};
        RenderResource<Pipeline> pipeline = {};
        SharedRenderResource<ShaderProgram> shaderProgram = {};
        RenderResource<VertexBuffer> vertexBuffer = {};
        RenderResource<IndexBuffer> indexBuffer = {};
        uint32_t projectionLocation = 0;

    private:

        SceneViewport(
            Renderer& renderer,
            Gui::Viewport<Gui::EditorTheme>& viewportWidget);

        void Draw(CommandBuffer& commandBuffer, Time deltaTime);

        Renderer& m_renderer;
        Gui::Viewport<Gui::EditorTheme>& m_viewportWidget;

    };

}

#endif