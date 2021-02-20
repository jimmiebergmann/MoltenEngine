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
        WidgetMixin<TSkin, Docker<TSkin>>(data),
        m_stateType(State::Type::Normal),
        m_currentCursor(Mouse::Cursor::Normal),
        m_forceUpdateBounds(false),
        m_pressedEdge(nullptr),
        m_pressedLeafElement(nullptr)    
    { }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    inline WidgetTypePointer<TWidgetType<TSkin>> Docker<TSkin>::CreateChild(
        const DockingPosition position,
        const bool dynamic,
        TArgs ... args)
    {
        auto widget = Widget<TSkin>::CreateChild<TWidgetType>(std::forward<TArgs>(args)...);
  
        auto it = m_leafInsertMap.find(widget.get());
        if (it == m_leafInsertMap.end())
        {
            return widget;
        }

        auto& leafInsert = it->second;
        leafInsert->position = position;
        leafInsert->isDynamic = dynamic;

        return widget;
    }

    template<typename TSkin>
    void Docker<TSkin>::Update()
    {
        if (InsertNewLeafs())
        {
            m_forceUpdateBounds = true;
        }

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

    template<typename TSkin>
    inline void Docker<TSkin>::OnAddChild(WidgetData<TSkin>& childData)
    {
        auto* widget = childData.GetWidget().get();

        bool isDynamic = widget->size.x <= 0.0f || widget->size.y <= 0.0f;
        auto pendingLeaf = std::make_shared<PendingLeafInsert>(DockingPosition::Right, isDynamic, &childData);

        m_leafInsertMap.insert({ widget, pendingLeaf });
        m_leafInsertQueue.push(pendingLeaf);
    }


    // Docker leaf implementations.
    template<typename TSkin>
    Docker<TSkin>::Leaf::Leaf(WidgetData<TSkin>* widgetData, const bool isDynamic) :
        widgetData(widgetData),
        draggableWidget(dynamic_cast<DraggableWidget*>(widgetData->GetWidget().get())),
        isDynamic(isDynamic)
    {}

    template<typename TSkin>
    bool Docker<TSkin>::Leaf::IsDynamic() const
    {
        return isDynamic;
    }


    // Docker grid implementations.
    template<typename TSkin>
    Docker<TSkin>::Grid::Grid(Direction direction) :
        direction(direction),
        calculatedResult(*this)
    {}

    template<typename TSkin>
    std::pair<typename Docker<TSkin>::Element*, typename Docker<TSkin>::Element*> Docker<TSkin>::Grid::InsertElement(ElementPointer&& element, const ElementPosition position)
    {
        auto it = position == ElementPosition::First ? elements.begin() : elements.end();
        auto* prevElementPtr = elements.empty() ? nullptr : (position == ElementPosition::First ? elements.front().get() : elements.back().get());
        auto* newElementPtr = element.get();

        elements.insert(it, std::move(element));

        // Make prev element to high and next element to low if we insert in front(first/left).
        if (position == ElementPosition::First)
        {
            std::swap(prevElementPtr, newElementPtr);
        }

        return { prevElementPtr, newElementPtr };
    }

    template<typename TSkin>
    bool Docker<TSkin>::Grid::IsDynamic() const
    {
        for (auto& element : elements)
        {
            if (element->IsDynamic())
            {
                return true;
            }
        }
        return false;
    }


    // Docker element implementations.
    template<typename TSkin>
    Docker<TSkin>::Element::Element(LeafPointer&& leaf) :
        size(GetDirectionalMinSize(leaf->widgetData->GetWidget()->size)),
        calculatedSize(size),
        type(ElementType::Leaf),
        data(std::move(leaf)),
        prevEdge(nullptr),
        nextEdge(nullptr)
    {}

    template<typename TSkin>
    Docker<TSkin>::Element::Element(GridPointer&& grid) :
        size(GetDirectionalMinSize({ 0.0f, 0.0f })),
        calculatedSize(size),
        type(ElementType::Grid),
        data(std::move(grid)),
        prevEdge(nullptr),
        nextEdge(nullptr)
    {}

    template<typename TSkin>
    bool Docker<TSkin>::Element::IsDynamic() const
    {     
        return std::visit(
            [&](const auto& element) { return element->IsDynamic(); },
            data);
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

    template<typename TSkin>
    Mouse::Cursor Docker<TSkin>::Edge::GetSizeCursor() const
    {
        return direction == Direction::Horizontal ? Mouse::Cursor::SizeUpDown : Mouse::Cursor::SizeLeftRight;
    }


    // Docker calculated grid result implementations.
    template<typename TSkin>
    Docker<TSkin>::CalculatedGridResult::CalculatedGridResult(Grid& grid) :
        grid(grid),
        totalStaticWidth(0.0f),
        totalDynamicWidth(0.0f)
    {}

    template<typename TSkin>
    void Docker<TSkin>::CalculatedGridResult::Clear()
    {
        grantedSize = {};
        grantedBounds = {};
        staticElements.clear();
        dynamicElements.clear();
        totalStaticWidth = 0.0f;
        totalDynamicWidth = 0.0f;
    }


    // Docker widget queued item implementations.
    template<typename TSkin>
    Docker<TSkin>::PendingLeafInsert::PendingLeafInsert(
        const DockingPosition position,
        const bool isDynamic,
        WidgetData<TSkin>* widgetData
    ) :
        position(position),
        isDynamic(isDynamic),
        widgetData(widgetData)
    {}


    // Private docket implementations.
    template<typename TSkin>
    bool Docker<TSkin>::InsertNewLeafs()
    {
        m_leafInsertMap.clear();
        
        if (m_leafInsertQueue.empty())
        {
            return false;
        }

        while (!m_leafInsertQueue.empty())
        {
            auto& pendingLeaf = m_leafInsertQueue.front();
            InsertLeaf(*pendingLeaf);
            m_leafInsertQueue.pop();
        }

        return true;
    }

    template<typename TSkin>
    void Docker<TSkin>::InsertLeaf(PendingLeafInsert& pendingLeaf)
    {
        const auto position = pendingLeaf.position;
        const auto insertPosition = GetInsertPosition(position);
        const auto gridDirection = GetInsertDirection(position); 

        auto newLeaf = std::make_unique<Leaf>(pendingLeaf.widgetData, pendingLeaf.isDynamic);     
        //m_leafs.insert(newLeaf.get());
        auto newElement = std::make_unique<Element>(std::move(newLeaf));  
        m_leafElements.insert(newElement.get());

        if (!m_rootElement)
        {
            m_rootElement = std::move(newElement);
        }
        else if (m_rootElement->type == ElementType::Leaf) // Leaf
        {
            m_rootElement = TransformElementToGrid(std::move(m_rootElement), std::move(newElement), gridDirection, insertPosition);
        }
        else // Grid
        {
            auto& grid = std::get<GridPointer>(m_rootElement->data);
            if (grid->direction != gridDirection)
            {
                m_rootElement = TransformElementToGrid(std::move(m_rootElement), std::move(newElement), gridDirection, insertPosition);
            }
            else
            {
                InsertElementInGrid(grid, std::move(newElement), insertPosition);
            }
        }
    }

    template<typename TSkin>
    void Docker<TSkin>::InsertElementInGrid(
        GridPointer& grid,
        ElementPointer&& element,
        const ElementPosition insertPosition)
    {
        auto [lowElementPtr, highElementPtr] = grid->InsertElement(std::move(element), insertPosition);

        if (lowElementPtr && highElementPtr)
        {
            auto edgeDirection = FlipDirection(grid->direction);
            auto newEdge = std::make_unique<Edge>(edgeDirection, lowElementPtr, highElementPtr);

            lowElementPtr->nextEdge = newEdge.get();
            highElementPtr->prevEdge = newEdge.get();

            m_edges.insert(std::move(newEdge));
        }
    }

    template<typename TSkin>
    typename Docker<TSkin>::ElementPointer Docker<TSkin>::TransformElementToGrid(
        ElementPointer&& oldElement,
        ElementPointer&& newElement,
        const Direction gridDirection,
        const ElementPosition insertPosition)
    {
        auto newGrid = std::make_unique<Grid>(gridDirection);
        
        newGrid->elements.push_back(std::move(oldElement));
        InsertElementInGrid(newGrid, std::move(newElement), insertPosition);

        return std::make_unique<Element>(std::move(newGrid));
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
    void Docker<TSkin>::SetCursor(const Mouse::Cursor cursor)
    {
        if (m_currentCursor != cursor)
        {
            onCursorChange(cursor);
            m_currentCursor = cursor;
        }
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
        m_pressedEdge = nullptr;
        m_pressedLeafElement = nullptr;

        // Edge bounds press
        m_pressedEdge = FindIntersectingEdge(mouseEvent.position);
        if (m_pressedEdge)
        {
            m_prevMousePosition = mouseEvent.position;
            return true;
        }

        m_pressedLeafElement = FindIntersectingDraggableLeaf(mouseEvent.position);
        if (m_pressedLeafElement)
        {
            return true;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (HandleEdgeMovement(mouseEvent))
        {
            return true;
        }
        if (HandleLeafMovement(mouseEvent))
        {
            return true;
        }        
        if (HandleMouseHover(mouseEvent))
        {
            return true;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleMouseReleaseEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (m_pressedEdge || m_pressedLeafElement)
        {
            m_pressedEdge = nullptr;
            m_pressedLeafElement = nullptr;
            return true;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEdgeMovement(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (!m_pressedEdge)
        {
            return false;
        }

        auto mouseMovement = Vector2f32(mouseEvent.position) - m_prevMousePosition;
        bool movedEdge = false;

        if (m_pressedEdge->direction == Direction::Horizontal)
        {
            movedEdge = HandleDirectionalEdgeMovement<Direction::Horizontal>(*m_pressedEdge, mouseMovement.y);
        }
        else
        {
            movedEdge = HandleDirectionalEdgeMovement<Direction::Vertical>(*m_pressedEdge, mouseMovement.x);
        }

        if (movedEdge)
        {
            m_forceUpdateBounds = true;
            m_prevMousePosition = mouseEvent.position;
        }

        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleLeafMovement(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (!m_pressedLeafElement)
        {
            return false;
        }

        //auto& leaf = *std::get<LeafPointer>(m_pressedLeafElement->data);

        // No implementation yet.
        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleMouseHover(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (auto* edge = FindIntersectingEdge(mouseEvent.position))
        {
            SetCursor(edge->GetSizeCursor());
            return true;
        }
        
        SetCursor(Mouse::Cursor::Normal);
        return false;
    }


    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VEdgeDirection>
    bool Docker<TSkin>::HandleDirectionalEdgeMovement(Edge& edge, float movement)
    {      
        movement = LockEdgeMovement<VEdgeDirection>(edge, movement);
        if (movement == 0.0f)
        {
            return false;
        }

        auto* prevElement = edge.prevElement;
        auto* nextElement = edge.nextElement;

        const bool prevIsDynamic = prevElement && prevElement->IsDynamic();
        const bool nextIsDynamic = nextElement && nextElement->IsDynamic();
        const bool prevAndNextIsDynamic = prevIsDynamic && nextIsDynamic;
        
        if (prevAndNextIsDynamic)
        {
            AddDirectionalHeight<VEdgeDirection>(prevElement->size, movement);
            AddDirectionalHeight<VEdgeDirection>(nextElement->size, -movement);
        }
        else
        {
            if (prevElement)
            {
                if (!prevElement->IsDynamic())
                {
                    AddDirectionalHeight<VEdgeDirection>(prevElement->size, movement);
                }
            }
            if (nextElement)
            {
                if (!nextElement->IsDynamic())
                {
                    AddDirectionalHeight<VEdgeDirection>(nextElement->size, -movement);
                }
            }
        }
         
        return true;
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VEdgeDirection>
    constexpr float Docker<TSkin>::LockEdgeMovement(const Edge& edge, const float movement)
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
    typename Docker<TSkin>::Edge* Docker<TSkin>::FindIntersectingEdge(const Vector2f32& point)
    {
        for (auto& edge : m_edges)
        {
            if (edge->bounds.Intersects(point))
            {
                return edge.get();
            }
        }
        return nullptr;
    }

    template<typename TSkin>
    typename Docker<TSkin>::Element* Docker<TSkin>::FindIntersectingDraggableLeaf(const Vector2f32& point)
    {
        for (auto& leafElement : m_leafElements)
        {
            auto& leaf = std::get<LeafPointer>(leafElement->data);
            auto* draggableWidget = leaf->draggableWidget;
            if (draggableWidget && draggableWidget->GetDragBounds().Intersects(point))
            {
                return leafElement;
            }
        }

        return nullptr;
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateBounds()
    {
        if (!m_rootElement)
        {
            return;
        }

        CalculateElementBounds(*m_rootElement.get(), GetGrantedBounds());
    }

    template<typename TSkin>
    void  Docker<TSkin>::CalculateElementBounds(Element& element, const Bounds2f32& grantedBounds)
    {
        std::visit([&](auto& data) 
        {            
            CalculateElementBounds(*data.get(), grantedBounds);
        }, element.data);
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateElementBounds(Leaf& leaf, const Bounds2f32& grantedBounds)
    {
        leaf.widgetData->SetGrantedBounds(grantedBounds);
    }
    
    template<typename TSkin>
    void Docker<TSkin>::CalculateElementBounds(Grid& grid, const Bounds2f32& grantedBounds)
    {
        if (grid.direction == Direction::Horizontal)
        {
            CalculateGridBounds<Direction::Horizontal>(grid, grantedBounds);
        }
        else
        {
            CalculateGridBounds<Direction::Vertical>(grid, grantedBounds);
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    void Docker<TSkin>::CalculateGridBounds(Grid& grid, const Bounds2f32& grantedBounds)
    {     
        grid.calculatedResult.Clear();
        grid.calculatedResult.grantedBounds = grantedBounds;
        grid.calculatedResult.grantedSize = grantedBounds.GetSize();

        CalculateGridElementsBounds<VGridDirection>(grid.calculatedResult);

        if(!grid.calculatedResult.dynamicElements.empty())
        {
            BalanceGridDynamicElements<VGridDirection>(grid.calculatedResult);
        }
        if(grid.calculatedResult.dynamicElements.empty() && !grid.calculatedResult.staticElements.empty())
        {
            BalanceGridStaticElements<VGridDirection>(grid.calculatedResult);
        }

        SetGridElementBounds<VGridDirection>(grid.calculatedResult);
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    void Docker<TSkin>::CalculateGridElementsBounds(CalculatedGridResult& calculatedGridResult)
    {
        const float grantedWidth = GetDirectionalWidth<VGridDirection>(calculatedGridResult.grantedSize);
        auto widthLeft = grantedWidth;

        // Iterate until widthleft is empty.
        auto& grid = calculatedGridResult.grid;
        auto it = grid.elements.rbegin();
        for (; it != grid.elements.rend(); it++)
        {
            if (widthLeft <= 0.0f)
            {
                break;
            }

            auto& element = *it;

            std::visit([&](auto& data)
            {
                if (data->IsDynamic())
                {
                    float minWidth = GetDirectionalMinWidth<VGridDirection>(element->size);
                    SetDirectionalWidth<VGridDirection>(element->calculatedSize, minWidth);
                    CutWidth(widthLeft, m_minElementWidth);

                    calculatedGridResult.totalDynamicWidth += minWidth;
                    calculatedGridResult.dynamicElements.push_back(element.get());
                }
                else
                {
                    float minWidth = GetDirectionalMinWidth<VGridDirection>(element->size);
                    CutAndClampWidth(widthLeft, minWidth);
                    SetDirectionalWidth<VGridDirection>(element->calculatedSize, minWidth);

                    calculatedGridResult.totalStaticWidth += minWidth;
                    calculatedGridResult.staticElements.push_back(element.get());
                }
            }, element->data);
        }

        // Hide invisible elements.
        HideElements(it, grid.elements.rend());

        calculatedGridResult.activeElementsBegin = grid.elements.rbegin();
        calculatedGridResult.activeElementsEnd = it;
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    void Docker<TSkin>::BalanceGridDynamicElements(const CalculatedGridResult& calculatedGridResult)
    {
        if (calculatedGridResult.dynamicElements.empty())
        {
            return;
        }

        const auto grantedWidth = GetDirectionalWidth<VGridDirection>(calculatedGridResult.grantedSize);
        const auto availableWidth = (grantedWidth - calculatedGridResult.totalStaticWidth) - calculatedGridResult.totalDynamicWidth;
        const auto partialWidth = availableWidth / static_cast<float>(calculatedGridResult.dynamicElements.size());

        auto dynamicWidthLeft = availableWidth;
        for (auto* element : calculatedGridResult.dynamicElements)
        {
            dynamicWidthLeft -= partialWidth;
            AddDirectionalWidth<VGridDirection>(element->calculatedSize, partialWidth);
            SetDirectionalWidth<VGridDirection>(element->size, GetDirectionalWidth<VGridDirection>(element->calculatedSize));
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    void Docker<TSkin>::BalanceGridStaticElements(const CalculatedGridResult& calculatedGridResult)
    {
        if (calculatedGridResult.staticElements.empty())
        {
            return;
        }
        
        const auto grantedWidth = GetDirectionalWidth<VGridDirection>(calculatedGridResult.grantedSize);
        const auto availableWidth = grantedWidth - calculatedGridResult.totalStaticWidth;
        const auto partialWidth = availableWidth / static_cast<float>(calculatedGridResult.staticElements.size());

        auto dynamicWidthLeft = availableWidth;
        for (auto* element : calculatedGridResult.staticElements)
        {
            dynamicWidthLeft -= partialWidth;
            AddDirectionalWidth<VGridDirection>(element->calculatedSize, partialWidth);
            SetDirectionalWidth<VGridDirection>(element->size, GetDirectionalWidth<VGridDirection>(element->calculatedSize));
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    void Docker<TSkin>::SetGridElementBounds(const CalculatedGridResult& calculatedGridResult)
    {
        constexpr Direction flippedDirection = FlipDirection(VGridDirection);

        auto begin = calculatedGridResult.activeElementsBegin;
        auto end = calculatedGridResult.activeElementsEnd;
        
        auto grantedBounds = calculatedGridResult.grantedBounds;
        auto boundsLeft = grantedBounds;

        for (auto it = begin; it != end; it++)
        {
            auto& element = *it->get();
            auto elementWidth = GetDirectionalWidth<VGridDirection>(element.calculatedSize);
            auto elementBounds = CutDirectionalBounds<VGridDirection>(boundsLeft, elementWidth);

            element.bounds = elementBounds;

            CalculateElementBounds(element, elementBounds);  
            SetElementPrevEdgeBounds<flippedDirection>(element, grantedBounds);

            CutDirectionalBounds<VGridDirection>(boundsLeft, spacing);   
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VEdgeDirection>
    void Docker<TSkin>::SetElementPrevEdgeBounds(Element& element, const Bounds2f32& grantedBounds)
    {
        auto* prevEdge = element.prevEdge;
        if (prevEdge)
        {
            float edgeWidthHalf = edgeWidth * 0.5f;
            float spacingHalf = spacing * 0.5f;

            if constexpr (VEdgeDirection == Direction::Horizontal)
            {
                prevEdge->bounds = Bounds2f32(grantedBounds.left, element.bounds.top, grantedBounds.right, element.bounds.top)
                    .AddMargins({ 0.0f, edgeWidthHalf, 0.0f, edgeWidthHalf })
                    .Move({ 0.0f, -spacingHalf });
            }
            else
            {
                prevEdge->bounds = Bounds2f32(element.bounds.left, grantedBounds.top, element.bounds.left, grantedBounds.bottom)
                    .AddMargins({ edgeWidthHalf, 0.0f, edgeWidthHalf, 0.0f })
                    .Move({ -spacingHalf, 0.0f });
            }
        }
    }

    template<typename TSkin>
    template<typename TIterator>
    void Docker<TSkin>::HideElements(TIterator begin, TIterator end)
    {
        for (; begin != end; begin++)
        {
            auto& element = *begin;

            if (element->type == ElementType::Leaf) // Leaf
            {
                auto& leaf = std::get<LeafPointer>(element->data);
                leaf->widgetData->SetGrantedBounds({ 0.0f, 0.0f, 0.0f, 0.0f });
            }
            else // Grid
            {
                auto& grid = std::get<GridPointer>(element->data);
                HideElements(grid->elements.rbegin(), grid->elements.rend());
            }
        }
    }

    template<typename TSkin>
    constexpr typename Docker<TSkin>::Direction Docker<TSkin>::FlipDirection(const Direction direction)
    {
        return direction == Direction::Horizontal ? Direction::Vertical : Direction::Horizontal;
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
    constexpr void Docker<TSkin>::SetDirectionalHeight(Vector2f32& size, const float height)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            size.y = height;
        }
        else
        {
            size.x = height;
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
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr void Docker<TSkin>::AddDirectionalHeight(Vector2f32& size, const float height)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            size.y += height;
        }
        else
        {
            size.x += height;
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

}
