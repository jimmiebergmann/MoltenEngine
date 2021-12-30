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

namespace Molten::Gui
{

    // Widget data implementations.
    template<typename TTheme>
    WidgetData<TTheme>::WidgetData(
        Canvas<TTheme>* canvas,
        Layer<TTheme>* layer
    ):
        m_canvas(canvas),
        m_layer(layer),
        m_tree(nullptr),
        m_parentWidget(nullptr),
        m_widgetSkinBase(nullptr)
    {}

    template<typename TTheme>
    Canvas<TTheme>* WidgetData<TTheme>::GetCanvas()
    {
        return m_canvas;
    }
    template<typename TTheme>
    const Canvas<TTheme>* WidgetData<TTheme>::GetCanvas() const
    {
        return m_canvas;
    }

    template<typename TTheme>
    Layer<TTheme>* WidgetData<TTheme>::GetLayer()
    {
        return m_layer;
    }
    template<typename TTheme>
    const Layer<TTheme>* WidgetData<TTheme>::GetLayer() const
    {
        return m_layer;
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::Tree* WidgetData<TTheme>::GetTree()
    {
        return m_tree;
    }
    template<typename TTheme>
    const typename WidgetData<TTheme>::Tree* WidgetData<TTheme>::GetTree() const
    {
        return m_tree;
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::TreeIterator WidgetData<TTheme>::GetTreeIterator()
    {
        return m_treeIterator;
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::TreeLane& WidgetData<TTheme>::GetChildren()
    {
        return m_treeIterator->GetChildren();
    }
    template<typename TTheme>
    const typename WidgetData<TTheme>::TreeLane& WidgetData<TTheme>::GetChildren() const
    {
        return m_treeIterator->GetChildren();
    }

    template<typename TTheme>
    Widget<TTheme>* WidgetData<TTheme>::GetParentWidget()
    {
        return m_parentWidget;
    }
    template<typename TTheme>
    const Widget<TTheme>* WidgetData<TTheme>::GetParentWidget() const
    {
        return m_parentWidget;
    }

    template<typename TTheme>
    Widget<TTheme>* WidgetData<TTheme>::GetWidget()
    {
        return m_widget.get();
    }
    template<typename TTheme>
    const Widget<TTheme>* WidgetData<TTheme>::GetWidget() const
    {
        return m_widget.get();
    }

    template<typename TTheme>
    WidgetSkinBase* WidgetData<TTheme>::GetWidgetSkinBase()
    {
        return m_widgetSkinBase;
    }
    template<typename TTheme>
    const WidgetSkinBase* WidgetData<TTheme>::GetWidgetSkinBase() const
    {
        return m_widgetSkinBase;
    }

    template<typename TTheme>
    const typename WidgetData<TTheme>::MouseEventFunction& WidgetData<TTheme>::GetMouseEventFunction() const
    {
        return m_mouseEventFunction;
    }

    template<typename TTheme>
    const AABB2f32& WidgetData<TTheme>::GetBounds() const
    {
        return m_bounds;
    }

    template<typename TTheme>
    void WidgetData<TTheme>::SetBounds(const AABB2f32& bounds)
    {
        m_bounds = bounds;
    }

    template<typename TTheme>
    void WidgetData<TTheme>::SetGrantedSize(const Vector2f32& size)
    {
        m_grantedSize = size;
    }

    template<typename TTheme>
    void WidgetData<TTheme>::SetSize(const Vector2f32& size)
    {
        m_bounds.size = size;
    }

    template<typename TTheme>
    void WidgetData<TTheme>::SetPosition(const Vector2f32& position)
    {
        m_bounds.position = position;
    }

    template<typename TTheme>
    Vector2f32 WidgetData<TTheme>::GetGrantedSize() const
    {
        return m_grantedSize;
    }

    template<typename TTheme>
    Vector2f32 WidgetData<TTheme>::GetSize() const
    {
        return m_bounds.size;
    }

    template<typename TTheme>
    Vector2f32 WidgetData<TTheme>::GetPosition() const
    {
        return m_bounds.position;
    }

    /*template<typename TTheme>
    void WidgetData<TTheme>::ShowWidget()
    {
        m_tree->EnableInPartialLane(m_treeIterator);
    }

    template<typename TTheme>
    void WidgetData<TTheme>::HideWidget()
    {
        m_tree->DisableInPartialLane(m_treeIterator);
    }*/


    template<typename TTheme>
    void WidgetData<TTheme>::ClearVisibleChildren()
    {
        m_visibleChildren.clear();
    }

    template<typename TTheme>
    void WidgetData<TTheme>::AddVisibleChild(WidgetData<TTheme>* childData)
    {
        m_visibleChildren.push_back(childData);
    }

    template<typename TTheme>
    std::vector<WidgetData<TTheme>*>& WidgetData<TTheme>::GetVisibleChildren()
    {
        return m_visibleChildren;
    }


    template<typename TTheme>
    void WidgetData<TTheme>::Initialize(
        Tree* tree,
        TreeIterator iterator,
        Widget<TTheme>* parentWidget,
        std::unique_ptr<Widget<TTheme>>&& widget,
        WidgetSkinBase* widgetSkinBase,
        MouseEventFunction&& mouseEventFunction)
    {
        m_tree = tree;
        m_treeIterator = iterator;
        m_parentWidget = parentWidget;
        m_widget = std::move(widget);
        m_widgetSkinBase = widgetSkinBase;
        m_mouseEventFunction = std::move(mouseEventFunction);
    }


    // Widget data mixin implementations.
    template<typename TTheme, template<typename> typename TWidget>
    WidgetDataMixin<TTheme, TWidget>::WidgetDataMixin(
        Canvas<TTheme>* canvas,
        Layer<TTheme>* layer
    ) :
        WidgetData<TTheme>(canvas, layer),
        m_widgetMixin(nullptr)
    {}

    template<typename TTheme, template<typename> typename TWidget>
    WidgetMixin<TTheme, TWidget>* WidgetDataMixin<TTheme, TWidget>::GetWidgetMixin()
    {
        return m_widgetMixin;
    }
    template<typename TTheme, template<typename> typename TWidget>
    const WidgetMixin<TTheme, TWidget>* WidgetDataMixin<TTheme, TWidget>::GetWidgetMixin() const
    {
        return m_widgetMixin;
    }

    template<typename TTheme, template<typename> typename TWidget>
    WidgetSkin<TTheme, TWidget>* WidgetDataMixin<TTheme, TWidget>::GetWidgetSkin()
    {
        return m_widgetSkin.get();
    }
    template<typename TTheme, template<typename> typename TWidget>
    const WidgetSkin<TTheme, TWidget>* WidgetDataMixin<TTheme, TWidget>::GetWidgetSkin() const
    {
        return m_widgetSkin.get();
    }

    template<typename TTheme, template<typename> typename TWidget>
    void WidgetDataMixin<TTheme, TWidget>::InitializeMixin(
        typename WidgetData<TTheme>::Tree* tree,
        typename WidgetData<TTheme>::TreeIterator iterator,
        Widget<TTheme>* parentWidget,
        std::unique_ptr<WidgetMixin<TTheme, TWidget>>&& widget,
        std::unique_ptr<WidgetSkin<TTheme, TWidget>>&& widgetSkin,
        typename WidgetData<TTheme>::MouseEventFunction&& mouseEventFunction)
    {
        m_widgetMixin = widget.get();
        m_widgetSkin = std::move(widgetSkin);

        WidgetData<TTheme>::Initialize(
            tree,
            iterator,
            parentWidget,
            std::move(widget),
            m_widgetSkin.get(),
            std::move(mouseEventFunction));
    }

}