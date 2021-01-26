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
    template<typename TSkin>
    inline Button<TSkin>::Button(WidgetData<TSkin>& data) :
        Widget<TSkin>(data),
        m_pressed(false)
    {}

    template<typename TSkin>
    void Button<TSkin>::Update()
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

    template<typename TSkin>
    inline bool Button<TSkin>::HandleEvent(const WidgetEvent& widgetEvent)
    {
        if (widgetEvent.type == WidgetEventType::Mouse)
        {
            switch (widgetEvent.subType)
            {
                case WidgetEventSubType::MouseEnter: SetSkinState(m_pressed ? WidgetSkinStateType::Pressed : WidgetSkinStateType::Hovered); return true;
                case WidgetEventSubType::MouseLeave: SetSkinState(m_pressed ? WidgetSkinStateType::Pressed : WidgetSkinStateType::Normal); return true;
                case WidgetEventSubType::MousePress: SetSkinState(WidgetSkinStateType::Pressed); m_pressed = true; return true;
                case WidgetEventSubType::MouseReleaseIn: SetSkinState(WidgetSkinStateType::Hovered); onPress(0); m_pressed = false; return true;
                case WidgetEventSubType::MouseReleaseOut: SetSkinState(WidgetSkinStateType::Normal); m_pressed = false; return true;
                default: break;
            }
        }
        return false;
    }

    template<typename TSkin>
    inline bool Button<TSkin>::OnAddChild(WidgetPointer<TSkin> /*widget*/)
    {
        return true;
    }

}
