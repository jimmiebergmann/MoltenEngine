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

    template<typename TTheme>
    Grid<TTheme>::Grid(
        WidgetMixinDescriptor<TTheme, Grid>& desc,
        const GridDirection direction
    ) :
        WidgetMixin<TTheme, Grid>(desc),
        direction(direction),
        cellSpacing(WidgetMixin<TTheme, Grid>::WidgetSkinType::cellSpacing)
    {}

    template<typename TTheme>
    void Grid<TTheme>::OnUpdate(WidgetUpdateContext<TTheme>& updateContext)
    {
        if (!this->PreCalculateBounds())
        {
            return;
        }

        AABB2f32 remainingContentBounds = this->GetBounds();
        remainingContentBounds.position += this->padding.low;
        remainingContentBounds.size -= this->padding.low + this->padding.high;
        Vector2f32 maxContentSize = {};

        for(auto it = this->GetChildrenBegin(); it != this->GetChildrenEnd(); ++it)
        {
            auto& child = *(it->get());

            if(!this->PreCalculateChildBounds(child, remainingContentBounds))
            {
                break;
            }

            updateContext.VisitChild(child);

            this->PostCalculateChildBounds(child, maxContentSize, remainingContentBounds, direction, this->cellSpacing);
            
			updateContext.DrawChild(child);
        }

        this->PostCalculateBounds(maxContentSize, direction, this->cellSpacing);
    }

}
