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
    template<typename TSkin>
    WidgetData<TSkin>::WidgetData() :
        canvas(nullptr),
        layer(nullptr),
        tree(nullptr),
        widgetSkin(nullptr)
    {}

    template<typename TSkin>
    WidgetPointer<TSkin> WidgetData<TSkin>::GetWidget()
    {
        return widget;
    }

    template<typename TSkin>
    typename WidgetData<TSkin>::TreeNormalLane WidgetData<TSkin>::GetChildrenNormalLane()
    {
        return (*iterator).GetChildren().template GetLane<TreeNormalLaneType>();
    }

    template<typename TSkin>
    typename WidgetData<TSkin>::TreePartialLane WidgetData<TSkin>::GetChildrenPartialLane()
    {
        return (*iterator).GetChildren().template GetLane<TreePartialLaneType>();
    }

    template<typename TSkin>
    const Bounds2f32& WidgetData<TSkin>::GetGrantedBounds() const
    {
        return m_grantedBounds;
    }

    template<typename TSkin>
    void WidgetData<TSkin>::SetGrantedBounds(const Bounds2f32& grantedBounds)
    {
        m_grantedBounds = grantedBounds;
    }


    // Widget data mixin implementations.
    template<typename TSkin, template<typename> typename TWidget>
    WidgetDataMixin<TSkin, TWidget>::WidgetDataMixin() :
        WidgetData<TSkin>(),
        widgetSkinMixin(nullptr)
    {}

}