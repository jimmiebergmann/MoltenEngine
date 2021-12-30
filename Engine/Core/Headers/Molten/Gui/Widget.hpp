/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_GUI_WIDGET_HPP
#define MOLTEN_CORE_GUI_WIDGET_HPP

#include "Molten/Gui/SpacingTypes.hpp"
#include "Molten/Gui/WidgetPosition.hpp"
#include "Molten/Gui/WidgetSize.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Math/AABB.hpp"
#include "Molten/System/Signal.hpp"
#include <vector>
#include <memory>
#include <utility>

namespace Molten::Gui
{

    template<typename TTheme> class Canvas;
    template<typename TTheme> class Layer;

    template<typename TTheme> class Widget;
    template<typename TTheme> using WidgetPointer = std::unique_ptr<Widget<TTheme>>;
    template<typename TTheme> using WidgetChildren = std::vector<WidgetPointer<TTheme>>;
    template<typename TTheme> using WidgetChildIterator = typename WidgetChildren<TTheme>::iterator;
    template<typename TTheme> using WidgetChildIteratorPair = std::pair<WidgetChildIterator<TTheme>, WidgetChildIterator<TTheme>>;
    template<typename TTheme, template<typename> typename TWidget> class WidgetMixin;

    template<typename TTheme, template<typename> typename TWidget> struct WidgetSkin;
    template<typename TTheme, template<typename> typename TWidget> using WidgetSkinPointer = std::unique_ptr<WidgetSkin<TTheme, TWidget>>;
    class WidgetSkinBase;

    template<typename TTheme> using WidgetMouseEventFunction = std::function<Widget<TTheme>* (const WidgetMouseEvent&)>;
    template<typename TTheme> class WidgetMouseEventTracker;

    class WidgetVisibilityTracker;

    enum class PreChildUpdateResult
    {
        Visit,
        Skip,
        SkipRemaining
    };


    template<typename TTheme>
    struct WidgetDescriptor
    {
        Widget<TTheme>* parent;
        Canvas<TTheme>* canvas;
        Layer<TTheme>* layer;
        TTheme& theme;
        SignalDispatcher& propertyDispatcher;
        WidgetVisibilityTracker& visibilityTracker;
    };


    template<typename TTheme, template<typename> typename TWidget>
    struct WidgetMixinDescriptor : WidgetDescriptor<TTheme>
    {
        WidgetSkinPointer<TTheme, TWidget> skin;
    };


    template<typename TTheme>
    class Widget
    {

    public:

        enum class State{ };

        WidgetPosition position;
        WidgetSize size;
        MarginType margin;
        PaddingType padding;

        Widget(const Widget&) = delete;
        Widget(Widget&&) = delete;
        Widget& operator= (const Widget&) = delete;
        Widget& operator= (Widget&&) = delete;

        virtual ~Widget() = default;

        template<template<typename> typename TWidget, typename ... TArgs>
        TWidget<TTheme>* CreateChild(TArgs ... args);

        [[nodiscard]] Widget<TTheme>* GetParent();
        [[nodiscard]] const Widget<TTheme>* GetParent() const;

        [[nodiscard]] Canvas<TTheme>* GetCanvas();
        [[nodiscard]] const Canvas<TTheme>* GetCanvas() const;

        [[nodiscard]] Layer<TTheme>* GetLayer();
        [[nodiscard]] const Layer<TTheme>* GetLayer() const;

        [[nodiscard]] const AABB2f32& GetBounds() const;

        [[nodiscard]] bool IsDestroyed() const;        

    protected:

        Widget(
            WidgetDescriptor<TTheme>& desc,
            const WidgetPosition& position,
            const WidgetSize& size);

        virtual void PreUpdate();
        virtual void PostUpdate();
        virtual PreChildUpdateResult PreChildUpdate(Widget<TTheme>& child);
        virtual void PostChildUpdate(Widget<TTheme>& child);

        virtual void OnCreate();
        virtual void OnAddChild(Widget<TTheme>& widget);
        virtual void OnRemoveChild(Widget<TTheme>& widget);

        typename WidgetChildren<TTheme>::iterator GetChildrenBegin();
        typename WidgetChildren<TTheme>::iterator GetChildrenEnd();

        bool PreCalculateBounds();
        bool PostCalculateBounds(Widget<TTheme>& child);
        bool PreCalculateBounds(Widget<TTheme>& child);

        [[nodiscard]] Vector2f32 GetGrantedSize();

        void SetPosition(const Vector2f32& newPosition);
        void SetPosition(Widget<TTheme>& child, const Vector2f32& childPosition);

        void SetSize(const Vector2f32& newSize);
        void SetGrantedSize(Widget<TTheme>& child, const Vector2f32& childGrantedSize);

        void UpdateChildRange(WidgetChildIteratorPair<TTheme> iteratorRange);
        void UpdateFirstChild();
        void UpdateAllChildren();

        void DrawChild(Widget<TTheme>& child);

    private:

        template<typename> friend class Canvas;
        template<typename> friend class Layer;
        template<typename, template<typename> typename> friend class WidgetMixin;
        template<typename> friend class WidgetMouseEventTracker;

        void PrepareUpdate();

        WidgetChildren<TTheme> m_children;
        AABB2f32 m_bounds;
        Vector2f32 m_grantedSize;
        Widget<TTheme>* m_parent;
        Canvas<TTheme>* m_canvas;
        Layer<TTheme>* m_layer;
        WidgetSkinBase* m_skinBase;
        WidgetChildIteratorPair<TTheme> m_updateChildren;
        std::vector<Widget*> m_drawChildren;
        WidgetMouseEventFunction<TTheme> m_mouseEventFunction;
        bool m_destroyed;

    };


    template<typename TTheme, template<typename> typename TWidget>
    class WidgetMixin : public Widget<TTheme>
    {

    public:

        WidgetMixin(const WidgetMixin&) = delete;
        WidgetMixin(WidgetMixin&&) = delete;
        WidgetMixin& operator= (const WidgetMixin&) = delete;
        WidgetMixin& operator= (WidgetMixin&&) = delete;

        ~WidgetMixin() override = default;

    protected:

        using WidgetSkinType = WidgetSkin<TTheme, TWidget>;

        explicit WidgetMixin(WidgetMixinDescriptor<TTheme, TWidget>& desc);

        [[nodiscard]] WidgetSkinType* GetWidgetSkin();
        [[nodiscard]] const WidgetSkinType* GetWidgetSkin() const;

        template<typename TState>
        [[nodiscard]] const TState& GetSkinState() const;

        template<typename TState>
        void SetSkinState(const TState& state);

    private:

        template<typename> friend class Canvas;
        template<typename> friend class Layer;

        WidgetSkinPointer<TTheme, TWidget> m_skin;

    };


   /* template<typename TTheme, template<typename> typename TWidget>
    class ManagedWidget
    {

    public:

        ManagedWidget();
        ManagedWidget(
            Layer<TTheme>& layer,
            TWidget<TTheme>& widget);
        ~ManagedWidget();

        ManagedWidget(ManagedWidget&& managedWidget) noexcept;
        ManagedWidget& operator =(ManagedWidget&& managedWidget) noexcept;

        ManagedWidget(const ManagedWidget&) = delete;
        ManagedWidget& operator =(const ManagedWidget&) = delete;

        [[nodiscard]] TWidget<TTheme>* operator ->();
        [[nodiscard]] const TWidget<TTheme>* operator ->() const;

        [[nodiscard]] operator bool() const;

        void Reset();

    private:

        Layer<TTheme>* m_layer;
        TWidget<TTheme>* m_widget;

    };*/

}

#include "Molten/Gui/Widget.inl"

#endif