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
#include "Molten/Utility/Utf8Decoder.hpp"
#include "ThirdParty/FreeType2/include/freetype/freetype.h"
#include "ThirdParty/FreeType2/include/freetype/ftcache.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>

namespace Molten::Gui
{

    // Global implementaitons.
    static bool GetPixelModeImageFormat(const FT_Pixel_Mode pixelMode, ImageFormat& imageFormat, uint32_t& imagePixelSize)
    {
        switch(pixelMode)
        {          
            case FT_Pixel_Mode::FT_PIXEL_MODE_MONO: 
            case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY: 
            case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY2:
            case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY4:
            case FT_Pixel_Mode::FT_PIXEL_MODE_LCD:
            case FT_Pixel_Mode::FT_PIXEL_MODE_LCD_V:  imageFormat = ImageFormat::URed8; imagePixelSize = 1; return true;
            case FT_Pixel_Mode::FT_PIXEL_MODE_BGRA:  imageFormat = ImageFormat::UBlue8Green8Red8Alpha8; imagePixelSize = 4; return true;
            case FT_Pixel_Mode::FT_PIXEL_MODE_NONE:
            default: break;
        }

        return false;
    }

    static void AppendBounds(Bounds2i32& bounds, const Vector2<int32_t>& bearing, const int32_t penPos, FT_BitmapGlyph bitmap)
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
    }

    static void ApplyKeringToPenPos(const FT_Face face, FT_UInt& prevGlyphIndex, FT_UInt glyphIndex, int32_t& penPos)
    {
        if (FT_HAS_KERNING(face) && prevGlyphIndex)
        {
            FT_Vector delta;
            FT_Get_Kerning(face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta);

            penPos += static_cast<int32_t>(delta.x >> 6);
        }
        prevGlyphIndex = glyphIndex;
    }

    static void UpdatePixelModel(FT_Pixel_Mode& mode, FT_Pixel_Mode newMode)
    {
        if (newMode >= mode)
        {
            mode = newMode;
        }
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
        m_bufferSize(0),
        m_imageDimensions(0, 0),
        m_imageFormat(ImageFormat::URed8),
        m_imagePixelSize(0),
        m_baseline(0)
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
        m_bufferSize(0),
        m_imageDimensions(0, 0),
        m_imageFormat(ImageFormat::URed8),
        m_imagePixelSize(0),
        m_baseline(0)
    {}

    FontSequence::FontSequence(FontSequence&& fontSequence) noexcept :
        m_fontImpl(fontSequence.m_fontImpl),
        m_cachedDpi(fontSequence.m_cachedDpi),
        m_cachedFactor(fontSequence.m_cachedFactor),
        m_buffer(std::move(fontSequence.m_buffer)),
        m_bufferSize(fontSequence.m_bufferSize),
        m_imageDimensions(fontSequence.m_imageDimensions),
        m_imageFormat(fontSequence.m_imageFormat),
        m_imagePixelSize(fontSequence.m_imagePixelSize),
        m_baseline(fontSequence.m_baseline)
    {
        fontSequence.m_fontImpl = nullptr;
        fontSequence.m_bufferSize = 0;
        fontSequence.m_imageDimensions = { 0, 0 };
        fontSequence.m_imagePixelSize = 0;
        fontSequence.m_baseline = 0;
    }

    FontSequence& FontSequence::operator =(FontSequence&& fontSequence) noexcept
    {
        m_fontImpl = fontSequence.m_fontImpl;
        m_cachedDpi = fontSequence.m_cachedDpi;
        m_cachedFactor = fontSequence.m_cachedFactor;
        m_buffer = std::move(fontSequence.m_buffer);
        m_bufferSize = fontSequence.m_bufferSize;
        m_imageDimensions = fontSequence.m_imageDimensions;
        m_imageFormat = fontSequence.m_imageFormat;
        m_imagePixelSize = fontSequence.m_imagePixelSize;
        m_baseline = fontSequence.m_baseline;

        fontSequence.m_fontImpl = nullptr;
        fontSequence.m_bufferSize = 0;
        fontSequence.m_imageDimensions = { 0, 0 };
        fontSequence.m_imagePixelSize = 0;
        fontSequence.m_baseline = 0;

        return *this;
    }

    FontSequenceBitmapResult FontSequence::CreateBitmap(
        const std::string& text,
        const uint32_t dpi,
        const uint32_t height)
    {
        const auto cachedDpi = std::max(dpi, m_cachedDpi);
        const auto cachedScale = static_cast<float>(cachedDpi) / static_cast<float>(MOLTEN_PLATFORM_BASE_DPI);
        const auto cachedScaleVec = m_cachedFactor * cachedScale;
        const auto fontScale = static_cast<float>(dpi) / static_cast<float>(MOLTEN_PLATFORM_BASE_DPI);
        const auto fontHeight = static_cast<uint32_t>(static_cast<float>(height) * fontScale);

        /*const auto calculationDpi = std::max(dpi, m_cachedDpi);
        const auto baseDpi = static_cast<uint32_t>(MOLTEN_PLATFORM_BASE_DPI);
        const auto calculationScale = static_cast<float>(calculationDpi) / static_cast<float>(baseDpi);
        const auto calculationHeight = static_cast<uint32_t>(static_cast<float>(height) * calculationScale);*/

        FT_Error error = 0;

        FT_Face face = nullptr;
        if ((error = FTC_Manager_LookupFace(
            m_fontImpl->ftCacheManager,
            0,
            &face)) != 0)
        {
            return FontSequenceBitmapResult::Failure;
        }

        //const FT_Int32 loadlags = FT_HAS_COLOR(face) ?
        //    (FT_LOAD_COLOR | FT_LOAD_RENDER | FT_LOAD_DEFAULT) :
        //    (FT_LOAD_RENDER | FT_LOAD_DEFAULT);
        const FT_Int32 loadlags = FT_LOAD_RENDER | FT_LOAD_DEFAULT | FT_LOAD_COLOR;

        //const auto hasColor = FT_HAS_COLOR(face);

        // TODO: Skipping dpi for now.
        FTC_ImageTypeRec ftcImageType;
        ftcImageType.face_id = 0;
        ftcImageType.width = static_cast<FT_UInt>(fontHeight);
        ftcImageType.height = static_cast<FT_UInt>(fontHeight);
        ftcImageType.flags = loadlags;

        auto bounds = Bounds2<int32_t>{
            std::numeric_limits<int32_t>::max(),
            std::numeric_limits<int32_t>::max(),
            std::numeric_limits<int32_t>::min(),
            std::numeric_limits<int32_t>::min() };

        
        int32_t penPos = 0;
        FT_UInt prevGlyphIndex = 0;
        FT_Pixel_Mode currentPixelMode = FT_Pixel_Mode::FT_PIXEL_MODE_NONE;

        // Run a pre-pass, getting bounds of text and validation of glyphs.
        const auto utf8Encoder = Utf8Decoder{ text };
        for(auto it = utf8Encoder.begin(); it != utf8Encoder.end(); ++it)
        {
            const FT_UInt32 charCode = static_cast<FT_UInt32>(*it);
            const auto glyphIndex = FTC_CMapCache_Lookup(m_fontImpl->ftCMapCache, 0, 0, charCode);
            if (glyphIndex == 0)
            {
                return FontSequenceBitmapResult::Failure;
            }

            FT_Glyph glyph = nullptr;
            if ((error = FTC_ImageCache_Lookup(m_fontImpl->ftImageCache, &ftcImageType, glyphIndex, &glyph, nullptr)) != 0)
            {
                // Skip missing/invalid glyph.
                continue;
            }  

            ApplyKeringToPenPos(face, prevGlyphIndex, glyphIndex, penPos);

            const auto bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
            const auto bearing = Vector2i32{ bitmapGlyph->left, bitmapGlyph->top };
            AppendBounds(bounds, bearing, penPos, bitmapGlyph);

            const auto pixelMode = static_cast<FT_Pixel_Mode>(bitmapGlyph->bitmap.pixel_mode);
            if (pixelMode != FT_Pixel_Mode::FT_PIXEL_MODE_GRAY && pixelMode != FT_Pixel_Mode::FT_PIXEL_MODE_BGRA)
            {
                // Skip unsupported pixel mode of glyph.
                continue;
            }

            UpdatePixelModel(currentPixelMode, pixelMode);
         
            const auto horiAdvance = static_cast<int32_t>(glyph->advance.x >> 16);
            penPos += horiAdvance;
        }

        // Process pre-pass, create new buffer if needed.
        if (currentPixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_NONE || bounds.IsEmpty())
        {
            return FontSequenceBitmapResult::Failure;
        }
        
        m_imageDimensions = Vector2ui32(bounds.GetSize());
        m_baseline = -bounds.top;
        if(!GetPixelModeImageFormat(currentPixelMode, m_imageFormat, m_imagePixelSize))
        {
            return FontSequenceBitmapResult::Failure;
        }

        const auto requiredBufferSize = (m_imageDimensions.x * m_imageDimensions.y) * m_imagePixelSize;
        if(requiredBufferSize == 0)
        {
            return FontSequenceBitmapResult::Empty;
        }

        const FontSequenceBitmapResult bitmapResult = AllocateNewBufferIfRequired(m_imageDimensions, cachedScaleVec) ?
            FontSequenceBitmapResult::NewBuffer :
            FontSequenceBitmapResult::UpdatedBuffer;

        // Write lambdas.
        auto writeGrayToBgra = [&](Vector2size position, const FT_Bitmap& bitmap)
        {
        };

        auto writeGrayToGray = [&](Vector2size position, const FT_Bitmap& bitmap)
        {
            const auto bitmapWidth = static_cast<size_t>(bitmap.width);
            const auto bitmapRows = static_cast<size_t>(bitmap.rows);

            const size_t yStart = position.y * static_cast<size_t>(m_imageDimensions.x);

            for (size_t y = 0; y < bitmapRows; y++)
            {
                const size_t bufferIndex = yStart + (y * static_cast<size_t>(m_imageDimensions.x)) + position.x;
                const size_t bitmapIndex = (y * bitmapWidth);
                auto* destination = m_buffer.get() + bufferIndex;
                const auto* source = bitmap.buffer + bitmapIndex;

                std::memcpy(destination, source, bitmapWidth);
            }
        };

        auto writeBgra = [&](Vector2size position, const FT_Bitmap& bitmap)
        {
            const auto bitmapWidth = static_cast<size_t>(bitmap.width);
            const auto bitmapRows = static_cast<size_t>(bitmap.rows);

            const size_t xPosPixelSize = position.x * static_cast<size_t>(m_imagePixelSize);
            const size_t imageWidthPixelSize = static_cast<size_t>(m_imageDimensions.x) * static_cast<size_t>(m_imagePixelSize);
            const size_t bitmapWidthPixelSize = bitmapWidth * static_cast<size_t>(m_imagePixelSize);

            for (size_t y = 0; y < bitmapRows; y++)
            {
                const size_t bufferIndex = (y * imageWidthPixelSize) + xPosPixelSize;
                const size_t bitmapIndex = (y * bitmapWidthPixelSize);
                auto* destination = m_buffer.get() + bufferIndex;
                const auto* source = bitmap.buffer + bitmapIndex;

                std::memcpy(destination, source, bitmapWidth * static_cast<size_t>(m_imagePixelSize));
            }
        };

        std::function writeGray = writeGrayToGray;
        if (currentPixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_BGRA)
        {
            writeGray = writeGrayToBgra;
        }

        // Write bitmaps to buffer.
        penPos = 0;
        prevGlyphIndex = 0;

        for(auto it = utf8Encoder.begin(); it != utf8Encoder.end(); ++it)
        {
            const FT_UInt32 charCode = static_cast<FT_UInt32>(*it);
            const auto glyphIndex = FTC_CMapCache_Lookup(m_fontImpl->ftCMapCache, 0, 0, charCode);
            if (glyphIndex == 0)
            {
                return FontSequenceBitmapResult::Failure;
            }

            FT_Glyph glyph = nullptr;
            if ((error = FTC_ImageCache_Lookup(m_fontImpl->ftImageCache, &ftcImageType, glyphIndex, &glyph, nullptr)) != 0)
            {
                // Skip missing/invalid glyph.
                continue;
            }

            ApplyKeringToPenPos(face, prevGlyphIndex, glyphIndex, penPos);

            const auto bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
            const auto bitmap = bitmapGlyph->bitmap;
            const auto pixelMode = static_cast<FT_Pixel_Mode>(bitmap.pixel_mode);

            const int32_t writePosY = 
                (static_cast<int32_t>(m_imageDimensions.y) - m_baseline) -
                (bitmapGlyph->top - static_cast<int32_t>(bitmap.rows)) -
                static_cast<int32_t>(bitmap.rows);

            auto writePosX = penPos + bitmapGlyph->left;
            if(writePosX < 0)
            {
                penPos -= writePosX;
                writePosX = 0;
            }

            if (pixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_GRAY)
            {
                writeGray({ writePosX, writePosY }, bitmap);
            }
            else if(pixelMode == FT_Pixel_Mode::FT_PIXEL_MODE_BGRA)
            {
                writeBgra({ writePosX, writePosY }, bitmap);
            }

            const auto horiAdvance = static_cast<int32_t>(glyph->advance.x >> 16);
            penPos += horiAdvance;
        }

        return bitmapResult;
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

    const uint8_t* FontSequence::GetBuffer() const
    {
        return m_buffer.get();
    }

    int32_t FontSequence::GetBaseline() const
    {
        return m_baseline;
    }

    ImageFormat FontSequence::GetImageFormat() const
    {
        return m_imageFormat;
    }

    const Vector2ui32& FontSequence::GetImageDimensions() const
    {
        return m_imageDimensions;
    }

    bool FontSequence::AllocateNewBufferIfRequired(const Vector2ui32& imageSize, const Vector2f32& cacheScale)
    {
        const uint32_t requiredSize = imageSize.x * imageSize.y * m_imagePixelSize;

        if(requiredSize <= m_bufferSize)
        {
            return false;
        }

        auto const allocationImageSize = Vector2ui32{
            std::ceil(static_cast<float>(imageSize.x) * cacheScale.x),
            std::ceil(static_cast<float>(imageSize.y) * cacheScale.y)
        };

        m_bufferSize = allocationImageSize.x * allocationImageSize.y * m_imagePixelSize;
        m_buffer = std::make_unique<uint8_t[]>(m_bufferSize);

        return true;
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
