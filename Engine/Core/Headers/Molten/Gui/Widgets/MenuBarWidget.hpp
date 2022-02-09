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

#ifndef MOLTEN_CORE_GUI_WIDGETS_MENUBARWIDGET_HPP
#define MOLTEN_CORE_GUI_WIDGETS_MENUBARWIDGET_HPP

#include "Molten/Gui/Widget.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Gui/Widgets/LabelWidget.hpp"
#include "Molten/Gui/Widgets/Overlays/MenuOverlayWidget.hpp"
#include "Molten/System/Signal.hpp"
#include <memory>
#include <vector>

namespace Molten::Gui
{

    template<typename TTheme>
    class MenuBarMenu;

    template<typename TTheme>
    class MenuBarItem;

    template<typename TTheme>
    class MenuBar : public WidgetMixin<TTheme, MenuBar>, public WidgetMouseEventHandler
    {

    public:

        float menuSpacing;

        struct State
        {
            enum class Type
            {
	            Normal,
                HoverMenu,
                PressedMenu
            };

            Type type;
            AABB2f32 typeBounds;
        };

        explicit MenuBar(WidgetMixinDescriptor<TTheme, MenuBar>& desc);

        std::shared_ptr<MenuBarMenu<TTheme>> AddMenu(const std::string& text);

    private:

        void OnAddChild(Widget<TTheme>& widget) override;

        bool OnMouseEvent(const WidgetMouseEvent& widgetMouseEvent) override;

        void PreUpdate() override;
        PreChildUpdateResult PreChildUpdate(Widget<TTheme>& child) override;
        void PostChildUpdate(Widget<TTheme>& child) override;

        AABB2f32 m_contentBounds;
        std::vector<AABB2f32> m_menuBounds;
        ManagedWidget<TTheme, MenuOverlay> m_menuWidget;

    };


    template<typename TTheme>
    class MenuBarMenu
    {

    public:

        Signal<> onClick;

        explicit MenuBarMenu(Label<TTheme>* label);

        std::shared_ptr<MenuBarItem<TTheme>> AddItem(const std::string& text);

    private:

        friend class MenuBar<TTheme>;

        Label<TTheme>* m_label;
        std::vector<std::shared_ptr<MenuBarItem<TTheme>>> m_items;

    };


    template<typename TTheme>
    class MenuBarItem
    {

    public:

        Signal<> onClick;

        explicit MenuBarItem(const std::string& text);

    private:

        friend class MenuBar<TTheme>;

        std::string m_text;

    };

}

#include "Molten/Gui/Widgets/MenuBarWidget.inl"

#endif
