/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    // Widget implementations.
    template<typename T, typename ... TArgs>
    std::shared_ptr<T> Widget::CreateChild(TArgs ... args)
    {

        auto descriptor = WidgetDescriptor{
            .style = m_descriptor.style,
            .mouseEventHandler = nullptr
        };

        auto widget = std::make_shared<T>(std::move(descriptor), std::forward<TArgs>(args)...);

        if constexpr (std::is_base_of_v<WidgetMouseEventHandler, T> == true)
        {
            widget->m_descriptor.mouseEventHandler = widget.get();
        } 

        m_children.push_back(widget);
        return widget;
    }

    
    // Styled widget implementations.
    template<typename TWidgetStyle>
    StyledWidget<TWidgetStyle>::StyledWidget(WidgetDescriptor&& desc) :
        Widget{ std::move(desc) },
        m_widgetStyle{ m_descriptor.style->Get<WidgetStyleType>() }
    {}

    template<typename TWidgetStyle>
    StyledWidget<TWidgetStyle>::StyledWidget(WidgetDescriptor&& desc, std::shared_ptr<const WidgetStyleType> style) :
        Widget{ std::move(desc) },
        m_widgetStyle{ style ? style : std::make_shared<const WidgetStyleType>()}
    {}

    template<typename TWidgetStyle>
    void StyledWidget<TWidgetStyle>::SetWidgetStyle(std::shared_ptr<const WidgetStyleType> widgetStyle)
    {
        m_widgetStyle = widgetStyle;
    }

}

