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

#ifndef MOLTEN_GRAPHICS_GUI2_WIDGET_HPP
#define MOLTEN_GRAPHICS_GUI2_WIDGET_HPP

#include "Molten/Graphics/Build.hpp"
#include "Molten/Graphics/Gui2/Gui2Namespace.hpp"

#include "Molten/Graphics/Gui2/WidgetStyle.hpp"
#include "Molten/Graphics/Gui2/WidgetPosition.hpp"
#include "Molten/Graphics/Gui2/WidgetSize.hpp"
#include "Molten/Graphics/Gui2/WidgetEvent.hpp"
#include "Molten/Graphics/Gui2/Style.hpp"
#include "Molten/Math/Bounds.hpp"
#include "Molten/Math/AABB.hpp"
#include <memory>
#include <vector>
#include <utility>

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    class Canvas;
    class CanvasRenderer;
    class Widget;
    class WidgetDrawContextPosition;
    class WidgetDrawContext;


    struct MOLTEN_GRAPHICS_API WidgetDescriptor
    {
        std::shared_ptr<Style> style;
        WidgetMouseEventHandler* mouseEventHandler = nullptr;
    };

    struct MOLTEN_GRAPHICS_API WidgetDrawBufferEntry
    {
        AABB2f32 bounds;
        Widget* widget;
    };

    using WidgetDrawBuffer = std::vector<WidgetDrawBufferEntry>;
    using WidgetDrawBoundsStack = std::vector<AABB2f32>;

    class MOLTEN_GRAPHICS_API WidgetBoundsGuard
    {

    public:

        explicit WidgetBoundsGuard(WidgetDrawBoundsStack& drawBoundsStack);
        ~WidgetBoundsGuard();

        WidgetBoundsGuard(const WidgetBoundsGuard&) = delete;
        WidgetBoundsGuard(WidgetBoundsGuard&&) = delete;
        WidgetBoundsGuard& operator = (const WidgetBoundsGuard&) = delete;
        WidgetBoundsGuard& operator = (WidgetBoundsGuard&&) = delete;

    private:

        WidgetDrawBoundsStack& m_drawBoundsStack;

    };

    class MOLTEN_GRAPHICS_API WidgetUpdateContext
    {

    public:

        //Vector2f32 maxSize = { 0.0f, 0.0f };

        Vector2f32 grantedSize = { 0.0f, 0.0f };

        WidgetUpdateContext(
            WidgetDrawBuffer& drawBuffer,
            WidgetDrawBoundsStack& drawBoundsStack);

        ~WidgetUpdateContext() = default;

        WidgetUpdateContext(const WidgetDrawContext&) = delete;
        WidgetUpdateContext(WidgetDrawContext&&) = delete;
        WidgetUpdateContext& operator = (const WidgetDrawContext&) = delete;
        WidgetUpdateContext& operator = (WidgetDrawContext&&) = delete;

        [[nodiscard]] WidgetBoundsGuard RegisterBounds(const AABB2f32& bounds);
        void RegisterDraw();

       /* void RegisterForDraw(const AABB2f32& bounds);

        Widget* GetCurrentWidget();
        DrawWidgetsType& GetDrawWidgets();*/
        
    private:


        WidgetDrawBuffer& m_drawBuffer;
        WidgetDrawBoundsStack& m_drawBoundsStack;

        friend class Widget;

        void SetCurrentWidget(Widget* widget);

        Widget* m_currentWidget = nullptr;

        /*void SetCurrentWidget(Widget* widget);

        Widget* m_currentWidget = nullptr;       
        DrawWidgetsType m_drawWidgets;
        bool m_registeredWidgetBounds = false;*/
    };


    /*class MOLTEN_GRAPHICS_API WidgetPositionGuard
    {

    public:

        ~WidgetPositionGuard();

        WidgetPositionGuard(const WidgetPositionGuard&) = delete;
        WidgetPositionGuard(WidgetPositionGuard&&) = delete;
        WidgetPositionGuard& operator = (const WidgetPositionGuard&) = delete;
        WidgetPositionGuard& operator = (WidgetPositionGuard&&) = delete;

    private:

        friend class WidgetDrawContext;

        explicit WidgetPositionGuard(std::vector<Vector2f32>& positionList);

        std::vector<Vector2f32>& m_positionList;

    };*/


    class MOLTEN_GRAPHICS_API WidgetDrawContext
    {

    public:

        Vector2f32 registeredSize = { 0.0f, 0.0f };

        WidgetDrawContext(CanvasRenderer& renderer, Vector2f32 scale);
        ~WidgetDrawContext() = default;

        WidgetDrawContext(const WidgetDrawContext&) = delete;
        WidgetDrawContext(WidgetDrawContext&&) = delete;
        WidgetDrawContext& operator = (const WidgetDrawContext&) = delete;
        WidgetDrawContext& operator = (WidgetDrawContext&&) = delete;

        void DrawQuad(Vector2f32 size, const Vector4f32& color);
        void DrawQuad(AABB2f32 bounds, const Vector4f32& color);

    private:

        friend class Canvas;

        void SetCurrentWidgetDrawBufferEntry(WidgetDrawBufferEntry* currentDrawBufferEntry);

        CanvasRenderer& m_renderer;
        WidgetDrawBufferEntry* m_currentDrawBufferEntry = 0;
        Vector2f32 m_scale;
        //std::vector<std::pair<AABB2f32, Widget*>> m_widgetBounds;
        
    };


    class MOLTEN_GRAPHICS_API Widget
    {

    public:

        WidgetPosition position;
        WidgetSize size;

        explicit Widget(WidgetDescriptor&& desc);
        virtual ~Widget() = default;

        template<typename T, typename ... TArgs>
        std::shared_ptr<T> CreateChild(TArgs ... args);

        void AddChild(std::shared_ptr<Widget> widget);

        Canvas* GetCanvas();
        const Canvas* GetCanvas() const;

        virtual Vector2f32 OnUpdate(WidgetUpdateContext& context) = 0;
        virtual void OnDraw(WidgetDrawContext& context) = 0;

    protected:

        friend class Canvas;

        static Vector2f32 Update(
            WidgetUpdateContext& context,
            Widget& widget,
            Vector2f32 grantedSize);

        /*static Vector2f32 Draw(
            WidgetDrawContext& context,
            Widget& widget);*/

        static bool IsvalidSize(const Vector2f32& size);
        static bool IsvalidBounds(const AABB2f32& bounds);

        static AABB2f32 WithoutMargins(const AABB2f32& bounds, Bounds2f32 margins);
        static AABB2f32 WithoutMargins(const Vector2f32& size, Bounds2f32 margins);

        static AABB2f32 WithoutMarginsAndPadding(const AABB2f32& bounds, Bounds2f32 margins, Bounds2f32 padding);
        static AABB2f32 WithoutMarginsAndPadding(const Vector2f32& size, Bounds2f32 margins, Bounds2f32 padding);

        virtual void OnAddWidget(std::shared_ptr<Widget>& widget);
        virtual void OnRemoveWidget(std::shared_ptr<Widget>& widget);
        virtual void OnChangeCanvas(Canvas* canvas);

        std::vector<std::shared_ptr<Widget>> m_children;
        WidgetDescriptor m_descriptor;
        Canvas* m_canvas;

    };


    template<typename TWidgetStyle>
    class StyledWidget : public Widget
    {

    public:

        using WidgetStyleType = TWidgetStyle;

        explicit StyledWidget(WidgetDescriptor&& desc);
        StyledWidget(WidgetDescriptor&& desc, std::shared_ptr<const WidgetStyleType> style);
        virtual ~StyledWidget() = default;

        void SetWidgetStyle(std::shared_ptr<const WidgetStyleType> widgetStyle);

    protected:

        std::shared_ptr<const WidgetStyleType> m_widgetStyle;

    };

    struct ChildWidgetWrapper
    {
        Vector2f32 position = {};
        Widget* widget = nullptr;
    };
  
}

#include "Molten/Graphics/Gui2/Widget.inl"

#endif