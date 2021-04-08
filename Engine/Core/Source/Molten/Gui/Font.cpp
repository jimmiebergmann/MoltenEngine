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

namespace Molten::Gui
{

    // Global PIMPL.

    // Member PIMPL.
    struct Font::Impl
    {
        Impl() :
            ftLibrary(nullptr),
            ftCacheManager(nullptr),
            ftCMapCache(nullptr),
            ftImageCache(nullptr)
        {}

        ~Impl()
        {
            Destroy();
        }

        Impl(const Impl&) = delete;
        Impl(Impl&& impl) noexcept :
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

        Impl& operator =(const Impl&) = delete;
        Impl& operator =(Impl&& impl) noexcept
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

        void Impl::Destroy()
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
            auto* impl = reinterpret_cast<Impl*>(reqData);

            FT_Error error = 0;

            // Load face from memory.
            if ((error = FT_New_Memory_Face(
                library,
                reinterpret_cast<FT_Byte*>(impl->fontData.data()),
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


    // Font implementations.
    Font::Font() :
        m_impl(new Impl)
    {}

    Font::~Font()
    {
        delete m_impl;
    }

    Font::Font(Font&& font) noexcept :
        m_impl(new Impl)
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

    bool Font::CreateBitmap(std::unique_ptr<uint8_t[]>& buffer, Vector2size& dimensions, const std::string& text, const uint32_t dpi, const uint32_t height)
    {
        FT_Error error = 0;

        FT_Face face = nullptr;
        if ((error = FTC_Manager_LookupFace(
            m_impl->ftCacheManager,
            0,
            &face)) != 0)
        {
            return false;
        }

        /*const FT_Int32 loadlags = FT_HAS_COLOR(face) ?
            (FT_LOAD_COLOR | FT_LOAD_RENDER | FT_LOAD_DEFAULT) :
            (FT_LOAD_RENDER | FT_LOAD_DEFAULT);*/
        const FT_Int32 loadlags = FT_LOAD_RENDER | FT_LOAD_DEFAULT;


        // TODO: Skipping dpi for now.
        FTC_ImageTypeRec ftcImageType;
        ftcImageType.face_id = 0;
        ftcImageType.width = static_cast<FT_UInt>(height);
        ftcImageType.height = static_cast<FT_UInt>(height);
        ftcImageType.flags = loadlags;

        // Calculate size.
        auto lowDim = Vector2<FT_Pos>{ std::numeric_limits<FT_Pos>::max(), std::numeric_limits<FT_Pos>::max() };
        auto highDim = Vector2<FT_Pos>{ std::numeric_limits<FT_Pos>::min(), std::numeric_limits<FT_Pos>::min() };
        FT_Pos penPos = 0;
        FT_Pos prevPenPos = 0;
        const bool hasKerning = FT_HAS_KERNING(face);
        FT_UInt prevGlyphIndex = 0;
        FT_BitmapGlyph bitmap = nullptr;

        for(size_t i = 0; i < text.size(); i++)
        {
            const FT_UInt32 charCode = static_cast<FT_UInt32>(text[i]);
            const auto glyphIndex = FTC_CMapCache_Lookup(m_impl->ftCMapCache, 0, 0, charCode);
            if (glyphIndex == 0)
            {
                return false;
            }

            //FTC_Node ftcNode = nullptr;
            FT_Glyph glyph = nullptr;
            if ((error = FTC_ImageCache_Lookup(m_impl->ftImageCache, &ftcImageType, glyphIndex, &glyph, nullptr/*&ftcNode*/)) != 0)
            {
                return false;
            }

            bitmap = reinterpret_cast<FT_BitmapGlyph>(glyph);

            // Move pen if this and previous face has kerning.
            if (hasKerning && prevGlyphIndex)
            {
                FT_Vector delta;
                FT_Get_Kerning(face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta);

                prevPenPos += delta.x >> 6;
                penPos = prevPenPos;
            }
            prevGlyphIndex = glyphIndex;

            auto& metrics = face->glyph->metrics;
            const FT_Pos horiBearingX = metrics.horiBearingX >> 6;
            const FT_Pos horiBearingY = metrics.horiBearingY >> 6;
            const FT_Pos horiAdvance = metrics.horiAdvance >> 6;

            // Calc X dimension.
            const FT_Pos lowX = penPos + horiBearingX;
            if (lowX < lowDim.x)
            {
                lowDim.x = lowX;
            }

            const FT_Pos highX = penPos + horiBearingX + bitmap->bitmap.width;
            if (highX > highDim.x)
            {
                highDim.x = highX;
            }

            // Calc Y dimension.
            const FT_Pos lowY = horiBearingY - bitmap->bitmap.rows;
            if (lowY < lowDim.y)
            {
                lowDim.y = lowY;
            }

            const FT_Pos highY = horiBearingY;
            if (highY > highDim.y)
            {
                highDim.y = highY;
            }

            // Move pen.
            penPos += horiAdvance;
            prevPenPos = penPos;
        }

        // Rasterize glyphs.
        const auto bitmapSize = Vector2<FT_Pos>{
            static_cast<size_t>(highDim.x - lowDim.x),
            static_cast<size_t>(highDim.y - lowDim.y) };

        const size_t bufferSize = static_cast<size_t>(bitmapSize.x) * static_cast<size_t>(bitmapSize.y) * 4;
        buffer = std::make_unique<uint8_t[]>(bufferSize);

        auto getColor = [](FT_Pos x, FT_Pos y) -> Vector4<uint8_t>
        {           
            const Vector4<uint8_t> colorA = { 255, 0, 0, 255 };
            const Vector4<uint8_t> colorB = { 0, 0, 0, 255 };

            const FT_Pos gridSizeX = 12;
            const FT_Pos gridSizeY = 3;

            const bool xFlag = (x / gridSizeX) % 2;
            const bool yFlag = (y / gridSizeY) % 2;

            return (xFlag ^ yFlag) ? colorA : colorB;
        };

        for (FT_Pos y = 0; y < bitmapSize.y; y++)
        {
            for (FT_Pos x = 0; x < bitmapSize.x; x++)
            {
                auto pos = (y * bitmapSize.x * 4) + (x * 4);
                const auto color = getColor(x, y);
              
                buffer[pos] = color.x;
                buffer[pos + 1] = color.y;
                buffer[pos + 2] = color.z;
                buffer[pos + 3] = color.w;

            }
        }

        dimensions = bitmapSize;

        /*for (size_t i = 0; i < text.size(); i++)
        {
        }*/

        return true;
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
            Impl::FtFaceRequester,
            reinterpret_cast<FT_Pointer>(m_impl),
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
