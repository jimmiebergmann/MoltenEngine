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

#include "Molten/Renderer/Font.hpp"
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
#include <cstring>

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
#include "Molten/Platform/Win32Headers.hpp"
#endif

namespace Molten::Gui
{

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
            FontNameRepository& nameRepository,
            const Vector2ui32& atlasDimensions);

        Font* GetFont(const std::string& fontFamily);

        Font* GetOrCreateFont(const std::string& fontFamily);

        static std::vector<uint8_t> ReadFontFile(const std::string& filename);

        bool AddAtlasNewEvent(FontAtlas* atlas);
        bool AddAtlasUpdateEvent(FontAtlas* atlas);

        void ForEachAtlasEvent(std::function<void(FontAtlasEventType, FontAtlas*)>&& callback);

        FontRepository& repository;
        FontNameRepository& nameRepository;
        const Vector2ui32 atlasDimensions;
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
        int32_t ascender;
        int32_t descender;
        FTC_Manager ftCacheManager;
        FTC_CMapCache ftCMapCache;
        FTC_ImageCache ftImageCache;    
        std::map<uint32_t, std::unique_ptr<FontAtlasBin>> m_atlasBins; ///< Height : FontAtlasBin

    };


    // PIMPL implementations.
    FontRepositoryImpl::FontRepositoryImpl(
        FontRepository& repository,
        FontNameRepository& nameRepository,
        const Vector2ui32& atlasDimensions
    ) :
        repository(repository),
        nameRepository(nameRepository),
        atlasDimensions(atlasDimensions),
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

        auto fontData = ReadFontFile(fontPath);
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
        ascender(0),
        descender(0),
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

        ascender = face->ascender >> 6;
        descender = face->descender >> 6;
       
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
             auto fontAtlasGlyph = atlasBin->CreateGlyph(codePoint, bitmap.buffer, glyphDimensions, imageFormat, lastAffectedAtlas);
            
            if(fontAtlasGlyph)
            {
                fontAtlasGlyph->glyphIndex = glyphIndex;
                fontAtlasGlyph->advance = { static_cast<int32_t>(glyph->advance.x >> 16), static_cast<int32_t>(glyph->advance.y >> 16) };
                fontAtlasGlyph->bearing = { bitmapGlyph->left, bitmapGlyph->top };
                fontAtlasGlyph->topPosition = -bitmapGlyph->top;
            }

            return fontAtlasGlyph;
        };

        FontGroupedSequence groupedSequence;
        FontAtlas* currentAtlas = nullptr;
        FontSequenceGroup* currentSequenceGroup = nullptr;
        auto getOrCreateSequenceGroup = [&](FontAtlas* fontAtlas) -> FontSequenceGroup*
        {
            // Use current if possible.
            if(fontAtlas == currentAtlas)
            {
                return currentSequenceGroup;
            }

            auto& groups = groupedSequence.groups;

            // Find existing.
            auto it = std::find_if(groups.begin(), groups.end(), [&](auto& sequenceGroup)
            {
                return sequenceGroup->atlas == fontAtlas;
            });

            if(it != groups.end())
            {
                currentAtlas = fontAtlas;
                currentSequenceGroup = it->get();
                return currentSequenceGroup;
            }

            // Create new.
            const auto newSequenceGroup = std::make_shared<FontSequenceGroup>(fontAtlas);

            newSequenceGroup->bounds = Bounds2<int32_t>{
               std::numeric_limits<int32_t>::max(),
               std::numeric_limits<int32_t>::max(),
               std::numeric_limits<int32_t>::min(),
               std::numeric_limits<int32_t>::min() };

            groups.push_back(newSequenceGroup);
            currentAtlas = fontAtlas;
            currentSequenceGroup = newSequenceGroup.get();

            return currentSequenceGroup;
        };

        const bool faceHasKerning = FT_HAS_KERNING(face);
        const FontAtlasGlyph* prevAtlasGlyph = nullptr;    
        auto getKerningFromPrevGlyph = [&](FontAtlasGlyph* atlasGlyph) -> int32_t
        {
            if(faceHasKerning && prevAtlasGlyph)
            {
                FT_Vector delta;
                FT_Get_Kerning(face, prevAtlasGlyph->glyphIndex, atlasGlyph->glyphIndex, FT_KERNING_DEFAULT, &delta);

                prevAtlasGlyph = atlasGlyph;
                return static_cast<int32_t>(delta.x >> 6);
            }

            prevAtlasGlyph = atlasGlyph;
            return 0;
        };

        groupedSequence.bounds = Bounds2<int32_t>{
            std::numeric_limits<int32_t>::max(),
            std::numeric_limits<int32_t>::max(),
            std::numeric_limits<int32_t>::min(),
            std::numeric_limits<int32_t>::min() };

        auto appendBounds = [](Bounds2<int32_t>& output, const Bounds2<int32_t>& input)
        {
            if (input.low.x < output.low.x)
            {
                output.low.x = input.low.x;
            }
            if (input.low.y < output.low.y)
            {
                output.low.y = input.low.y;
            }
            if (input.high.x > output.high.x)
            {
                output.high.x = input.high.x;
            }
            if (input.high.y > output.high.y)
            {
                output.high.y = input.high.y;
            }
        };

        int32_t penPos = 0;
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

            auto* sequenceGroup = getOrCreateSequenceGroup(fontAtlasGlyph->atlas);
            const auto glyphPosition = Vector2i32{ penPos + fontAtlasGlyph->bearing.x, fontAtlasGlyph->topPosition };
            const auto glyphTextureSize = fontAtlasGlyph->textureBounds.GetSize();

            auto newGlyph = std::make_shared<FontGlyph>(
                fontAtlasGlyph, 
                Bounds2i32{ glyphPosition, glyphPosition + glyphTextureSize } );

            appendBounds(sequenceGroup->bounds, newGlyph->bounds);
            appendBounds(groupedSequence.bounds, newGlyph->bounds);      

            sequenceGroup->glyphs.push_back(newGlyph);
            groupedSequence.glyphs.push_back(newGlyph);

            penPos += getKerningFromPrevGlyph(fontAtlasGlyph) + fontAtlasGlyph->advance.x;
        }

        // Found no glyphs for this text, so we return.
        if(groupedSequence.groups.empty())
        {
            return {};
        }

        // Finished.
        return groupedSequence;
    }

    FontAtlasBin* FontImpl::GetOrCreateAtlasBin(const uint32_t fontHeight)
    {
        if(auto it = m_atlasBins.find(fontHeight); it != m_atlasBins.end())
        {
            return it->second.get();
        }

        auto it = m_atlasBins.insert({ fontHeight, std::make_unique<FontAtlasBin>(fontRepositoryImpl.repository, fontRepositoryImpl.atlasDimensions) });
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

        SmartFunction destroyer([&]()
        {
            if (windowsFontDirectoryPtr)
            {
                CoTaskMemFree(windowsFontDirectoryPtr);
            }
        });

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
    FontAtlasBin::FontAtlasBin(
        FontRepository& fontRepository,
        const Vector2ui32& atlasDimensions
    ) :
        m_fontRepository(&fontRepository),
        m_atlasDimensions(atlasDimensions)
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

    FontAtlasGlyph* FontAtlasBin::CreateGrayGlyph(
        const uint32_t codePoint,
        const void* buffer,
        const Vector2ui32& dimensions,
        FontAtlas*& lastAffectedAtlas)
    {
        auto createAtlas = [&]()
        {
            const size_t bufferSize = m_atlasDimensions.x * m_atlasDimensions.y;
            auto newBuffer = std::make_unique<uint8_t[]>(bufferSize);

            auto newAtlas = std::make_unique<FontAtlas>(
                std::move(newBuffer),
                bufferSize,
                FontAtlasImageFormat::Gray,
                m_atlasDimensions);

            auto* newAtlasPtr = newAtlas.get();

            m_fontRepository->AddAtlasNewEvent(newAtlasPtr);
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

        const auto atlasDimensions = fontAtlas->GetImageDimensions();
        auto* atlasBufferPos = fontAtlas->GetBuffer() + (1 + glyphBounds.low.x) + ( (1 + glyphBounds.low.y) * atlasDimensions.x);
        auto* glyphBufferPos = static_cast<const uint8_t*>(buffer);

        for(uint32_t y = 0 ; y < dimensions.y; y++)
        {
            std::memcpy(atlasBufferPos, glyphBufferPos, dimensions.x);
            atlasBufferPos += atlasDimensions.x;
            glyphBufferPos += dimensions.x;
        }

        if(lastAffectedAtlas != fontAtlas)
        {
            m_fontRepository->AddAtlasUpdateEvent(fontAtlas);
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
        auto createAtlas = [&]()
        {
            const size_t bufferSize = m_atlasDimensions.x * m_atlasDimensions.y * 4;
            auto newBuffer = std::make_unique<uint8_t[]>(bufferSize);

            auto newAtlas = std::make_unique<FontAtlas>(
                std::move(newBuffer),
                bufferSize,
                FontAtlasImageFormat::BlueGreenRedAlpha,
                m_atlasDimensions);

            auto* newAtlasPtr = newAtlas.get();

            m_fontRepository->AddAtlasNewEvent(newAtlasPtr);
            lastAffectedAtlas = newAtlasPtr;

            m_bgraAtlases.push_back(std::move(newAtlas));
            return newAtlasPtr;
        };

        FontAtlas* fontAtlas = nullptr;
        Bounds2ui32 glyphBounds;
        if (m_bgraAtlases.empty())
        {
            fontAtlas = createAtlas();
            glyphBounds = fontAtlas->RequestSize(dimensions);
            if (glyphBounds.IsEmpty())
            {
                return nullptr;
            }
        }
        else
        {
            fontAtlas = m_bgraAtlases.back().get();
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

        const auto atlasDimensions = fontAtlas->GetImageDimensions();
        auto* atlasBufferPos = fontAtlas->GetBuffer() + ((1 + glyphBounds.low.x) * 4) + (((1 + glyphBounds.low.y) * atlasDimensions.x) * 4);
        auto* glyphBufferPos = static_cast<const uint8_t*>(buffer);

        const auto rgbaAtlasDimensionsX = atlasDimensions.x * 4;
        const auto rgbaDimensionsX = dimensions.x * 4;

        for (uint32_t y = 0; y < dimensions.y; y++)
        {
            std::memcpy(atlasBufferPos, glyphBufferPos, rgbaDimensionsX);
            atlasBufferPos += rgbaAtlasDimensionsX;
            glyphBufferPos += rgbaDimensionsX;
        }

        if (lastAffectedAtlas != fontAtlas)
        {
            m_fontRepository->AddAtlasUpdateEvent(fontAtlas);
            lastAffectedAtlas = fontAtlas;
        }

        auto atlasGlyph = std::make_shared<FontAtlasGlyph>(codePoint, fontAtlas, glyphBounds);
        auto it = m_glyphs.insert({ codePoint, atlasGlyph });
        return it.first->second.get();
    }


    // Font atlas glyph
    FontAtlasGlyph::FontAtlasGlyph(
        uint32_t codePoint,
        FontAtlas* atlas,
        const Bounds2ui32& textureBounds
    ) :
        codePoint(codePoint),
        atlas(atlas),     
        textureBounds(textureBounds),
        glyphIndex(0),
        advance(0, 0),
        bearing(0, 0),
        topPosition(0)
    {}


    // Font repository implementations.
    FontRepository::FontRepository(
        FontNameRepository& nameRepository,
        const Vector2ui32& atlasDimensions
    ) :
        m_impl(new FontRepositoryImpl(
            *this,
            nameRepository,
            Vector2ui32{ std::max(atlasDimensions.x, 128u), std::max(atlasDimensions.y, 128u) }))
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

    int32_t Font::GetAscender() const
    {
        return m_impl->ascender;
    }

    int32_t Font::GetDescender() const
    {
        return m_impl->descender;
    }

    FontGroupedSequence Font::CreateGroupedSequence(
        const std::string& text,
        const uint32_t dpi,
        const uint32_t height)
    {
        return m_impl->CreateGroupedSequence(text, dpi, height);
    }

    float Font::CalculateHeightOffset(const Bounds2f32& bounds)
    {
        const float ascender = static_cast<float>(m_impl->ascender);
        const float descender = static_cast<float>(m_impl->descender);
        const float factor = ascender / (ascender - descender);

        const float boundsHeight = bounds.high.y - bounds.low.y;
        return boundsHeight * factor;
    }


    // Font grouped glyph implementations.
    FontGlyph::FontGlyph(
        FontAtlasGlyph* atlasGlyph,
        const Bounds2i32& bounds
    ) :
        atlasGlyph(atlasGlyph),
        bounds(bounds)
    {}


    // Font sequence grouped implementations.
    FontSequenceGroup::FontSequenceGroup(FontAtlas* atlas) :
        atlas(atlas),
        color(1.0f, 1.0f, 1.0f, 1.0f)
    {}

}
