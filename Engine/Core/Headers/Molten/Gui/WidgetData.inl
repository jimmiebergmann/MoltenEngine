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
        m_widgetSkin(nullptr)
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
    typename WidgetData<TTheme>::TreeNormalIterator WidgetData<TTheme>::GetTreeNormalIterator()
    {
        return m_treeIterator;
    }
    template<typename TTheme>
    typename WidgetData<TTheme>::TreeNormalConstIterator WidgetData<TTheme>::GetTreeNormalIterator() const
    {
        return m_treeIterator;
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::TreePartialIterator WidgetData<TTheme>::GetTreePartialIterator()
    {
        return m_treeIterator;
    }
    template<typename TTheme>
    typename WidgetData<TTheme>::TreePartialConstIterator WidgetData<TTheme>::GetTreePartialIterator() const
    {
        return m_treeIterator;
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::TreeNormalLane WidgetData<TTheme>::GetChildrenNormalLane()
    {
        return (*m_treeIterator).GetChildren().template GetLane<TreeNormalLaneType>();
    }
    template<typename TTheme>
    typename WidgetData<TTheme>::TreeNormalConstLane WidgetData<TTheme>::GetChildrenNormalLane() const
    {
        return (*m_treeIterator).GetChildren().template GetLane<TreeNormalLaneType>();
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::TreePartialLane WidgetData<TTheme>::GetChildrenPartialLane()
    {
        return (*m_treeIterator).GetChildren().template GetLane<TreePartialLaneType>();
    }
    template<typename TTheme>
    typename WidgetData<TTheme>::TreePartialConstLane WidgetData<TTheme>::GetChildrenPartialLane() const
    {
        return (*m_treeIterator).GetChildren().template GetLane<TreePartialLaneType>();
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
    WidgetSkinBase* WidgetData<TTheme>::GetWidgetSkin()
    {
        return m_widgetSkin;
    }
    template<typename TTheme>
    const WidgetSkinBase* WidgetData<TTheme>::GetWidgetSkin() const
    {
        return m_widgetSkin;
    }

    template<typename TTheme>
    const typename WidgetData<TTheme>::MouseEventFunction& WidgetData<TTheme>::GetMouseEventFunction() const
    {
        return m_mouseEventFunction;
    }

    template<typename TTheme>
    const Bounds2f32& WidgetData<TTheme>::GetGrantedBounds() const
    {
        return m_grantedBounds;
    }

    template<typename TTheme>
    void WidgetData<TTheme>::SetGrantedBounds(const Bounds2f32& grantedBounds)
    {
        m_grantedBounds = grantedBounds;
    }

    template<typename TTheme>
    void WidgetData<TTheme>::ShowWidget()
    {
        m_tree->EnableInPartialLane(m_treeIterator);
    }

    template<typename TTheme>
    void WidgetData<TTheme>::HideWidget()
    {
        m_tree->DisableInPartialLane(m_treeIterator);
    }

    template<typename TTheme>
    void WidgetData<TTheme>::Initialize(
        Tree* tree,
        TreeNormalIterator iterator,
        std::unique_ptr<Widget<TTheme>>&& widget,
        WidgetSkinBase* widgetSkin,
        MouseEventFunction&& mouseEventFunction)
    {
        m_tree = tree;
        m_treeIterator = iterator;
        m_widget = std::move(widget);
        m_widgetSkin = widgetSkin;
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
    WidgetSkinMixin<TTheme, TWidget>* WidgetDataMixin<TTheme, TWidget>::GetWidgetSkinMixin()
    {
        return m_widgetSkinMixin.get();
    }
    template<typename TTheme, template<typename> typename TWidget>
    const WidgetSkinMixin<TTheme, TWidget>* WidgetDataMixin<TTheme, TWidget>::GetWidgetSkinMixin() const
    {
        return m_widgetSkinMixin.get();
    }

    template<typename TTheme, template<typename> typename TWidget>
    void WidgetDataMixin<TTheme, TWidget>::InitializeMixin(
        typename WidgetData<TTheme>::Tree* tree,
        typename WidgetData<TTheme>::TreeNormalIterator iterator,
        std::unique_ptr<WidgetMixin<TTheme, TWidget>>&& widget,
        std::unique_ptr<WidgetSkinMixin<TTheme, TWidget>>&& widgetSkinMixin,
        typename WidgetData<TTheme>::MouseEventFunction&& mouseEventFunction)
    {
        m_widgetMixin = widget.get();
        m_widgetSkinMixin = std::move(widgetSkinMixin);

        WidgetData<TTheme>::Initialize(
            tree,
            iterator,
            std::move(widget),
            m_widgetSkinMixin.get(),
            std::move(mouseEventFunction));
    }

}