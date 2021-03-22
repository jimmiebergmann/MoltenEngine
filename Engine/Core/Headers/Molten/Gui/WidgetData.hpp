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

#ifndef MOLTEN_CORE_GUI_WIDGETDATA_HPP
#define MOLTEN_CORE_GUI_WIDGETDATA_HPP

#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Utility/BypassTree.hpp"
#include "Molten/Math/Bounds.hpp"
#include <memory>
#include <functional>

namespace Molten::Gui
{

    template<typename TTheme>
    class Canvas;

    template<typename TTheme>
    class Layer;

    template<typename TTheme, template<typename> typename TLayer>
    class LayerMixin;

    class WidgetSkinBase; // DELETE
    template<typename TTheme, template<typename> typename TWidget>
    class WidgetSkinMixin;

    template<typename TTheme>
    class Widget;
    template<typename TTheme, template<typename> typename TWidget>
    class WidgetMixin;

    template<typename TTheme>
    class WidgetData;

    template<typename TTheme>
    using WidgetDataPointer = std::shared_ptr<WidgetData<TTheme>>;

    template<typename TTheme>
    class WidgetData
    {

    public:

        using Tree = BypassTree<std::unique_ptr<WidgetData<TTheme>>>;
        using TreeNormalLaneType = typename Tree::NormalLaneType;
        using TreePartialLaneType = typename Tree::PartialLaneType;
        using TreePartialLane = typename Tree::template Lane<TreePartialLaneType>;
        using TreePartialConstLane = typename Tree::template ConstLane<TreePartialLaneType>;
        using TreeNormalLane = typename Tree::template Lane<TreeNormalLaneType>;
        using TreeNormalConstLane = typename Tree::template ConstLane<TreeNormalLaneType>;
        using TreeNormalIterator = typename Tree::template Iterator<TreeNormalLaneType>;
        using TreeNormalConstIterator = typename Tree::template ConstIterator<TreeNormalLaneType>;
        using TreePartialIterator = typename Tree::template Iterator<TreePartialLaneType>;
        using TreePartialConstIterator = typename Tree::template ConstIterator<TreePartialLaneType>;
        using MouseEventFunction = std::function<Widget<TTheme>* (const WidgetMouseEvent&)>;

        WidgetData(
            Canvas<TTheme>* canvas,
            Layer<TTheme>* layer);
        virtual ~WidgetData() = default;

        WidgetData(const WidgetData&) = delete;
        WidgetData(WidgetData&&) = delete;
        WidgetData& operator= (const WidgetData&) = delete;
        WidgetData& operator= (WidgetData&&) = delete;

        [[nodiscard]] Canvas<TTheme>* GetCanvas();
        [[nodiscard]] const Canvas<TTheme>* GetCanvas() const;

        [[nodiscard]] Layer<TTheme>* GetLayer();
        [[nodiscard]] const Layer<TTheme>* GetLayer() const;

        [[nodiscard]] Tree* GetTree();
        [[nodiscard]] const Tree* GetTree() const;

        [[nodiscard]] TreeNormalIterator GetTreeNormalIterator();
        [[nodiscard]] TreeNormalConstIterator GetTreeNormalIterator() const;

        [[nodiscard]] TreePartialIterator GetTreePartialIterator();
        [[nodiscard]] TreePartialConstIterator GetTreePartialIterator() const;

        [[nodiscard]] TreeNormalLane GetChildrenNormalLane();
        [[nodiscard]] TreeNormalConstLane GetChildrenNormalLane() const;

        [[nodiscard]] TreePartialLane GetChildrenPartialLane();
        [[nodiscard]] TreePartialConstLane GetChildrenPartialLane() const;

        [[nodiscard]] Widget<TTheme>* GetWidget();
        [[nodiscard]] const Widget<TTheme>* GetWidget() const;

        [[nodiscard]] WidgetSkinBase* GetWidgetSkin();
        [[nodiscard]] const WidgetSkinBase* GetWidgetSkin() const;

        [[nodiscard]] const MouseEventFunction& GetMouseEventFunction() const;

        [[nodiscard]] const Bounds2f32& GetGrantedBounds() const;
        void SetGrantedBounds(const Bounds2f32& grantedBounds);

        void ShowWidget();
        void HideWidget();

    protected:

        void Initialize(
            Tree* tree,
            TreeNormalIterator iterator,
            std::unique_ptr<Widget<TTheme>>&& widget,
            WidgetSkinBase* widgetSkin,
            MouseEventFunction&& mouseEventFunction);

    private:

        Canvas<TTheme>* m_canvas;
        Layer<TTheme>* m_layer;
        Tree* m_tree;
        TreeNormalIterator m_treeIterator;
        std::unique_ptr<Widget<TTheme>> m_widget;
        WidgetSkinBase* m_widgetSkin;
        MouseEventFunction m_mouseEventFunction;

        Bounds2f32 m_grantedBounds;

    };


    template<typename TTheme, template<typename> typename TWidget>
    class WidgetDataMixin : public WidgetData<TTheme>
    {

    public:

        WidgetDataMixin(
            Canvas<TTheme>* canvas,
            Layer<TTheme>* layer);
        ~WidgetDataMixin() override = default;

        WidgetDataMixin(const WidgetDataMixin&) = delete;
        WidgetDataMixin(WidgetDataMixin&&) = delete;
        WidgetDataMixin& operator= (const WidgetDataMixin&) = delete;
        WidgetDataMixin& operator= (WidgetDataMixin&&) = delete; 

        [[nodiscard]] WidgetMixin<TTheme, TWidget>* GetWidgetMixin();
        [[nodiscard]] const WidgetMixin<TTheme, TWidget>* GetWidgetMixin() const;

        [[nodiscard]] WidgetSkinMixin<TTheme, TWidget>* GetWidgetSkinMixin();
        [[nodiscard]] const WidgetSkinMixin<TTheme, TWidget>* GetWidgetSkinMixin() const;

    protected:

        template<typename TThemeOther>
        friend class Layer;

        void InitializeMixin(
            typename WidgetData<TTheme>::Tree* tree,
            typename WidgetData<TTheme>::TreeNormalIterator iterator,
            std::unique_ptr<WidgetMixin<TTheme, TWidget>>&& widget,
            std::unique_ptr<WidgetSkinMixin<TTheme, TWidget>>&& widgetSkinMixin,
            typename WidgetData<TTheme>::MouseEventFunction&& mouseEventFunction);

    private:

        WidgetMixin<TTheme, TWidget>* m_widgetMixin;
        std::unique_ptr<WidgetSkinMixin<TTheme, TWidget>> m_widgetSkinMixin;

    };

}

#include "Molten/Gui/WidgetData.inl"

#endif