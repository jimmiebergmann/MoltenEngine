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
    VerticalGrid<TTheme>::VerticalGrid(WidgetDataMixin<TTheme, VerticalGrid>& data) :
        WidgetMixin<TTheme, VerticalGrid>(data),
        cellSpacing(0.0f)
    {}

    template<typename TTheme>
    void VerticalGrid<TTheme>::Update()
    {
        this->ApplyMarginsToGrantedBounds();
        auto contentBounds = this->GetGrantedBounds().WithoutMargins(this->padding).ClampHighToLow();

        auto childLane = this->GetData().GetChildrenPartialLane();

        const float halfChildSpacing = childLane.GetSize() > 1 ? cellSpacing * 0.5f : 0.0f;
        const float verticalIncrease = (contentBounds.right - contentBounds.left) / static_cast<float>(childLane.GetSize()) - halfChildSpacing;
        Bounds2f32 childBounds = { contentBounds.left, contentBounds.top, contentBounds.left, contentBounds.bottom }; 
            
        for (auto& child : childLane)
        {
            childBounds.left = childBounds.right;
            childBounds.right += verticalIncrease;

            auto& childData = child.GetValue();
            auto childGrantedBounds = childBounds;
            childData->SetGrantedBounds(childGrantedBounds);

            childBounds.right += cellSpacing;
        }
    }

}
