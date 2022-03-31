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
#include <type_traits>
#include <utility>

namespace Molten::Gui
{

    template<typename TTheme> class Canvas;
    template<typename TTheme> class Layer;

    template<typename TTheme> class Widget;
    template<typename TTheme> using WidgetPointers = std::vector<Widget<TTheme>*>;
    template<typename TTheme> using WidgetOwnedPointer = std::unique_ptr<Widget<TTheme>>;
    template<typename TTheme> using WidgetChildren = std::vector<WidgetOwnedPointer<TTheme>>;
    template<typename TTheme> using WidgetChildIterator = typename WidgetChildren<TTheme>::iterator;
    template<typename TTheme, template<typename> typename TWidget> class WidgetMixin;

    template<typename TTheme, template<typename> typename TWidget> struct WidgetSkin;
    template<typename TTheme, template<typename> typename TWidget> using WidgetSkinOwnedPointer = std::unique_ptr<WidgetSkin<TTheme, TWidget>>;
    class WidgetSkinBase;

    template<typename TTheme> using WidgetMouseEventFunction = std::function<Widget<TTheme>* (const WidgetMouseEvent&)>;
    template<typename TTheme> class WidgetMouseEventTracker;

    class WidgetVisibilityTracker;

    enum class GridDirection
    {
        Horizontal,
        Vertical
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
        WidgetSkinOwnedPointer<TTheme, TWidget> skin;
    };


    template<typename TTheme>
    class WidgetUpdateContext
    {

    public:

        void VisitChild(Widget<TTheme>& child);
        void DrawChild(Widget<TTheme>& child);

    private:

        explicit WidgetUpdateContext(WidgetPointers<TTheme>& widgetDrawQueue);

        template<typename> friend class Canvas;
        template<typename> friend class Layer;

        WidgetPointers<TTheme>& m_widgetDrawQueue;

    };


    template<typename TTheme, bool VIsConst>
    class WidgetChildrenWrapperIterator
    {

    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Widget<TTheme>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<VIsConst, const Widget<TTheme>*, Widget<TTheme>*>;
        using reference = std::conditional_t<VIsConst, const Widget<TTheme>&, Widget<TTheme>&>;
        using ContainerIterator = std::conditional_t<VIsConst, typename WidgetChildren<TTheme>::const_iterator, typename WidgetChildren<TTheme>::iterator>;

        explicit WidgetChildrenWrapperIterator(ContainerIterator containerIterator);

        template<bool IsConstIterator = VIsConst>
        std::enable_if_t<!IsConstIterator, reference> operator *();
        template<bool IsConstIterator = VIsConst>
        std::enable_if_t<IsConstIterator, reference> operator *() const;

        template<bool IsConstIterator = VIsConst>
        std::enable_if_t<!IsConstIterator, pointer> operator ->();
        template<bool IsConstIterator = VIsConst>
        std::enable_if_t<IsConstIterator, pointer> operator ->() const;

        WidgetChildrenWrapperIterator& operator ++ ();
        WidgetChildrenWrapperIterator& operator -- ();
        WidgetChildrenWrapperIterator operator ++ (int);
        WidgetChildrenWrapperIterator operator -- (int);

        bool operator == (const WidgetChildrenWrapperIterator& rhs) const;
        bool operator != (const WidgetChildrenWrapperIterator& rhs) const;

    private:

        ContainerIterator m_iterator;
	    
    };

    template<typename TTheme, bool VIsConst>
    class WidgetChildrenWrapper
    {

    public:

        using ChildrenType = std::conditional_t<VIsConst, const WidgetChildren<TTheme>, WidgetChildren<TTheme>>;
        using Iterator = WidgetChildrenWrapperIterator<TTheme, VIsConst>;

        explicit WidgetChildrenWrapper(ChildrenType& children);

        size_t GetCount() const;

        Iterator begin();
        Iterator end();

    private:

        ChildrenType& m_children;

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

        [[nodiscard]] WidgetChildrenWrapper<TTheme, false> GetChildren();
        [[nodiscard]] WidgetChildrenWrapper<TTheme, true> GetChildren() const;

        [[nodiscard]] Canvas<TTheme>* GetCanvas();
        [[nodiscard]] const Canvas<TTheme>* GetCanvas() const;

        [[nodiscard]] Layer<TTheme>* GetLayer();
        [[nodiscard]] const Layer<TTheme>* GetLayer() const;

        [[nodiscard]] const AABB2f32& GetBounds() const;

    protected:

        

        Widget(
            WidgetDescriptor<TTheme>& desc,
            const WidgetPosition& position,
            const WidgetSize& size);

        virtual void OnUpdate(WidgetUpdateContext<TTheme>& updateContext);
        virtual void OnCreate();
        virtual void OnAddChild(Widget<TTheme>& widget);
        virtual void OnRemoveChild(Widget<TTheme>& widget);

        void UpdateAsEmpty();
        void UpdateAsSingleParent(WidgetUpdateContext<TTheme>& updateContext, const PaddingType extraPadding = {});
        void UpdateAsGridParent(WidgetUpdateContext<TTheme>& updateContext, const GridDirection gridDirection, const float childSpacing = 0.0f, const PaddingType extraPadding = {});

        bool PreCalculateBounds();
        bool PreCalculateChildBounds(Widget<TTheme>& child, const PaddingType extraPadding = {});

        bool PreCalculateChildBounds(Widget<TTheme>& child, const AABB2f32& remainingContentBounds);

        void PostCalculateChildBounds(const Widget<TTheme>& child, Vector2f32& contentSize, AABB2f32& remainingContentBounds, const GridDirection gridDirection, const float childSpacing);

        bool PostCalculateBounds();
        bool PostCalculateBounds(Widget<TTheme>& child);
        bool PostCalculateBounds(const Vector2f32& contentSize, const GridDirection gridDirection, const float childSpacing);

        [[nodiscard]] Vector2f32 GetGrantedSize() const;

        void SetPosition(const Vector2f32& newPosition);
        void SetPosition(Widget<TTheme>& child, const Vector2f32& childPosition);

        void SetSize(const Vector2f32& newSize);
        void SetGrantedSize(Widget<TTheme>& child, const Vector2f32& childGrantedSize);

    private:

        template<typename> friend class Canvas;
        template<typename> friend class Layer;
        template<typename> friend class WidgetUpdateContext;
        template<typename, template<typename> typename> friend class WidgetMixin;
        template<typename> friend class WidgetMouseEventTracker;

        WidgetChildren<TTheme> m_children;
        AABB2f32 m_bounds;
        Vector2f32 m_grantedSize;
        Widget<TTheme>* m_parent;
        Canvas<TTheme>* m_canvas;
        Layer<TTheme>* m_layer;
        WidgetSkinBase* m_skinBase;
        WidgetMouseEventFunction<TTheme> m_mouseEventFunction;

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

        [[nodiscard]] const auto& GetSkinState() const;
        template<typename TState>
        void SetSkinState(const TState& state);

    private:

        template<typename> friend class Canvas;
        template<typename> friend class Layer;

        WidgetSkinOwnedPointer<TTheme, TWidget> m_skin;

    };


    template<typename TTheme, template<typename> typename TWidget>
    class ManagedWidget
    {

    public:

        ManagedWidget();
        ManagedWidget(
            Layer<TTheme>* layer,
            TWidget<TTheme>* widget);
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

        friend class Layer<TTheme>;

        Layer<TTheme>* m_layer;
        TWidget<TTheme>* m_widget;

    };


}

#include "Molten/Gui/Widget.inl"

#endif