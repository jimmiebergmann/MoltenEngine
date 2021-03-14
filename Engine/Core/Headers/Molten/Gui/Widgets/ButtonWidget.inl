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
        ApplyMarginsToGrantedBounds();

        auto childLane = GetChildrenPartialLane();

        if (childLane.GetSize() > 0)
        {
            auto& childData = (*childLane.begin()).GetValue();
            auto contentBounds = GetGrantedBounds().WithoutMargins(padding).ClampHighToLow();
            childData->SetGrantedBounds(contentBounds);
        }
    }

    template<typename TTheme>
    bool Button<TTheme>::HandleEvent(const WidgetEvent& widgetEvent)
    {
        if (widgetEvent.type == WidgetEventType::Mouse)
        {
            switch (widgetEvent.subType)
            {
                case WidgetEventSubType::MouseEnter: SetSkinState(m_pressed ? State::Pressed : State::Hovered); return true;
                case WidgetEventSubType::MouseLeave: SetSkinState(m_pressed ? State::Pressed : State::Normal); return true;
                case WidgetEventSubType::MouseButtonPressed: SetSkinState(State::Pressed); m_pressed = true; return true;
                case WidgetEventSubType::MouseButtonReleasedIn: SetSkinState(State::Hovered); onPress(0); m_pressed = false; return true;
                case WidgetEventSubType::MouseButtonReleasedOut: SetSkinState(State::Normal); m_pressed = false; return true;
                default: break;
            }
        }
        return false;
    }

}
