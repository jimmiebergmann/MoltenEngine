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
#include "Molten/System/Resource.hpp"
#include <stack>

namespace Molten
{
    class Renderer;
    class Pipeline;  
    class VertexBuffer;
    class IndexBuffer;
    class Texture;
    class Logger;
}

namespace Molten::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Molten::Gui
{

    class MOLTEN_API CanvasRenderer
    {

    public:

        static CanvasRendererPointer Create(Molten::Renderer& renderer, Logger * logger = nullptr, const Vector2f32& size = { 0.0f, 0.0f });

        CanvasRenderer(Molten::Renderer& renderer, Logger* logger = nullptr, const Vector2f32& size = {0.0f, 0.0f});
        ~CanvasRenderer();

        void Close();

        void Resize(const Vector2f32& size);

        void BeginDraw();

        void PushPosition(const Vector2f32& position);

        void PopPosition();

        void DrawRect(const Bounds2f32& bounds, const Vector4f32& color);

        void EndDraw();

    private:

        CanvasRenderer(const CanvasRenderer&) = delete;
        CanvasRenderer(CanvasRenderer&&) = delete;
        CanvasRenderer& operator =(const CanvasRenderer&) = delete;
        CanvasRenderer& operator =(CanvasRenderer&&) = delete;

        struct RenderInstance
        {
            RenderInstance();

            Resource<Pipeline> pipeline;
            Resource<VertexBuffer> vertexBuffer;
            Resource<IndexBuffer> indexBuffer;
            Resource<Texture> texture;
            Shader::Visual::VertexScript* vertexScript;
            Shader::Visual::FragmentScript* fragmentScript;
            uint32_t projectionLocation;
            uint32_t positionLocation;
            uint32_t sizeLocation;
            uint32_t colorLocation;
        };

        void LoadRectRenderInstance();
        static void DestroyRenderInstance(RenderInstance& instance);

        using PositionStack = std::stack<Vector2f32>;

        Logger* m_logger;
        Molten::Renderer& m_backendRenderer;
        Matrix4x4f32 m_projection;
        RenderInstance m_rectInstance;

        PositionStack m_positionStack;

    };

}

#endif