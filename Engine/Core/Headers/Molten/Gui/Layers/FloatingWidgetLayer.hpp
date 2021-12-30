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


#ifndef MOLTEN_CORE_GUI_LAYERS_WINDOWLAYER_HPP
#define MOLTEN_CORE_GUI_LAYERS_WINDOWLAYER_HPP

#include "Molten/Gui/Layers/SingleRootLayer.hpp"
#include "Molten/Gui/Widgets/WindowWidget.hpp"

namespace Molten::Gui
{

    template<typename TTheme>
    class FloatingWidgetLayer : public SingleRootLayer<TTheme>
    {

    public:

        FloatingWidgetLayer(
            TTheme& theme,
            LayerData<TTheme>& data,
            SignalDispatcher& widgetPropertyDispatcher/*,
            const std::string& label,
            const WidgetSize& size*/);
        ~FloatingWidgetLayer() override = default;

        FloatingWidgetLayer(FloatingWidgetLayer&&) = delete;
        FloatingWidgetLayer(const FloatingWidgetLayer&) = delete;
        FloatingWidgetLayer& operator= (FloatingWidgetLayer&&) = delete;
        FloatingWidgetLayer& operator= (const FloatingWidgetLayer&) = delete;

        void Update(const Time& deltaTime) override;

    };

}

#include "Molten/Gui/Layers/FloatingWidgetLayer.inl"

#endif