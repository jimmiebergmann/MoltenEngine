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

    // Docket state implementations.
    template<typename TSkin>
    Docker<TSkin>::State::State() :
        type(Type::Normal),
        leafDragState{ Bounds2f32{} }
    {}

    template<typename TSkin>
    Docker<TSkin>::State::State(const LeafDragState& leafDragState) :
        type(Type::LeafDrag),
        leafDragState(leafDragState)
    {}


    // Docker public implementations.
    template<typename TSkin>
    Docker<TSkin>::Docker(WidgetDataMixin<TSkin, Docker>& data) :
        WidgetMixin<TSkin, Docker>(data),  
        m_stateType{},
        m_currentCursor(Mouse::Cursor::Normal),
        m_rootElement(nullptr),
        m_mouseInputUpdateFunc(&Docker<TSkin>::HandleNormalMouseEvent),
        m_forceUpdateBounds(false),
        m_oldGrantedBounds{},
        m_leafInsertQueue{},
        m_leafInsertMap{},
        m_edges{},
        m_edgeDragData{},
        m_leafs{},
        m_leafDragData{}
    { }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    WidgetTypePointer<TWidgetType<TSkin>> Docker<TSkin>::CreateChild(
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

        if (GetSkinState<State>().type == State::Type::LeafDrag && m_leafDragData.dragIsActivated) // Ugly stuff going on here.
        {
            GetDataMixin().canvas->PushTopDrawCommand([this]()
            {
                GetDataMixin().widgetSkinMixin->DrawLeafDocking();
            });
        }
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEvent(const WidgetEvent& widgetEvent)
    {
        switch (widgetEvent.type)
        {
            case WidgetEventType::Mouse: return (*this.*m_mouseInputUpdateFunc)(widgetEvent); break;
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    void Docker<TSkin>::OnAddChild(WidgetData<TSkin>& childData)
    {
        auto* widget = childData.GetWidget().get(); // FIX! Add new function for getting raw pointer to widget.

        bool isDynamic = widget->size.x <= 0.0f || widget->size.y <= 0.0f;
        auto pendingLeaf = std::make_shared<PendingLeafInsert>(DockingPosition::Right, isDynamic, &childData);

        m_leafInsertMap.insert({ widget, pendingLeaf });
        m_leafInsertQueue.push(pendingLeaf);
    }


    // Docker leaf implementations.
    template<typename TSkin>
    Docker<TSkin>::Leaf::Leaf(
        WidgetData<TSkin>* widgetData,
        const bool isDynamic
    ) :
        m_owner(nullptr),
        widgetData(widgetData),
        draggableWidget(dynamic_cast<DraggableWidget*>(widgetData->GetWidget().get())), // FIX! Add new function for getting raw pointer to widget.
        isDynamic(isDynamic)
    {}

    template<typename TSkin>
    typename Docker<TSkin>::Element* Docker<TSkin>::Leaf::AsElement()
    {
        return m_owner;
    }

    template<typename TSkin>
    bool Docker<TSkin>::Leaf::IsDynamic() const
    {
        return isDynamic;
    }


    // Docker grid implementations.
    template<typename TSkin>
    Docker<TSkin>::Grid::Grid(Direction direction) :
        m_owner(nullptr),
        direction(direction)
    {}

    template<typename TSkin>
    typename Docker<TSkin>::Element* Docker<TSkin>::Grid::AsElement()
    {
        return m_owner;
    }

    template<typename TSkin>
    bool Docker<TSkin>::Grid::IsDynamic() const
    {
        return !dynamicElements.empty();
    }


    // Docker element implementations.
    template<typename TSkin>
    Docker<TSkin>::Element::Element(LeafPointer&& leaf, const Vector2f32& requestedSize) :
        m_type(ElementType::Leaf),
        m_data(std::move(leaf)),
        m_parent(nullptr),
        m_prevEdge(nullptr),
        m_nextEdge(nullptr),
        requestedSize(requestedSize)
    {
        std::get<LeafPointer>(m_data)->m_owner = this;
    }

    template<typename TSkin>
    Docker<TSkin>::Element::Element(GridPointer&& grid) :
        m_type(ElementType::Grid),
        m_data(std::move(grid)),
        m_parent(nullptr),
        m_prevEdge(nullptr),
        m_nextEdge(nullptr)
    {
        auto& gridData = std::get<GridPointer>(m_data);
        gridData->m_owner = this;

        for (auto& element : gridData->elements)
        {
            element->m_parent = this;
        }
    }

    template<typename TSkin>
    typename Docker<TSkin>::ElementType Docker<TSkin>::Element::GetType() const
    {
        return m_type;
    }

    template<typename TSkin>
    typename Docker<TSkin>::Leaf* Docker<TSkin>::Element::GetLeaf()
    {
        return std::get<LeafPointer>(m_data).get();
    }

    template<typename TSkin>
    typename Docker<TSkin>::Grid* Docker<TSkin>::Element::GetGrid()
    {
        return std::get<GridPointer>(m_data).get();
    }

    template<typename TSkin>
    typename Docker<TSkin>::Element* Docker<TSkin>::Element::GetParent()
    {
        return m_parent;
    }

    template<typename TSkin>
    template<typename TCallback>
    auto Docker<TSkin>::Element::VisitData(TCallback&& callback)
    {
        return std::visit(callback, m_data);
    }

    template<typename TSkin>
    bool Docker<TSkin>::Element::IsDynamic() const
    {
        return std::visit([&](auto& data)
        {
            return data->IsDynamic();
        }, m_data);
    }

    template<typename TSkin>
    typename Docker<TSkin>::EdgePointer Docker<TSkin>::Element::InsertElement(
        ElementPointer&& element,
        const DockingPosition position)
    {   
        auto [insertElement, insertIt] = InsertElementPreProcess(position);

        auto* newElementPtr = element.get();
        newElementPtr->m_parent = insertElement;

        auto* insertGrid = insertElement->GetGrid();
        auto insertedIt = insertGrid->elements.insert(insertIt, std::move(element));
        
        insertElement->AddDynamicElement(newElementPtr);

        // Create new grid if needed.
        Element* prevElementPtr = insertedIt->get() != insertGrid->elements.front().get() ? std::prev(insertedIt)->get() : nullptr;
        Element* nextElementPtr = insertedIt->get() != insertGrid->elements.back().get() ? std::next(insertedIt)->get() : nullptr;

        EdgePointer newEdge = nullptr;

        if (prevElementPtr && nextElementPtr)
        {
            newEdge = std::make_unique<Edge>(FlipDirection(insertGrid->direction), prevElementPtr, newElementPtr);
            newElementPtr->m_prevEdge = newEdge.get();
            newElementPtr->m_nextEdge = nextElementPtr->m_prevEdge;
            prevElementPtr->m_nextEdge = newEdge.get();

            if (nextElementPtr->m_prevEdge)
            {
                nextElementPtr->m_prevEdge->m_prevElement = newElementPtr;
            }
        }
        else if (prevElementPtr)
        {
            newEdge = std::make_unique<Edge>(FlipDirection(insertGrid->direction), prevElementPtr, newElementPtr);
            newElementPtr->m_prevEdge = newEdge.get();
            prevElementPtr->m_nextEdge = newEdge.get();
        }
        else if (nextElementPtr)
        {
            newEdge = std::make_unique<Edge>(FlipDirection(insertGrid->direction), newElementPtr, nextElementPtr);
            newElementPtr->m_nextEdge = newEdge.get();
            nextElementPtr->m_prevEdge = newEdge.get();
        }

        return newEdge;
    }

    template<typename TSkin>
    std::pair<typename Docker<TSkin>::ElementPointer, typename Docker<TSkin>::Edge*> Docker<TSkin>::Element::ExtractElement(Element* element)
    {
        if (m_type != ElementType::Grid)
        {
            return { nullptr, nullptr };
        }

        auto& grid = std::get<GridPointer>(m_data);

        // Find extract and erase from grid.
        auto elementIt = std::find_if(grid->elements.begin(), grid->elements.end(), [&](auto& child)
        {
            return child.get() == element;
        });

        if (elementIt == grid->elements.end())
        {
            return { nullptr, nullptr };
        }

        auto extractedElement = std::move(*elementIt);
        auto* extractedElementPtr = extractedElement.get();

        grid->elements.erase(elementIt);

        RemoveDynamicElement(extractedElementPtr);

        // Transform to this grid to a leaf or merge this grid if needed.
        MergeGrid();

        // Remove at least one edge if possible.
        Edge* extactedEdge = nullptr;
        if ((extactedEdge = extractedElementPtr->m_prevEdge) != nullptr)
        {
            auto* nextEdge = extractedElementPtr->m_nextEdge;
            auto* prevElement = extactedEdge->m_prevElement;

            if (prevElement)
            {
                prevElement->m_nextEdge = nextEdge;
            }
            if (nextEdge)
            {
                nextEdge->m_prevElement = prevElement;
            }
        }
        else if ((extactedEdge = extractedElementPtr->m_nextEdge) != nullptr)
        {
            auto* prevEdge = extractedElementPtr->m_prevEdge;
            auto* nextElement = extactedEdge->m_nextElement;

            if (nextElement)
            {
                nextElement->m_prevEdge = prevEdge;
            }
            if (prevEdge)
            {
                prevEdge->m_nextElement = nextElement;
            }
        }

        // Final cleanup and return extractions.
        extractedElementPtr->m_parent = nullptr;
        extractedElementPtr->m_prevEdge = nullptr;
        extractedElementPtr->m_nextEdge = nullptr;

        return { std::move(extractedElement), extactedEdge };
    }


    template<typename TSkin>
    std::pair<typename Docker<TSkin>::Element*, typename Docker<TSkin>::ElementPointerList::iterator> Docker<TSkin>::Element::InsertElementPreProcess(const DockingPosition position)
    {
        const auto gridDirection = GetInsertDirection(position); 
        const auto insertPosition = GetInsertPosition(position);

        Element* insertElement = nullptr;
        ElementPointerList::iterator insertIterator = {};

        if (m_type == ElementType::Grid)
        {
            if (GetGrid()->direction != gridDirection)
            {
                TransformToGrid(gridDirection);
            }
            insertElement = this;
            insertIterator = insertPosition == ElementPosition::First ? GetGrid()->elements.begin() : GetGrid()->elements.end();
        }
        else // Leaf
        {
            if (m_parent)
            {
                auto* parentGrid = m_parent->GetGrid();
                if (parentGrid->direction != gridDirection)
                {
                    TransformToGrid(gridDirection);
                    insertElement = this;
                    insertIterator = insertPosition == ElementPosition::First ? GetGrid()->elements.begin() : GetGrid()->elements.end();
                }
                else
                {
                    insertElement = m_parent;
                    insertIterator = std::find_if(parentGrid->elements.begin(), parentGrid->elements.end(), [&](auto& child)
                    {
                        return child.get() == this;
                    });
                    if (insertIterator != parentGrid->elements.end())
                    {
                        if (insertPosition == ElementPosition::Last)
                        {
                            insertIterator = std::next(insertIterator);
                        }
                    }
                }
            }
            else
            {
                TransformToGrid(gridDirection);
                insertElement = this;
                insertIterator = insertPosition == ElementPosition::First ? GetGrid()->elements.begin() : GetGrid()->elements.end();
            }
        }

        return { insertElement, insertIterator };
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::TransformToGrid(const Direction direction)
    {
        ElementPointer newElement = nullptr;

        if (m_type == ElementType::Grid)
        {
            auto& grid = std::get<GridPointer>(m_data);
            if (grid->direction == direction)
            {
                return;
            }

            newElement = std::make_unique<Element>(std::move(std::get<GridPointer>(m_data)));         
        }
        else // Leaf
        {
            newElement = std::make_unique<Element>(std::move(std::get<LeafPointer>(m_data)), requestedSize); // FIX? Move "move" to a step before?
        }

        auto* newElementPtr = newElement.get();
        newElementPtr->m_parent = this;
        
        auto newGrid = std::make_unique<Grid>(direction);
        newGrid->m_owner = this;
        newGrid->elements.push_back(std::move(newElement));
        if (newElementPtr->IsDynamic())
        {
            newGrid->dynamicElements.insert(newElementPtr);
        }
  
        m_data = std::move(newGrid);
        m_type = ElementType::Grid;
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::MergeGrid()
    {
        if (m_type != ElementType::Grid)
        {
            return;
        }

        auto& grid = std::get<GridPointer>(m_data);
        if (grid->elements.size() != 1)
        {
            return;
        }

        auto& child = grid->elements.front();
        if (child->m_type == ElementType::Grid)
        {
            auto extractedGrid = std::move(std::get<GridPointer>(child->m_data));
            extractedGrid->m_owner = this;

            grid->direction = extractedGrid->direction;
            grid->elements = std::move(extractedGrid->elements);
            grid->dynamicElements = std::move(extractedGrid->dynamicElements);

            for (auto& element : grid->elements)
            {
                element->m_parent = this;
            }
        }
        else // Leaf
        {           
            requestedSize = child->requestedSize;   

            auto extractedLeaf = std::move(std::get<LeafPointer>(child->m_data));
            extractedLeaf->m_owner = this;
            m_data = std::move(extractedLeaf);
            m_type = ElementType::Leaf;
        }
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::AddDynamicElement(Element* element)
    {
        if (!element->IsDynamic())
        {
            return;
        }

        auto* grid = GetGrid();

        auto wasDynamic = !grid->dynamicElements.empty();
        grid->dynamicElements.insert(element);
        auto isDynamic = !grid->dynamicElements.empty();

        if (m_parent && !wasDynamic && isDynamic)
        {
            m_parent->AddDynamicElement(this);
        }
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::RemoveDynamicElement(Element* element)
    {
        if (!element->IsDynamic())
        {
            return;
        }

        auto* grid = GetGrid();
        auto wasDynamic = !grid->dynamicElements.empty();
        grid->dynamicElements.erase(element);
        auto isDynamic = !grid->dynamicElements.empty();

        if (m_parent && wasDynamic && !isDynamic)
        {
            m_parent->RemoveDynamicElement(this);
        }
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
        m_prevElement(prevElement),
        m_nextElement(nextElement)
    {}

    template<typename TSkin>
    Mouse::Cursor Docker<TSkin>::Edge::GetSizeCursor() const
    {
        return direction == Direction::Horizontal ? Mouse::Cursor::SizeUpDown : Mouse::Cursor::SizeLeftRight;
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
        auto newLeaf = std::make_unique<Leaf>(pendingLeaf.widgetData, pendingLeaf.isDynamic);
        m_leafs.insert(newLeaf.get());

        const auto widgetSize = pendingLeaf.widgetData->GetWidget()->size; // FIX! Add new function for getting raw pointer to widget.
        const auto requestedSize = pendingLeaf.isDynamic ?
            minElementSize :
            Vector2f32{
                std::max(minElementSize.x, widgetSize.x),
                std::max(minElementSize.y, widgetSize.y)
        };

        auto newElement = std::make_unique<Element>(std::move(newLeaf), requestedSize);

        if (m_rootElement)
        {
            auto newEdge = m_rootElement->InsertElement(std::move(newElement), pendingLeaf.position);
            if (newEdge)
            {
                m_edges.insert(std::move(newEdge));
            }
        }
        else
        {
            m_rootElement = std::move(newElement);
        } 
    }

    template<typename TSkin>
    typename Docker<TSkin>::ElementPointer Docker<TSkin>::ExtractElement(Element* element)
    {
        auto* parent = element->GetParent();
        if (!parent)
        {
            return nullptr;
        }

        auto [extractedElement, extractedEdge] = parent->ExtractElement(element);

        if (extractedEdge)
        {
            auto edgeIt = std::find_if(m_edges.begin(), m_edges.end(), [&](auto& edge)
            {
                return edge.get() == extractedEdge;
            });
            if (edgeIt != m_edges.end())
            {
                m_edges.erase(edgeIt);
            }        
        }

        return std::move(extractedElement);
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
    void Docker<TSkin>::ActivateNormalUpdate()
    {
        m_edgeDragData.Reset();
        m_leafDragData.Reset();

        m_mouseInputUpdateFunc = &Docker<TSkin>::HandleNormalMouseEvent;
    }

    template<typename TSkin>
    void Docker<TSkin>::ActivateEdgeDragUpdate(Edge* pressedEdge, const Vector2f32& mousePosition)
    {
        m_edgeDragData.Reset();
        m_leafDragData.Reset();

        m_edgeDragData.pressedEdge = pressedEdge;
        m_edgeDragData.prevMousePosition = mousePosition;

        m_mouseInputUpdateFunc = &Docker<TSkin>::HandleEdgeDragMouseEvent;
        GetData().canvas->OverrideMouseEventsUntilMouseRelease(*this);
        SetCursor(pressedEdge->GetSizeCursor());
    }

    template<typename TSkin>
    void Docker<TSkin>::ActivateLeafDragUpdate(Leaf* pressedLeaf, const Vector2f32& mousePosition)
    {
        m_edgeDragData.Reset();
        m_leafDragData.Reset();

        m_leafDragData.pressedLeaf = pressedLeaf;
        m_leafDragData.initialMousePosition = mousePosition;

        m_mouseInputUpdateFunc = &Docker<TSkin>::HandleLeafDragMouseEvent;
        GetData().canvas->OverrideMouseEventsUntilMouseRelease(*this);
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleNormalMouseEvent(const WidgetEvent& widgetEvent)
    {
        switch (widgetEvent.subType)
        {
            case WidgetEventSubType::MouseButtonPressed: return HandleNormalMousePressEvent(widgetEvent.mouseEvent);
            case WidgetEventSubType::MouseMove: return HandleNormalMouseMoveEvent(widgetEvent.mouseEvent);
            case WidgetEventSubType::MouseLeave: SetCursor(Mouse::Cursor::Normal); break;
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEdgeDragMouseEvent(const WidgetEvent& widgetEvent)
    {
        switch(widgetEvent.subType)
        {
            case WidgetEventSubType::MouseMove: return HandleEdgeDragMouseMoveEvent(widgetEvent.mouseEvent);
            case WidgetEventSubType::MouseButtonReleasedIn:
            case WidgetEventSubType::MouseButtonReleasedOut: return HandleEdgeDragMouseReleaseEvent(widgetEvent.mouseEvent);
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleLeafDragMouseEvent(const WidgetEvent& widgetEvent)
    {
        switch(widgetEvent.subType)
        {
            case WidgetEventSubType::MouseMove: return HandleLeafDragMouseMoveEvent(widgetEvent.mouseEvent);
            case WidgetEventSubType::MouseButtonReleasedIn:
            case WidgetEventSubType::MouseButtonReleasedOut: return HandleLeafDragMouseReleaseEvent(widgetEvent.mouseEvent);
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleNormalMousePressEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        auto* pressedEdge = FindIntersectingEdge(mouseEvent.position);
        if (pressedEdge)
        {         
            ActivateEdgeDragUpdate(pressedEdge, mouseEvent.position);
            return true;
        }

        auto* pressedLeaf = FindIntersectingDraggableLeaf(mouseEvent.position);
        if (pressedLeaf)
        {
            ActivateLeafDragUpdate(pressedLeaf, mouseEvent.position);
            return true;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleNormalMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        auto* hoveredEdge = FindIntersectingEdge(mouseEvent.position);
        if (hoveredEdge)
        {
            SetCursor(hoveredEdge->GetSizeCursor());
        }
        else
        {
            SetCursor(Mouse::Cursor::Normal);
        }
        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEdgeDragMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (!m_edgeDragData.pressedEdge)
        {
            return false;
        }

        SetCursor(m_edgeDragData.pressedEdge->GetSizeCursor());

        auto mouseMovement = Vector2f32(mouseEvent.position) - m_edgeDragData.prevMousePosition;
        bool movedEdge = false;

        if (m_edgeDragData.pressedEdge->direction == Direction::Horizontal)
        {
            movedEdge = HandleDirectionalEdgeMovement<Direction::Horizontal>(*m_edgeDragData.pressedEdge, mouseMovement.y);
        }
        else
        {
            movedEdge = HandleDirectionalEdgeMovement<Direction::Vertical>(*m_edgeDragData.pressedEdge, mouseMovement.x);
        }

        if (movedEdge)
        {
            m_forceUpdateBounds = true;
            m_edgeDragData.prevMousePosition = mouseEvent.position;
        }

        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEdgeDragMouseReleaseEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        SetCursor(Mouse::Cursor::Normal);
        ActivateNormalUpdate();
        SetSkinState(State{});

        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleLeafDragMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (!m_leafDragData.dragIsActivated)
        {
            float distance = (m_leafDragData.initialMousePosition - mouseEvent.position).Length();
            if (distance < widgetDragActivationDistance)
            {
                return false;
                
            }
            m_leafDragData.dragIsActivated = true;
        }

        SetCursor(Mouse::Cursor::SizeAll);

        auto* leaf = FindIntersectingLeaf(mouseEvent.position);
        if (!leaf || leaf == m_leafDragData.pressedLeaf)
        {
            SetSkinState(State{ });
            return false;
        }

        Bounds2f32 dockingBounds = {};
        if (GetDockingPositionInElement(mouseEvent.position, *leaf, dockingBounds, m_leafDragData.dockingPosition))
        {
            m_leafDragData.dockingLeaf = leaf;
            SetSkinState(State{ State::LeafDragState{ dockingBounds } });
        }
        else
        {
            m_leafDragData.dockingLeaf = nullptr;
            SetSkinState(State{ });
        }

        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleLeafDragMouseReleaseEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (m_leafDragData.dockingLeaf && m_leafDragData.pressedLeaf)
        {
            auto* fromElementPtr = m_leafDragData.pressedLeaf->AsElement();
            auto* toElementPtr = m_leafDragData.dockingLeaf->AsElement();

            if (fromElementPtr && toElementPtr)
            {
                ElementPointer fromElement = ExtractElement(fromElementPtr);
                if (fromElement)
                {
                    toElementPtr = m_leafDragData.dockingLeaf->AsElement(); // Get as element again. Very important to do after a call to ExtractElement.
                    auto newEdge = toElementPtr->InsertElement(std::move(fromElement), m_leafDragData.dockingPosition);
                    if (newEdge)
                    {
                        m_edges.insert(std::move(newEdge));
                    }

                    m_forceUpdateBounds = true;
                }
            }
        }

        SetCursor(Mouse::Cursor::Normal);
        ActivateNormalUpdate();
        SetSkinState(State{});

        return true;
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VEdgeDirection>
    bool Docker<TSkin>::HandleDirectionalEdgeMovement(Edge& edge, float movement)
    {   
        constexpr Direction flippedDirection = FlipDirection(VEdgeDirection);
        
        auto* prevElement = edge.m_prevElement;
        auto* nextElement = edge.m_nextElement;

        movement = std::max(
            movement,
            GetDirectionalWidth<flippedDirection>(prevElement->minSize) - GetDirectionalWidth<flippedDirection>(prevElement->renderSize));
        
        movement = std::min(
            movement,
            GetDirectionalWidth<flippedDirection>(nextElement->renderSize) - GetDirectionalWidth<flippedDirection>(nextElement->minSize));
              
        if (movement == 0.0f)
        {
            return false;
        }        

        if (prevElement)
        {
            AddDirectionalHeight<VEdgeDirection>(prevElement->requestedSize, movement);
        }
        if (nextElement)
        {
            AddDirectionalHeight<VEdgeDirection>(nextElement->requestedSize, -movement);
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
    typename Docker<TSkin>::Leaf* Docker<TSkin>::FindIntersectingDraggableLeaf(const Vector2f32& point)
    {
        for (auto* leaf : m_leafs)
        {
            auto* draggableWidget = leaf->draggableWidget;
            if (draggableWidget && draggableWidget->GetDragBounds().Intersects(point))
            {
                return leaf;
            }
        }

        return nullptr;
    }

    template<typename TSkin>
    typename Docker<TSkin>::Leaf* Docker<TSkin>::FindIntersectingLeaf(const Vector2f32& point)
    {
        for (auto* leaf : m_leafs)
        {
            if (leaf->AsElement()->bounds.Intersects(point))
            {
                return leaf;
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

        PreCalculateElementBounds(*m_rootElement.get());
        CalculateElementBounds(*m_rootElement.get(), GetGrantedBounds());
    }

    template<typename TSkin>
    void Docker<TSkin>::PreCalculateElementBounds(Element& element)
    {
        element.VisitData([&](auto& data)
        {
            PreCalculateElementBounds(element, *data.get());
        });
    }

    template<typename TSkin>
    void Docker<TSkin>::PreCalculateElementBounds(Element& element, Leaf& leaf)
    {
        element.minSize = minElementSize;
        element.renderSize = minElementSize;
    }

    template<typename TSkin>
    void Docker<TSkin>::PreCalculateElementBounds(Element& element, Grid& grid )
    {
        if (grid.direction == Direction::Horizontal)
        {
            PreCalculateElementBounds<Direction::Horizontal>(element, grid);
        }
        else
        {
            PreCalculateElementBounds<Direction::Vertical>(element, grid);
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    void Docker<TSkin>::PreCalculateElementBounds(Element& element, Grid& grid)
    {
        auto minSize = Vector2f32{ };
        auto requestedSize = Vector2f32{ };

        for (auto& childPtr : grid.elements)
        {
            auto& child = *childPtr.get();
            PreCalculateElementBounds(child);

            if constexpr (VGridDirection == Direction::Horizontal)
            {
                minSize.x += child.minSize.x;
                requestedSize.x += child.requestedSize.x;

                if (child.minSize.y > minSize.y)
                {
                    minSize.y = child.minSize.y;
                }
                if (child.requestedSize.y > requestedSize.y)
                {
                    requestedSize.y = child.requestedSize.y;
                }
            }
            else
            {
                minSize.y += child.minSize.y;
                requestedSize.y += child.requestedSize.y;

                if (child.minSize.x > minSize.x)
                {
                    minSize.x = child.minSize.x;
                }
                if (child.requestedSize.x > requestedSize.x)
                {
                    requestedSize.x = child.requestedSize.x;
                }
            }        
        }

        element.minSize = minSize;
        element.renderSize = minSize;

        SetDirectionalWidth<VGridDirection>(element.requestedSize, GetDirectionalWidth<VGridDirection>(requestedSize));
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateElementBounds(Element& element, const Bounds2f32& grantedBounds)
    {
        element.VisitData([&](auto& data)
        {
            CalculateElementBounds(element , *data.get(), grantedBounds);
        });
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateElementBounds(Element& element, Leaf& leaf, const Bounds2f32& grantedBounds)
    {
        leaf.widgetData->SetGrantedBounds(grantedBounds);
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateElementBounds(Element& element, Grid& grid, const Bounds2f32& grantedBounds)
    {
        if (grid.direction == Direction::Horizontal)
        {
            return CalculateElementBounds<Direction::Horizontal>(element, grid, grantedBounds);
        }
        else
        {
            return CalculateElementBounds<Direction::Vertical>(element, grid, grantedBounds);
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    void Docker<TSkin>::CalculateElementBounds(Element& element, Grid& grid, const Bounds2f32& grantedBounds)
    {
        if (grid.elements.empty())
        {
            return;
        }

        const auto grantedSize = grantedBounds.GetSize();
        const auto spacingTotalWidth = grid.elements.size() > 1 ? static_cast<float>(grid.elements.size() - 1) * spacing : 0.0f;
        const auto grantedWidth = GetDirectionalWidth<VGridDirection>(grantedSize) - spacingTotalWidth;
        const auto requestedWidth = GetDirectionalWidth<VGridDirection>(element.requestedSize);
        auto widthDiff = grantedWidth - requestedWidth;

        // Distribute dynamic size.
        if (!grid.dynamicElements.empty())
        {
            auto partialWidthDiff = widthDiff / static_cast<float>(grid.dynamicElements.size());
            for (auto it = grid.dynamicElements.rbegin(); it != grid.dynamicElements.rend(); it++)
            {
                auto& childElement = *(*it);

                auto childPartialWidthDiff = std::max(
                    partialWidthDiff, 
                    GetDirectionalWidth<VGridDirection>(childElement.minSize) - GetDirectionalWidth<VGridDirection>(childElement.requestedSize));

                AddDirectionalWidth<VGridDirection>(childElement.requestedSize, childPartialWidthDiff);
                childElement.renderSize = childElement.requestedSize;

                widthDiff -= childPartialWidthDiff;
            }
        }

        // Distribute remaining diff
        for (auto it = grid.elements.begin(); it != grid.elements.end(); it++)
        {
            auto& childElement = *it->get();

            if (childElement.IsDynamic())
            {
                continue;
            }
                
            auto childWidthDiff = std::max(
                widthDiff, 
                GetDirectionalWidth<VGridDirection>(childElement.minSize) - GetDirectionalWidth<VGridDirection>(childElement.requestedSize));

            childElement.renderSize = childElement.requestedSize;
            AddDirectionalWidth<VGridDirection>(childElement.renderSize, childWidthDiff);

            widthDiff -= childWidthDiff;
        }

        // Set bounds.
        constexpr Direction flippedDirection = FlipDirection(VGridDirection);
        auto boundsLeft = grantedBounds;
        auto elementBounds = Bounds2f32{ };

        auto it = grid.elements.rbegin();
        for (; it != grid.elements.rend(); it++)
        {
            if (boundsLeft.IsEmpty())
            {
                break;
            }

            auto& childElement = *it->get();

            DirectionalShrinkBounds<VGridDirection>(boundsLeft, elementBounds, GetDirectionalWidth<VGridDirection>(childElement.renderSize));

            childElement.bounds = elementBounds;
            SetElementPrevEdgeBounds<flippedDirection>(childElement, grantedBounds);

            CalculateElementBounds(childElement, elementBounds);

            DirectionalShrinkBounds<VGridDirection>(boundsLeft, spacing);
        }

        HideElements(it, grid.elements.rend());
    }

    template<typename TSkin>
    template<typename TIterator>
    void Docker<TSkin>::HideElements(TIterator begin, TIterator end)
    {
        // FIX, should implement partial tree node removal and use it instead of SetGrantedBounds.
        for (; begin != end; begin++)
        {
            auto& element = *begin;

            if (element->GetType() == ElementType::Leaf) // Leaf
            {
                auto* leaf = element->GetLeaf();
                leaf->widgetData->SetGrantedBounds({ 0.0f, 0.0f, 0.0f, 0.0f });
            }
            else // Grid
            {
                auto* grid = element->GetGrid();
                HideElements(grid->elements.rbegin(), grid->elements.rend());
            }
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    constexpr void Docker<TSkin>::DirectionalShrinkBounds(Bounds2f32& bounds, float shrinkValue)
    {
        if constexpr (VGridDirection == Direction::Horizontal)
        {
            float shrinkPosition = std::max(bounds.left, bounds.right - shrinkValue);
            bounds.right -= std::abs(shrinkPosition - bounds.right);
        }
        else
        {
            float shrinkPosition = std::max(bounds.top, bounds.bottom - shrinkValue);
            bounds.bottom -= std::abs(shrinkPosition - bounds.bottom);
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    constexpr void Docker<TSkin>::DirectionalShrinkBounds(Bounds2f32& bounds, Bounds2f32& shrinkedBounds, float shrinkValue)
    {
        if constexpr (VGridDirection == Direction::Horizontal)
        {
            float shrinkPosition = std::max(bounds.left, bounds.right - shrinkValue);
            shrinkValue = std::abs(shrinkPosition - bounds.right);

            shrinkedBounds = {
                bounds.right - shrinkValue, bounds.top, bounds.right, bounds.bottom
            };

            bounds.right -= shrinkValue;
        }
        else
        {
            float shrinkPosition = std::max(bounds.top, bounds.bottom - shrinkValue);
            shrinkValue = std::abs(shrinkPosition - bounds.bottom);

            shrinkedBounds = {
                bounds.left, bounds.bottom - shrinkValue, bounds.right, bounds.bottom
            };

            bounds.bottom -= shrinkValue;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VEdgeDirection>
    void Docker<TSkin>::SetElementPrevEdgeBounds(Element& element, const Bounds2f32& grantedBounds)
    {
        auto* prevEdge = element.m_prevEdge;
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
    constexpr typename Docker<TSkin>::Direction Docker<TSkin>::FlipDirection(const Direction direction)
    {
        return direction == Direction::Horizontal ? Direction::Vertical : Direction::Horizontal;
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
    bool Docker<TSkin>::GetDockingPositionInElement(
        const Vector2f32& mousePosition,
        Leaf& leaf,
        Bounds2f32& dockingBounds,
        DockingPosition& position)
    {
        auto* element = leaf.AsElement();
        if (!element)
        {
            return false;
        }

        auto oneThirdSize = element->bounds.GetSize() / 3.0f;

        // Wow, WTF, optimize this, please...
        const auto leftBounds = Bounds2f32{ 
            element->bounds.left,
            element->bounds.top + oneThirdSize.y,
            element->bounds.left + oneThirdSize.x,
            element->bounds.bottom - oneThirdSize.y
        };
        if (leftBounds.Intersects(mousePosition))
        {
            dockingBounds = leftBounds;
            position = DockingPosition::Left;
            return true;
        }

        const auto topBounds = Bounds2f32{
           element->bounds.left + oneThirdSize.x,
           element->bounds.top,
           element->bounds.right - oneThirdSize.x,
           element->bounds.top + oneThirdSize.y
        };
        if (topBounds.Intersects(mousePosition))
        {
            dockingBounds = topBounds;
            position = DockingPosition::Top;
            return true;
        }

        const auto rightBounds = Bounds2f32{
           element->bounds.right - oneThirdSize.x,
           element->bounds.top + oneThirdSize.y,
           element->bounds.right,
           element->bounds.bottom - oneThirdSize.y
        };
        if (rightBounds.Intersects(mousePosition))
        {
            dockingBounds = rightBounds;
            position = DockingPosition::Right;
            return true;
        }

        const auto bottomBounds = Bounds2f32{
           element->bounds.left + oneThirdSize.x,
           element->bounds.bottom - oneThirdSize.y,
           element->bounds.right - oneThirdSize.x,
           element->bounds.bottom
        };
        if (bottomBounds.Intersects(mousePosition))
        {
            dockingBounds = bottomBounds;
            position = DockingPosition::Bottom;
            return true;
        }

        return false;
    }

    template<typename TSkin>
    Docker<TSkin>::EdgeDragData::EdgeDragData() :
        pressedEdge(nullptr),
        prevMousePosition(0.0f, 0.0f)
    { }

    template<typename TSkin>
    void Docker<TSkin>::EdgeDragData::Reset()
    {
        pressedEdge = nullptr;
        prevMousePosition = { 0.0f, 0.0f };
    }

    template<typename TSkin>
    Docker<TSkin>::LeafDragData::LeafDragData() :
        pressedLeaf(nullptr),
        initialMousePosition(0.0f, 0.0f),
        dockingLeaf(nullptr),
        dockingPosition(DockingPosition::Left),
        dragIsActivated(false)
    {}
    
    template<typename TSkin>
    void Docker<TSkin>::LeafDragData::Reset()
    {
        pressedLeaf = nullptr;
        initialMousePosition = {0.0f, 0.0f};
        dockingLeaf = nullptr;
        dockingPosition = DockingPosition::Left;
        dragIsActivated = false;
    }

}
