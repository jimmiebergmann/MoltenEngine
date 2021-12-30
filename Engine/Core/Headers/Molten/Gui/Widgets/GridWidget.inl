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
    Grid<TTheme>::Grid(
        WidgetMixinDescriptor<TTheme, Grid>& desc,
        const GridDirection direction
    ) :
        WidgetMixin<TTheme, Grid>(desc),
        direction(direction),
        cellSpacing(WidgetMixin<TTheme, Grid>::WidgetSkinType::cellSpacing)
    {}


    template<typename TTheme>
    void Grid<TTheme>::PreUpdate()
    {
        if (this->PreCalculateBounds())
        {
            m_contentBounds = this->GetBounds();
            m_contentBounds.position += this->padding.low;
            m_contentBounds.size -= this->padding.low + this->padding.high;
            m_maxContentSize = {};
            this->UpdateAllChildren();
        }
    }

    template<typename TTheme>
    void Grid<TTheme>::PostUpdate()
    {
        if (m_maxContentSize.x <= 0.0f || m_maxContentSize.y <= 0.0f)
        {
            return; // TODO: Use min size?
        }

        auto newSize = this->GetBounds().size;

        if (VariantEqualsType<Size::Fit>(this->size.x) && std::get<Size::Fit>(this->size.x) == Size::Fit::Content)
        {
            if(direction == GridDirection::Horizontal)
            {
                m_maxContentSize.x -= this->cellSpacing;
            }

            newSize.x = m_maxContentSize.x + this->padding.left + this->padding.right;
        }
        if (VariantEqualsType<Size::Fit>(this->size.y) && std::get<Size::Fit>(this->size.y) == Size::Fit::Content)
        {
            if (direction == GridDirection::Vertical)
            {
                m_maxContentSize.y -= this->cellSpacing;
            }

            newSize.y = m_maxContentSize.y + this->padding.left + this->padding.right;
        }

        this->SetSize(newSize);
    }

    template<typename TTheme>
    PreChildUpdateResult Grid<TTheme>::PreChildUpdate(Widget<TTheme>& child)
    {
        if (m_contentBounds.size.x <= 0.0f || m_contentBounds.size.y <= 0.0f)
        {
            if (!(VariantEqualsType<Size::Fit>(this->size.x) && std::get<Size::Fit>(this->size.x) == Size::Fit::Content) &&
                !(VariantEqualsType<Size::Fit>(this->size.y) && std::get<Size::Fit>(this->size.y) == Size::Fit::Content))
            {
                return PreChildUpdateResult::SkipRemaining;
            }
        }

        const auto childPosition = this->GetBounds().position + this->padding.low;

        this->SetPosition(child, m_contentBounds.position);
        this->SetGrantedSize(child, m_contentBounds.size);

        return PreChildUpdateResult::Visit;
    }

    template<typename TTheme>
    void Grid<TTheme>::PostChildUpdate(Widget<TTheme>& child)
    {
        const auto childSize = child.GetBounds().size;
        if(direction == GridDirection::Horizontal)
        {
            auto const diff = childSize.x + this->cellSpacing;
            m_contentBounds.position.x += diff;
            m_contentBounds.size.x -= diff;

            m_maxContentSize.x += diff;
            m_maxContentSize.y = std::max(m_maxContentSize.y, childSize.y);
        }
        else
        {
            auto const diff = childSize.y + this->cellSpacing;
            m_contentBounds.position.y += diff;
            m_contentBounds.size.y -= diff;

            m_maxContentSize.x = std::max(m_maxContentSize.x, childSize.x);
            m_maxContentSize.y += diff;
        }

        this->DrawChild(child);
    }

}
