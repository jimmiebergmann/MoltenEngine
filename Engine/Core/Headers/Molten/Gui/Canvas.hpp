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

#ifndef MOLTEN_CORE_GUI_CANVAS_HPP
#define MOLTEN_CORE_GUI_CANVAS_HPP

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Gui/DockingWidget.hpp"
#include "Molten/Gui/Widgets/DockerWidget.hpp"
#include "Molten/Gui/WidgetData.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Gui/Layer.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Time.hpp"
#include "Molten/System/UserInput.hpp"
#include <memory>
#include <list>
#include <set>
#include <vector>

namespace Molten
{
    class Renderer;
    class Logger;
}

namespace Molten::Gui
{

    class CanvasRenderer;


    template<typename TSkin>
    class Canvas
    {

    public:

        explicit Canvas(Renderer& backendRenderer, CanvasRendererPointer renderer = nullptr);
        ~Canvas();

        void SetRenderer(CanvasRendererPointer renderer);
        CanvasRendererPointer GetRenderer();
        const CanvasRendererPointer GetRenderer() const;

        void PushUserInputEvent(const UserInput::Event& inputEvent);

        void Update(const Time& deltaTime);

        void Draw();

        void SetSize(const Vector2f32& size);
        void SetScale(const Vector2f32& scale);

        const Vector2f32& GetSize() const;
        const Vector2f32& GetScale() const;

        //template<template<typename> typename TLayer>
        //LayerTypePointer<TLayer<TSkin>> CreateChild(const LayerPosition position = LayerPosition::Top);

        //template<template<typename> typename TWidgetType, typename ... TArgs>
        //WidgetTypePointer<TWidgetType<TSkin>> CreateChild(TArgs ... args);

        //template<template<typename> typename TLayerType, typename ... TArgs>
        //LayerTypePointer<TLayerType<TSkin>> CreateRootLayer(TArgs ... args);

        template<template<typename> typename TWidgetType, typename ... TArgs>
        WidgetTypePointer<TWidgetType<TSkin>> CreateChild(TArgs ... args);

        template<template<typename> typename TWidgetType, typename ... TArgs>
        WidgetTypePointer<TWidgetType<TSkin>> CreateChild(Widget<TSkin>& parent, TArgs ... args);

    private:

        Canvas(Canvas&&) = delete;
        Canvas(const Canvas&) = delete;
        Canvas& operator= (Canvas&&) = delete;
        Canvas& operator= (const Canvas&) = delete;

        void UpdateUserInputs();
        void UpdateWidgetSkins();

        void HandleMouseMove(UserInput::Event& mouseEvent);
        void HandleMousePressed(UserInput::Event& mouseEvent);
        void HandleMouseReleased(UserInput::Event& mouseEvent);


        //bool AddWidgetToTree(Widget<TSkin>* parent, WidgetDataPointer<TSkin> childData);

        //using LayerPointerList = std::list<LayerPointer<TSkin>>;
        //using LayerPointerSet = std::set<LayerPointer<TSkin>>;

        Renderer& m_backendRenderer;
        CanvasRendererPointer m_renderer;

        TSkin m_skin;
        typename WidgetData<TSkin>::Tree m_widgetTree;

        //LayerPointerSet m_allLayers;
        //LayerPointerList m_activeLayers;
        //LayerPointerSet m_inactiveLayers;       
     
        Vector2f32 m_size;
        Vector2f32 m_scale;

        std::vector<UserInput::Event> m_userInputEvents;
        WidgetPointer<TSkin> m_hoveredWidget;
        WidgetPointer<TSkin> m_pressedWidget;

        

    };

}

#include "Molten/Gui/Canvas.inl"

#endif