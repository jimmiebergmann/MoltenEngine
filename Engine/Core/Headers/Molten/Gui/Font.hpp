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
#include <string>
#include <memory>

namespace Molten::Gui
{

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

        bool CreateBitmap(std::unique_ptr<uint8_t[]>& buffer, Vector2size& dimensions, const std::string& text, const uint32_t dpi, const uint32_t height);

    private:

        bool Initialize();

        // PIMPL
        struct Impl;
        Impl* m_impl;

    };

}

#endif