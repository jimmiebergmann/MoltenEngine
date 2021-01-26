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

namespace Molten::Gui
{

    // Docker public implementations.

    template<typename TSkin>
    inline Docker<TSkin>::Docker(WidgetData<TSkin>& data) :
        Widget<TSkin>(data),
        m_rootGrid(nullptr),
        m_pressedEdge(nullptr)
    { }

    template<typename TSkin>
    void Docker<TSkin>::Update()
    {
        ApplyMarginsToGrantedBounds();
        UpdateBounds();
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEvent(const WidgetEvent& widgetEvent)
    {
        switch (widgetEvent.type)
        {
            case WidgetEventType::Mouse: return HandleMouseEvent(widgetEvent.subType, widgetEvent.mouseEvent);
            default: break;
        }

        return false;
    }


    // Docker widget element implementations.
    template<typename TSkin>
    Docker<TSkin>::WidgetElement::WidgetElement(
        WidgetData<TSkin>& widgetData
    ) :
        widgetData(&widgetData),
        dynamicSize(IsDynamicSize(widgetData)),
        size(dynamicSize == false ? widgetData.GetWidget()->size : Vector2f32{ 0.0f, 0.0f })
    {}

    template<typename TSkin>
    bool Docker<TSkin>::WidgetElement::IsDynamicSize(WidgetData<TSkin>& widgetData)
    {
        auto* widget = widgetData.GetWidget().get();
        return widget->size.x <= 0.0f || widget->size.y <= 0.0f;
    }


    // Docker grid element implementations.
    template<typename TSkin>
    Docker<TSkin>::GridElement::GridElement(Direction direction) :
        direction(direction)
    {}

    template<typename TSkin>
    template<typename TElementValue>
    void Docker<TSkin>::GridElement::PushBackElement(TElementValue&& elementValue)
    {
        elements.push_back(std::make_unique<Element>(std::move(elementValue)));
    }

    template<typename TSkin>
    template<typename TElementValue>
    std::pair<typename Docker<TSkin>::Element*, typename Docker<TSkin>::Element*> 
        Docker<TSkin>::GridElement::InsertElement(TElementValue&& elementValue, const ElementPosition position)
    {
        auto it = position == ElementPosition::First ? elements.begin() : elements.end();
        auto* prevElementPtr = elements.empty() ? nullptr : (position == ElementPosition::First ? elements.front().get() : elements.back().get());
        
        auto newElement = std::make_unique<Element>(std::move(elementValue));
        auto* newElementPtr = newElement.get();
        
        elements.insert(it, std::move(newElement));

        // Make prev element to high and next element to low if we insert in front(first/left).
        if (position == ElementPosition::First)
        {
            std::swap(prevElementPtr, newElementPtr);
        }

        return { prevElementPtr, newElementPtr };
    }


    // Docker element implementations.
    template<typename TSkin>
    Docker<TSkin>::Element::Element(WidgetElement* widgetElement) :
        type(ElementType::Widget),
        data(widgetElement),
        prevEdge(nullptr),
        nextEdge(nullptr)
    {}

    template<typename TSkin>
    Docker<TSkin>::Element::Element(std::unique_ptr<GridElement>&& gridElement) :
        type(ElementType::Grid),
        data(std::move(gridElement)),
        prevEdge(nullptr),
        nextEdge(nullptr)
    {}


    // Docker edge implementations.
    template<typename TSkin>
    Docker<TSkin>::Edge::Edge(
        Direction direction,
        Element* prevElement,
        Element* nextElement
    ) :
        bounds(
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min()
        ),
        direction(direction),
        prevElement(prevElement),
        nextElement(nextElement)
    {}


    // Docker private implementations.
    template<typename TSkin>
    inline void Docker<TSkin>::OnAddChild(WidgetData<TSkin>& childData)
    {
        auto* paneWidget = dynamic_cast<Pane<TSkin>*>(childData.GetWidget().get()); // FIX THIS!

        auto position = paneWidget->GetInitialPosition();
        auto gridDirection = GetInsertDirection(position);

        if (m_rootGrid == nullptr)
        {
            m_rootGrid = std::make_unique<GridElement>(gridDirection);
        }
        else if (m_rootGrid->direction != gridDirection)
        {
            auto newGrid = std::make_unique<GridElement>(gridDirection);
            newGrid->PushBackElement(std::move(m_rootGrid));
            m_rootGrid = std::move(newGrid);
        }

        auto insertPosition = GetInsertPosition(position);

        auto newWidgetElement = std::make_unique<WidgetElement>(childData);
        auto* newWidgetElementPtr = newWidgetElement.get();
        m_widgetElements.insert(std::move(newWidgetElement));

        auto [lowElementPtr, highElementPtr] = m_rootGrid->InsertElement(newWidgetElementPtr, insertPosition);
        if (lowElementPtr && highElementPtr)
        {
            auto edgeDirection = FlipDirection(gridDirection);
            auto newEdge = std::make_unique<Edge>(edgeDirection, lowElementPtr, highElementPtr);
            auto* newEdgePtr = newEdge.get();
            m_edges.insert(std::move(newEdge));

            lowElementPtr->nextEdge = newEdgePtr;
            highElementPtr->prevEdge = newEdgePtr;
        }
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleMouseEvent(const WidgetEventSubType subType, const WidgetEvent::MouseEvent& mouseEvent)
    {
        switch (subType)
        {
            case WidgetEventSubType::MousePress:
            {
                // Edge bounds press
                m_pressedEdge = FindIntersectingEdge(mouseEvent.position);
                if (m_pressedEdge)
                {
                    m_prevMousePosition = mouseEvent.position;
                    return true;
                }

                // Drag bounds press.
                // ..

            } break;
            case WidgetEventSubType::MouseEnter: // MouseMove FIX IN Canvas.inl
            {
                if (m_pressedEdge)
                {
                    if (m_pressedEdge->direction == Direction::Horizontal)
                    {

                    }
                    else
                    {
                        auto edgeDiff = mouseEvent.position.x - m_prevMousePosition.x;
                        m_pressedEdge->bounds.left += edgeDiff;
                        m_pressedEdge->bounds.right += edgeDiff;

                        if (m_pressedEdge->nextElement)
                        {
                            auto* widgetElement = std::get<WidgetElement*>(m_pressedEdge->nextElement->data);
                            widgetElement->size.x -= edgeDiff;
                        }
                        /*if (m_pressedEdge->prevElement)
                        {
                            auto* widgetElement = std::get<WidgetElement*>(m_pressedEdge->prevElement->data);
                            widgetElement->size.x += edgeDiff;
                        }*/
                    }

                    m_prevMousePosition = mouseEvent.position;
                    return true;
                }
            } break;
            case WidgetEventSubType::MouseReleaseIn:
            case WidgetEventSubType::MouseReleaseOut:
            {
                if (m_pressedEdge)
                {
                    m_pressedEdge = nullptr;
                    return true;
                }
            } break;
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    void Docker<TSkin>::UpdateBounds()
    {
        if (!m_rootGrid)
        {
            return;
        }
   
        UpdateGridElementBounds(*m_rootGrid.get(), GetGrantedBounds());
    }

    template<typename TSkin>
    void Docker<TSkin>::UpdateGridElementBounds(GridElement& gridElement, const Bounds2f32& grantedBounds)
    {
        if (gridElement.direction == Direction::Horizontal)
        {
            UpdateDirectionalGridElementBounds<Direction::Horizontal>(gridElement, grantedBounds);
        }
        else
        {
            UpdateDirectionalGridElementBounds<Direction::Vertical>(gridElement, grantedBounds);
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    void Docker<TSkin>::UpdateDirectionalGridElementBounds(GridElement& gridElement, const Bounds2f32& grantedBounds)
    {
        auto boundsLeft = grantedBounds;

        auto it = gridElement.elements.rbegin();
        for (; it != gridElement.elements.rend(); it++)
        {
            if (boundsLeft.IsEmpty())
            {
                break;
            }

            auto& element = *it;

            if (element->type == ElementType::Widget)
            {
                auto* widgetElement = std::get<WidgetElement*>(element->data);
                float width = GetDirectionalSizeElement<VDirection>(widgetElement->size);
                auto widgetBounds = CutDirectionalHighBounds<VDirection>(boundsLeft, width);

                widgetElement->widgetData->SetGrantedBounds(widgetBounds);

                if (element->prevEdge)
                {
                    element->prevEdge->bounds = Bounds2f32(widgetBounds.left, widgetBounds.top, widgetBounds.left, widgetBounds.bottom)
                        .WithMargins({ 5.0f, 0.0f, 5.0f, 0.0f});
                }
            }
            else
            {
                // GRID ???
            }
        }

        // Dummy widget hiding
        for (; it != gridElement.elements.rend(); it++)
        {
            auto& element = *it;

            if (element->type == ElementType::Widget)
            {
                auto* widgetElement = std::get<WidgetElement*>(element->data);
                widgetElement->widgetData->SetGrantedBounds({ 0.0f, 0.0f, 0.0f, 0.0f });
            }
            else
            {
                // GRID ???
            }
        }
    }

    /*template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    float Docker<TSkin>::GetDirectionalBoundsSize(const Bounds2f32& bounds)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            return bounds.right - bounds.left;
        }
        else
        {
            return bounds.bottom - bounds.top;
        }
    }*/

    template<typename TSkin>
    void Docker<TSkin>::UpdateWidgetElementBounds(WidgetElement& widgetElement, const Bounds2f32& grantedBounds)
    {
        /*element.bounds = boundsLeft;

        if (element.type == ElementType::Widget)
        {
            auto* widgetData = element.data.widgetData;
            widgetData->SetGrantedBounds(boundsLeft);
        }*/
    }

    template<typename TSkin>
    constexpr typename Docker<TSkin>::Direction typename Docker<TSkin>::FlipDirection(const Direction direction)
    {
        return direction == Direction::Horizontal ? Direction::Vertical : Direction::Horizontal;
    }

    template<typename TSkin>
    constexpr typename Docker<TSkin>::Direction Docker<TSkin>::GetInsertDirection(DockingPosition position)
    {
        return position == DockingPosition::Left || position == DockingPosition::Right ? Direction::Horizontal : Direction::Vertical;
    }

    template<typename TSkin>
    constexpr typename Docker<TSkin>::ElementPosition Docker<TSkin>::GetInsertPosition(DockingPosition position)
    {
        return position == DockingPosition::Left || position == DockingPosition::Top ? ElementPosition::First : ElementPosition::Last;
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr float Docker<TSkin>::GetDirectionalSizeElement(const Vector2f32& size)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            return size.x;
        }
        else
        {
            return size.y;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    Bounds2f32 Docker<TSkin>::CutDirectionalHighBounds(Bounds2f32& grantedBounds, const float size)
    {
        const auto grantedSize = grantedBounds.GetSize();
        auto newBounds = grantedBounds;

        if constexpr (VDirection == Direction::Horizontal)
        {
            auto newEdge = std::max(grantedBounds.left, grantedBounds.right - size);
            newBounds.left = newEdge;

            grantedBounds.right -= size;
            grantedBounds.ClampHighToLow();

            return newBounds;
        }
        else
        {
            auto newEdge = std::max(grantedBounds.top, grantedBounds.bottom - size);
            newBounds.top = newEdge;

            grantedBounds.bottom -= size;
            grantedBounds.ClampHighToLow();

            return newBounds;
        }
    }

    template<typename TSkin>
    typename Docker<TSkin>::Edge* Docker<TSkin>::FindIntersectingEdge(const Vector2f32& point)
    {
        for(auto& edge : m_edges)
        {
            if (edge->bounds.Intersects(point))
            {
                return edge.get();
            }
        }
        return nullptr;
    }

}
