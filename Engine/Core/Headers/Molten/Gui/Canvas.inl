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

    template<typename TSkin>
    inline Canvas<TSkin>::Canvas(Renderer& backendRenderer, CanvasRendererPointer renderer) :
        m_backendRenderer(backendRenderer),
        m_renderer(renderer),
        m_skin(backendRenderer, *m_renderer.get()),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f)
    {}

    template<typename TSkin>
    inline Canvas<TSkin>::~Canvas()
    {}

    template<typename TSkin>
    inline void Canvas<TSkin>::SetRenderer(CanvasRendererPointer renderer)
    {
        m_renderer = renderer;
    }

    template<typename TSkin>
    inline CanvasRendererPointer Canvas<TSkin>::GetRenderer()
    {
        return m_renderer;
    }
    template<typename TSkin>
    inline const CanvasRendererPointer Canvas<TSkin>::GetRenderer() const
    {
        return m_renderer;
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::PushUserInputEvent(const UserInput::Event& inputEvent)
    {
        m_userInputEvents.push_back(inputEvent);
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::Update(const Time& deltaTime)
    {
        if (m_userInputEvents.size())
        {
            for (auto it = m_activeLayers.rbegin(); it != m_activeLayers.rend(); it++)
            {
                auto& layer = *it;
                layer->PushUserInputEvents(m_userInputEvents);
                if (!m_userInputEvents.size())
                {
                    break;
                }
            }
            m_userInputEvents.clear();
        }

        for (auto& layers : m_activeLayers)
        {
            layers->Update(deltaTime);
        }
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::Draw()
    {
        if (!m_renderer)
        {
            return;
        }

        m_renderer->BeginDraw();
        for (auto& layers : m_activeLayers)
        {
            layers->Draw(*m_renderer);
        }
        m_renderer->EndDraw();
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::SetSize(const Vector2f32& size)
    {
        if (size != m_size && size.x != 0.0f && size.y != 0.0f)
        {
            m_renderer->Resize(size);
            for (auto& layers : m_activeLayers)
            {
                layers->Resize(size);
            }
        }
        m_size = size;
    }

    template<typename TSkin>
    inline void Canvas<TSkin>::SetScale(const Vector2f32& scale)
    {
        m_scale = scale;
    }

    template<typename TSkin>
    inline const Vector2f32& Canvas<TSkin>::GetSize() const
    {
        return m_size;
    }

    template<typename TSkin>
    inline const Vector2f32& Canvas<TSkin>::GetScale() const
    {
        return m_scale;
    }

    template<typename TSkin>
    template<template<typename> typename TLayer>
    inline LayerTypePointer<TLayer<TSkin>> Canvas<TSkin>::CreateChild(const LayerPosition position)
    {
        static_assert(std::is_base_of<Gui::Layer<TSkin>, TLayer<TSkin>>::value, "TLayerType is not base of Layer.");

        auto layer = std::make_shared<TLayer<TSkin>>(m_skin);

        m_allLayers.insert(layer);

        if (position == LayerPosition::Top)
        {
            m_activeLayers.push_back(layer);
        }
        else
        {
            m_activeLayers.push_front(layer);
        }

        return layer;
    }

}