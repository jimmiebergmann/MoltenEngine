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

#ifndef MOLTEN_CORE_GUI_SKINS_DEFAULTSKIN_HPP
#define MOLTEN_CORE_GUI_SKINS_DEFAULTSKIN_HPP

#include "Molten/Gui/Skin.hpp"
#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Gui/WidgetData.hpp"
#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Math/Bounds.hpp"

#include "Molten/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Gui/Widgets/DockerWidget.hpp"
#include "Molten/Gui/Widgets/PaddingWidget.hpp"
#include "Molten/Gui/Widgets/PaneWidget.hpp"
#include "Molten/Gui/Widgets/SpacerWidget.hpp"
#include "Molten/Gui/Widgets/VerticalGridWidget.hpp"

#include <memory>

namespace Molten::Gui
{

    class DefaultSkin
    {

    public:

        template<typename TWidget>
        struct WidgetSkin;


        // Widgets
        template<>
        struct WidgetSkin<Button<DefaultSkin>> : WidgetSkinMixin<Button<DefaultSkin>>
        {

            static constexpr bool test = true;

            WidgetSkin(DefaultSkin& skin, Button<DefaultSkin>& button, WidgetData<DefaultSkin>& buttonData) :
                skin(skin),
                button(button),
                buttonData(buttonData),
                color{ 1.0f, 0.0f, 0.0f, 1.0f }
            {}

            void Draw() override
            {
                skin.m_canvasRenderer.DrawRect(buttonData.GetGrantedBounds(), color);
            }

            void OnStateChange(const State& state) override
            {
                switch (state)
                {
                    case State::Normal: color = Vector4f32{ 1.0f, 0.0f, 0.0f, 1.0f }; break;
                    case State::Hovered: color = Vector4f32{ 0.0f, 1.0f, 0.0f, 1.0f }; break;
                    case State::Pressed: color = Vector4f32{ 0.0f, 0.0f, 1.0f, 1.0f }; break;
                    case State::Disabled: color = Vector4f32{ 0.7f, 0.7, 0.7f, 1.0f }; break;
                }
            }

            DefaultSkin& skin;
            Button<DefaultSkin>& button;
            WidgetData<DefaultSkin>& buttonData;
            Vector4f32 color;
        };

        template<>
        struct WidgetSkin<Docker<DefaultSkin>> : WidgetSkinMixin<Docker<DefaultSkin>>
        {

            static constexpr float edgeWidth = 10.0f;
            static constexpr float spacing = 10.0f;
            static constexpr float widgetDragActivationDistance = 5.0f;


            WidgetSkin(DefaultSkin& skin, Docker<DefaultSkin>& widget, WidgetData<DefaultSkin>& widgetData) :
                skin(skin),
                widget(widget),
                widgetData(widgetData)
            {}

            void DrawLeafDocking()
            {
                skin.m_canvasRenderer.DrawRect(GetState().leafDragState.dockingBounds, Vector4f32{ 0.4f, 0.4f, 1.0f, 0.4f });
            }

            DefaultSkin& skin;
            Docker<DefaultSkin>& widget;
            WidgetData<DefaultSkin>& widgetData;
        };

        template<>
        struct WidgetSkin<Spacer<DefaultSkin>> : WidgetSkinBase
        {
            WidgetSkin(DefaultSkin& skin, Spacer<DefaultSkin>& widget, WidgetData<DefaultSkin>& widgetData) :
                skin(skin),
                widget(widget),
                widgetData(widgetData)
            {}

            DefaultSkin& skin;
            Spacer<DefaultSkin>& widget;
            WidgetData<DefaultSkin>& widgetData;
        };

        template<>
        struct WidgetSkin<VerticalGrid<DefaultSkin>> : WidgetSkinBase
        {
            WidgetSkin(DefaultSkin& skin, VerticalGrid<DefaultSkin>& verticalGrid, WidgetData<DefaultSkin>& verticalGridData) :
                skin(skin),
                verticalGrid(verticalGrid),
                verticalGridData(verticalGridData)
            {}

            DefaultSkin& skin;
            VerticalGrid<DefaultSkin>& verticalGrid;
            WidgetData<DefaultSkin>& verticalGridData;
        };

        template<>
        struct WidgetSkin<Pane<DefaultSkin>> : WidgetSkinBase
        {
            WidgetSkin(DefaultSkin& skin, Pane<DefaultSkin>& pane, WidgetData<DefaultSkin>& paneData) :
                skin(skin),
                pane(pane),
                paneData(paneData)
            {}

            void Draw() override
            {
                auto& grantedBounds = paneData.GetGrantedBounds();

                auto contentBounds = grantedBounds.WithoutMargins({ 2.0f , 2.0f , 2.0f , 2.0f });

                skin.m_canvasRenderer.DrawRect(grantedBounds, Vector4f32{ 0.2f, 0.2f, 0.2f, 1.0f });
                skin.m_canvasRenderer.DrawRect(contentBounds, Vector4f32{ 0.8f, 0.8f, 0.8f, 1.0f });
            }

            DefaultSkin& skin;
            Pane<DefaultSkin>& pane;
            WidgetData<DefaultSkin>& paneData;
        };


        explicit DefaultSkin(Renderer& renderer, CanvasRenderer& canvasRenderer) :
            m_renderer(renderer),
            m_canvasRenderer(canvasRenderer)
        {
            /*uint8_t textureData[] = { 255, 0, 0, 255,   0, 255, 0, 255,   0, 0, 255, 255,   255, 255, 0, 255 };
            TextureDescriptor textureDesc = { };
            textureDesc.dimensions = { 2, 2 };
            textureDesc.data = textureData;
            m_buttonTexture = m_renderer.CreateTexture(textureDesc);*/
        }

        template<typename TWidget>
        std::unique_ptr<WidgetSkin<TWidget>> Create(TWidget& widget, WidgetData<DefaultSkin>& widgetData)
        {
            return std::make_unique<WidgetSkin<TWidget>>(*this, widget, widgetData);
        };

    private:

        template<typename T>
        struct WidgetSkin;

        Renderer& m_renderer;
        CanvasRenderer& m_canvasRenderer;
        //Resource<Texture> m_buttonTexture;

    };

}

#endif