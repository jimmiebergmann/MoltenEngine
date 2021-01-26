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

/*namespace Molten::Gui
{

    template<typename TSkin>
    inline RootLayer<TSkin>::RootLayer(TSkin& skin) :
        Layer<TSkin>(skin),
        m_size(0.0f, 0.0f),
        m_scale(1.0f, 1.0f)
    {}

    template<typename TSkin>
    void RootLayer<TSkin>::PushUserInputEvents(std::vector<UserInput::Event>& inputEvents)
    {
        std::vector<UserInput::Event> mouseEvents;
        std::vector<UserInput::Event> keyboardEvents;

        for (auto& inputEvent : inputEvents)
        {
            switch (inputEvent.type)
            {
                case UserInput::EventType::Mouse: mouseEvents.push_back(inputEvent);
                case UserInput::EventType::Keyboard: keyboardEvents.push_back(inputEvent);
                default: break;
            }
        }

        for (auto& mouseEvent : mouseEvents)
        {
            switch (mouseEvent.subType)
            {
                case UserInput::EventSubType::MouseMove: HandleMouseMove(mouseEvent); break;
                case UserInput::EventSubType::MouseButtonPressed: HandleMousePressed(mouseEvent); break;
                case UserInput::EventSubType::MouseButtonReleased: HandleMouseReleased(mouseEvent); break;
                default: break;
            }
        }
    }

    template<typename TSkin>
    inline void RootLayer<TSkin>::Update(const Time&)
    {
        // Update widget skins.
        auto rootLane = m_widgetTree.GetLane<typename WidgetData<TSkin>::TreePartialLaneType>();
        for (auto& rootWidget : rootLane)
        {
            rootWidget.GetValue()->widgetSkin->SetGrantedBounds({ { 0.0f, 0.0f }, m_size });
        }

        m_widgetTree.template ForEachPreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            widgetData->widgetSkin->Update();
        });
    }

    template<typename TSkin>
    inline void RootLayer<TSkin>::Draw(CanvasRenderer& renderer)
    {
        m_widgetTree.template ForEachPreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            //renderer.MaskArea(renderData.position, renderData.size);
            widgetData->widgetSkin->Draw();
        });
    }

    template<typename TSkin>
    inline void RootLayer<TSkin>::Resize(const Vector2f32& size)
    {
        m_size = size;
    }

    template<typename TSkin>
    inline void RootLayer<TSkin>::SetScale(const Vector2f32& scale)
    {
        m_scale = scale;
    }

    template<typename TSkin>
    inline bool RootLayer<TSkin>::OnAddChild(Widget<TSkin>* parent, WidgetDataPointer<TSkin> childData)
    {
        if (!parent)
        {
            auto normalLane = m_widgetTree.template GetLane<typename WidgetData<TSkin>::TreeNormalLaneType>();
            auto partialLane = m_widgetTree.template GetLane<typename WidgetData<TSkin>::TreePartialLaneType>();
            if (!normalLane.IsEmpty())
            {
                return false;
            }

            childData->tree = &m_widgetTree;
            childData->iterator = m_widgetTree.Insert(partialLane, normalLane.end(), childData);
            
            m_widgetMap.insert({ childData->widget.get(), childData });
        }
        else
        {
            auto parentIt = m_widgetMap.find(parent);
            if (parentIt == m_widgetMap.end())
            {
                return false;
            }

            auto* parentData = parentIt->second.get();
            auto* parentTreeItem = std::addressof(*parentData->iterator);
            auto parentNormalLane = parentTreeItem->GetChildren().template GetLane<typename WidgetData<TSkin>::TreeNormalLaneType>();
            auto parentPartialLane = parentTreeItem->GetChildren().template GetLane<typename WidgetData<TSkin>::TreePartialLaneType>();

            childData->tree = &m_widgetTree;
            childData->iterator = m_widgetTree.Insert(parentPartialLane, parentNormalLane.end(), childData);
            
            m_widgetMap.insert({ childData->widget.get(), childData });
        }

        return true;
    }

    template<typename TSkin>
    void RootLayer<TSkin>::HandleMouseMove(UserInput::Event& mouseEvent)
    {
        bool hitWidget = false;

        m_widgetTree.template ForEachReversePreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            if (!widgetData->mouseEventHandler)
            {
                return true;
            }

            if (widgetData->widgetSkin->GetGrantedBounds().Intersects(mouseEvent.mouseMoveEvent.position))
            {
                WidgetEvent widgetEvent;
                widgetEvent.type = WidgetEventType::Mouse;

                if (m_hoveredWidget && m_hoveredWidget != widgetData->widget)
                {
                    widgetEvent.subType = WidgetEventSubType::MouseLeave;
                    widgetEvent.mouseEvent.position = mouseEvent.mouseMoveEvent.position;
                    auto& leaveWidgetHandler = Layer<TSkin>::GetWidgetData(*m_hoveredWidget).mouseEventHandler;
                    leaveWidgetHandler->HandleEvent(widgetEvent);
                }

                m_hoveredWidget = widgetData->widget;

                widgetEvent.subType = WidgetEventSubType::MouseEnter;
                widgetEvent.mouseEvent.position = mouseEvent.mouseMoveEvent.position;
                auto& leaveWidgetHandler = Layer<TSkin>::GetWidgetData(*m_hoveredWidget).mouseEventHandler;
                leaveWidgetHandler->HandleEvent(widgetEvent);

                hitWidget = true;

                return false;
            }

            return true;
        });

        if (!hitWidget && m_hoveredWidget)
        {
            WidgetEvent widgetEvent;
            widgetEvent.type = WidgetEventType::Mouse;
            widgetEvent.subType = WidgetEventSubType::MouseLeave;
            widgetEvent.mouseEvent.position = mouseEvent.mouseMoveEvent.position;
            auto& leaveWidgetHandler = Layer<TSkin>::GetWidgetData(*m_hoveredWidget).mouseEventHandler;
            leaveWidgetHandler->HandleEvent(widgetEvent);
        }
    }

    template<typename TSkin>
    void RootLayer<TSkin>::HandleMousePressed(UserInput::Event& mouseEvent)
    {
        m_widgetTree.template ForEachReversePreorder<typename WidgetData<TSkin>::TreePartialLaneType>(
            [&](auto& widgetData)
        {
            if (widgetData->widgetSkin->GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
            {
                if (!widgetData->mouseEventHandler)
                {
                    return true;
                }

                WidgetEvent widgetEvent;
                widgetEvent.type = WidgetEventType::Mouse;

                if (m_hoveredWidget != widgetData->widget)
                {
                    if(m_hoveredWidget)
                    {
                        widgetEvent.subType = WidgetEventSubType::MouseLeave;
                        widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;
                        auto& leaveWidgetHandler = Layer<TSkin>::GetWidgetData(*m_hoveredWidget).mouseEventHandler;
                        leaveWidgetHandler->HandleEvent(widgetEvent);
                    }
                    m_hoveredWidget = widgetData->widget;

                    widgetEvent.subType = WidgetEventSubType::MouseEnter;
                    widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;
                    auto& enterWidgetHandler = Layer<TSkin>::GetWidgetData(*m_hoveredWidget).mouseEventHandler;
                    enterWidgetHandler->HandleEvent(widgetEvent);
                }

                m_pressedWidget = widgetData->widget;

                widgetEvent.subType = WidgetEventSubType::MousePress;
                widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;

                auto& widgetHandler = Layer<TSkin>::GetWidgetData(*m_pressedWidget).mouseEventHandler;
                widgetHandler->HandleEvent(widgetEvent);

                return false;
            }
            return true;
        });
    }

    template<typename TSkin>
    void RootLayer<TSkin>::HandleMouseReleased(UserInput::Event& mouseEvent)
    {
        if (!m_pressedWidget)
        {
            return;
        }

        WidgetEvent widgetEvent;
        widgetEvent.type = WidgetEventType::Mouse;
        widgetEvent.mouseEvent.position = mouseEvent.mouseButtonEvent.position;

        auto& widgetData = Layer<TSkin>::GetWidgetData(*m_pressedWidget);

        if (widgetData.widgetSkin->GetGrantedBounds().Intersects(mouseEvent.mouseButtonEvent.position))
        {
            widgetEvent.subType = WidgetEventSubType::MouseReleaseIn;
        }
        else
        {
            widgetEvent.subType = WidgetEventSubType::MouseReleaseOut;
        }

        auto* widgetHandler = widgetData.mouseEventHandler;
        widgetHandler->HandleEvent(widgetEvent);

        m_pressedWidget = nullptr;
    }


}*/