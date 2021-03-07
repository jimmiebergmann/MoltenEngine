/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Gui/Skin.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Utility/BypassTree.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Bounds.hpp"
#include <functional>

namespace Molten::Gui
{

    template<typename TSkin>
    struct WidgetData;

    template<typename TSkin>
    using WidgetDataPointer = std::shared_ptr<WidgetData<TSkin>>;

    template<typename TSkin>
    using WidgetDataMap = std::unordered_map<Widget<TSkin>*, WidgetDataPointer<TSkin>>;

    template<typename TSkin>
    struct WidgetData
    {
        using Tree = typename Molten::BypassTree<WidgetDataPointer<TSkin>>;
        using TreeNormalLaneType = typename Tree::NormalLaneType;
        using TreePartialLaneType = typename Tree::PartialLaneType;
        using TreePartialLane = typename Tree::template Lane<TreePartialLaneType>;
        using TreeNormalLane = typename Tree::template Lane<TreeNormalLaneType>;
        using TreeNormalIterator = typename Tree::template Iterator<TreeNormalLaneType>;
        using TreeNormalConstIterator = typename Tree::template ConstIterator<TreeNormalLaneType>;
        using TreePartialIterator = typename Tree::template Iterator<TreePartialLaneType>;
        using TreePartialConstIterator = typename Tree::template ConstIterator<TreePartialLaneType>;

        WidgetData();

        WidgetPointer<TSkin> GetWidget();

        TreeNormalLane GetChildrenNormalLane();
        TreePartialLane GetChildrenPartialLane();

        const Bounds2f32& GetGrantedBounds() const;

        void SetGrantedBounds(const Bounds2f32& grantedBounds);

        Canvas<TSkin>* canvas;
        Layer<TSkin>* layer;
        Tree* tree;
        TreeNormalIterator iterator;
        WidgetPointer<TSkin> widget;
        std::unique_ptr<WidgetSkinBase> widgetSkin;
        std::function<Widget<TSkin>*(const WidgetEvent&)> mouseEventFunction;

    private:

        Bounds2f32 m_grantedBounds;

    };

    template<typename TSkin, template<typename> typename TWidget>
    struct WidgetDataMixin : WidgetData<TSkin>
    {

    public:

        WidgetDataMixin();

        virtual ~WidgetDataMixin() = default;

        using WidgetSkinMixin = typename TSkin::template WidgetSkin<TWidget<TSkin>>;

        WidgetSkinMixin* widgetSkinMixin;

    };

}

#include "Molten/Gui/WidgetData.inl"

#endif