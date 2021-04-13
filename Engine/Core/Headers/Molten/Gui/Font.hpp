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
#include "Molten/Renderer/ImageFormat.hpp"
#include <string>
#include <memory>

namespace Molten::Gui
{

    struct FontImplementation; ///< PIMPL


    /** Font sequence object with caching capabilities, returned by Font class.*/
    class MOLTEN_API FontSequence
    {

    public:

        FontSequence();
        explicit FontSequence(
            FontImplementation& fontImpl,
            const uint32_t cachedDpi = 0,
            const Vector2f32& cachedFactor = { 1.0f, 1.0f });

        FontSequence(FontSequence&& fontSequence) noexcept;
        FontSequence& operator =(FontSequence&& fontSequence) noexcept;

        FontSequence(const FontSequence&) = delete;
        FontSequence& operator =(const FontSequence&) = delete;

        bool CreateBitmap(
            const std::string& text,
            const uint32_t dpi,
            const uint32_t height);

        void SetCachedDpi(uint32_t dpi);
        void SetCachedFactor(const Vector2f32& factor);

        [[nodiscard]] uint32_t GetCachedDpi() const;
        [[nodiscard]] const Vector2f32& GetCachedFactor() const;
        [[nodiscard]] const Vector2ui32& GetBufferDimensions() const;
        [[nodiscard]] const Bounds2ui32& GetBufferTextBounds() const;
        [[nodiscard]] const uint8_t* GetBuffer() const;
        [[nodiscard]] uint32_t GetBaseline() const;
        [[nodiscard]] ImageFormat GetImageFormat() const;

    private:

        void AllocateNewBuffer(const ImageFormat imageFormat,
            const uint32_t pixelSize,
            const Vector2ui32& bufferDimensions,
            const Vector2ui32& bufferTextDimensions);

        void CleanOldBuffer(const Vector2ui32& bitmapSize);

        FontImplementation* m_fontImpl;
        uint32_t m_cachedDpi;
        Vector2f32 m_cachedFactor;

        std::unique_ptr<uint8_t[]> m_buffer;
        Vector2ui32 m_bufferDimensions;
        Bounds2ui32 m_bufferTextBounds;
        uint32_t m_baseline;
        ImageFormat m_imageFormat;
        uint32_t m_pixelSize;
        
    };


    class MOLTEN_API Font
    {

    public:

        Font();
        ~Font();

        Font(const Font&) = delete;
        Font(Font&& font) noexcept;

        Font& operator =(const Font&) = delete;
        Font& operator =(Font&& font) noexcept;

        bool ReadFromFile(const std::string& filename);

        FontSequence CreateSequence(
            const uint32_t cachedDpi = 0,
            const Vector2f32& cachedFactor = { 1.0f, 1.0f });

    private:

        bool Initialize();
      
        FontImplementation* m_impl;

    };

}

#endif