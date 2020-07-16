/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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

#ifndef CURSE_CORE_GUI_WIDGETTEMPLATE_HPP
#define CURSE_CORE_GUI_WIDGETTEMPLATE_HPP


#include "Curse/Gui/Constants.hpp"
#include "Curse/Gui/WidgetDescriptor.hpp"
#include "Curse/Gui/RenderObject.hpp"
#include <functional>

namespace Curse
{

    namespace Gui
    {

        template<typename Template>
        static inline const WidgetDescriptor WidgetDescriptorTemplate = {};

        template<typename Template>
        static inline const auto WidgetDimensionsTemplate = [](Widget&) {};

        template<typename Template>
        static inline const auto RenderObjectTemplate = [](RenderObject &){};

    }

}

#endif