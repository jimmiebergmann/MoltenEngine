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
#include "Molten/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Gui/Widgets/PaddingWidget.hpp"
#include "Molten/Gui/Widgets/SpacerWidget.hpp"
#include "Molten/Gui/Widgets/VerticalGridWidget.hpp"
#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Math/Bounds.hpp"
#include <memory>

namespace Molten::Gui
{

    class DefaultSkin
    {

    public:

        template<typename TWidget>
        struct WidgetSkin;

        template<>
        struct WidgetSkin<Button<DefaultSkin>> : WidgetSkinBase
        {
            WidgetSkin(DefaultSkin& skin, Button<DefaultSkin>& button, WidgetData<DefaultSkin>& buttonData) :
                skin(skin),
                button(button),
                buttonData(buttonData)
            {}

            void Draw() override
            {
                switch (m_state)
                {
                    case WidgetSkinStateType::Hovered: skin.m_canvasRenderer.DrawRect(m_grantedBounds, Vector4f32{ 0.0f, 1.0f, 0.0f, 1.0f }); break;
                    case WidgetSkinStateType::Pressed: skin.m_canvasRenderer.DrawRect(m_grantedBounds, Vector4f32{ 0.0f, 0.0f, 1.0f, 1.0f }); break;
                    case WidgetSkinStateType::Disabled: skin.m_canvasRenderer.DrawRect(m_grantedBounds, Vector4f32{ 0.7f, 0.7, 0.7f, 1.0f }); break;
                    default: skin.m_canvasRenderer.DrawRect(m_grantedBounds, Vector4f32{ 1.0f, 0.0f, 0.0f, 1.0f }); break;
                }                
            }

            void Update() override
            {
                CalculateBounds(button.margin);

                auto childLane = buttonData.GetChildrenPartialLane();

                if (childLane.GetSize() > 0)
                {
                    auto& childData = (*childLane.begin()).GetValue();
                    auto contentBounds = CalculateContentBounds(button.padding);
                    childData->widgetSkin->SetGrantedBounds(contentBounds);
                }
            }

            DefaultSkin& skin;
            Button<DefaultSkin>& button;
            WidgetData<DefaultSkin>& buttonData;
        };

        /*template<>
        struct WidgetSkin<Padding<DefaultSkin>> : WidgetSkinBase
        {
            WidgetSkin(DefaultSkin& skin, Padding<DefaultSkin>& widget) :
                skin(skin),
                widget(widget)
            {}

            void Draw()
            {

            }

            void Update()
            {
            }

            DefaultSkin& skin;
            Padding<DefaultSkin>& widget;

        };*/

        template<>
        struct WidgetSkin<Spacer<DefaultSkin>> : WidgetSkinBase
        {
            WidgetSkin(DefaultSkin& skin, Spacer<DefaultSkin>& widget, WidgetData<DefaultSkin>& widgetData) :
                skin(skin),
                widget(widget),
                widgetData(widgetData)
            {}

            void Draw()
            {
                
            }

            void Update()
            {
            }

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

            void Draw()
            { }

            void Update()
            {
                CalculateBounds(verticalGrid.margin);
                auto contentBounds = CalculateContentBounds(verticalGrid.padding);

                auto childLane = verticalGridData.GetChildrenPartialLane();
                
                const float halfChildSpacing = childLane.GetSize() > 1 ? verticalGrid.cellSpacing * 0.5f : 0.0f;
                const float verticalIncrease = (contentBounds.right - contentBounds.left) / static_cast<float>(childLane.GetSize()) - halfChildSpacing;
                Bounds2f32 childBounds = { contentBounds.left, contentBounds.top, contentBounds.left, contentBounds.bottom };
                
                for (auto& child : childLane)
                {
                    childBounds.left = childBounds.right;
                    childBounds.right += verticalIncrease;

                    auto& childData = child.GetValue();
                    auto childGrantedBounds = childBounds;
                    childData->widgetSkin->SetGrantedBounds(childGrantedBounds);

                    childBounds.right += verticalGrid.cellSpacing;
                }
            }

            DefaultSkin& skin;
            VerticalGrid<DefaultSkin>& verticalGrid;
            WidgetData<DefaultSkin>& verticalGridData;
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