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
#include "Molten/Utility/SmartFunction.hpp"
#include "ThirdParty/FreeType2/include/freetype/freetype.h"
#include "ThirdParty/FreeType2/include/freetype/ftcache.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <exception>

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
#include "Molten/Platform/Win32Headers.hpp"
#endif

namespace Molten::Gui
{

    // Global implementaitons.
    /*static bool GetPixelModeImageFormat(const FT_Pixel_Mode pixelMode, ImageFormat& imageFormat, uint32_t& imagePixelSize)
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
    }*/


    //  PIMPL implementations.
    /*struct FontImplementation
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
    };*/


    // Font sequence implementations.
    /*FontSequence::FontSequence() :
        m_fontImpl(nullptr),
        m_cachedDpi(0),
        m_cachedFactor(1.0f),
        m_buffer(nullptr),
        m_bufferSize(0),
        m_cachedImageDimensions(0, 0),
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
        m_cachedImageDimensions(0, 0),
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
        m_cachedImageDimensions(fontSequence.m_cachedImageDimensions),
        m_imageDimensions(fontSequence.m_imageDimensions),
        m_imageFormat(fontSequence.m_imageFormat),
        m_imagePixelSize(fontSequence.m_imagePixelSize),
        m_baseline(fontSequence.m_baseline)
    {
        fontSequence.m_fontImpl = nullptr;
        fontSequence.m_bufferSize = 0;
        fontSequence.m_cachedImageDimensions = { 0, 0 };
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
        m_cachedImageDimensions = fontSequence.m_cachedImageDimensions;
        m_imageDimensions = fontSequence.m_imageDimensions;
        m_imageFormat = fontSequence.m_imageFormat;
        m_imagePixelSize = fontSequence.m_imagePixelSize;
        m_baseline = fontSequence.m_baseline;

        fontSequence.m_fontImpl = nullptr;
        fontSequence.m_bufferSize = 0;
        fontSequence.m_cachedImageDimensions = { 0, 0 };
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

    const Vector2ui32& FontSequence::GetCachedImageDimensions() const
    {
        return m_cachedImageDimensions;
    }

    bool FontSequence::AllocateNewBufferIfRequired(const Vector2ui32& imageSize, const Vector2f32& cacheScale)
    {
        const uint32_t requiredSize = imageSize.x * imageSize.y * m_imagePixelSize;

        if(requiredSize <= m_bufferSize)
        {
            return false;
        }

        m_cachedImageDimensions = Vector2ui32{
            std::ceil(static_cast<float>(imageSize.x) * cacheScale.x),
            std::ceil(static_cast<float>(imageSize.y) * cacheScale.y)
        };

        m_bufferSize = m_cachedImageDimensions.x * m_cachedImageDimensions.y * m_imagePixelSize;
        m_buffer = std::make_unique<uint8_t[]>(m_bufferSize);

        return true;
    }*/


    // Global implementations.
    static bool ConvertPixelModeToImageFormat(FT_Pixel_Mode pixelMode, FontAtlasImageFormat& imageFormat)
    {
        switch(pixelMode)
        {
            case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY: imageFormat = FontAtlasImageFormat::Gray; return true;
            case FT_Pixel_Mode::FT_PIXEL_MODE_BGRA: imageFormat = FontAtlasImageFormat::BlueGreenRedAlpha; return true;
            default: break;
        }
        return false;
    }


    // PIMPL declarations.
    struct FontRepositoryImpl
    {
        FontRepositoryImpl(
            FontRepository& repository,
            FontNameRepository& nameRepository);

        Font* GetFont(const std::string& fontFamily);

        Font* GetOrCreateFont(const std::string& fontFamily);

        static std::vector<uint8_t> ReadFontFile(const std::string& filename);

        bool AddAtlasNewEvent(FontAtlas* atlas);
        bool AddAtlasUpdateEvent(FontAtlas* atlas);

        void ForEachAtlasEvent(std::function<void(FontAtlasEventType, FontAtlas*)>&& callback);

        FontRepository& repository;
        FontNameRepository& nameRepository;
        FT_Library ftLibrary;        
        std::map<std::string, std::unique_ptr<Font>> fonts;

        FontAtlas* lastAffectedAtlas;
        std::vector<FontAtlas*> atlasUpdateEvents;
        std::vector<FontAtlas*> atlasNewEvents;

    };

    struct FontImpl
    {
        FontImpl(
            FontRepositoryImpl& fontRepositoryImpl,
            const std::vector<uint8_t>&& fontData);

        FT_Error LoadFont();

        FontGroupedSequence CreateGroupedSequence(
            const std::string& text,
            const uint32_t dpi,
            const uint32_t height);

        FontAtlasBin* GetOrCreateAtlasBin(const uint32_t fontHeight);

        static FT_Error FtFaceRequester(
             FTC_FaceID faceId,
             FT_Library library,
             FT_Pointer reqData,
             FT_Face* face);

        FontRepositoryImpl& fontRepositoryImpl;
        const std::vector<uint8_t> data;
        FTC_Manager ftCacheManager;
        FTC_CMapCache ftCMapCache;
        FTC_ImageCache ftImageCache;    
        std::map<uint32_t, std::unique_ptr<FontAtlasBin>> m_atlasBins; ///< Height : FontAtlasBin

    };


    // PIMPL implementations.
    FontRepositoryImpl::FontRepositoryImpl(
        FontRepository& repository,
        FontNameRepository& nameRepository
    ) :
        repository(repository),
        nameRepository(nameRepository),
        ftLibrary(nullptr),
        lastAffectedAtlas(nullptr)
    {
        FT_Error error = 0;

        if ((error = FT_Init_FreeType(&ftLibrary)) != 0)
        {
            throw std::runtime_error("Failed to initialize FreeType library, error code: " + std::to_string(error));
        }
    }

    Font* FontRepositoryImpl::GetFont(const std::string& fontFamily)
    {
        if (auto it = fonts.find(fontFamily); it != fonts.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    Font* FontRepositoryImpl::GetOrCreateFont(const std::string& fontFamily)
    {
        if (auto it = fonts.find(fontFamily); it != fonts.end())
        {
            return it->second.get();
        }

        const auto fontPath = nameRepository.FindFontFamilyPath(fontFamily);
        if (fontPath.empty())
        {
            return nullptr;
        }

        const auto fontData = ReadFontFile(fontPath);
        if (fontData.empty())
        {
            return nullptr;
        }

        auto fontImpl = std::make_unique<FontImpl>(*this, std::move(fontData));
        if(fontImpl->LoadFont() != 0)
        {
            return nullptr;
        }

        auto newFont = std::make_unique<Font>(fontImpl.release());
        auto* newFontPtr = newFont.get();

        fonts.insert({ fontFamily, std::move(newFont) });

        return newFontPtr;
    }

    std::vector<uint8_t> FontRepositoryImpl::ReadFontFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ifstream::binary);
        if (!file.is_open())
        {
            return {};
        }

        file.seekg(0, std::fstream::end);
        const auto fileSize = static_cast<size_t>(file.tellg());
        if (!fileSize)
        {
            return {};
        }
        file.seekg(0, std::fstream::beg);

        std::vector<uint8_t> fontData;
        fontData.resize(fileSize);
        file.read(reinterpret_cast<char*>(fontData.data()), fileSize);
        return fontData;
    }

    bool FontRepositoryImpl::AddAtlasNewEvent(FontAtlas* atlas)
    {
        for (auto* eventNew : atlasNewEvents)
        {
            if (atlas == eventNew)
            {
                return true;
            }
        }

        atlasNewEvents.push_back(atlas);
        lastAffectedAtlas = atlas;
        return false;
    }

    bool FontRepositoryImpl::AddAtlasUpdateEvent(FontAtlas* atlas)
    {
        for (auto* eventUpdate : atlasUpdateEvents)
        {
            if (atlas == eventUpdate)
            {
                return true;
            }
        }

        atlasUpdateEvents.push_back(atlas);
        lastAffectedAtlas = atlas;
        return false;
    }

    void FontRepositoryImpl::ForEachAtlasEvent(std::function<void(FontAtlasEventType, FontAtlas*)>&& callback)
    {
        for (auto* eventNew : atlasNewEvents)
        {
            callback(FontAtlasEventType::New, eventNew);
        }
        atlasNewEvents.clear();

        for (auto* updateEvent : atlasUpdateEvents)
        {
            callback(FontAtlasEventType::Update, updateEvent);
        }
        atlasUpdateEvents.clear();

        lastAffectedAtlas = nullptr;
    }

    FontImpl::FontImpl(
        FontRepositoryImpl& fontRepositoryImpl,
        const std::vector<uint8_t>&& fontData
    ) :
        fontRepositoryImpl(fontRepositoryImpl),
        data(fontData),
        ftCacheManager(nullptr),
        ftCMapCache(nullptr),
        ftImageCache(nullptr)
    {}

    FT_Error FontImpl::LoadFont()
    {
        const FT_UInt maxFaces = 1;
        const FT_UInt maxSizes = 1;
        const FT_ULong maxBytes = 1024 * 1024;

        FT_Error error = 0;
        if ((error = FTC_Manager_New(fontRepositoryImpl.ftLibrary, maxFaces, maxSizes, maxBytes, FtFaceRequester, this, &ftCacheManager)) != 0)
        {
            return error;
        }

        if ((error = FTC_CMapCache_New(ftCacheManager, &ftCMapCache)) != 0)
        {
            return error;
        }

        if ((error = FTC_ImageCache_New(ftCacheManager, &ftImageCache)) != 0)
        {
            return error;
        }

        FT_Face face = nullptr;
        if ((error = FTC_Manager_LookupFace(ftCacheManager, 0, &face)) != 0)
        {
            return error;
        }

        if (face == nullptr)
        {
            return error;
        }
       
        return 0;
    }

    FontGroupedSequence FontImpl::CreateGroupedSequence(
        const std::string& text,
        const uint32_t dpi,
        const uint32_t height)
    {
        FT_Face face = nullptr;
        if (FTC_Manager_LookupFace(ftCacheManager, 0, &face) != 0)
        {
            return {};
        }

        const auto fontScale = static_cast<float>(dpi) / static_cast<float>(MOLTEN_PLATFORM_BASE_DPI);
        const auto fontHeight = static_cast<uint32_t>(static_cast<float>(height) * fontScale);

        FontAtlasBin* atlasBin = GetOrCreateAtlasBin(fontHeight);

        const FT_Int32 loadlags = FT_HAS_COLOR(face) ?
            (FT_LOAD_DEFAULT | FT_LOAD_RENDER | FT_LOAD_COLOR) :
            (FT_LOAD_DEFAULT | FT_LOAD_RENDER);

        FTC_ImageTypeRec ftcImageType;
        ftcImageType.face_id = 0;
        ftcImageType.width = static_cast<FT_UInt>(fontHeight);
        ftcImageType.height = static_cast<FT_UInt>(fontHeight);
        ftcImageType.flags = loadlags;

        auto* lastAffectedAtlas = fontRepositoryImpl.repository.GetLastAffectedAtlas();
        auto createNewGlyph = [&](const uint32_t codePoint) -> FontAtlasGlyph*
        {
            const auto glyphIndex = FTC_CMapCache_Lookup(ftCMapCache, 0, 0, codePoint);
            if (glyphIndex == 0)
            {
                return nullptr;
            }

            FT_Glyph glyph = nullptr;
            if (FTC_ImageCache_Lookup(ftImageCache, &ftcImageType, glyphIndex, &glyph, nullptr) != 0)
            {
                return nullptr;
            }

            const auto* bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
            auto& bitmap = bitmapGlyph->bitmap;

            const auto pixelMode = static_cast<FT_Pixel_Mode>(bitmap.pixel_mode);
            FontAtlasImageFormat imageFormat;
            if (!ConvertPixelModeToImageFormat(pixelMode, imageFormat))
            {
                return nullptr;
            }

            const auto glyphDimensions = Vector2ui32{ bitmap.width, bitmap.rows };
            return atlasBin->CreateGlyph(codePoint, bitmap.buffer, glyphDimensions, imageFormat, lastAffectedAtlas);
        };


        auto textBounds = Bounds2<int32_t>{
            std::numeric_limits<int32_t>::max(),
            std::numeric_limits<int32_t>::max(),
            std::numeric_limits<int32_t>::min(),
            std::numeric_limits<int32_t>::min()
        };

        FontGroupedSequence groupedSequence;
        std::vector<std::shared_ptr<FontSequenceGroup>> sequenceGroups;
        FontAtlas* currentAtlas = nullptr;
        //FontSequenceGroup* currentSequenceGroup = nullptr;
        //int32_t penPos = 0;

        for(const auto codePoint : Utf8Decoder{ text })
        {
            FontAtlasGlyph* fontAtlasGlyph = atlasBin->FindGlyph(codePoint);
            if(fontAtlasGlyph == nullptr)
            {
                fontAtlasGlyph = createNewGlyph(codePoint);
                if(fontAtlasGlyph == nullptr)
                {
                    continue;
                }        
            }

            if(fontAtlasGlyph->atlas != currentAtlas)
            {
                currentAtlas = fontAtlasGlyph->atlas;
            }
            
          

        }

        // Found no glyphs for this text, so we return.
        if(sequenceGroups.empty())
        {
            return {};
        }

        // Finished.
        groupedSequence.size = textBounds.GetSize();
        return groupedSequence;
    }

    FontAtlasBin* FontImpl::GetOrCreateAtlasBin(const uint32_t fontHeight)
    {
        if(auto it = m_atlasBins.find(fontHeight); it != m_atlasBins.end())
        {
            return it->second.get();
        }

        auto it = m_atlasBins.insert({ fontHeight, std::make_unique<FontAtlasBin>(fontRepositoryImpl.repository) });       
        return it.first->second.get();
    }

    FT_Error FontImpl::FtFaceRequester(
        FTC_FaceID,
        FT_Library library,
        FT_Pointer reqData,
        FT_Face* face)
    {
        auto* fontImpl = static_cast<FontImpl*>(reqData);

        FT_Error error = 0;

        // Load face from memory.
        if ((error = FT_New_Memory_Face(
            library,
            reinterpret_cast<const FT_Byte*>(fontImpl->data.data()),
            static_cast<FT_Long>(fontImpl->data.size()),
            0,
            face)) != 0)
        {
            return error;
        }

        // Select the unicode character map
        if ((error = FT_Select_Charmap(
            *face,
            FT_ENCODING_UNICODE)) != 0)
        {
            return error;
        }

        return error;
    }


    // Font name solver implementations.
    void FontNameRepository::ClearPathCache()
    {
        m_cachedFontPaths.clear();
    }

    bool FontNameRepository::AddDirectory(const std::string& directory)
    {
        std::filesystem::path path{ directory };
        if(!std::filesystem::is_directory(path))
        {
            return false;
        }

        std::string genericDirectory = path.generic_u8string();

        if(auto it = std::find(m_fontDirectories.begin(), m_fontDirectories.end(), genericDirectory); it != m_fontDirectories.end())
        {
            return true;
        }

        m_fontDirectories.push_back(std::move(genericDirectory));
        return true;
    }

    void FontNameRepository::AddSystemDirectories()
    {
    #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

        PWSTR windowsFontDirectoryPtr = nullptr;

        SmartFunction destroyer = [&]()
        {
            if (windowsFontDirectoryPtr)
            {
                CoTaskMemFree(windowsFontDirectoryPtr);
            }
        };

        std::string windowsFontDirectory;

        // This is some ugly code, not supporting unicode paths.
        bool parsedPath = false;
        if (SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &windowsFontDirectoryPtr) == S_OK)
        {
            for (size_t i = 0; i < 512; i++)
            {
                if (windowsFontDirectoryPtr[i] == 0)
                {
                    parsedPath = true;
                    break;
                }
                windowsFontDirectory.push_back(static_cast<char>(windowsFontDirectoryPtr[i]));
            }
        }

        if (parsedPath && !windowsFontDirectory.empty())
        {
            AddDirectory(windowsFontDirectory);
        }

    #endif
    }

    std::string FontNameRepository::FindFontFamilyPath(const std::string& fontFamily)
    {
        auto lowercaseFontFamily = fontFamily;
        std::transform(lowercaseFontFamily.begin(), lowercaseFontFamily.end(), lowercaseFontFamily.begin(), [](const auto c)
        {
            return static_cast<char>(std::tolower(static_cast<int>(c)));
        });

        if (auto it = m_cachedFontPaths.find(fontFamily); it != m_cachedFontPaths.end())
        {
            return it->second;
        }

        for(const auto& fontDirectory : m_fontDirectories)
        {
            for (auto& dirEntry : std::filesystem::directory_iterator(fontDirectory))
            {
                if(!dirEntry.is_regular_file())
                {
                    continue;
                }

                auto dirEntryName = dirEntry.path().stem().u8string();
                std::transform(dirEntryName.begin(), dirEntryName.end(), dirEntryName.begin(), [](const auto c)
                {
                    return static_cast<char>(std::tolower(static_cast<int>(c)));
                });

                if(dirEntryName == lowercaseFontFamily)
                {
                    auto fontPath = dirEntry.path().generic_u8string();
                    m_cachedFontPaths.insert({ lowercaseFontFamily, fontPath });
                    return fontPath;
                }
            }
        }

        return "";
    }


    // Font atlas implementations.
    FontAtlas::FontAtlas(
        std::unique_ptr<uint8_t[]>&& buffer,
        const size_t bufferSize,
        const FontAtlasImageFormat imageFormat,
        const Vector2ui32& imageDimensions
    ) :
        metaData(nullptr),
        m_buffer(std::move(buffer)),
        m_bufferSize(bufferSize),
        m_imageFormat(imageFormat),
        m_imageDimensions(imageDimensions),
        m_currentPosition(0, 0),
        m_nextRow(0)
    {}

    uint8_t* FontAtlas::GetBuffer()
    {
        return m_buffer.get();
    }
    const uint8_t* FontAtlas::GetBuffer() const
    {
        return m_buffer.get();
    }

    size_t FontAtlas::GetBufferSize() const
    {
        return m_bufferSize;
    }

    FontAtlasImageFormat FontAtlas::GetImageFormat() const
    {
        return m_imageFormat;
    }

    const Vector2ui32& FontAtlas::GetImageDimensions() const
    {
        return m_imageDimensions;
    }

    Bounds2ui32 FontAtlas::RequestSize(const Vector2ui32& size)
    {
        const auto requestSize = size + Vector2ui32{ 2, 2 };

        if(requestSize.x >= m_imageDimensions.x)
        {
            return {};
        }

        if(m_currentPosition.x + requestSize.x >= m_imageDimensions.x)
        {
            m_currentPosition = { 0, m_nextRow };
        }

        if (m_currentPosition.y + requestSize.y >= m_imageDimensions.y)
        {
            return {};
        }

        if(const auto nextRow = m_currentPosition.y + requestSize.y; nextRow > m_nextRow)
        {
            m_nextRow = nextRow;
        }

        const auto foundBounds = Bounds2ui32{ m_currentPosition, m_currentPosition + requestSize };
        m_currentPosition.x += requestSize.x;

        return foundBounds;
    }


    // Font atlas bin implementations.
    FontAtlasBin::FontAtlasBin(FontRepository& fontRepository) :
        fontRepository(&fontRepository)
    {}

    FontAtlasGlyph* FontAtlasBin::FindGlyph(const uint32_t codePoint)
    {
        if(auto it = m_glyphs.find(codePoint); it != m_glyphs.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    FontAtlasGlyph* FontAtlasBin::CreateGlyph(
        const uint32_t codePoint,
        const void* buffer,
        const Vector2ui32& dimensions,
        FontAtlasImageFormat imageFormat,
        FontAtlas*& lastAffectedAtlas)
    {
        if(imageFormat == FontAtlasImageFormat::Gray)
        {
            return CreateGrayGlyph(codePoint, buffer, dimensions, lastAffectedAtlas);
        }
        return CreateBgraGlyph(codePoint, buffer, dimensions, lastAffectedAtlas);
    }

    /*bool FontAtlasBin::ForEachAtlasEvent(std::function<bool(FontAtlasEventType, FontAtlas*)>&& callback)
    {
        for (auto* atlas : m_eventNewAtlas)
        {
            if (!callback(FontAtlasEventType::New, atlas))
            {
                return false;
            }
        }
        for (auto* atlas : m_eventUpdateAtlas)
        {
            if (!callback(FontAtlasEventType::Update, atlas))
            {
                return false;
            }
        }

        return true;
    }

    bool FontAtlasBin::AddAtlasNewEvent(FontAtlas* atlas)
    {
        for (auto* eventNew : m_eventNewAtlas)
        {
            if (atlas == eventNew)
            {
                return true;
            }
        }

        m_eventNewAtlas.push_back(atlas);
        return false;
    }

    bool FontAtlasBin::AddAtlasUpdateEvent(FontAtlas* atlas)
    {
        for(auto* eventNew : m_eventUpdateAtlas)
        {
            if(atlas == eventNew)
            {
                return true;
            }
        }

        m_eventUpdateAtlas.push_back(atlas);
        return false;
    }*/

    FontAtlasGlyph* FontAtlasBin::CreateGrayGlyph(
        const uint32_t codePoint,
        const void* buffer,
        const Vector2ui32& dimensions,
        FontAtlas*& lastAffectedAtlas)
    {
        auto createAtlas = [&]()
        {
            const auto imageDimensions = Vector2ui32{ 512, 512 };
            const size_t bufferSize = imageDimensions.x * imageDimensions.y;
            auto buffer = std::make_unique<uint8_t[]>(bufferSize);

            auto newAtlas = std::make_unique<FontAtlas>(
                std::move(buffer),
                bufferSize,
                FontAtlasImageFormat::Gray,
                imageDimensions);

            auto* newAtlasPtr = newAtlas.get();

            fontRepository->AddAtlasNewEvent(newAtlasPtr);
            lastAffectedAtlas = newAtlasPtr;

            m_grayAtlases.push_back(std::move(newAtlas));
            return newAtlasPtr;
        };

        
        FontAtlas* fontAtlas = nullptr;
        Bounds2ui32 glyphBounds;
        if (m_grayAtlases.empty())
        {
            fontAtlas = createAtlas();
            glyphBounds = fontAtlas->RequestSize(dimensions);
            if(glyphBounds.IsEmpty())
            {
                return nullptr;
            }
        }
        else
        {
            fontAtlas = m_grayAtlases.back().get();
            glyphBounds = fontAtlas->RequestSize(dimensions);
            if (glyphBounds.IsEmpty())
            {
                fontAtlas = createAtlas();
                glyphBounds = fontAtlas->RequestSize(dimensions);
                if (glyphBounds.IsEmpty())
                {
                    return nullptr;
                }
            }
        }

        const auto imageDimensions = fontAtlas->GetImageDimensions();
        auto* imageBuffer = fontAtlas->GetBuffer() + 1 + imageDimensions.x;
        for(uint32_t y = 0 ; y < dimensions.y; y++)
        {
            std::memcpy(imageBuffer, buffer, dimensions.x);
            imageBuffer += imageDimensions.x;
        }

        if(lastAffectedAtlas != fontAtlas)
        {
            fontRepository->AddAtlasUpdateEvent(fontAtlas);
            lastAffectedAtlas = fontAtlas;
        }

        auto atlasGlyph = std::make_shared<FontAtlasGlyph>(codePoint, fontAtlas, glyphBounds);
        auto it = m_glyphs.insert({ codePoint, atlasGlyph });
        return it.first->second.get();
    }

    FontAtlasGlyph* FontAtlasBin::CreateBgraGlyph(
        const uint32_t codePoint,
        const void* buffer,
        const Vector2ui32& dimensions,
        FontAtlas*& lastAffectedAtlas)
    {
        return nullptr;
    }

    /*FontAtlas* FontAtlasBin::GetOrCreateGrayAtlas()
    {
        

        return nullptr;
    }

    FontAtlas* FontAtlasBin::GetOrCreateBgraAtlas()
    {
        return nullptr;
    }*/


    // Font atlas glyph
    FontAtlasGlyph::FontAtlasGlyph(
        uint32_t codePoint,
        FontAtlas* atlas,
        const Bounds2ui32& textureBounds
    ) :
        codePoint(codePoint),
        atlas(atlas),     
        textureBounds(textureBounds)
    {}


    // Font repository implementations.
    FontRepository::FontRepository(
        FontNameRepository& nameRepository
    ) :
        m_impl(new FontRepositoryImpl(*this, nameRepository))
    {}

    FontRepository::~FontRepository()
    {
        delete m_impl;
    }

    Font* FontRepository::GetOrCreateFont(const std::string& fontFamily)
    {
        return m_impl->GetOrCreateFont(fontFamily);
    }
    const Font* FontRepository::GetOrCreateFont(const std::string& fontFamily) const
    {
        return m_impl->GetOrCreateFont(fontFamily);
    }

    Font* FontRepository::GetFont(const std::string& fontFamily)
    {
        return m_impl->GetFont(fontFamily);
    }
    const Font* FontRepository::GetFont(const std::string& fontFamily) const
    {
        return m_impl->GetFont(fontFamily);
    }

    bool FontRepository::AddAtlasNewEvent(FontAtlas* atlas)
    {
        return m_impl->AddAtlasNewEvent(atlas);
    }

    bool FontRepository::AddAtlasUpdateEvent(FontAtlas* atlas)
    {
        return m_impl->AddAtlasUpdateEvent(atlas);
    }

    FontAtlas* FontRepository::GetLastAffectedAtlas()
    {
        return m_impl->lastAffectedAtlas;
    }

    void FontRepository::ForEachAtlasEvent(std::function<void(FontAtlasEventType, FontAtlas*)>&& callback)
    {
        m_impl->ForEachAtlasEvent(std::move(callback));
    }


    // Font implementations.
    Font::Font(FontImpl* implementation) :
        m_impl(implementation)
    {}

    Font::~Font()
    {
        delete m_impl;
    }

    FontGroupedSequence Font::CreateGroupedSequence(
        const std::string& text,
        const uint32_t dpi,
        const uint32_t height)
    {
        return m_impl->CreateGroupedSequence(text, dpi, height);
    }


    // Font grouped glyph implementations.
    FontGlyph::FontGlyph(
        FontAtlasGlyph* atlasGlyph,
        const Vector2i32& position
    ) :
        atlasGlyph(atlasGlyph),
        position(position)
    {}


    // Font sequence grouped implementations.
    FontSequenceGroup::FontSequenceGroup(
        FontAtlas* atlas,
        const Bounds2i32& bounds,
        const Vector4f32& color
    ) :
        atlas(atlas),
        bounds(bounds),
        color(color)
    {}

}
