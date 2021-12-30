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
//#include "Molten/Utility/BypassTree.hpp"
#include "Molten/Utility/Tree.hpp"
#include "Molten/Math/Bounds.hpp" // ??
#include "Molten/Math/AABB.hpp"
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
    struct WidgetSkin;
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

        using Tree = Tree<std::unique_ptr<WidgetData<TTheme>>>;
        using TreeIterator = typename Tree::Iterator;
        using TreeLane = typename Tree::Lane;
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

        [[nodiscard]] TreeIterator GetTreeIterator();

        [[nodiscard]] TreeLane& GetChildren();
        [[nodiscard]] const TreeLane& GetChildren() const;

        [[nodiscard]] Widget<TTheme>* GetParentWidget();
        [[nodiscard]] const Widget<TTheme>* GetParentWidget() const;

        [[nodiscard]] Widget<TTheme>* GetWidget();
        [[nodiscard]] const Widget<TTheme>* GetWidget() const;

        [[nodiscard]] WidgetSkinBase* GetWidgetSkinBase();
        [[nodiscard]] const WidgetSkinBase* GetWidgetSkinBase() const;

        [[nodiscard]] const MouseEventFunction& GetMouseEventFunction() const;

        [[nodiscard]] const AABB2f32& GetBounds() const;
        void SetBounds(const AABB2f32& bounds);

        void SetGrantedSize(const Vector2f32& size);
        void SetSize(const Vector2f32& size);
        void SetPosition(const Vector2f32& position);

        [[nodiscard]] Vector2f32 GetGrantedSize() const;
        [[nodiscard]] Vector2f32 GetSize() const;
        [[nodiscard]] Vector2f32 GetPosition() const; 

        /*void ShowWidget();
        void HideWidget();*/

        void ClearVisibleChildren();
        void AddVisibleChild(WidgetData<TTheme>* childData);
        std::vector<WidgetData<TTheme>*>& GetVisibleChildren();

    protected:

        void Initialize(
            Tree* tree,
            TreeIterator iterator,
            Widget<TTheme>* parentWidget,
            std::unique_ptr<Widget<TTheme>>&& widget,
            WidgetSkinBase* widgetSkinBase,
            MouseEventFunction&& mouseEventFunction);

    private:

        template<typename TOtherTheme> friend class Widget;

        Canvas<TTheme>* m_canvas;
        Layer<TTheme>* m_layer;
        Tree* m_tree;
        TreeIterator m_treeIterator;
        Widget<TTheme>* m_parentWidget;
        std::unique_ptr<Widget<TTheme>> m_widget;
        WidgetSkinBase* m_widgetSkinBase;
        MouseEventFunction m_mouseEventFunction;

        Vector2f32 m_grantedSize;
        AABB2f32 m_bounds;
        std::vector<WidgetData<TTheme>*> m_visibleChildren;

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

        [[nodiscard]] WidgetSkin<TTheme, TWidget>* GetWidgetSkin();
        [[nodiscard]] const WidgetSkin<TTheme, TWidget>* GetWidgetSkin() const;

    protected:

        template<typename TThemeOther>
        friend class Layer;

        void InitializeMixin(
            typename WidgetData<TTheme>::Tree* tree,
            typename WidgetData<TTheme>::TreeIterator iterator,
            Widget<TTheme>* parentWidget,
            std::unique_ptr<WidgetMixin<TTheme, TWidget>>&& widget,
            std::unique_ptr<WidgetSkin<TTheme, TWidget>>&& widgetSkin,
            typename WidgetData<TTheme>::MouseEventFunction&& mouseEventFunction);

    private:

        WidgetMixin<TTheme, TWidget>* m_widgetMixin;
        std::unique_ptr<WidgetSkin<TTheme, TWidget>> m_widgetSkin;

    };

}

#include "Molten/Gui/WidgetData.inl"

#endif