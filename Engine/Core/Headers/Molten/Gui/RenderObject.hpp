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

#ifndef MOLTEN_CORE_GUI_RENDEROBJECT_HPP
#define MOLTEN_CORE_GUI_RENDEROBJECT_HPP

#include "Molten/Math/Vector.hpp"
#include <memory>

/*namespace Molten
{
    class Renderer;
    class VertexBuffer;
    class IndexBuffer;
}*/

namespace Molten::Gui
{
        
    class Renderer;

    class MOLTEN_API RenderObject
    {

    public:

        explicit RenderObject(Renderer* renderer);

        ~RenderObject();

        void Draw();

        void AddRect(const Vector2f32& relativePosition, const Vector2f32& size);

        const Vector2f32& GetPosition() const;
        void SetPosition(const Vector2f32& position);

    private:

        struct SubObject
        {
            Vector2f32 position;
            Vector2f32 size;
        };

        /*struct Object
        {
            VertexBuffer* vertexBuffer;
            IndexBuffer* indexBuffer;
            Vector2f32 position;
            Vector2f32 size;
        };*/

        Renderer* m_renderer;
        Vector2f32 m_position;
        std::vector<SubObject> m_objects;

    };

}

#endif