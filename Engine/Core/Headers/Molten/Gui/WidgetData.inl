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
    WidgetData<TTheme>::WidgetData() :
        canvas(nullptr),
        layer(nullptr),
        tree(nullptr),
        widgetSkin(nullptr)
    {}

    template<typename TTheme>
    Widget<TTheme>* WidgetData<TTheme>::GetWidget()
    {
        return widget.get();
    }

    template<typename TTheme>
    void WidgetData<TTheme>::ShowWidget()
    {
        tree->EnableInPartialLane(iterator);
    }

    template<typename TTheme>
    void WidgetData<TTheme>::HideWidget()
    {
        tree->DisableInPartialLane(iterator);
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::TreeNormalLane WidgetData<TTheme>::GetChildrenNormalLane()
    {
        return (*iterator).GetChildren().template GetLane<TreeNormalLaneType>();
    }

    template<typename TTheme>
    typename WidgetData<TTheme>::TreePartialLane WidgetData<TTheme>::GetChildrenPartialLane()
    {
        return (*iterator).GetChildren().template GetLane<TreePartialLaneType>();
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


    // Widget data mixin implementations.
    template<typename TTheme, template<typename> typename TWidget>
    WidgetDataMixin<TTheme, TWidget>::WidgetDataMixin() :
        WidgetData<TTheme>(),
        widgetSkinMixin(nullptr)
    {}

}