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
    inline Padding<TSkin>::Padding(
        TSkin& skin,
        const float left,
        const float top,
        const float right,
        const float bottom
    ) :
        PaddingData(left, top, right, bottom)
    {
        skin.template Create<Padding>(*this);
    }

    template<typename TSkin>
    inline void Padding<TSkin>::Update(const Time& /*deltaTime*/)
    {
    }

    template<typename TSkin>
    inline void Padding<TSkin>::Draw(CanvasRenderer& /*renderer*/)
    {
    }

    template<typename TSkin>
    inline Vector2f32 Padding<TSkin>::CalculateSize(const Vector2f32& grantedSize)
    {
        return grantedSize;
    }

    template<typename TSkin>
    inline void Padding<TSkin>::CalculateChildrenGrantedSize(typename WidgetTreeData<TSkin>::Tree::template ConstLane<typename WidgetTreeData<TSkin>::Tree::PartialLaneType> children)
    {
        Vector2f32 childSize = Widget<TSkin>::GetGrantedSize() - (low + high);

        SetRenderData(children.begin(), low, childSize);
    }

    template<typename TSkin>
    inline bool Padding<TSkin>::OnAddChild(WidgetPointer<TSkin> /*widget*/)
    {
        return true;
    }

}
