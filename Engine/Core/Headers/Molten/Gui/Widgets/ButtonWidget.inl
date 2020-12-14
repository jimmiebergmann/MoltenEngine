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
    inline Button<TSkin>::Button(TSkin& skin)
    {
        m_skin = skin.template Create<Button>(*this);
    }

    template<typename TSkin>
    inline void Button<TSkin>::Update(const Time& /*deltaTime*/)
    {
    }

    template<typename TSkin>
    inline void Button<TSkin>::Draw(CanvasRenderer& renderer)
    {
        m_skin->Draw(renderer, GetRenderData().size);
    }

    template<typename TSkin>
    inline Vector2f32 Button<TSkin>::CalculateSize(const Vector2f32& grantedSize)
    {
        return grantedSize;
    }

    template<typename TSkin>
    inline void Button<TSkin>::CalculateChildrenGrantedSize(typename WidgetTreeData<TSkin>::Tree::template ConstLane<typename WidgetTreeData<TSkin>::Tree::PartialLaneType> children)
    {
        SetRenderData(children.begin(), { 0.0f, 0.0f }, Widget<TSkin>::GetGrantedSize());
    }

    template<typename TSkin>
    inline bool Button<TSkin>::OnAddChild(WidgetPointer<TSkin> /*widget*/)
    {
        return true;
    }

}
