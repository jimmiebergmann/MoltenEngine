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

#ifndef MOLTEN_EDITOR_GUI_THEME_EDITORTHEME_HPP
#define MOLTEN_EDITOR_GUI_THEME_EDITORTHEME_HPP

#include "Molten/Gui/WidgetSkin.hpp"
#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Gui/WidgetData.hpp"
#include "Molten/Renderer/Renderer.hpp"

#include "Molten/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Gui/Widgets/DockerWidget.hpp"
#include "Molten/Gui/Widgets/PaneWidget.hpp"
#include "Molten/Gui/Widgets/SpacerWidget.hpp"
#include "Molten/Gui/Widgets/VerticalGridWidget.hpp"
#include "Molten/Gui/Widgets/LabelWidget.hpp"

#include <memory>

namespace Molten::Gui
{

    class EditorTheme
    {

    public:

        Vector4f32 backgroundColor = { 0.11f, 0.11f, 0.13f, 1.0f};

        EditorTheme(CanvasRenderer& canvasRenderer, FontNameRepository& fontNameRepository) :
            m_canvasRenderer(canvasRenderer),
            m_fontRepository(fontNameRepository, {512, 512})
        {}

        void Update()
        {
            UpdateFontRepository();
        }

        void UpdateFontRepository()
        {
            m_fontRepository.ForEachAtlasEvent([&](FontAtlasEventType eventType, FontAtlas* fontAtlas)
            {
                if (eventType == FontAtlasEventType::New)
                {
                    auto newTexture = std::make_unique<CanvasRendererTexture>();

                    const auto swizzleMapping = fontAtlas->GetImageFormat() == FontAtlasImageFormat::Gray ?
                        ImageSwizzleMapping{ ImageComponentSwizzle::One, ImageComponentSwizzle::One, ImageComponentSwizzle::One, ImageComponentSwizzle::Red } :
                        ImageSwizzleMapping{ };

                    const TextureDescriptor2D textureDesc = {
                        fontAtlas->GetBuffer(),
                        fontAtlas->GetImageDimensions(),
                        fontAtlas->GetImageFormat() == FontAtlasImageFormat::Gray ? ImageFormat::URed8 : ImageFormat::UBlue8Green8Red8Alpha8,
                        swizzleMapping
                    };

                    *newTexture = m_canvasRenderer.CreateTexture(textureDesc);
                    fontAtlas->metaData = newTexture.get();
                    m_fontAtlasTextures.insert({ fontAtlas, std::move(newTexture) });
                }
                else
                {
                    auto* texture = static_cast<CanvasRendererTexture*>(fontAtlas->metaData);

                    const auto textureUpdateDesc = TextureUpdateDescriptor2D{
                       fontAtlas->GetBuffer(),
                       fontAtlas->GetImageDimensions()
                    };

                    m_canvasRenderer.UpdateTexture(*texture, textureUpdateDesc);
                }
            });
        }

        template<template<typename> typename TWidget>
        std::unique_ptr<WidgetSkin<EditorTheme, TWidget>> Create(TWidget<EditorTheme>& widget, WidgetData<EditorTheme>& widgetData)
        {
            return std::make_unique<WidgetSkin<EditorTheme, TWidget>>(WidgetSkinDescriptor<EditorTheme, TWidget>{ *this, widget, widgetData});
        }

    private:

        template<typename TTheme, template<typename> typename TWidget>
        friend struct WidgetSkin;

        CanvasRenderer& m_canvasRenderer;
        FontRepository m_fontRepository;
        std::map<FontAtlas*, std::unique_ptr<CanvasRendererTexture>> m_fontAtlasTextures;

    };

    template<>
    struct WidgetSkin<EditorTheme, Button> : WidgetSkinMixin<EditorTheme, Button>
    {

        //static constexpr WidgetSize defaultSize = WidgetSize{ 100.0f, 20.0f };

        WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Button>& descriptor) :
            WidgetSkinMixin<EditorTheme, Button>(descriptor),
            color{ 1.0f, 0.0f, 0.0f, 1.0f }
        {}

        void Draw() override
        {
            theme.m_canvasRenderer.DrawRect(widgetData.GetGrantedBounds(), color);
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

        Vector4f32 color;
    };

    template<>
    struct WidgetSkin<EditorTheme, Docker> : WidgetSkinMixin<EditorTheme, Docker>
    {

        static constexpr float edgeWidth = 6.0f;
        static constexpr float spacing = 6.0f;
        static constexpr float widgetDragActivationDistance = 5.0f;

        WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Docker>& descriptor) :
            WidgetSkinMixin<EditorTheme, Docker>(descriptor)
        {}

    };

    template<>
    struct WidgetSkin<EditorTheme, DockerOverlay> : WidgetSkinMixin<EditorTheme, DockerOverlay>
    {
        WidgetSkin(const WidgetSkinDescriptor<EditorTheme, DockerOverlay>& descriptor) :
            WidgetSkinMixin<EditorTheme, DockerOverlay>(descriptor)
        {}

        void Draw() override
        {
            const Bounds2f32 overlayBounds = { widget.position, widget.position + widget.size.value };
            theme.m_canvasRenderer.DrawRect(overlayBounds, Vector4f32{ 0.4f, 0.4f, 1.0f, 0.4f });
        }
    };

    template<>
    struct WidgetSkin<EditorTheme, Spacer> : WidgetSkinMixin<EditorTheme, Spacer>
    {
        WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Spacer>& descriptor) :
            WidgetSkinMixin<EditorTheme, Spacer>(descriptor)
        {}

    };

    template<>
    struct WidgetSkin<EditorTheme, VerticalGrid> : WidgetSkinMixin<EditorTheme, VerticalGrid>
    {
        WidgetSkin(const WidgetSkinDescriptor<EditorTheme, VerticalGrid>& descriptor) :
            WidgetSkinMixin<EditorTheme, VerticalGrid>(descriptor)
        {}

    };

    template<>
    struct WidgetSkin<EditorTheme, Label> : WidgetSkinMixin<EditorTheme, Label>
    {
        WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Label>& descriptor) :
            WidgetSkinMixin<EditorTheme, Label>(descriptor)
        {
            // arial.ttf
            // seguiemj.ttf

            const auto fontFamily = widget.fontFamily.empty() ? "arial" : widget.fontFamily;
            const auto height = widget.height == 0 ? 16 : widget.height;
            m_font = theme.m_fontRepository.GetOrCreateFont(fontFamily);
            m_fontSequence = m_font->CreateGroupedSequence(widget.text, 96, height);

            theme.UpdateFontRepository();
            m_canvasFontSequence = theme.m_canvasRenderer.CreateFontSequence(m_fontSequence);
        }

        void Draw() override
        {
            auto grantedBounds = widgetData.GetGrantedBounds();
            grantedBounds.low.x -= m_fontSequence.bounds.low.x;
            grantedBounds.low.y += m_font->CalculateHeightOffset(grantedBounds);
            
            theme.m_canvasRenderer.DrawFontSequence(grantedBounds.low, m_canvasFontSequence);
        }

    private:

        Font* m_font;
        FontGroupedSequence m_fontSequence;
        CanvasRendererFontSequence m_canvasFontSequence;

    };

    template<>
    struct WidgetSkin<EditorTheme, Pane> : WidgetSkinMixin<EditorTheme, Pane>
    {

        static constexpr float headerBarHeight = 30.0f;

        WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Pane>& descriptor) :
            WidgetSkinMixin<EditorTheme, Pane>(descriptor)
        {}

        void Draw() override
        {
            auto& grantedBounds = widgetData.GetGrantedBounds();

            theme.m_canvasRenderer.DrawRect(grantedBounds, Vector4f32{ 0.43f, 0.45f, 0.49f, 1.0f });                
            theme.m_canvasRenderer.DrawRect(
                grantedBounds.WithoutMargins({ 0.0f, headerBarHeight, 0.0f, 0.0f }),
                Vector4f32{ 0.24f, 0.25f, 0.27f, 1.0f });
        }

    };


}

#endif