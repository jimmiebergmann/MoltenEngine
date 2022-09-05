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


#include "Molten/Editor/View/RootView.hpp"
#include "Molten/Graphics/Gui/Canvas.hpp"

namespace Molten::Editor
{

    std::unique_ptr<RootView> RootView::Create(const RootViewDescriptor& descriptor)
    {
        auto result = std::unique_ptr<RootView>(new RootView{});

        result->canvasRenderer = Gui::CanvasRenderer::Create(descriptor.renderer, descriptor.logger);
        result->canvas = std::make_shared<Gui::Canvas<Gui::EditorTheme>>(*result->canvasRenderer, descriptor.fontNameRepository);
        result->rootLayer = result->canvas->CreateLayer<Gui::SingleRootLayer>(Gui::LayerPosition::Top);

        auto* rootGrid = result->rootLayer->CreateChild<Gui::Grid>(Gui::GridDirection::Vertical);
        {
            auto* menuBar = rootGrid->CreateChild<Gui::MenuBar>();
            {
                menuBar->CreateChild<Gui::MenuBarItem>("File");
                menuBar->CreateChild<Gui::MenuBarItem>("Help");
            }
        }

        result->pageView = rootGrid->CreateChild<Gui::PageView>();

        return result;
    }

}
