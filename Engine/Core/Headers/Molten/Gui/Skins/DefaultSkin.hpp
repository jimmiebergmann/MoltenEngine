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

#ifndef MOLTEN_CORE_GUI_SKINS_DEFAULTSKIN_HPP
#define MOLTEN_CORE_GUI_SKINS_DEFAULTSKIN_HPP

#include "Molten/Gui/Skin.hpp"
#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Gui/Widgets/PaddingWidget.hpp"
#include "Molten/Gui/Widgets/SpacerWidget.hpp"
#include "Molten/Gui/Widgets/VerticalGridWidget.hpp"
#include <memory>

namespace Molten::Gui
{

    class DefaultSkin
    {

    public:

        template<typename TWidget>
        struct WidgetSkin;

        template<>
        struct WidgetSkin<Button<DefaultSkin>>
        {
            explicit WidgetSkin(Button<DefaultSkin>& widget) :
                widget(widget)
            {}

            void Draw(CanvasRenderer& renderer, const Vector2f32& size)
            {
                renderer.DrawRect(size, Vector4f32{ 1.0f, 0.0f, 0.0f, 1.0f });
            }

            Button<DefaultSkin>& widget;
        };

        template<>
        struct WidgetSkin<Padding<DefaultSkin>>
        {
            explicit WidgetSkin(Padding<DefaultSkin>& widget)
            {}

            void Draw()
            {

            }
        };

        template<>
        struct WidgetSkin<Spacer<DefaultSkin>>
        {
            explicit WidgetSkin(Spacer<DefaultSkin>& widget)
            {}

            void Draw()
            {

            }
        };

        template<>
        struct WidgetSkin<VerticalGrid<DefaultSkin>>
        {
            explicit WidgetSkin(VerticalGrid<DefaultSkin>& widget)
            {}

            void Draw()
            {

            }
        };


        explicit DefaultSkin(CanvasRenderer& canvasRenderer) :
            m_canvasRenderer(canvasRenderer)
        {}

        template<typename TWidget>
        std::unique_ptr<WidgetSkin<TWidget>> Create(TWidget& widget)
        {
            return std::make_unique<WidgetSkin<TWidget>>(widget);
        };

    private:

        CanvasRenderer& m_canvasRenderer;

    };

}

#endif