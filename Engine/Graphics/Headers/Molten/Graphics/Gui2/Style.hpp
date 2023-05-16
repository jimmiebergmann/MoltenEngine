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

#ifndef MOLTEN_GRAPHICS_GUI2_STYLE_HPP
#define MOLTEN_GRAPHICS_GUI2_STYLE_HPP

#include "Molten/Graphics/Gui2/Gui2Namespace.hpp"
#include "Molten/Graphics/Gui2/WidgetStyle.hpp"
#include <memory>
#include <any>
#include <map>
#include <type_traits>

namespace Molten::MOLTEN_GUI2NAMESPACE
{

    class Style
    {

    public:

        Style() = default;
        ~Style() = default;

        Style(const Style&) = delete;
        Style(Style&&) = delete;
        Style& operator = (const Style&) = delete;
        Style& operator = (Style&&) = delete;

        template<typename T>
        std::shared_ptr<T> Get();

    private:

        std::map<size_t, std::any> m_widgetStyles;

    };

}

#include "Molten/Graphics/Gui2/Style.inl"

#endif