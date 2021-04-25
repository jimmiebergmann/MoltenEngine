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

#ifndef MOLTEN_CORE_GUI_FONT_HPP
#define MOLTEN_CORE_GUI_FONT_HPP

#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Bounds.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace Molten::Gui
{

    class FontNameRepository;  
    class FontAtlas;
    class FontAtlasBin;
    struct FontAtlasGlyph;
    class FontRepository;  
    class Font;  
    struct FontGlyph;
    struct FontGroupedSequence;
    struct FontSequenceGroup;

    struct FontRepositoryImpl;
    struct FontImpl;

    /** Font name repository, finding and caching font file names. */
    class MOLTEN_API FontNameRepository
    {

    public:

        FontNameRepository() = default;
        ~FontNameRepository() = default;

        FontNameRepository(const FontNameRepository&) = delete;
        FontNameRepository(FontNameRepository&&) noexcept = default;

        FontNameRepository& operator = (const FontNameRepository&) = delete;
        FontNameRepository& operator = (FontNameRepository&&) noexcept = default;

        /** Clear all cached font paths. */
        void ClearPathCache();

        /** Add font search directory. */
        bool AddDirectory(const std::string& directory);

        /** Add default system font search directories. */
        void AddSystemDirectories();

        /** Find font path by font name. Returns empty string if not found. */
        [[nodiscard]] std::string FindFontFamilyPath(const std::string& fontFamily);

    private:

        std::vector<std::string> m_fontDirectories;
        std::map<std::string, std::string> m_cachedFontPaths;

    };


    enum class FontAtlasEventType
    {
        New,
        Update
    };

    enum class FontAtlasImageFormat
    {
        Gray,
        BlueGreenRedAlpha
    };


    class MOLTEN_API FontAtlas
    {

    public:

        FontAtlas(
            std::unique_ptr<uint8_t[]>&& buffer,
            const size_t bufferSize,
            const FontAtlasImageFormat imageFormat,
            const Vector2ui32& imageDimensions);
        ~FontAtlas() = default;

        FontAtlas(const FontAtlas&) = delete;
        FontAtlas(FontAtlas&&) noexcept = default;

        FontAtlas& operator = (const FontAtlas&) = delete;
        FontAtlas& operator = (FontAtlas&&) noexcept = default;

        [[nodiscard]] uint8_t* GetBuffer();
        [[nodiscard]] const uint8_t* GetBuffer() const;
        [[nodiscard]] size_t GetBufferSize() const;
        [[nodiscard]] FontAtlasImageFormat GetImageFormat() const;
        [[nodiscard]] const Vector2ui32& GetImageDimensions() const;

        Bounds2ui32 RequestSize(const Vector2ui32& size);

        void* metaData; ///< Meta data attached by user. This is useful for storing texture GPU resources.

    private:

        std::unique_ptr<uint8_t[]> m_buffer;
        size_t m_bufferSize;
        FontAtlasImageFormat m_imageFormat;
        Vector2ui32 m_imageDimensions;
        Vector2ui32 m_currentPosition;
        uint32_t m_nextRow;

    };


    /** Font atlas bin. Contains buffers of atlas images. */
    class MOLTEN_API FontAtlasBin
    {

    public:

        explicit FontAtlasBin(FontRepository& fontRepository);
        ~FontAtlasBin() = default;

        FontAtlasBin(const FontAtlasBin&) = delete;
        FontAtlasBin(FontAtlasBin&&) noexcept = default;

        FontAtlasBin& operator = (const FontAtlasBin&) = delete;
        FontAtlasBin& operator = (FontAtlasBin&&) noexcept = default;


        [[nodiscard]] FontAtlasGlyph* FindGlyph(const uint32_t codePoint);
        [[nodiscard]] FontAtlasGlyph* CreateGlyph(
            const uint32_t codePoint,
            const void* buffer,
            const Vector2ui32& dimensions,
            FontAtlasImageFormat imageFormat,
            FontAtlas*& lastAffectedAtlas);

    private:

        [[nodiscard]] FontAtlasGlyph* CreateGrayGlyph(
            const uint32_t codePoint,
            const void* buffer,
            const Vector2ui32& dimensions,
            FontAtlas*& lastAffectedAtlas);

        [[nodiscard]] FontAtlasGlyph* CreateBgraGlyph(
            const uint32_t codePoint,
            const void* buffer,
            const Vector2ui32& dimensions,
            FontAtlas*& lastAffectedAtlas);

        FontRepository* fontRepository;
        std::map<uint32_t, std::shared_ptr<FontAtlasGlyph>> m_glyphs;
        std::vector<std::unique_ptr<FontAtlas>> m_grayAtlases;
        std::vector<std::unique_ptr<FontAtlas>> m_bgraAtlases;

    };


    /** Font glyph representation in atlas. */
    struct MOLTEN_API FontAtlasGlyph
    {
        FontAtlasGlyph(
            uint32_t codePoint,
            FontAtlas* atlas,
            const Bounds2ui32& textureBounds);
        ~FontAtlasGlyph() = default;

        FontAtlasGlyph(const FontAtlasGlyph&) = default;
        FontAtlasGlyph(FontAtlasGlyph&&) noexcept = default;

        FontAtlasGlyph& operator = (const FontAtlasGlyph&) = default;
        FontAtlasGlyph& operator = (FontAtlasGlyph&&) noexcept = default;       

        uint32_t codePoint;
        FontAtlas* atlas;
        Bounds2ui32 textureBounds;
        uint32_t glyphIndex;
        Vector2i32 advance;
        Vector2i32 bearing;
        int32_t topPosition;
    };


    /** Font repository, caching loaded font files. */
    class MOLTEN_API FontRepository
    {

    public:

        explicit FontRepository(FontNameRepository& nameRepository);

        ~FontRepository();

        FontRepository(const FontRepository&) = delete;
        FontRepository(FontRepository&&) noexcept = delete;

        FontRepository& operator = (const FontRepository&) = delete;
        FontRepository& operator = (FontRepository&&) noexcept = delete;

        /* Gets existing font. Returns nullptr if not found.
         * Font pointer is owned and deleted by repository automatically.
         */
        /**@{*/
        [[nodiscard]] Font* GetFont(const std::string& fontFamily);
        [[nodiscard]] const Font* GetFont(const std::string& fontFamily) const;
        /**@}*/

        /* Gets existing font of try to create one. Returns nullptr if not found.
         * Font pointer is owned and deleted by repository automatically.
         */
        /**@{*/
        [[nodiscard]] Font* GetOrCreateFont(const std::string& fontFamily);
        [[nodiscard]] const Font* GetOrCreateFont(const std::string& fontFamily) const;
        /**@}*/

        /** Adds atlas event. Returns true if event already is added or if an New event already exists. */
        /**@{*/
        bool AddAtlasNewEvent(FontAtlas* atlas);
        bool AddAtlasUpdateEvent(FontAtlas* atlas);
        /**@}*/

        /** Gets last atlas affected by an event. */
        FontAtlas* GetLastAffectedAtlas();

        /** Fetching all update and new atlas events. Queue is cleared when finished. */
        void ForEachAtlasEvent(std::function<void(FontAtlasEventType, FontAtlas*)>&& callback);

    private:

        FontRepositoryImpl* m_impl;

    };


    /** Font object, used for generating font sequences. */
    class MOLTEN_API Font
    {

    public:

        Font(FontImpl* implementation);
        ~Font();

        Font(const Font&) = delete;
        Font(Font&&) noexcept = default;

        Font& operator = (const Font&) = delete;
        Font& operator = (Font&&) noexcept = default;

        [[nodiscard]] FontGroupedSequence CreateGroupedSequence(
            const std::string& text,
            const uint32_t dpi,
            const uint32_t height);

    private:

        FontImpl* m_impl;

    };


    /** Font glyph object, representing a codepoint and area in font atlas. Position of glyph is relative to group. */
    struct MOLTEN_API FontGlyph
    {
        FontGlyph(
            FontAtlasGlyph* atlasGlyph,
            const Bounds2i32& bounds);
        ~FontGlyph() = default;

        FontGlyph(const FontGlyph&) = default;
        FontGlyph(FontGlyph&&) noexcept = default;

        FontGlyph& operator = (FontGlyph&&) noexcept = default;
        FontGlyph& operator = (const FontGlyph&) = default;

        FontAtlasGlyph* atlasGlyph;
        Bounds2i32 bounds;
    };


    /** Group of font glyphs, composes a sequence of glyphs. All glyphs originates from the same atlas. */
    struct MOLTEN_API FontSequenceGroup
    {
        FontSequenceGroup(
            FontAtlas* atlas,
            const Vector4f32& color);
        ~FontSequenceGroup() = default;

        FontSequenceGroup(const FontSequenceGroup&) = delete;
        FontSequenceGroup(FontSequenceGroup&&) noexcept = default;

        FontSequenceGroup& operator = (FontSequenceGroup&&) noexcept = default;
        FontSequenceGroup& operator = (const FontSequenceGroup&) = delete;   

        FontAtlas* atlas;
        Vector4f32 color;
        std::vector<std::shared_ptr<FontGlyph>> glyphs;
    };


    /** A grouped sequence, generated by Font object. All groups originates from different atlases. */
    struct MOLTEN_API FontGroupedSequence
    {
        FontGroupedSequence() = default;
        ~FontGroupedSequence() = default;

        FontGroupedSequence(const FontGroupedSequence&) = delete;
        FontGroupedSequence(FontGroupedSequence&&) noexcept = default;

        FontGroupedSequence& operator = (const FontGroupedSequence&) = delete;
        FontGroupedSequence& operator = (FontGroupedSequence&&) noexcept = default;

        Bounds2i32 bounds;
        std::vector<std::shared_ptr<FontGlyph>> glyphs;
        std::vector<std::shared_ptr<FontSequenceGroup>> groups;
    };

}

#endif