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

#ifndef MOLTEN_EDITOR_VIEW_ROOTVIEW_HPP
#define MOLTEN_EDITOR_VIEW_ROOTVIEW_HPP

#include "Molten/Editor/Gui/Themes/EditorTheme.hpp"
#include "Molten/Graphics/Gui/Layers/SingleRootLayer.hpp"

namespace Molten::Editor
{

    struct RootViewDescriptor
    {
        Renderer& renderer;
        Gui::FontNameRepository& fontNameRepository;
        Logger* logger = nullptr;
    };

    /** Editor class. */
    class RootView
    {

    public:

        ~RootView() = default;

        /* Deleted operations. */
        /**@{*/
        RootView(const RootView&) = delete;
        RootView(RootView&&) = delete;
        RootView& operator =(const RootView&) = delete;
        RootView& operator =(RootView&&) = delete;
        /**@}*/

        static std::unique_ptr<RootView> Create(const RootViewDescriptor& descriptor);

        Gui::CanvasRendererPointer canvasRenderer = {};
        Gui::CanvasPointer<Gui::EditorTheme> canvas = {};
        Gui::SingleRootLayer<Gui::EditorTheme>* rootLayer = nullptr;
        Gui::PageView<Gui::EditorTheme>* pageView = nullptr;

    private:

        RootView() = default;

    };

}

#endif