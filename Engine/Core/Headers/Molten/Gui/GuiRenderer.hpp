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

#ifndef MOLTEN_CORE_GUI_GUIRENDERER_HPP
#define MOLTEN_CORE_GUI_GUIRENDERER_HPP

#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Matrix.hpp"

namespace Molten
{
    class Renderer;
    class Pipeline;  
    class VertexBuffer;
    class IndexBuffer;
}

namespace Molten::Shader
{
    class VertexStage;
    class FragmentStage;
}

namespace Molten::Shader::Visual
{
    class VertexScript;
    class FragmentScript;
}

namespace Molten::Gui
{

    class MOLTEN_API Renderer
    {

    public:

        Renderer();
        ~Renderer();

        void Open(Molten::Renderer* backendRenderer);
        void Close();

        void DrawRect(const Vector2f32& position, const Vector2f32& size, const Vector4f32& color);

    private:

        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator =(const Renderer&) = delete;
        Renderer& operator =(Renderer&&) = delete;

        struct RenderInstance
        {
            RenderInstance();

            Pipeline* pipeline;
            VertexBuffer* vertexBuffer;
            IndexBuffer* indexBuffer;
            Shader::Visual::VertexScript* vertexScript;
            Shader::Visual::FragmentScript* fragmentScript;
            Shader::VertexStage* vertexStage;
            Shader::FragmentStage* fragmentStage;
        };

        void LoadRectRenderInstance();
        void DestroyRenderInstance(RenderInstance& instance);

        Molten::Renderer* m_backendRenderer;
        Matrix4x4f32 m_projection;
        RenderInstance m_rect;

    };

}

#endif