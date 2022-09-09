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


#ifndef MOLTEN_GRAPHICS_GUI_LAYERS_MULTIROOTGLAYER_HPP
#define MOLTEN_GRAPHICS_GUI_LAYERS_MULTIROOTGLAYER_HPP

#include "Molten/Graphics/Gui/Layer.hpp"

namespace Molten::Gui
{

    template<typename TTheme>
    class MultiRootLayer : public Layer<TTheme>
    {

    public:

        explicit MultiRootLayer(const LayerDescriptor<TTheme>& descriptor);

        ~MultiRootLayer() override = default;

        MultiRootLayer(MultiRootLayer&&) = delete;
        MultiRootLayer(const MultiRootLayer&) = delete;
        MultiRootLayer& operator= (MultiRootLayer&&) = delete;
        MultiRootLayer& operator= (const MultiRootLayer&) = delete;

        bool AllowsMultipleRoots() override;

    };

}

#include "Molten/Graphics/Gui/Layers/MultiRootLayer.inl"

#endif