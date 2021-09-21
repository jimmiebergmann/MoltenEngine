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

#ifndef MOLTEN_CORE_GUI_CANVASRENDERER_HPP
#define MOLTEN_CORE_GUI_CANVASRENDERER_HPP

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Matrix.hpp"
#include "Molten/Math/Bounds.hpp"
#include "Molten/Renderer/RenderResource.hpp"
#include "Molten/Renderer/Sampler.hpp"
#include "Molten/Renderer/ShaderProgram.hpp"
#include "Molten/Renderer/Texture.hpp"
#include "Molten/Renderer/VertexBuffer.hpp"
#include "Molten/Renderer/Font.hpp"

namespace Molten
{
    class Renderer;
    class CommandBuffer;
    class Pipeline;  
    class VertexBuffer;
    class IndexBuffer;
    class DescriptorSet;
    class FramedDescriptorSet;
    class Logger;
}

namespace Molten::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Molten::Gui
{

    class CanvasRenderer;


    struct MOLTEN_API CanvasRendererTexture
    {

        CanvasRendererTexture() = default;
        CanvasRendererTexture(const CanvasRendererTexture&) = delete;
        CanvasRendererTexture(CanvasRendererTexture&&) noexcept = default;

        CanvasRendererTexture& operator =(const CanvasRendererTexture&) = delete;
        CanvasRendererTexture& operator =(CanvasRendererTexture&&) noexcept = default;

        SharedRenderResource<Texture2D> texture;
        Vector2ui32 dimensions;
        RenderResource<DescriptorSet> descriptorSet;

    };

    struct MOLTEN_API CanvasRendererFramedTexture
    {

        CanvasRendererFramedTexture() = default;
        ~CanvasRendererFramedTexture() = default;
        CanvasRendererFramedTexture(const CanvasRendererTexture&) = delete;
        CanvasRendererFramedTexture(CanvasRendererFramedTexture&&) noexcept = default;

        CanvasRendererFramedTexture& operator =(const CanvasRendererFramedTexture&) = delete;
        CanvasRendererFramedTexture& operator =(CanvasRendererFramedTexture&&) noexcept = default;

        SharedRenderResource<FramedTexture2D> framedTexture;
        RenderResource<FramedDescriptorSet> framedDescriptorSet;

    };

    struct MOLTEN_API CanvasRendererFontSequence
    {
        struct Group
        {
            Group() = default;
            Group(const Group&) = delete;
            Group(Group&&) noexcept = default;
            Group& operator =(const Group&) = delete;
            Group& operator =(Group&&) noexcept = default;

            CanvasRendererTexture* texture;
            RenderResource<VertexBuffer> vertexBuffer;
        };

        CanvasRendererFontSequence() = default;
        CanvasRendererFontSequence(const CanvasRendererFontSequence&) = delete;
        CanvasRendererFontSequence(CanvasRendererFontSequence&&) noexcept = default;
        CanvasRendererFontSequence& operator =(const CanvasRendererFontSequence&) = delete;
        CanvasRendererFontSequence& operator =(CanvasRendererFontSequence&&) noexcept = default;

        std::vector<Group> groups;
    };

    class MOLTEN_API CanvasRenderer
    {

    public:

        static CanvasRendererPointer Create(Renderer& renderer, Logger * logger = nullptr, const Vector2f32& size = { 0.0f, 0.0f });

        CanvasRenderer(Renderer& renderer, Logger* logger = nullptr, const Vector2f32& size = {0.0f, 0.0f});
        ~CanvasRenderer();

        CanvasRenderer(const CanvasRenderer&) = delete;
        CanvasRenderer(CanvasRenderer&&) = delete;
        CanvasRenderer& operator =(const CanvasRenderer&) = delete;
        CanvasRenderer& operator =(CanvasRenderer&&) = delete;

        void Close();

        void Resize(const Vector2f32& size);

        CanvasRendererTexture CreateTexture(const TextureDescriptor2D& textureDescriptor);
        bool UpdateTexture(CanvasRendererTexture& texture, const TextureUpdateDescriptor2D& textureUpdateDescriptor);

        CanvasRendererFramedTexture CreateFramedTexture(SharedRenderResource<FramedTexture2D> framedTexture);

        CanvasRendererFontSequence CreateFontSequence(FontGroupedSequence& fontGroupedSequence);

        void SetCommandBuffer(CommandBuffer& commandBuffer);

        void DrawRect(const Bounds2f32& bounds, const Vector4f32& color);

        void DrawRect(const Bounds2f32& bounds, CanvasRendererTexture& texture);

        void DrawRect(const Bounds2f32& bounds, const Bounds2f32& textureCoords, CanvasRendererTexture& texture);

        void DrawRect(const Bounds2f32& bounds, const Bounds2f32& textureCoords, CanvasRendererFramedTexture& framedtexture);

        void DrawFontSequence(const Vector2f32& position, CanvasRendererFontSequence& fontSequence);

    private:

        struct ColoredRectData
        {
            ColoredRectData();

            RenderResource<Pipeline> pipeline;
            RenderResource<VertexBuffer> vertexBuffer;
            RenderResource<IndexBuffer> indexBuffer;

            uint32_t projectionLocation;
            uint32_t positionLocation;
            uint32_t sizeLocation;
            uint32_t colorLocation;
        };

        struct TexturedRectData
        {
            TexturedRectData();

            RenderResource<Pipeline> pipeline;
            RenderResource<VertexBuffer> vertexBuffer;
            RenderResource<IndexBuffer> indexBuffer;

            uint32_t projectionLocation;
            uint32_t positionLocation;
            uint32_t sizeLocation;
            uint32_t uvPositionLocation;
            uint32_t uvSizeLocation;
        };

        struct FontRenderData
        {
            FontRenderData();

            RenderResource<Pipeline> pipeline;

            uint32_t projectionLocation;
            uint32_t positionLocation;
        };

        static void DestroyColoredRect(ColoredRectData& data);
        static void DestroyTexturedRect(TexturedRectData& data);

        void LoadColoredRect();
        void LoadTexturedRect();
        void LoadFontRenderData();

        //Logger* m_logger;
        Renderer& m_backendRenderer;
        CommandBuffer* m_commandBuffer;
        Matrix4x4f32 m_projection;
        SharedRenderResource<Sampler2D> m_sampler2D;
        ColoredRectData m_coloredRect;
        TexturedRectData m_texturedRect;
        FontRenderData m_fontRenderData;

    };

}

#endif