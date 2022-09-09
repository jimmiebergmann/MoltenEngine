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

#ifndef MOLTEN_EDITOR_GUI_THEME_EDITORTHEME_HPP
#define MOLTEN_EDITOR_GUI_THEME_EDITORTHEME_HPP

#include "Molten/Graphics/Gui/WidgetSkin.hpp"
#include "Molten/Graphics/Gui/CanvasRenderer.hpp"
#include "Molten/Graphics/Gui/GuiHelpers.hpp"
#include "Molten/Graphics/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/DockerWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/GridWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/LabelWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/MenuBarWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/PageViewWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/PaneWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/ProgressBarWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/ViewportWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/WindowWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/Overlays/DockerOverlayWidget.hpp"
#include <memory>

namespace Molten::Gui
{

    class EditorTheme
    {

    public:
        
        Vector4f32 backgroundColor = {  35.0f / 255.0f, 35.0f / 255.0f, 35.0f / 255.0f, 1.0f };

        EditorTheme(CanvasRenderer& canvasRenderer, FontNameRepository& fontNameRepository) :
            m_canvasRenderer(canvasRenderer),
            m_fontRepository(fontNameRepository, { 512, 512 })
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
                        TextureType::Color,
                        TextureUsage::ReadOnly,
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
        std::unique_ptr<WidgetSkin<EditorTheme, TWidget>> Create(TWidget<EditorTheme>& widget)
        {
            return std::make_unique<WidgetSkin<EditorTheme, TWidget>>(WidgetSkinDescriptor<EditorTheme, TWidget>{ *this, widget });
        }

    private:

        template<typename TTheme, template<typename> typename TWidget>
        friend struct WidgetSkin;
        friend struct WidgetSkinLabel;

        CanvasRenderer& m_canvasRenderer;
        FontRepository m_fontRepository;
        std::map<FontAtlas*, std::unique_ptr<CanvasRendererTexture>> m_fontAtlasTextures;

    };


    struct WidgetSkinLabel
    {
        explicit WidgetSkinLabel(EditorTheme& theme) :
            m_theme(theme)
        {}

        void Load(const std::string& text, const std::string& fontFamily, const uint32_t height)
        {
            auto* font = m_theme.m_fontRepository.GetOrCreateFont(fontFamily);
            m_fontSequence = font->CreateGroupedSequence(text, 96, height);

            m_theme.UpdateFontRepository();
            m_canvasFontSequence = m_theme.m_canvasRenderer.CreateFontSequence(m_fontSequence);
        }

        [[nodiscard]] AABB2i32 GetBounds() const
        {
            return { m_fontSequence.bounds.low, m_fontSequence.bounds.GetSize() };
        }

        template<typename T>
        [[nodiscard]] AABB2<T> CalculateFontHeightBounds() const
        {
            return m_fontSequence.CalculateFontHeightBounds<T>();
        }

        void Draw(const Vector2f32& position)
        {
            m_theme.m_canvasRenderer.DrawFontSequence(position, m_canvasFontSequence);
        }

        EditorTheme& m_theme;
        FontGroupedSequence m_fontSequence;
        CanvasRendererFontSequence m_canvasFontSequence;
    };


    // Widget skin implementations.
    template<>
    struct WidgetSkin<EditorTheme, Button> : WidgetSkinMixin<EditorTheme, Button>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Pixels{ 100.0f }, Size::Pixels{ 30.0f } };

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Button>& descriptor) :
            WidgetSkinMixin<EditorTheme, Button>(descriptor),
            color{ 1.0f, 0.0f, 0.0f, 1.0f }
        {}

        void Draw() override
        {
            theme.m_canvasRenderer.DrawRect(widget.GetBounds(), color);
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
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Parent, Size::Fit::Parent };

        static constexpr auto edgeWidth = 6.0f;
        static constexpr auto cellSpacing = 6.0f;
        static constexpr auto minCellSize = Vector2f32{ 30.0f, 30.0f };
        static constexpr auto defaultCellSize = Vector2f32{ 100.0f, 100.0f };

        //static constexpr auto widgetDragActivationDistance = 5.0f;

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Docker>& descriptor) :
            WidgetSkinMixin<EditorTheme, Docker>(descriptor)
        {}

    };

    template<>
    struct WidgetSkin<EditorTheme, Grid> : WidgetSkinMixin<EditorTheme, Grid>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Parent, Size::Fit::Parent };

        static constexpr auto cellSpacing = 6.0f;

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Grid>& descriptor) :
            WidgetSkinMixin<EditorTheme, Grid>(descriptor)
        {}

    };

    template<>
    struct WidgetSkin<EditorTheme, Label> : WidgetSkinMixin<EditorTheme, Label>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Parent, Size::Fit::Parent };

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Label>& descriptor) :
            WidgetSkinMixin<EditorTheme, Label>(descriptor),
            m_label(descriptor.theme)
        {
            Load();
            widget.text.onChange.Connect([&]()
            {
               Load();
            });
        }

        void Draw() override
        {
            m_label.Draw(widget.GetBounds().position);
        }

        template<typename T>
        [[nodiscard]] AABB2<T> CalculateFontHeightBounds()
        {
            return m_label.CalculateFontHeightBounds<T>();
        }

    private:

        void Load()
        {
            // arial.ttf
            // seguiemj.ttf

            //const auto fontFamily = widget.fontFamily().empty() ? "OpenSans-VariableFont_wdth,wght" : widget.fontFamily();
            const auto fontFamily = widget.fontFamily().empty() ? "Arial" : widget.fontFamily();
            const auto height = widget.height() == 0 ? 16 : widget.height();

            m_label.Load(widget.text(), fontFamily, height);
        }

    public:

        WidgetSkinLabel m_label;

    };

    template<>
    struct WidgetSkin<EditorTheme, MenuBar> : WidgetSkinMixin<EditorTheme, MenuBar>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Parent, Size::Pixels{ 30.0f } };

        static constexpr auto backgroundColor = Vector4f32{ 60.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 1.0f };
        static constexpr auto hoverColor = Vector4f32{ 1.0f, 60.0f / 255.0f, 60.0f / 255.0f, 1.0f };
        static constexpr auto pressColor = Vector4f32{ 60.0f / 255.0f, 1.0f, 60.0f / 255.0f, 1.0f };
        static constexpr auto menuSpacing = 6.0f;

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, MenuBar>& descriptor) :
            WidgetSkinMixin<EditorTheme, MenuBar>(descriptor)
        {}

        void Draw() override
        {
            theme.m_canvasRenderer.DrawRect(widget.GetBounds(), backgroundColor);
        }
    };

    template<>
    struct WidgetSkin<EditorTheme, MenuBarItem> : WidgetSkinMixin<EditorTheme, MenuBarItem>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Content, Size::Fit::Content };

        WidgetSkinLabel label;

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, MenuBarItem>& descriptor) :
            WidgetSkinMixin<EditorTheme, MenuBarItem>(descriptor),
			label(descriptor.theme)
        {
            LoadLabel();
            widget.label.onChange.Connect([&]() {
                LoadLabel();
            });
        }

        void Draw() override
        {
            const auto bounds = widget.GetBounds();

            auto labelBounds = label.CalculateFontHeightBounds<float>();
            labelBounds.position.y = -labelBounds.position.y;

            const auto labelOffset = GetCenterOffset(labelBounds.size, bounds.size);

            label.Draw(bounds.position + labelBounds.position + labelOffset);
        }

    private:

        void LoadLabel()
        {
            //label.Load(widget.label(), "OpenSans-VariableFont_wdth,wght", 16);
            label.Load(widget.label(), "Arial", 16);
        }

    };

    template<>
    struct WidgetSkin<EditorTheme, PageView> : WidgetSkinMixin<EditorTheme, PageView>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Parent, Size::Fit::Parent };


        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, PageView>& descriptor) :
            WidgetSkinMixin<EditorTheme, PageView>(descriptor)
        {}

    };

    template<>
    struct WidgetSkin<EditorTheme, Pane> : WidgetSkinMixin<EditorTheme, Pane>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Parent, Size::Fit::Parent };

        static constexpr auto headerBarHeight = 30.0f;
        static constexpr auto headerColor = Vector4f32{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f };
        static constexpr auto backgroundColor = Vector4f32{ 60.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 1.0f };

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Pane>& descriptor) :
            WidgetSkinMixin<EditorTheme, Pane>(descriptor),
            m_label(descriptor.theme)
        {
            LoadLabel();
            widget.label.onChange.Connect([&]() {
				LoadLabel();
            });
        }

        void Draw() override
        {
            theme.m_canvasRenderer.DrawRect(widget.GetBounds(), backgroundColor);

            auto headerBounds = widget.GetBounds();
            headerBounds.size.y = std::min(headerBarHeight, headerBounds.size.y);
            theme.m_canvasRenderer.DrawRect(headerBounds, headerColor);

            auto labelBounds = m_label.CalculateFontHeightBounds<float>();
            labelBounds.position.y = -labelBounds.position.y;

            const auto labelOffset = Vector2f32{
                4,
                GetCenterOffset(labelBounds.size.y, headerBounds.size.y)
            };

        	m_label.Draw(headerBounds.position + labelBounds.position + labelOffset);
        }

    private:

        void LoadLabel()
        {
            //m_label.Load(widget.label(), "OpenSans-VariableFont_wdth,wght", 16);
            m_label.Load(widget.label(), "Arial", 16);
        }

        WidgetSkinLabel m_label;
    };

    template<>
    struct WidgetSkin<EditorTheme, ProgressBar> : WidgetSkinMixin<EditorTheme, ProgressBar>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Parent, Size::Pixels{ 30.0f } };

        static constexpr auto backgroundColor = Vector4f32{ 1.0f, 1.0f, 1.0f, 1.0f };
        static constexpr auto progressColor = Vector4f32{ 0.2f, 1.0f, 0.2f, 1.0f };

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, ProgressBar>& descriptor) :
            WidgetSkinMixin<EditorTheme, ProgressBar>(descriptor)
        {}

        void Draw() override
        {
            theme.m_canvasRenderer.DrawRect(widget.GetBounds(), backgroundColor);

            const auto value = static_cast<float>(std::min(std::max(widget.value, 0.0), 100.0));
            auto progressBounds = widget.GetBounds();
            progressBounds.size.x *= value / 100.0f;
            theme.m_canvasRenderer.DrawRect(progressBounds, progressColor);

            auto tintBounds = progressBounds;
            tintBounds.size.y *= 0.3f;
            theme.m_canvasRenderer.DrawRect(tintBounds, { 1.0f, 1.0f, 1.0f, 0.5f });
        }

    };

    template<>
    struct WidgetSkin<EditorTheme, Viewport> : WidgetSkinMixin<EditorTheme, Viewport>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Parent, Size::Fit::Parent  };

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Viewport>& descriptor) :
            WidgetSkinMixin<EditorTheme, Viewport>(descriptor)
        {}

        void Draw() override
        {
			if (!m_framedTexture.framedTexture)
			{
				return;
			}

			const auto bounds = widget.GetBounds();
            const auto upperTextureCoords = bounds.size / Vector2f32{ m_framedTexture.framedTexture->GetDimensions() };
            const auto textureBounds = Bounds2f32{ { 0.0f, 0.0f }, upperTextureCoords };

			theme.m_canvasRenderer.DrawRect(bounds, textureBounds, m_framedTexture);
        }

        void OnStateChange(const State& state) override
        {
            if (m_framedTexture.framedTexture == state.framedTexture)
            {
                return;
            }

            m_framedTexture = theme.m_canvasRenderer.CreateFramedTexture(state.framedTexture);
        }

    private:

        CanvasRendererFramedTexture m_framedTexture;

    };

    template<>
    struct WidgetSkin<EditorTheme, Window> : WidgetSkinMixin<EditorTheme, Window>
    {
        static constexpr float headerBarHeight = 30.0f;

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, Window>& descriptor) :
            WidgetSkinMixin<EditorTheme, Window>(descriptor)
        {}

    };


	// Widget overlay skin implementations.
    template<>
    struct WidgetSkin<EditorTheme, DockerOverlay> : WidgetSkinMixin<EditorTheme, DockerOverlay>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Pixels{ 0.0f }, Size::Pixels{ 0.0f } };
        static constexpr auto backgroundColor = Vector4f32{ 0.4f, 0.4f, 1.0f, 0.4f };

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, DockerOverlay>& descriptor) :
            WidgetSkinMixin<EditorTheme, DockerOverlay>(descriptor)
        {}

        void Draw() override
        {
            theme.m_canvasRenderer.DrawRect(widget.GetBounds(), backgroundColor);
        }
    };

    template<>
    struct WidgetSkin<EditorTheme, MenuOverlay> : WidgetSkinMixin<EditorTheme, MenuOverlay>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::Content, Size::Fit::Content };

        static constexpr auto borderColor = Vector4f32{ 60.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 1.0f };
        static constexpr auto backgroundColor = Vector4f32{ 60.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 1.0f };

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, MenuOverlay>& descriptor) :
            WidgetSkinMixin<EditorTheme, MenuOverlay>(descriptor)
        {}

        void Draw() override
        {
            // So much overdraw...

            auto bounds = widget.GetBounds();
            theme.m_canvasRenderer.DrawRect(bounds, borderColor);

            bounds.position += Vector2f32{ 1.0f, 1.0f };
            bounds.size -= Vector2f32{ 2.0f, 2.0f };
            theme.m_canvasRenderer.DrawRect(bounds, backgroundColor);
        }
    };

    template<>
    struct WidgetSkin<EditorTheme, MenuOverlayItem> : WidgetSkinMixin<EditorTheme, MenuOverlayItem>
    {
        static constexpr auto defaultPosition = WidgetPosition{ Position::Pixels{ 0.0f }, Position::Pixels{ 0.0f } };
        static constexpr auto defaultSize = WidgetSize{ Size::Fit::ContentThenParent, Size::Fit::Content };

        static constexpr auto hoverColor = Vector4f32{ 1.0f, 60.0f / 255.0f, 60.0f / 255.0f, 1.0f };

        explicit WidgetSkin(const WidgetSkinDescriptor<EditorTheme, MenuOverlayItem>& descriptor) :
            WidgetSkinMixin<EditorTheme, MenuOverlayItem>(descriptor)
        {}

        void OnStateChange(const State& /*state*/) override
        {
            /*switch (state)
            {
            case State::Normal: color = Vector4f32{ 1.0f, 0.0f, 0.0f, 1.0f }; break;
            case State::Hovered: color = Vector4f32{ 0.0f, 1.0f, 0.0f, 1.0f }; break;
            }*/
        }

        void Draw() override
        {
            if(this->GetState() != State::Hovered)
            {
                return;
            }

            theme.m_canvasRenderer.DrawRect(widget.GetBounds(), hoverColor);
        }
    };

}

#endif