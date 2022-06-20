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

namespace Molten::Gui
{
    // Menu bar implementations.
    template<typename TTheme>
    MenuBar<TTheme>::MenuBar(WidgetMixinDescriptor<TTheme, MenuBar>& desc) :
        WidgetMixin<TTheme, MenuBar>(desc),
        menuSpacing(WidgetMixin<TTheme, MenuBar>::WidgetSkinType::menuSpacing)
    {}

   /* template<typename TTheme>
    std::shared_ptr<MenuBarMenu<TTheme>> MenuBar<TTheme>::AddMenu(const std::string& text)
    {
        auto* label = this->template CreateChild<Label>(text, 18);
        auto menu = std::make_shared<MenuBarMenu<TTheme>>(label);
        return menu;
    }*/

    template<typename TTheme>
    void MenuBar<TTheme>::OnUpdate(WidgetUpdateContext<TTheme>& updateContext)
    {
        this->UpdateAsGridParent(updateContext, GridDirection::Horizontal, 0.0f);

        /*m_menuBounds.clear();

        if (!this->PreCalculateBounds())
        {
            return;
        }

        m_contentBounds = this->GetBounds();
        m_contentBounds.position += this->padding.low;
        m_contentBounds.size -= this->padding.low + this->padding.high;

        // Children update.
        for (auto& child : this->GetChildren())
        {
            // Pre update
            this->SetPosition(child, m_contentBounds.position);
            this->SetGrantedSize(child, m_contentBounds.size);

            // Child update.
            updateContext.VisitChild(child);

            // Post update
            const auto childSize = child.GetBounds().size;
            const auto diff = childSize.x + menuSpacing;

            m_menuBounds.push_back(AABB2f32{ m_contentBounds.position, Vector2f32{diff, m_contentBounds.size.y} });

            m_contentBounds.position.x += diff;
            m_contentBounds.size.x -= diff;

            updateContext.DrawChild(child);
        }*/
    }

    template<typename TTheme>
    void MenuBar<TTheme>::OnAddChild(Widget<TTheme>& )
    {
        //auto menuWidget = this->GetLayer()->template CreateOverlayChild<MenuOverlay>();
        //m_menuWidgets.push_back(std::move(menuWidget));
    }

    template<typename TTheme>
    bool MenuBar<TTheme>::OnMouseEvent(const WidgetMouseEvent& )
    {
        /*switch (widgetMouseEvent.type)
        {
			case WidgetMouseEventType::MouseMove: 
            {
                if(this->GetSkinState().type == State::Type::PressedMenu)
                {
                    return true;
                }
				for(const auto& menuBound : m_menuBounds)
                {
                    if(menuBound.Intersects(widgetMouseEvent.position))
                    {
                        this->SetSkinState(State{ State::Type::HoverMenu, menuBound });
                        return true;
                    }
                }

                if(this->GetSkinState().type != State::Type::Normal)
                {
                    this->SetSkinState(State{ State::Type::Normal, {} });
                }
            } break;
            case WidgetMouseEventType::MouseButtonPressed:
            {
                for (const auto& menuBound : m_menuBounds)
                {
                    if (menuBound.Intersects(widgetMouseEvent.position))
                    {
                        this->SetSkinState(State{ State::Type::PressedMenu, menuBound });
                        m_menuWidget = this->GetLayer()->template CreateOverlayChild<MenuOverlay>();
                        return true;
                    }
                }

                if (this->GetSkinState().type != State::Type::Normal)
                {
                    m_menuWidget.Reset();
                    this->SetSkinState(State{ State::Type::Normal, {} });
                }
            } break;
			default: break;
        }*/
        return true;
    }


    // Menu bar item implementations.
    template<typename TTheme>
    MenuBarItem<TTheme>::MenuBarItem(
        WidgetMixinDescriptor<TTheme, MenuBarItem>& desc,
        const std::string& label
    ) :
        WidgetMixin<TTheme, MenuBarItem>(desc),
        label(desc.propertyDispatcher, label)
    {}

    template<typename TTheme>
    void MenuBarItem<TTheme>::OnUpdate(WidgetUpdateContext<TTheme>&)
    {
        const auto textBounds = this->GetWidgetSkin()->label.template CalculateFontHeightBounds<float>();
        this->SetPosition(this->GetBounds().position + Vector2f32{ textBounds.position.x, -textBounds.position.y });
        this->SetSize(textBounds.size);
    }




}
