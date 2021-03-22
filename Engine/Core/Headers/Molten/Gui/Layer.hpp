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

#ifndef MOLTEN_CORE_GUI_LAYER_HPP
#define MOLTEN_CORE_GUI_LAYER_HPP

#include "Molten/Gui/LayerData.hpp"
#include "Molten/Gui/WidgetData.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Time.hpp"
#include "Molten/System/UserInput.hpp"
#include <memory>

namespace Molten::Gui
{

    enum class LayerPosition
    {
        Top,
        Bottom
    };

    /** Repository sent to multiple layers, for tracking widget states over multiple layers.*/
    template<typename TTheme>
    struct MultiLayerRepository
    {
        MultiLayerRepository();

        Widget<TTheme>* hoveredWidget;
        Widget<TTheme>* pressedWidget;
    };

    /** Layer base class. Layers should inherit from LayerMixin instead of this class template.*/
    template<typename TTheme>
    class Layer
    {

    public:

        Layer(
            TTheme& theme,
            LayerData<TTheme>& data);
        virtual ~Layer() = default;

        Layer(Layer&&) = delete;
        Layer(const Layer&) = delete;
        Layer& operator= (Layer&&) = delete;
        Layer& operator= (const Layer&) = delete;

        virtual bool HandleUserInput(
            const UserInput::Event& userInputEvent,
            MultiLayerRepository<TTheme>& multiLayerRepository);

        virtual void Update(const Time& deltaTime);

        virtual void Draw();

        virtual void SetSize(const Vector2f32& size);

        virtual void SetScale(const Vector2f32& scale);

        virtual bool AllowsMultipleRoots() = 0;

        template<template<typename> typename TWidget, typename ... TArgs>
        TWidget<TTheme>* CreateChild(TArgs ... args);

        template<template<typename> typename TWidget, typename ... TArgs>
        TWidget<TTheme>* CreateChild(
            Widget<TTheme>& parent,
            TArgs ... args);

    protected:

        [[nodiscard]] TTheme& GetTheme();
        [[nodiscard]] const TTheme& GetTheme() const;

        [[nodiscard]] LayerData<TTheme>& GetData();
        [[nodiscard]] const LayerData<TTheme>& GetData() const;

        [[nodiscard]] typename WidgetData<TTheme>::Tree& GetWidgetTree();
        [[nodiscard]] const typename WidgetData<TTheme>::Tree& GetWidgetTree() const;

        template<template<typename> typename TWidget>
        [[nodiscard]] static WidgetData<TTheme>& GetWidgetData(TWidget<TTheme>& widget);
        template<template<typename> typename TWidget>
        [[nodiscard]] static const WidgetData<TTheme>& GetWidgetData(const Widget<TTheme>& widget);

    private:

        template<template<typename> typename TWidget, typename ... TArgs, typename TLane, typename TTreeIterator>
        [[nodiscard]] TWidget<TTheme>* InternalCreateChild(
            TLane& lane,
            TTreeIterator iterator,
            Widget<TTheme>* parent,
            TArgs ... args);

        template<template<typename> typename TWidget>
        [[nodiscard]] typename WidgetData<TTheme>::MouseEventFunction CreateChildMouseEventFunction(
            TWidget<TTheme>* child,
            Widget<TTheme>* parent);

        TTheme& m_theme;
        LayerData<TTheme>& m_data;       
        typename WidgetData<TTheme>::Tree m_widgetTree;
        Vector2f32 m_size;
        Vector2f32 m_scale;

    };

}

#include "Molten/Gui/Layer.inl"

#endif