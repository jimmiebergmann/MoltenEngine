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

    template<typename TTheme>
    Pane<TTheme>::Pane(
        WidgetDataMixin<TTheme, Pane>& data,
        const WidgetSize& size
    ) :
        WidgetMixin<TTheme, Pane>(data, size)
    {}

    template<typename TTheme>
    void Pane<TTheme>::Update()
    {
        this->ApplyMarginsToGrantedBounds();

        auto& grantedBounds = this->GetGrantedBounds();

        m_dragBounds = grantedBounds;
        m_dragBounds.bottom = m_dragBounds.top + WidgetSkinType::headerBarHeight;

        auto childLane = this->GetChildrenPartialLane();

        if (childLane.GetSize() > 0)
        {
            auto& childData = (*childLane.begin()).GetValue();
            auto contentBounds = grantedBounds
                .WithoutMargins({0.0f, 20.0f, 0.0f, 0.0f})
                .WithoutMargins(this->padding)
                .ClampHighToLow();
            childData->SetGrantedBounds(contentBounds);
        }

    }

    template<typename TTheme>
    bool Pane<TTheme>::OnMouseEvent(const WidgetMouseEvent& widgetMouseEvent)
    {
        return false;
    }

    template<typename TTheme>
    const Bounds2f32& Pane<TTheme>::GetDragBounds() const
    {
        return m_dragBounds;
    }

}
