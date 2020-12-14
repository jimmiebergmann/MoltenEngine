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

namespace Molten::Gui
{

    template<typename TSkin>
    inline VerticalGrid<TSkin>::VerticalGrid(
        TSkin& skin,
        const float spacing,
        const PaddingData& outerPadding,
        const PaddingData& innerPadding
    ) :
        spacing(spacing),
        outerPadding(outerPadding),
        innerPadding(innerPadding)
    {
        skin.template Create<VerticalGrid>(*this);
    }

    template<typename TSkin>
    inline void VerticalGrid<TSkin>::Update(const Time& /*deltaTime*/)
    {
    }

    template<typename TSkin>
    inline void VerticalGrid<TSkin>::Draw(CanvasRenderer& /*renderer*/)
    {
    }

    template<typename TSkin>
    inline Vector2f32 VerticalGrid<TSkin>::CalculateSize(const Vector2f32& grantedSize)
    {
        return grantedSize;
    }

    template<typename TSkin>
    inline void VerticalGrid<TSkin>::CalculateChildrenGrantedSize(typename WidgetTreeData<TSkin>::Tree::template ConstLane<typename WidgetTreeData<TSkin>::Tree::PartialLaneType> children)
    {
        const auto& grantedSize = Widget<TSkin>::GetGrantedSize();
        
        const Vector2f32 outer = outerPadding.low + outerPadding.high;
        const Vector2f32 inner = innerPadding.low + innerPadding.high;

        Vector2f32 childPosition = outerPadding.low + innerPadding.low;

        const Vector2f32 grantedSizeMinusOuter = grantedSize - outer;
        const float halfChildSpacing = (children.GetSize() > 1 ? 1.0f : 0.0f) * spacing * 0.5f;

        const Vector2f32 childSize = {
            (grantedSizeMinusOuter.x / static_cast<float>(children.GetSize())) - inner.x - halfChildSpacing,
            grantedSizeMinusOuter.y - inner.y
        };

        for (auto it = children.begin(); it != children.end(); it++)
        {
            Widget<TSkin>::SetRenderData(it, childPosition, childSize);
            childPosition.x += childSize.x + inner.x + spacing;
        }
    }

    template<typename TSkin>
    inline bool VerticalGrid<TSkin>::OnAddChild(WidgetPointer<TSkin> /*widget*/)
    {
        return true;
    }

}
