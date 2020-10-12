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

#include <type_traits>

namespace Molten::Gui
{

    template<typename ... TArgs>
    inline CanvasPointer Canvas::Create(TArgs ... args)
    {
        auto canvas = std::make_shared<Canvas>(args...);
        return canvas;
    }

    template<typename TLayerType>
    inline LayerTypePointer<TLayerType> Canvas::CreateChild(CanvasPointer parent)
    {
        static_assert(std::is_base_of<Gui::Layer, TLayerType>::value, "TLayerType is not base of Layer.");

        if (!parent)
        {
            return nullptr;
        }

        auto layer = std::make_shared<TLayerType>(*parent);

        parent->m_allLayers.insert(layer);
        parent->m_inactiveLayers.insert(layer);

        return layer;
    }

    template<typename TLayerType>
    inline LayerTypePointer<TLayerType> Canvas::CreateChild(CanvasPointer parent, LayerPosition position)
    {
        static_assert(std::is_base_of<Gui::Layer, TLayerType>::value, "TLayerType is not base of Layer.");

        if (!parent)
        {
            return nullptr;
        }

        auto layer = std::make_shared<TLayerType>(*parent);

        parent->m_allLayers.insert(layer);

        if (position == LayerPosition::Top)
        {
            parent->m_activeLayers.push_back(layer);
        }
        else
        {
            parent->m_activeLayers.push_front(layer);
        }

        return layer;
    }

}