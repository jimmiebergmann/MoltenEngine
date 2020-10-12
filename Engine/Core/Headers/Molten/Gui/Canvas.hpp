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

#ifndef MOLTEN_CORE_GUI_CANVAS_HPP
#define MOLTEN_CORE_GUI_CANVAS_HPP

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Gui/Layer.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Time.hpp"
#include <memory>
#include <list>
#include <set>

namespace Molten
{
    class Renderer;
    class Logger;
}

namespace Molten::Gui
{

    class Renderer;

    class MOLTEN_API Canvas
    {

    public:

        template<typename ... TArgs>
        static CanvasPointer Create(TArgs ... args);

        template<typename TLayerType>
        static LayerTypePointer<TLayerType> CreateChild(CanvasPointer parent);
        template<typename TLayerType>
        static LayerTypePointer<TLayerType> CreateChild(CanvasPointer parent, LayerPosition position);

        //static bool AddChild(CanvasPointer parent, LayerPointer child);
        //static bool AddChild(CanvasPointer parent, LayerPointer child, LayerPosition position);

        Canvas(CanvasRendererPointer renderer = nullptr);
        ~Canvas();

        void SetRenderer(CanvasRendererPointer renderer);
        CanvasRendererPointer GetRenderer();
        const CanvasRendererPointer GetRenderer() const;

        void Update(const Time& deltaTime);

        void Draw();

        void SetSize(const Vector2f32& size);
        void SetScale(const Vector2f32& scale);

        const Vector2f32& GetSize() const;
        const Vector2f32& GetScale() const;

    private:

        Canvas(Canvas&&) = delete;
        Canvas(const Canvas&) = delete;
        Canvas& operator= (Canvas&&) = delete;
        Canvas& operator= (const Canvas&) = delete;

        //void OnAddChild(LayerPointer layer);
       // void OnAddChild(LayerPointer layer, LayerPosition position);

        using LayerPointerList = std::list<LayerPointer>;
        using LayerPointerSet = std::set<LayerPointer>;

        LayerPointerSet m_allLayers;
        LayerPointerList m_activeLayers;
        LayerPointerSet m_inactiveLayers;       

        CanvasRendererPointer m_renderer;
        Vector2f32 m_size;
        Vector2f32 m_scale;

    };

}

#include "Molten/Gui/Canvas.inl"

#endif