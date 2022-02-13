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

namespace Molten::Gui
{

    // Widget update context implementations.
    template<typename TTheme>
    void WidgetUpdateContext<TTheme>::VisitChild(Widget<TTheme>& child)
    {
        child.OnUpdate(*this);
    }

    template<typename TTheme>
    void WidgetUpdateContext<TTheme>::DrawChild(Widget<TTheme>& child)
    {
        m_widgetDrawQueue.push_back(&child);
    }

    template<typename TTheme>
    WidgetUpdateContext<TTheme>::WidgetUpdateContext(WidgetPointers<TTheme>& widgetDrawQueue) :
		m_widgetDrawQueue(widgetDrawQueue)
    {}


    // Widget base class implementations.
    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    TWidget<TTheme>* Widget<TTheme>::CreateChild(TArgs ... args)
    {
        return m_layer->template CreateChildFor<TWidget>(this, std::forward<TArgs>(args)...);
    }

    template<typename TTheme>
    Widget<TTheme>* Widget<TTheme>::GetParent()
    {
        return m_parent;
    }
    template<typename TTheme>
    const Widget<TTheme>* Widget<TTheme>::GetParent() const
    {
        return m_parent;
    }

    template<typename TTheme>
    Canvas<TTheme>* Widget<TTheme>::GetCanvas()
    {
        return m_canvas;
    }
    template<typename TTheme>
    const Canvas<TTheme>* Widget<TTheme>::GetCanvas() const
    {
        return m_canvas;
    }

    template<typename TTheme>
    Layer<TTheme>* Widget<TTheme>::GetLayer()
    {
        return m_layer;
    }
    template<typename TTheme>
    const Layer<TTheme>* Widget<TTheme>::GetLayer() const
    {
        return m_layer;
    }

    template<typename TTheme>
    const AABB2f32& Widget<TTheme>::GetBounds() const
    {
        return m_bounds;
    }

    template<typename TTheme>
    Widget<TTheme>::Widget(
        WidgetDescriptor<TTheme>& desc,
        const WidgetPosition& position,
        const WidgetSize& size
    ) :
		position(position),
		size(size),
        m_parent(desc.parent),
        m_canvas(desc.canvas),
        m_layer(desc.layer),
        m_skinBase(nullptr)
    {}

    template<typename TTheme>
    void Widget<TTheme>::OnUpdate(WidgetUpdateContext<TTheme>&)
    {
        this->PreCalculateBounds();
    }

    template<typename TTheme>
    void Widget<TTheme>::OnCreate()
    {}

    template<typename TTheme>
    void Widget<TTheme>::OnAddChild(Widget<TTheme>&)
    {}

    template<typename TTheme>
    void Widget<TTheme>::OnRemoveChild(Widget<TTheme>&)
    {}

    template<typename TTheme>
	WidgetChildIterator<TTheme> Widget<TTheme>::GetChildrenBegin()
    {
        return m_children.begin();
    }

    template<typename TTheme>
    WidgetChildIterator<TTheme> Widget<TTheme>::GetChildrenEnd()
    {
        return m_children.end();
    }

    template<typename TTheme>
    bool Widget<TTheme>::PreCalculateBounds()
    {
        m_bounds.position += this->margin.low;

        std::visit([&](const auto& element)
        {
            using T = std::decay_t<decltype(element)>;

            if constexpr (std::is_same_v<T, Size::Pixels>)
            {
                m_bounds.size.x = element.value;
            }
            else if constexpr (std::is_same_v<T, Size::Percent>)
            {
                m_bounds.size.x = m_grantedSize.x * (element.value / 100.0f);
            }
            else if constexpr (std::is_same_v<T, Size::Fit>)
            {
                m_bounds.size.x = m_grantedSize.x - this->margin.left - this->margin.right;
            }
        }, this->size.x);

        std::visit([&](const auto& element)
        {
            using T = std::decay_t<decltype(element)>;

            if constexpr (std::is_same_v<T, Size::Pixels>)
            {
                m_bounds.size.y = element.value;
            }
            else if constexpr (std::is_same_v<T, Size::Percent>)
            {
                m_bounds.size.y = m_grantedSize.y * (element.value / 100.0f);
            }
            else if constexpr (std::is_same_v<T, Size::Fit>)
            {
                m_bounds.size.y = m_grantedSize.y - this->margin.top - this->margin.bottom;
            }
        }, this->size.y);

        if (m_bounds.IsEmpty())
        {
            if (!(VariantEqualsType<Size::Fit>(this->size.x) && std::get<Size::Fit>(this->size.x) == Size::Fit::Content) &&
                !(VariantEqualsType<Size::Fit>(this->size.y) && std::get<Size::Fit>(this->size.y) == Size::Fit::Content))
            {
                return false;
            }
        }

        return true;
    }

    template<typename TTheme>
    bool Widget<TTheme>::PreCalculateChildBounds(Widget<TTheme>& child)
    {
        const auto grantedChildSize = m_bounds.size - this->padding.low - this->padding.high;
        if (grantedChildSize.x <= 0.0f || grantedChildSize.y <= 0.0f)
        {
            if (!(VariantEqualsType<Size::Fit>(this->size.x) && std::get<Size::Fit>(this->size.x) == Size::Fit::Content) &&
                !(VariantEqualsType<Size::Fit>(this->size.y) && std::get<Size::Fit>(this->size.y) == Size::Fit::Content))
            {
                return false;
            }
        }

        const auto childPosition = m_bounds.position + this->padding.low;

        child.m_bounds.position = childPosition;
        child.m_grantedSize = grantedChildSize;
        return true;
    }

    template<typename TTheme>
    bool Widget<TTheme>::PreCalculateChildBounds(Widget<TTheme>& child, const AABB2f32& remainingContentBounds)
    {
        if (remainingContentBounds.size.x <= 0.0f || remainingContentBounds.size.y <= 0.0f)
        {
            if (!(VariantEqualsType<Size::Fit>(this->size.x) && std::get<Size::Fit>(this->size.x) == Size::Fit::Content) &&
                !(VariantEqualsType<Size::Fit>(this->size.y) && std::get<Size::Fit>(this->size.y) == Size::Fit::Content))
            {
                return false;
            }
        }

        this->SetPosition(child, remainingContentBounds.position);
        this->SetGrantedSize(child, remainingContentBounds.size);
        return true;
    }

    template<typename TTheme>
    void Widget<TTheme>::PostCalculateChildBounds(const Widget<TTheme>& child, Vector2f32& contentSize, AABB2f32& remainingContentBounds, const GridDirection gridDirection, const float childSpacing)
    {
        const auto childSize = child.GetBounds().size;
        if (gridDirection == GridDirection::Horizontal)
        {
            auto const diff = childSize.x + childSpacing;
            remainingContentBounds.position.x += diff;
            remainingContentBounds.size.x -= diff;

            contentSize.x += diff;
            contentSize.y = std::max(contentSize.y, childSize.y);
        }
        else
        {
            auto const diff = childSize.y + childSpacing;
            remainingContentBounds.position.y += diff;
            remainingContentBounds.size.y -= diff;

            contentSize.x = std::max(contentSize.x, childSize.x);
            contentSize.y += diff;
        }
    }
    
    template<typename TTheme>
    bool Widget<TTheme>::PostCalculateBounds(Widget<TTheme>& child)
    {
        std::visit([&](const auto& element)
        {
            using T = std::decay_t<decltype(element)>;

            if constexpr (std::is_same_v<T, Size::Fit>)
            {
                if (element == Size::Fit::Content)
                {
                    m_bounds.size.x = child.m_bounds.size.x + this->padding.left + this->padding.right;
                }
            }
        }, this->size.x);

        std::visit([&](const auto& element)
        {
            using T = std::decay_t<decltype(element)>;

            if constexpr (std::is_same_v<T, Size::Fit>)
            {
                if (element == Size::Fit::Content)
                {
                    m_bounds.size.y = child.m_bounds.size.y + this->padding.top + this->padding.bottom;
                }
            }
        }, this->size.y);

        return m_bounds.size.x > 0.0f && m_bounds.size.y > 0.0f;
    }

    template<typename TTheme>
    bool Widget<TTheme>::PostCalculateBounds(const Vector2f32& contentSize, const GridDirection gridDirection, const float childSpacing)
    {
        if (contentSize.x <= 0.0f || contentSize.y <= 0.0f)
        {
            return false;
        }

        auto newSize = this->GetBounds().size;
        if (VariantEqualsType<Size::Fit>(this->size.x) && std::get<Size::Fit>(this->size.x) == Size::Fit::Content)
        {
            newSize.x = contentSize.x + this->padding.left + this->padding.right;

            if (gridDirection == GridDirection::Horizontal)
            {
                newSize.x -= childSpacing;
            }
        }
        if (VariantEqualsType<Size::Fit>(this->size.y) && std::get<Size::Fit>(this->size.y) == Size::Fit::Content)
        {
            newSize.y = contentSize.y + this->padding.left + this->padding.right;

            if (gridDirection == GridDirection::Vertical)
            {
                newSize.y -= childSpacing;
            }
        }

        this->SetSize(newSize);
        return true;
    }

    template<typename TTheme>
    Vector2f32 Widget<TTheme>::GetGrantedSize() const
    {
        return m_grantedSize;
    }

    template<typename TTheme>
    void Widget<TTheme>::SetPosition(const Vector2f32& newPosition)
    {
        m_bounds.position = newPosition;
    }

    template<typename TTheme>
    void Widget<TTheme>::SetPosition(Widget<TTheme>& child, const Vector2f32& childPosition)
    {
        child.m_bounds.position = childPosition;
    }

    template<typename TTheme>
    void Widget<TTheme>::SetSize(const Vector2f32& newSize)
    {
        m_bounds.size = newSize;
    }

    template<typename TTheme>
    void Widget<TTheme>::SetGrantedSize(Widget<TTheme>& child, const Vector2f32& childGrantedSize)
    {
        child.m_grantedSize = childGrantedSize;
    }

    template<typename TTheme, template<typename> typename TWidget>
    WidgetMixin<TTheme, TWidget>::WidgetMixin(WidgetMixinDescriptor<TTheme, TWidget>& desc) :
        Widget<TTheme>(desc, WidgetSkin<TTheme, TWidget>::defaultPosition, WidgetSkin<TTheme, TWidget>::defaultSize)
    {}

    template<typename TTheme, template<typename> typename TWidget>
    typename WidgetMixin<TTheme, TWidget>::WidgetSkinType* WidgetMixin<TTheme, TWidget>::GetWidgetSkin()
    {
        return m_skin.get();
    }
    template<typename TTheme, template<typename> typename TWidget>
    const typename WidgetMixin<TTheme, TWidget>::WidgetSkinType* WidgetMixin<TTheme, TWidget>::GetWidgetSkin() const
    {
        return m_skin.get();
    }

    template<typename TTheme, template<typename> typename TWidget>
    const auto& WidgetMixin<TTheme, TWidget>::GetSkinState() const
    {
        return m_skin->GetState();
    }
    template<typename TTheme, template<typename> typename TWidget>
    template<typename TState>
    void WidgetMixin<TTheme, TWidget>::SetSkinState(const TState& state)
    {
        m_skin->SetState(state);
    }


    // Managed widget implementations.
    template<typename TTheme, template<typename> typename TWidget>
    ManagedWidget<TTheme, TWidget>::ManagedWidget() :
        m_layer(nullptr),
        m_widget(nullptr)
    {}

    template<typename TTheme, template<typename> typename TWidget>
    ManagedWidget<TTheme, TWidget>::ManagedWidget(
        Layer<TTheme>* layer,
        TWidget<TTheme>* widget
    ) :
        m_layer(layer),
        m_widget(widget)
    {}

    template<typename TTheme, template<typename> typename TWidget>
    ManagedWidget<TTheme, TWidget>::~ManagedWidget()
    {
        Reset();
    }

    template<typename TTheme, template<typename> typename TWidget>
    ManagedWidget<TTheme, TWidget>::ManagedWidget(ManagedWidget&& managedWidget) noexcept :
        m_layer(managedWidget.m_layer),
        m_widget(managedWidget.m_widget)
    {
        managedWidget.m_layer = nullptr;
        managedWidget.m_widget = nullptr;
    }

    template<typename TTheme, template<typename> typename TWidget>
    ManagedWidget<TTheme, TWidget>& ManagedWidget<TTheme, TWidget>::operator =(ManagedWidget<TTheme, TWidget>&& managedWidget) noexcept
    {
        if(m_widget)
        {
            Reset();
        }

        m_layer = managedWidget.m_layer;
        m_widget = managedWidget.m_widget;
        managedWidget.m_layer = nullptr;
        managedWidget.m_widget = nullptr;
        return *this;
    }

    template<typename TTheme, template<typename> typename TWidget>
    TWidget<TTheme>* ManagedWidget<TTheme, TWidget>::operator ->()
    {
        return m_widget;
    }

    template<typename TTheme, template<typename> typename TWidget>
    const TWidget<TTheme>* ManagedWidget<TTheme, TWidget>::operator ->() const
    {
        return m_widget;
    }

    template<typename TTheme, template<typename> typename TWidget>
    ManagedWidget<TTheme, TWidget>::operator bool() const
    {
        return m_widget != nullptr;
    }

    template<typename TTheme, template<typename> typename TWidget>
    void ManagedWidget<TTheme, TWidget>::Reset()
    {
        if (m_widget)
        {
            m_layer->DestroyOverlayChild(*this);
        }
    }

}
