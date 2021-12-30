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
    Pane<TTheme>::Pane(
        WidgetMixinDescriptor<TTheme, Pane>& desc,
        const std::string& label
    ) :
        WidgetMixin<TTheme, Pane>(desc),
        label(desc.propertyDispatcher, label)
    {}

    template<typename TTheme>
    void Pane<TTheme>::PreUpdate()
    {
        if (this->PreCalculateBounds())
        {
            this->UpdateFirstChild();
        }
    }

    template<typename TTheme>
    void Pane<TTheme>::PostUpdate()
    {
        const auto& bounds = this->GetBounds();

        m_dragBounds = {
            bounds.position,
            { bounds.size.x, WidgetMixin<TTheme, Pane>::WidgetSkinType::headerBarHeight }
        };
    }


    template<typename TTheme>
    PreChildUpdateResult Pane<TTheme>::PreChildUpdate(Widget<TTheme>& child)
    {

        auto childBounds = this->GetBounds();

        childBounds.size -= this->padding.low + this->padding.high + Vector2f32{ 0.0f, WidgetMixin<TTheme, Pane>::WidgetSkinType::headerBarHeight };

        if(childBounds.IsEmpty())
        {
            return PreChildUpdateResult::Skip;
        }

        childBounds.position += this->padding.low + Vector2f32{ 0.0f, WidgetMixin<TTheme, Pane>::WidgetSkinType::headerBarHeight };

        this->SetPosition(child, childBounds.position);
        this->SetGrantedSize(child, childBounds.size);

        return PreChildUpdateResult::Visit;
    }

    template<typename TTheme>
    void Pane<TTheme>::PostChildUpdate(Widget<TTheme>& child)
    {
        this->DrawChild(child);
    }

}
