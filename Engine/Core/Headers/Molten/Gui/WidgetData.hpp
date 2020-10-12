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

#ifndef MOLTEN_CORE_GUI_WIDGETDATA_HPP
#define MOLTEN_CORE_GUI_WIDGETDATA_HPP

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Utility/BypassTree.hpp"
#include "Molten/Math/Vector.hpp"

namespace Molten::Gui
{
    struct WidgetTreeData
    {

        using Tree = typename BypassTree<WidgetPointer>;
        
        LayerWeakPointer layer;
        WidgetPointer widget;
        Tree::Iterator<Tree::NormalLaneType> iterator;
        Tree::Item* item;
    };

    struct WidgetRenderData
    {
        Vector2f32 grantedSize;
        Vector2f32 position;
        Vector2f32 size;
    };

}

#endif