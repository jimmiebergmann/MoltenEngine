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

#ifndef MOLTEN_GRAPHICS_GUI2_CANVAS_HPP
#define MOLTEN_GRAPHICS_GUI2_CANVAS_HPP

#include "Molten/Graphics/Gui2/Gui2Namespace.hpp"
#include "Molten/Graphics/Gui2/Widget.hpp"
#include "Molten/Graphics/Build.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/UserInput.hpp"
#include <memory>
#include <utility>

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    class CanvasRenderer;
    class Style;


    struct CanvasStyle : WidgetStyle
    {
        struct Colors {
            Vector4f32 background = { 35.0f / 255.0f, 35.0f / 255.0f, 35.0f / 255.0f, 1.0f };
        };

        Colors colors = {};
        Bounds2f32 padding = { 5.0f, 5.0f, 5.0f, 5.0f };
        Bounds2f32 margins = { 5.0f, 5.0f, 5.0f, 5.0f };
    };


    class MOLTEN_GRAPHICS_API Canvas
    {

    public:

        explicit Canvas(
            std::shared_ptr<CanvasRenderer> canvasRenderer,
            std::shared_ptr<Style> style = nullptr);

        ~Canvas() = default;

        Canvas(Canvas&&) = delete;
        Canvas(const Canvas&) = delete;
        Canvas& operator= (Canvas&&) = delete;
        Canvas& operator= (const Canvas&) = delete;

        template<typename T, typename ... TArgs>
        std::shared_ptr<T> CreateChild(TArgs ... args);

        void AddChild(std::shared_ptr<Widget> widget);

        void PushUserInputEvent(const UserInput::Event& inputEvent);

        void Update();
        void Draw();

        void SetSize(const Vector2f32& size);
        void SetScale(const Vector2f32& scale);

    private:

        void UpdateUserInputs();
        void HandleMouseEvent(const UserInput::Event& mouseEvent);
        [[nodiscard]] bool HandleMouseMoveEvent(const UserInput::MouseMoveEvent& mouseMoveEvent);
        [[nodiscard]] bool HandleMouseButtonPressedEvent(const UserInput::MouseButtonEvent& mouseButtonEvent);
        [[nodiscard]] bool HandleMouseButtonReleasedEvent(const UserInput::MouseButtonEvent& mouseButtonEvent);

        void UpdateChildren();


        //void Update(WidgetUpdateContext context) override;
        //void Draw(WidgetDrawContext context) override;

        std::shared_ptr<CanvasRenderer> m_canvasRenderer;
        std::shared_ptr<Style> m_style;
        std::shared_ptr<const CanvasStyle> m_widgetStyle;
        Vector2f32 m_size;
        Vector2f32 m_scale;
        std::vector<std::shared_ptr<Widget>> m_children;
        AABB2f32 m_childBounds;
        std::vector<UserInput::Event> m_userInputEvents;

        WidgetDrawBuffer m_widgetDrawBuffer;
        WidgetDrawBoundsStack m_widgetDrawBoundsStack;

    };

}

#include "Molten/Graphics/Gui2/Canvas.inl"

#endif