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

#ifndef MOLTEN_CORE_GUI_LAYER_HPP
#define MOLTEN_CORE_GUI_LAYER_HPP

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Gui/WidgetData.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Time.hpp"

namespace Molten::Gui
{

    enum class LayerPosition
    {
        Top,
        Bottom
    };

    class MOLTEN_API Layer
    {

    public:

        template<typename TLayerType, typename ... TArgs>
        static LayerTypePointer<TLayerType> Create(Canvas & canvas, TArgs ... args);

        template<typename TWidgetType, typename ... TArgs>
        static WidgetTypePointer<TWidgetType> CreateChild(LayerPointer parent, TArgs ... args);

        static bool AddChild(LayerPointer parent, WidgetPointer child);
        static bool AddChild(WidgetPointer parent, WidgetPointer child);


        Layer(Canvas& canvas);

        virtual ~Layer() = default;

        virtual void Update(const Time& deltaTime) = 0;

        virtual void Draw(CanvasRenderer& renderer) = 0;

        virtual void Resize(const Vector2f32& size) = 0;

        virtual void SetScale(const Vector2f32& scale) = 0;

        Canvas& GetCanvas();
        const Canvas& GetCanvas() const;

    protected:

        virtual bool OnAddChild(WidgetPointer parent, WidgetPointer child) = 0;

        static WidgetTreeData& GetWidgetTreeData(Widget& widget);
        static WidgetRenderData& GetWidgetRenderData(Widget& widget);

        static bool CallWidgetOnAddChild(WidgetPointer parent, WidgetPointer child);

    private:

        Layer(Layer&&) = delete;
        Layer(const Layer&) = delete;
        Layer& operator= (Layer&&) = delete;
        Layer& operator= (const Layer&) = delete;

        Canvas& m_canvas;

    };

}

#include "Molten/Gui/Layer.inl"

#endif