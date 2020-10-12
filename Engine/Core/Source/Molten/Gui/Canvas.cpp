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

#include "Molten/Gui/Canvas.hpp"
#include "Molten/Gui/CanvasRenderer.hpp"

namespace Molten::Gui
{

    Canvas::Canvas(CanvasRendererPointer renderer) :
        m_renderer(renderer),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f)
    {}

    Canvas::~Canvas()
    {}

    void Canvas::SetRenderer(CanvasRendererPointer renderer)
    {
        m_renderer = renderer;
    }

    CanvasRendererPointer Canvas::GetRenderer()
    {
        return m_renderer;
    }
    const CanvasRendererPointer Canvas::GetRenderer() const
    {
        return m_renderer;
    }

    void Canvas::Update(const Time& deltaTime)
    {
        for (auto& layers : m_activeLayers)
        {
            layers->Update(deltaTime);
        }
    }

    void Canvas::Draw()
    {
        if (!m_renderer)
        {
            return;
        }

        m_renderer->BeginDraw();
        for (auto& layers : m_activeLayers)
        {
            layers->Draw(*m_renderer);
        }
        m_renderer->EndDraw();
    }

    void Canvas::SetSize(const Vector2f32& size)
    {
        if (size != m_size && size.x != 0.0f && size.y != 0.0f)
        {
            m_renderer->Resize(size);
            for (auto& layers : m_activeLayers)
            {
                layers->Resize(size);
            }
        }
        m_size = size;
    }
    void Canvas::SetScale(const Vector2f32& scale)
    {
        m_scale = scale;
    }

    const Vector2f32& Canvas::GetSize() const
    {
        return m_size;
    }
    const Vector2f32& Canvas::GetScale() const
    {
        return m_scale;
    }

}