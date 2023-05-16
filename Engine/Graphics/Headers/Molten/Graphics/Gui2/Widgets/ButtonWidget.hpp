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

#ifndef MOLTEN_GRAPHICS_GUI2_WIDGETS_BUTTON_HPP
#define MOLTEN_GRAPHICS_GUI2_WIDGETS_BUTTON_HPP

#include "Molten/Graphics/Gui2/Widget.hpp"
#include "Molten/Graphics/Gui2/WidgetEvent.hpp"
#include "Molten/System/Signal.hpp"

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    struct ButtonStyle : WidgetStyle
    {
        struct Colors {
            Vector4f32 normal = { 1.0f, 0.0f, 0.0f, 1.0f };
            Vector4f32 hovered = { 0.0f, 1.0f, 0.0f, 1.0f };
            Vector4f32 pressed = { 0.0f, 0.0f, 1.0f, 1.0f };
            Vector4f32 disabled = { 1.0f, 1.0f, 0.0f, 1.0f };
        };

        Colors colors = {};
        Bounds2f32 padding = { 5.0f, 5.0f, 5.0f, 5.0f };
        Bounds2f32 margins = { 5.0f, 5.0f, 5.0f, 5.0f };
    };


    class MOLTEN_GRAPHICS_API Button : public StyledWidget<ButtonStyle>, public WidgetMouseEventHandler
    {

    public:

        Signal<Mouse::Button> onPress;

        Button(WidgetDescriptor&& desc);
        Button(WidgetDescriptor&& desc, std::shared_ptr<const ButtonStyle> style);
       
    private:

        Vector2f32 OnUpdate(WidgetUpdateContext& context) override;
        void OnDraw(WidgetDrawContext& context) override;
        bool OnMouseEvent(const WidgetMouseEvent& mouseEvent) override;

        Vector2f32 UpdateChild(WidgetUpdateContext& context, AABB2f32& parentBounds);

        //AABB2f32 m_bounds;
        Vector4f32 m_color;
        bool m_pressed;
        //ChildWidgetWrapper m_drawChild;
        

    };

}

#endif