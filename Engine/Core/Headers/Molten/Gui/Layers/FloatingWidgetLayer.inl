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

namespace Molten::Gui
{

    template<typename TTheme>
    FloatingWidgetLayer<TTheme>::FloatingWidgetLayer(
        TTheme& theme,
        LayerData<TTheme>& data,
        SignalDispatcher& widgetPropertyDispatcher/*,
        const std::string& label,
        const WidgetSize& size*/
    ) :
        SingleRootLayer<TTheme>(theme, data, widgetPropertyDispatcher)
    {}

    template<typename TTheme>
    void FloatingWidgetLayer<TTheme>::Update(const Time&)
    {
        auto rootLane = m_widgetTree.template GetLane<typename WidgetData<TTheme>::TreePartialLaneType>();

        if(rootLane.IsEmpty())
        {
            return;
        }

        auto& rootChildData = (*rootLane.begin()).GetValue();
        auto* rootWidget = rootChildData->GetWidget();

        const Bounds2f32 rootBounds = { rootWidget->position , rootWidget->position + rootWidget->size.value };
        rootChildData->SetGrantedBounds(rootBounds);

        m_widgetTree.template ForEachPreorder<typename WidgetData<TTheme>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            widgetData->GetWidget()->Update();
        });
    }

   /* template<typename TTheme>
    Window<TTheme>& FloatingWidgetLayer<TTheme>::GetWindow()
    {
        return *m_windowWidget;
    }*/
}
