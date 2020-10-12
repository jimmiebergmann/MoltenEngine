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

#ifndef MOLTEN_CORE_GUI_GUITYPES_HPP
#define MOLTEN_CORE_GUI_GUITYPES_HPP

#include "Molten/Types.hpp"
#include <memory>

namespace Molten::Gui
{

    class Canvas;
    using CanvasPointer = std::shared_ptr<Canvas>;

    class CanvasRenderer;
    using CanvasRendererPointer = std::shared_ptr<CanvasRenderer>;

    class Layer;
    using LayerPointer = std::shared_ptr<Layer>;
    using LayerWeakPointer = std::weak_ptr<Layer>;
    
    template<typename TLayerType>
    using LayerTypePointer = std::shared_ptr<TLayerType>;

    class Widget;
    using WidgetPointer = std::shared_ptr<Widget>;
    using WidgetWeakPointer = std::shared_ptr<Widget>;

    template<typename TWidgetType>
    using WidgetTypePointer = std::shared_ptr<TWidgetType>;

}

#endif