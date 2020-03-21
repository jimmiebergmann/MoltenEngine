/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

#include "Curse/Gui/GuiCanvas.hpp"

namespace Curse
{

    namespace Gui
    {

        Canvas::Canvas(Renderer& renderer, const Vector2f32& size, const Vector2f32& position) :
            m_renderer(&renderer),
            m_position(position),
            m_size(size)
        {
            static_cast<Control&>(m_plane).SetCanvasInternal(this);
        }

        Canvas::~Canvas()
        { }

        void Canvas::Update()
        {
            static_cast<Control&>(m_plane).Update();
        }

        void Canvas::Draw()
        {
            static_cast<Control&>(m_plane).Draw();
        }

        Vector2f32 Canvas::GetPosiion() const
        {
            return m_position;
        }
    
        Vector2f32 Canvas::GetSize() const
        {
            return m_size;
        }

        Plane& Canvas::GetPlane()
        {
            return m_plane;
        }
        const Plane& Canvas::GetPlane() const
        {
            return m_plane;
        }

        void Canvas::GetPosiion(const Vector2f32& position)
        {
            m_position = position;
        }

        void Canvas::SetSize(const Vector2f32& size)
        {
            m_size = size;
        }

    }

}