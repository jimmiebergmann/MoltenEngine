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

#include "Curse/Gui/RenderObject.hpp"
#include "Curse/Gui/GuiRenderer.hpp"

namespace Curse::Gui
{

    RenderObject::RenderObject(Renderer * renderer) :
        m_renderer(renderer)
    { }

    RenderObject::~RenderObject()
    {
    }

    void RenderObject::Draw()
    {
        for (auto& object : m_objects)
        {
            m_renderer->DrawRect(object.first, object.second, Vector4f32(1.0f, 0.0f, 0.0f, 1.0f));
        }
    }

    void RenderObject::AddRect(const Vector2f32& localPosition, const Vector2f32& size)
    {
        m_objects.push_back({ localPosition , size});
    }

    const Vector2f32& RenderObject::GetPosition() const
    {
        return m_position;
    }

    void RenderObject::SetPosition(const Vector2f32& position)
    {
        m_position = position;
    }

}