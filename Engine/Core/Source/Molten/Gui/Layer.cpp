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

#include "Molten/Gui/Layer.hpp"
#include "Molten/Gui/Widget.hpp"

namespace Molten::Gui
{

    bool Layer::AddChild(LayerPointer parent, WidgetPointer child)
    {
        if (!parent || !child)
        {
            return false;
        }

        if (parent->OnAddChild(nullptr, child))
        {
            auto& childData = GetWidgetTreeData(*child);
            childData.layer = parent;
            return true;
        }
        return false;
    }

    bool Layer::AddChild(WidgetPointer parent, WidgetPointer child)
    {
        if (!parent || !child)
        {
            return false;
        }

        auto& parentData = GetWidgetTreeData(*parent);
        if (auto layer = parentData.layer.lock())
        {
            if (layer->OnAddChild(parent, child))
            {
                auto& childData = GetWidgetTreeData(*child);
                childData.layer = layer;
            }
            return true;
        }
       
        return false;
    }

    Layer::Layer(Canvas& canvas) :
        m_canvas(canvas)
    { }

    Canvas& Layer::GetCanvas()
    {
        return m_canvas;
    }
    const Canvas& Layer::GetCanvas() const
    {
        return m_canvas;
    }

    WidgetTreeData& Layer::GetWidgetTreeData(Widget& widget)
    {
        return widget.m_treeData;
    }

    WidgetRenderData& Layer::GetWidgetRenderData(Widget& widget)
    {
        return widget.m_renderData;
    }

    bool Layer::CallWidgetOnAddChild(WidgetPointer parent, WidgetPointer child)
    {
        return parent->OnAddChild(child);
    }

}