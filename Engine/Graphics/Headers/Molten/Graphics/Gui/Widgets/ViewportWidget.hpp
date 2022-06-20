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

#ifndef MOLTEN_CORE_GUI_WIDGETS_VIEWPORTWIDGET_HPP
#define MOLTEN_CORE_GUI_WIDGETS_VIEWPORTWIDGET_HPP

#include "Molten/Gui/Widget.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Gui/VisibilityWidget.hpp"
#include "Molten/System/Signal.hpp"
#include "Molten/Renderer/RenderResource.hpp"
#include "Molten/Renderer/Texture.hpp"


namespace Molten::Gui
{

    template<typename TTheme>
    class Viewport : public WidgetMixin<TTheme, Viewport>, public WidgetMouseEventHandler, public VisibilityWidget
    {

    public:

        Signal<Vector2ui32> onResize;

        struct State
        {
            SharedRenderResource<FramedTexture2D> framedTexture;
        };

        explicit Viewport(WidgetMixinDescriptor<TTheme, Viewport>& desc);

        void SetTexture(SharedRenderResource<FramedTexture2D> framedTexture);

    private:

        void OnUpdate(WidgetUpdateContext<TTheme>& updateContext) override;
        bool OnMouseEvent(const WidgetMouseEvent& widgetMouseEvent) override;

        using Mixin = WidgetMixin<TTheme, Viewport>;

        Vector2f32 m_prevSize;

    };

}

#include "Molten/Gui/Widgets/ViewportWidget.inl"

#endif
