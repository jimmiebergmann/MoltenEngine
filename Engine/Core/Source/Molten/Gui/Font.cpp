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

#include "Molten/Gui/Font.hpp"
#include "ThirdParty/FreeType2/include/freetype/freetype.h"
#include "ThirdParty/FreeType2/include/freetype/ftcache.h"
#include <vector>
#include <fstream>
#include <algorithm>

namespace Molten::Gui
{

    // Global implementaitons.
    static ImageFormat GetPixelModeImageFormat(const FT_Pixel_Mode pixelMode, uint32_t& pixelSize)
    {
        switch(pixelMode)
        {          
            case FT_Pixel_Mode::FT_PIXEL_MODE_MONO: 
            case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY: 
            case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY2:
            case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY4:
            case FT_Pixel_Mode::FT_PIXEL_MODE_LCD:
            case FT_Pixel_Mode::FT_PIXEL_MODE_LCD_V: pixelSize = 1; return ImageFormat::URed8;
            case FT_Pixel_Mode::FT_PIXEL_MODE_BGRA: pixelSize = 4; return ImageFormat::UBlue8Green8Red8Alpha8;
            case FT_Pixel_Mode::FT_PIXEL_MODE_NONE:
            default: break;
        }

        pixelSize = 0;
        return ImageFormat::SRed8;
    }

    //  PIMPL implementations.
    struct FontImplementation
    {
        FontImplementation() :
            ftLibrary(nullptr),
            ftCacheManager(nullptr),
            ftCMapCache(nullptr),
            ftImageCache(nullptr)
        {}

        ~FontImplementation()
        {
            Destroy();
        }

        FontImplementation(const FontImplementation&) = delete;
        FontImplementation(FontImplementation&& impl) noexcept :
            fontData(std::move(impl.fontData)),
            ftLibrary(impl.ftLibrary),
            ftCacheManager(impl.ftCacheManager),
            ftCMapCache(impl.ftCMapCache),
            ftImageCache(impl.ftImageCache)
        {
            impl.ftLibrary = nullptr;
            impl.ftCacheManager = nullptr;
            impl.ftImageCache = nullptr;
        }

        FontImplementation& operator =(const FontImplementation&) = delete;
        FontImplementation& operator =(FontImplementation&& impl) noexcept
        {
            Destroy();

            fontData = std::move(impl.fontData);
            ftLibrary = impl.ftLibrary;
            ftCacheManager = impl.ftCacheManager;
            ftCMapCache = impl.ftCMapCache;
            ftImageCache = impl.ftImageCache;

            impl.ftLibrary = nullptr;
            impl.ftCacheManager = nullptr;
            impl.ftCMapCache = nullptr;
            impl.ftImageCache = nullptr;

            return *this;
        }

        void FontImplementation::Destroy()
        {
            if (ftCacheManager)
            {
                FTC_Manager_Done(ftCacheManager);
                ftCacheManager = nullptr;
                ftCMapCache = nullptr;
                ftImageCache = nullptr;
            }
            if (ftLibrary)
            {
                FT_Done_FreeType(ftLibrary);
                ftLibrary = nullptr;
            }
            fontData.clear();
        }

        static FT_Error FtFaceRequester(
            FTC_FaceID faceId,
            FT_Library library,
            FT_Pointer reqData,
            FT_Face* face)
        {
            auto* impl = reinterpret_cast<FontImplementation*>(reqData);

            FT_Error error = 0;

            // Load face from memory.
            if ((error = FT_New_Memory_Face(
                library,
                static_cast<FT_Byte*>(impl->fontData.data()),
                static_cast<FT_Long>(impl->fontData.size()),
                0,
                face)) != 0)
            {
                //  "Failed to load font face, FreeType error: %i\n", error);
                return error;
            }

            // Select the unicode character map
            if ((error = FT_Select_Charmap(
                *face,
                FT_ENCODING_UNICODE)) != 0)
            {
                //  "Can not select the unicode character map, FreeType error: %i\n", error);
                return error;
            }

            return 0;
        }

        std::vector<uint8_t> fontData;
        FT_Library ftLibrary;
        FTC_Manager ftCacheManager;
        FTC_CMapCache ftCMapCache;
        FTC_ImageCache ftImageCache;
    };


    // Font sequence implementations.
    FontSequence::FontSequence() :
        m_fontImpl(nullptr),
        m_cachedDpi(0),
        m_cachedFactor(1.0f),
        m_buffer(nullptr),
        m_bufferDimensions(0, 0),
        m_bufferTextBounds(0, 0, 0, 0),
        m_baseline(0),
        m_imageFormat(ImageFormat::URed8Green8Blue8),
        m_pixelSize(3)
    {}

    FontSequence::FontSequence(
        FontImplementation& fontImpl,
        const uint32_t cachedDpi,
        const Vector2f32& cachedFactor
    ) :
        m_fontImpl(&fontImpl),
        m_cachedDpi(cachedDpi),
        m_cachedFactor(std::max(cachedFactor.x, 1.0f), std::max(cachedFactor.y, 1.0f)),
        m_buffer(nullptr),
        m_bufferDimensions(0, 0),
        m_bufferTextBounds(0, 0, 0, 0),
        m_baseline(0),
        m_imageFormat(ImageFormat::URed8Green8Blue8),
        m_pixelSize(3)
    {}

    FontSequence::FontSequence(FontSequence&& fontSequence) noexcept :
        m_fontImpl(fontSequence.m_fontImpl),
        m_cachedDpi(fontSequence.m_cachedDpi),
        m_cachedFactor(fontSequence.m_cachedFactor),
        m_buffer(std::move(fontSequence.m_buffer)),
        m_bufferDimensions(fontSequence.m_bufferDimensions),
        m_bufferTextBounds(fontSequence.m_bufferTextBounds),
        m_baseline(fontSequence.m_baseline),
        m_imageFormat(ImageFormat::URed8Green8Blue8)
    {
        fontSequence.m_fontImpl = nullptr;
        fontSequence.m_bufferDimensions = { 0, 0 };
        fontSequence.m_bufferTextBounds = { 0, 0, 0, 0 };
        fontSequence.m_baseline = 0;
    }

    FontSequence& FontSequence::operator =(FontSequence&& fontSequence) noexcept
    {
        m_fontImpl = fontSequence.m_fontImpl;
        m_cachedDpi = fontSequence.m_cachedDpi;
        m_cachedFactor = fontSequence.m_cachedFactor;
        m_buffer = std::move(fontSequence.m_buffer);
        m_bufferDimensions = fontSequence.m_bufferDimensions;
        m_bufferTextBounds = fontSequence.m_bufferTextBounds;
        m_baseline = fontSequence.m_baseline;

        fontSequence.m_fontImpl = nullptr;
        fontSequence.m_bufferDimensions = { 0, 0 };
        fontSequence.m_bufferTextBounds = { 0, 0, 0, 0 };
        fontSequence.m_baseline = 0;

        return *this;
    }

    bool FontSequence::CreateBitmap(
        const std::string& text,
        const uint32_t dpi,
        const uint32_t height)
    {
        const auto calculationDpi = std::max(dpi, m_cachedDpi);
        const auto baseDpi = static_cast<uint32_t>(MOLTEN_PLATFORM_BASE_DPI);
        const auto calculationScale = static_cast<float>(calculationDpi) / static_cast<float>(baseDpi);
        const auto calculationHeight = static_cast<uint32_t>(static_cast<float>(height) * calculationScale);

        FT_Error error = 0;

        FT_Face face = nullptr;
        if ((error = FTC_Manager_LookupFace(
            m_fontImpl->ftCacheManager,
            0,
            &face)) != 0)
        {
            return false;
        }

        //const FT_Int32 loadlags = FT_HAS_COLOR(face) ?
        //    (FT_LOAD_COLOR | FT_LOAD_RENDER | FT_LOAD_DEFAULT) :
        //    (FT_LOAD_RENDER | FT_LOAD_DEFAULT);
        const FT_Int32 loadlags = FT_LOAD_RENDER | FT_LOAD_DEFAULT;

        //const auto hasColor = FT_HAS_COLOR(face);

        // TODO: Skipping dpi for now.
        FTC_ImageTypeRec ftcImageType;
        ftcImageType.face_id = 0;
        ftcImageType.width = static_cast<FT_UInt>(calculationHeight);
        ftcImageType.height = static_cast<FT_UInt>(calculationHeight);
        ftcImageType.flags = loadlags;

        auto applyKering = [](const FT_Face face, FT_UInt& prevGlyphIndex, FT_UInt glyphIndex, int32_t& prevPenPos, int32_t& penPos)
        {
            if (FT_HAS_KERNING(face) && prevGlyphIndex)
            {
                FT_Vector delta;
                FT_Get_Kerning(face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta);

                prevPenPos += static_cast<int32_t>(delta.x >> 6);
                penPos = prevPenPos;
            }
            prevGlyphIndex = glyphIndex;
        };

        auto appendBounds = [](Bounds2i32& bounds, const Vector2<int32_t>& bearing, const int32_t penPos, FT_BitmapGlyph bitmap)
        {
            // Calc X dimension.
            const FT_Pos lowX = penPos + bearing.x;
            if (lowX < bounds.low.x)
            {
                bounds.low.x = lowX;
            }

            const FT_Pos highX = penPos + bearing.x + bitmap->bitmap.width;
            if (highX > bounds.high.x)
            {
                bounds.high.x = highX;
            }

            // Calc Y dimension.
            const FT_Pos lowY = bearing.y - bitmap->bitmap.rows;
            if (lowY < bounds.low.y)
            {
                bounds.low.y = lowY;
            }

            const FT_Pos highY = bearing.y;
            if (highY > bounds.high.y)
            {
                bounds.high.y = highY;
            }
        };

        auto movePen = [](int32_t& penPos, int32_t& prevPenPos, int32_t advance)
        {
            penPos += advance;
            prevPenPos = penPos;
        };

        auto updatePixelModel = [](FT_Pixel_Mode& mode, FT_Pixel_Mode newMode)
        {
            if(newMode >= mode)
            {
                mode = newMode;
            }
        };

        // Calculate bounds
        auto bounds = Bounds2<int32_t>{
            std::numeric_limits<int32_t>::max(),
            std::numeric_limits<int32_t>::max(),
            std::numeric_limits<int32_t>::min(),
            std::numeric_limits<int32_t>::min() };

        int32_t penPos = 0;
        int32_t prevPenPos = 0;
        FT_UInt prevGlyphIndex = 0;
        FT_Pixel_Mode currentPixelMode = FT_Pixel_Mode::FT_PIXEL_MODE_NONE;

        for (size_t i = 0; i < text.size(); i++)
        {
            const FT_UInt32 charCode = static_cast<FT_UInt32>(text[i]);
            const auto glyphIndex = FTC_CMapCache_Lookup(m_fontImpl->ftCMapCache, 0, 0, charCode);
            if (glyphIndex == 0)
            {
                return false;
            }

            FT_Glyph glyph = nullptr;          
            if ((error = FTC_ImageCache_Lookup(m_fontImpl->ftImageCache, &ftcImageType, glyphIndex, &glyph, nullptr)) != 0)
            {
                return false;
            }

            applyKering(face, prevGlyphIndex, glyphIndex, prevPenPos, penPos);

            const auto bitmap = reinterpret_cast<FT_BitmapGlyph>(glyph);
            const auto& metrics = face->glyph->metrics;
            const auto bearing = Vector2<int32_t>{ static_cast<int32_t>(metrics.horiBearingX >> 6), static_cast<int32_t>(metrics.horiBearingY >> 6) };
            appendBounds(bounds, bearing, penPos, bitmap);

            const auto pixelMode = static_cast<FT_Pixel_Mode>(bitmap->bitmap.pixel_mode);
            updatePixelModel(currentPixelMode, pixelMode);

            const auto horiAdvance = static_cast<int32_t>(metrics.horiAdvance >> 6);
            movePen(penPos, prevPenPos, horiAdvance);
        }

        if (currentPixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_NONE ||
            bounds.IsEmpty())
        {
            return false;
        }

        // Calalculate buffer size
        const auto bufferTextSize = Vector2ui32(bounds.GetSize());
        uint32_t pixelSize = 0;
        const auto imageFormat = GetPixelModeImageFormat(currentPixelMode, pixelSize);
        if(pixelSize == 0)
        {
            return false;
        }

        if(bufferTextSize.x > m_bufferDimensions.x || bufferTextSize.y > m_bufferDimensions.y || m_imageFormat != imageFormat)
        {
            AllocateNewBuffer(imageFormat, pixelSize, bufferTextSize, bufferTextSize);
        }
        else
        {
            CleanOldBuffer(bufferTextSize);
        }


        for (size_t i = 0; i < 1; i++)
        {
            const FT_UInt32 charCode = static_cast<FT_UInt32>(text[i]);
            const auto glyphIndex = FTC_CMapCache_Lookup(m_fontImpl->ftCMapCache, 0, 0, charCode);
            if (glyphIndex == 0)
            {
                return false;
            }

            FT_Glyph glyph = nullptr;
            if ((error = FTC_ImageCache_Lookup(m_fontImpl->ftImageCache, &ftcImageType, glyphIndex, &glyph, nullptr)) != 0)
            {
                return false;
            }

            const auto bitmap = reinterpret_cast<FT_BitmapGlyph>(glyph)->bitmap;
            const auto bitmapBuffer = bitmap.buffer;

            for (size_t y = 0; y < static_cast<size_t>(bitmap.rows); y++)
            {
                for (size_t x = 0; x < static_cast<size_t>(bitmap.width); x++)
                {
                    const size_t bufferIndex =
                        (x * static_cast<size_t>(m_pixelSize)) + 
                        (y * static_cast<size_t>(m_pixelSize) * static_cast<size_t>(m_bufferDimensions.x));

                    const size_t bitmapIndex = (y * static_cast<size_t>(bitmap.width)) + x;

                    auto val = bitmapBuffer[bitmapIndex];
                    m_buffer[bufferIndex] = val;
                }
            }

        }


        return true;
    }

    void FontSequence::AllocateNewBuffer(
        const ImageFormat imageFormat,
        const uint32_t pixelSize,
        const Vector2ui32& bufferDimensions,
        const Vector2ui32& bufferTextDimensions)
    {
        m_imageFormat = imageFormat;
        m_pixelSize = pixelSize;
        m_bufferDimensions = bufferDimensions;
        m_bufferTextBounds = { { 0, 0 }, bufferTextDimensions };

        const size_t bufferSize = 
            static_cast<size_t>(bufferDimensions.x) * 
            static_cast<size_t>(bufferDimensions.y) * 
            static_cast<size_t>(pixelSize);

        m_buffer = std::make_unique<uint8_t[]>(bufferSize);

        for(size_t y = 0; y < bufferTextDimensions.y; y++)
        {
            const auto bufferWidth = y * static_cast<size_t>(bufferDimensions.x) * static_cast<size_t>(pixelSize);
            auto* bufferPos = m_buffer.get() + bufferWidth;
            const auto textWidth = static_cast<size_t>(bufferTextDimensions.x) * static_cast<size_t>(pixelSize);
            std::memset(bufferPos, 0, textWidth);
        }
    }

    void FontSequence::CleanOldBuffer(const Vector2ui32& bitmapSize)
    {
        
    }

    void FontSequence::SetCachedDpi(uint32_t dpi)
    {
        m_cachedDpi = dpi;
    }

    void FontSequence::SetCachedFactor(const Vector2f32& factor)
    {
        m_cachedFactor = Vector2f32{ std::max(factor.x, 1.0f), std::max(factor.y, 1.0f) };
    }

    uint32_t FontSequence::GetCachedDpi() const
    {
        return m_cachedDpi;
    }

    const Vector2f32& FontSequence::GetCachedFactor() const
    {
        return m_cachedFactor;
    }

    const Vector2ui32& FontSequence::GetBufferDimensions() const
    {
        return m_bufferDimensions;
    }

    const Bounds2ui32& FontSequence::GetBufferTextBounds() const
    {
        return m_bufferTextBounds;
    }

    const uint8_t* FontSequence::GetBuffer() const
    {
        return m_buffer.get();
    }

    uint32_t FontSequence::GetBaseline() const
    {
        return m_baseline;
    }

    ImageFormat FontSequence::GetImageFormat() const
    {
        return m_imageFormat;
    }


    // Font implementations.
    Font::Font() :
        m_impl(new FontImplementation)
    {}

    Font::~Font()
    {
        delete m_impl;
    }

    Font::Font(Font&& font) noexcept :
        m_impl(new FontImplementation)
    {
        *m_impl = std::move(*font.m_impl);
    }

    Font& Font::operator =(Font&& font) noexcept
    {
        *m_impl = std::move(*font.m_impl);
        return *this;
    }

    bool Font::ReadFromFile(const std::string& filename)
    {
        m_impl->Destroy();

        std::ifstream file(filename, std::ifstream::binary);
        if(!file.is_open())
        {
            return false;
        }

        file.seekg(0, std::fstream::end);
        const auto fileSize = static_cast<size_t>(file.tellg());
        if (!fileSize)
        {
            return false;
        }
        file.seekg(0, std::fstream::beg);

        m_impl->fontData.resize(fileSize);
        file.read(reinterpret_cast<char*>(m_impl->fontData.data()), fileSize);

        return Initialize();
    }

    FontSequence Font::CreateSequence(const uint32_t cachedDpi, const Vector2f32& cachedFactor)
    {
        return FontSequence{ *m_impl, cachedDpi, cachedFactor };
    }

    bool Font::Initialize()
    {
        FT_Error error = 0;

        if ((error = FT_Init_FreeType(&(m_impl->ftLibrary))) != 0)
        {
            //  "Failed to initialize the FreeType library, error code: %i\n", error);
            return false;
        }

        if((error = FTC_Manager_New(
            m_impl->ftLibrary,
            1,
            1,
            1024 * 1024,
            FontImplementation::FtFaceRequester,
            m_impl,
            &m_impl->ftCacheManager)) != 0)
        {
            //  "Failed to create FreeType cache manager, error code: %i\n", error);
            return false;
        }

        if((error = FTC_CMapCache_New(m_impl->ftCacheManager, &m_impl->ftCMapCache)) != 0)
        {
            //  "Failed to create FreeType map cache, error code: %i\n", error);
            return false;
        }

        if ((error = FTC_ImageCache_New(m_impl->ftCacheManager, &m_impl->ftImageCache)) != 0)
        {
            //  "Failed to create FreeType image cache, error code: %i\n", error);
            return false;
        }

        // Load face to cache.
        FT_Face face = nullptr;
        if((error = FTC_Manager_LookupFace(
            m_impl->ftCacheManager,
            0,
            &face)) != 0)
        {
            return false;
        }

        if(face == nullptr)
        {
            return false;
        }

        return true;
    }

}
