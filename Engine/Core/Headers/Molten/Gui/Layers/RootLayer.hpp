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
/*
#ifndef MOLTEN_CORE_GUI_ROOTLAYER_HPP
#define MOLTEN_CORE_GUI_ROOTLAYER_HPP

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Gui/Layer.hpp"
#include "Molten/Gui/Skin.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Bounds.hpp"
#include "Molten/System/Time.hpp"
#include <map>
#include <algorithm>

namespace Molten::Gui
{

    template<typename TSkin>
    class RootLayer : public Layer<TSkin>
    {

    public:

        explicit RootLayer(TSkin& skin);

        ~RootLayer() = default;

        void PushUserInputEvents(std::vector<UserInput::Event>& inputEvents) override;

        void Update(const Time& deltaTime) override;

        void Draw(CanvasRenderer & renderer) override;

        void Resize(const Vector2f32& size) override;

        void SetScale(const Vector2f32& scale) override;

    private:

        bool OnAddChild(Widget<TSkin>* parent, WidgetDataPointer<TSkin> childData) override;

        void HandleMouseMove(UserInput::Event& mouseEvent);
        void HandleMousePressed(UserInput::Event& mouseEvent);
        void HandleMouseReleased(UserInput::Event& mouseEvent);

        typename WidgetData<TSkin>::Tree m_widgetTree;
        WidgetDataMap<TSkin> m_widgetMap;

        Vector2f32 m_size;
        Vector2f32 m_scale;
        std::vector<UserInput::Event> m_userInputEvents;
        WidgetPointer<TSkin> m_hoveredWidget;
        WidgetPointer<TSkin> m_pressedWidget;

    };

}

#include "Molten/Gui/Layers/RootLayer.inl"

#endif*/