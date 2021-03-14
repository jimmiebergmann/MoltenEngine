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

    template<typename TTheme>
    class Canvas;
    template<typename TTheme>
    using CanvasPointer = std::shared_ptr<Canvas<TTheme>>;

    class CanvasRenderer;
    using CanvasRendererPointer = std::shared_ptr<CanvasRenderer>;

    template<typename TTheme>
    class Layer;
    template<typename TTheme>
    using LayerPointer = std::shared_ptr<Layer<TTheme>>;
    
    template<typename TLayerType>
    using LayerTypePointer = std::shared_ptr<TLayerType>;

    template<typename TTheme>
    class Widget;
    template<typename TTheme>
    using WidgetPointer = std::shared_ptr<Widget<TTheme>>;

    template<typename TWidget>
    using WidgetTypePointer = std::shared_ptr<TWidget>;

    class WidgetEventHandler;

}

#endif