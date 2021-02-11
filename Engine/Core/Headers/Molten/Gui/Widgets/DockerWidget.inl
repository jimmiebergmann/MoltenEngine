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

#include <iostream>

namespace Molten::Gui
{

    // Docker public implementations.

    template<typename TSkin>
    inline Docker<TSkin>::Docker(WidgetData<TSkin>& data) :
        Widget<TSkin>(data),
        m_forceUpdateBounds(false),
        m_rootGrid(nullptr),
        m_pressedEdge(nullptr)
    { }

    template<typename TSkin>
    void Docker<TSkin>::Update()
    {
        ApplyMarginsToGrantedBounds();

        if (m_forceUpdateBounds || m_oldGrantedBounds != GetGrantedBounds())
        {
            m_forceUpdateBounds = false;
            CalculateBounds(); 
        }

        m_oldGrantedBounds = GetGrantedBounds();
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
        widgetData(&widgetData)
    {}


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
    Docker<TSkin>::Element::Element(WidgetElement* widgetElement, Element* parent) :
        parent(parent),
        type(ElementType::Widget),
        data(widgetElement),
        dynamicSize(IsDynamicSize(*widgetElement->widgetData)),
        size(GetDirectionalMinSize(widgetElement->widgetData->GetWidget()->size)),
        calculatedSize(size),
        prevEdge(nullptr),
        nextEdge(nullptr)
    {}

    template<typename TSkin>
    Docker<TSkin>::Element::Element(std::unique_ptr<GridElement>&& gridElement, Element* parent) :
        parent(parent),
        type(ElementType::Grid),
        data(std::move(gridElement)),
        dynamicSize(true),
        size(GetDirectionalMinSize({0.0f, 0.0f})),
        calculatedSize(size),
        prevEdge(nullptr),
        nextEdge(nullptr)
    {}

    template<typename TSkin>
    bool Docker<TSkin>::Element::IsDynamicSize(WidgetData<TSkin>& widgetData)
    {
        auto* widget = widgetData.GetWidget().get();
        return widget->size.x <= 0.0f || widget->size.y <= 0.0f;
    }


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

        m_forceUpdateBounds = true;
    }

    template<typename TSkin>
    constexpr typename Docker<TSkin>::Direction Docker<TSkin>::FlipDirection(const Direction direction)
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
    bool Docker<TSkin>::HandleMouseEvent(const WidgetEventSubType subType, const WidgetEvent::MouseEvent& mouseEvent)
    {
        switch (subType)
        {
            case WidgetEventSubType::MousePress: return HandleMousePressEvent(mouseEvent);
            case WidgetEventSubType::MouseEnter: return HandleMouseMoveEvent(mouseEvent); // MouseMove FIX IN Canvas.inl 
            case WidgetEventSubType::MouseReleaseIn:
            case WidgetEventSubType::MouseReleaseOut: return HandleMouseReleaseEvent(mouseEvent);
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleMousePressEvent(const WidgetEvent::MouseEvent& mouseEvent)
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

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (!m_pressedEdge)
        {
            return false;
        }
       
        auto mouseMovement = Vector2f32(mouseEvent.position) - m_prevMousePosition;
        bool movedEdge = false;
       
        if (m_pressedEdge->direction == Direction::Horizontal)
        {
            movedEdge = HandleDirectionalEdgeMovement<Direction::Horizontal>(mouseMovement.y);
        }
        else
        {
            movedEdge = HandleDirectionalEdgeMovement<Direction::Vertical>(mouseMovement.x);
        }

        if (movedEdge)
        {
            m_forceUpdateBounds = true;
            m_prevMousePosition = mouseEvent.position;
        }       

        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleMouseReleaseEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (m_pressedEdge)
        {
            m_pressedEdge = nullptr;
            return true;
        }

        return false;
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VEdgeDirection>
    bool Docker<TSkin>::HandleDirectionalEdgeMovement(float movement)
    {      
        movement = LockEdgeMovement<VEdgeDirection>(movement, *m_pressedEdge);
        if (movement == 0.0f)
        {
            return false;
        }

        auto* prevElement = m_pressedEdge->prevElement;
        auto* nextElement = m_pressedEdge->nextElement;
        if (prevElement && nextElement && prevElement->dynamicSize && nextElement->dynamicSize)
        {
            prevElement->size.x += movement;
            nextElement->size.x -= movement;
        }
        else
        {
            if (prevElement)
            {
                if (!prevElement->dynamicSize)
                {
                    prevElement->size.x += movement;
                }
            }
            if (nextElement)
            {
                if (!nextElement->dynamicSize)
                {
                    nextElement->size.x -= movement;
                }
            }
        }
       
        
        return true;
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VEdgeDirection>
    constexpr float Docker<TSkin>::LockEdgeMovement(const float movement, const Edge& edge)
    {
        auto calculateMinMovement = [](const Element* element, float decrease)
        {
            constexpr Direction flippedDirection = FlipDirection(VEdgeDirection);

            auto elementWidth = GetDirectionalWidth<FlipDirection(VEdgeDirection)>(element->size);
            auto allowedDecrease = elementWidth - m_minElementWidth;
            auto overshoot = -std::min(allowedDecrease - decrease, 0.0f);
            return decrease - overshoot;
        };

        auto newMovement = movement;

        if (newMovement < 0.0f)
        {
            newMovement = -calculateMinMovement(edge.prevElement, -newMovement);
        }
        else if (newMovement > 0.0f)
        {
            newMovement = calculateMinMovement(edge.nextElement, newMovement);
        }

        return newMovement;
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateBounds()
    {
        if (!m_rootGrid)
        {
            return;
        }
   
        CalculateGridElementBounds(*m_rootGrid.get(), GetGrantedBounds());
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateGridElementBounds(GridElement& gridElement, const Bounds2f32& grantedBounds)
    {
        if (gridElement.direction == Direction::Horizontal)
        {
            CalculateDirectionalGridElementSizes<Direction::Horizontal>(gridElement, grantedBounds);
        }
        else
        {
            CalculateDirectionalGridElementSizes<Direction::Vertical>(gridElement, grantedBounds);
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    void Docker<TSkin>::CalculateDirectionalGridElementSizes(GridElement& gridElement, const Bounds2f32& grantedBounds)
    {
        /*
        
        float totalStaticWidth = 0.0f;
        float totalDynamicWidth = 0.0f;
        std::vector<Element*> dynamicElements = {}

        // CalculateElementSizes<VDirection>(totalStaticWidth, totalDynamicWidth, dynamicElements)
        // CalculateDynamicSizes<VDirection>(...);
        // SetElementBounds<VDirection>(...);


        */



        const auto grantedSize = grantedBounds.GetSize();
        const float grantedWidth = GetDirectionalWidth<VDirection>(grantedSize);
        auto widthLeft = grantedWidth;

        float totalStaticWidth = 0.0f;
        float totalDynamicWidth = 0.0f;
        std::vector<Element*> dynamicElements = {};     

        // Iterate until widthleft is empty.
        auto it = gridElement.elements.rbegin();
        for (; it != gridElement.elements.rend(); it++)
        {
            if (widthLeft <= 0.0f)
            {
                break;
            }

            auto& element = *it;
            if (element->type == ElementType::Widget) // Widget
            {
                //auto* widgetElement = std::get<WidgetElement*>(element->data);

                if (element->dynamicSize)
                {     
                    float minWidth = GetDirectionalMinWidth<VDirection>(element->size);
                    SetDirectionalWidth<VDirection>(element->calculatedSize, minWidth);
                    CutWidth(widthLeft, m_minElementWidth);
                    totalDynamicWidth += minWidth;

                    dynamicElements.push_back(element.get());
                }
                else
                {
                    float minWidth = GetDirectionalMinWidth<VDirection>(element->size);
                    CutAndClampWidth(widthLeft, minWidth);
                    SetDirectionalWidth<VDirection>(element->calculatedSize, minWidth);
                    totalStaticWidth += minWidth;
                }
            }
            else // Grid
            {
                // WHAT SHOULD WE DO HERE???
                /*
                //auto& childGridElement = std::get<std::unique_ptr<GridElement>>(element->data);

                float minWidth = GetDirectionalMinWidth<VDirection>(element->size);

                CutWidth(widthLeft, minWidth);
                dynamicElements.push_back(element.get());*/
            }
        }

        // Calculate dynamic sizes
        const auto availableDynamicWidth = (grantedWidth - totalStaticWidth) - totalDynamicWidth;
        const auto partialDynamicWidth = dynamicElements.size() ? availableDynamicWidth / static_cast<float>(dynamicElements.size()) : 0.0f;

        auto dynamicWidthLeft = availableDynamicWidth;
        for (auto* dynamicElement : dynamicElements)
        {
            dynamicWidthLeft -= partialDynamicWidth;
            AddDirectionalWidth<VDirection>(dynamicElement->calculatedSize, partialDynamicWidth);
            SetDirectionalWidth<VDirection>(dynamicElement->size, GetDirectionalWidth<VDirection>(dynamicElement->calculatedSize));
        }

        // Hide invisible elements.
        HideElements(it, gridElement.elements.rend());

        // Set bounds from calculate sizes.
        auto endIt = it;
        auto boundsLeft = grantedBounds;
        for (it = gridElement.elements.rbegin(); it != endIt; it++)
        {
            auto* element = (*it).get();
            auto elementWidth = GetDirectionalWidth<VDirection>(element->calculatedSize);
            auto elementBounds = CutDirectionalBounds<VDirection>(boundsLeft, elementWidth);
            
            if (element->type == ElementType::Widget) // Widget
            {
                auto* widgetElement = std::get<WidgetElement*>(element->data);
                widgetElement->widgetData->SetGrantedBounds(elementBounds);

                auto* prevEdge = element->prevEdge;
                if (prevEdge)
                {
                    prevEdge->bounds = Bounds2f32(elementBounds.left, grantedBounds.top, elementBounds.left, grantedBounds.bottom)
                        .AddMargins({ 5.0f, 0.0f, 5.0f, 0.0f});
                }
            }
            else // Grid
            {
                // WHAT SHOULD WE DO HERE???
            }
        }

    }

    template<typename TSkin>
    constexpr Vector2f32 Docker<TSkin>::GetDirectionalMinSize(const Vector2f32& size)
    {
        return {
            std::max(size.x, m_minElementWidth),
            std::max(size.y, m_minElementWidth)
        };
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr float Docker<TSkin>::GetDirectionalMinWidth(const Vector2f32& size)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            return std::max(size.x, m_minElementWidth);
        }
        else
        {
            return std::max(size.y, m_minElementWidth);
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr float Docker<TSkin>::GetDirectionalWidth(const Vector2f32& size)
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
    constexpr float Docker<TSkin>::GetDirectionalHeight(const Vector2f32& size)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            return size.y;
        }
        else
        {
            return size.x;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr void Docker<TSkin>::SetDirectionalWidth(Vector2f32& size, const float width)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            size.x = width;
        }
        else
        {
            size.y = width;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr void Docker<TSkin>::AddDirectionalWidth(Vector2f32& size, const float width)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            size.x += width;
        }
        else
        {
            size.y += width;
        }
    }

    template<typename TSkin>
    constexpr void Docker<TSkin>::CutWidth(float& widthLeft, const float width)
    {
        if (width > widthLeft)
        {
            widthLeft = 0.0f;
        }
        else
        {
            widthLeft -= width;
        }
    }

    template<typename TSkin>
    constexpr void Docker<TSkin>::CutAndClampWidth(float& widthLeft, float& width)
    {
        if (width > widthLeft)
        {
            width = widthLeft;
            widthLeft = 0.0f;
        }
        else
        {
            widthLeft -= width;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr Bounds2f32 Docker<TSkin>::CutDirectionalBounds(Bounds2f32& grantedBounds, const float size)
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
    template<typename TIterator>
    void Docker<TSkin>::HideElements(TIterator begin, TIterator end)
    {
        for (; begin != end; begin++)
        {
            auto& element = *begin;

            if (element->type == ElementType::Widget)
            {
                auto* widgetElement = std::get<WidgetElement*>(element->data);
                widgetElement->widgetData->SetGrantedBounds({ 0.0f, 0.0f, 0.0f, 0.0f });
            }
            else // Grid
            {
                auto& gridElement = std::get<std::unique_ptr<GridElement>>(element->data);
                HideElements(gridElement->elements.rbegin(), gridElement->elements.rend());
            }
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
