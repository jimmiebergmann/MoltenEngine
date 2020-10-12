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

#include "Molten/Gui/Widgets/PaddingWidget.hpp"

namespace Molten::Gui
{

    Padding::Padding(
        const float left,
        const float top,
        const float right,
        const float bottom
    ) :
        PaddingData(left, top, right, bottom)
    {}

    void Padding::Update(const Time& deltaTime)
    {
    }

    void Padding::Draw(CanvasRenderer& renderer)
    {
    }

    Vector2f32 Padding::CalculateSize(const Vector2f32& grantedSize)
    {
        return grantedSize;
    }

    void Padding::CalculateChildrenGrantedSize(WidgetTreeData::Tree::ConstLane<WidgetTreeData::Tree::PartialLaneType> children)
    {
        Vector2f32 childSize = GetGrantedSize() - (low + high);

        SetRenderData(children.begin(), low, childSize);
    }

    bool Padding::OnAddChild(WidgetPointer widget)
    {
        return true;
    }

}
