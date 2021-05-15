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
    Button<TTheme>::Button(WidgetDataMixin<TTheme, Button>& data) :
        WidgetMixin<TTheme, Button>(data),
        m_pressed(false)
    {}

    template<typename TTheme>
    void Button<TTheme>::Update()
    {
        this->ApplyMarginsToGrantedBounds();

        auto childLane = this->GetChildrenPartialLane();

        if (childLane.GetSize() > 0)
        {
            auto& childData = (*childLane.begin()).GetValue();
            auto contentBounds = this->GetGrantedBounds().WithoutMargins(this->padding).ClampHighToLow();
            childData->SetGrantedBounds(contentBounds);
        }
    }

    template<typename TTheme>
    bool Button<TTheme>::OnMouseEvent(const WidgetMouseEvent& widgetMouseEvent)
    {
        switch (widgetMouseEvent.type)
        {
            case WidgetMouseEventType::MouseMove: break;
            case WidgetMouseEventType::MouseEnter: Mixin::SetSkinState(m_pressed ? State::Pressed : State::Hovered); break;
            case WidgetMouseEventType::MouseLeave: Mixin::SetSkinState(m_pressed ? State::Pressed : State::Normal); break;
            case WidgetMouseEventType::MouseButtonPressed: Mixin::SetSkinState(State::Pressed); m_pressed = true; break;
            case WidgetMouseEventType::MouseButtonReleasedIn: Mixin::SetSkinState(State::Hovered); onPress(0); m_pressed = false; break;
            case WidgetMouseEventType::MouseButtonReleasedOut: Mixin::SetSkinState(State::Normal); m_pressed = false; break;
        }
        return true;
    }

}
